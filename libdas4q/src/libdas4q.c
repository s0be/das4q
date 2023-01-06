/**
 * Copyright 2023 Patrick Erley <paerley@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "libdas4q.h"

#include <errno.h>
#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * The purpose and meaning of these is still pretty unknown.
 * it is transmitted in 7 byte chunks, with 0x01 prepended to each packet.
 */
typedef struct __attribute__((__packed__)) das4q_set_cmd {
    uint8_t magic;       // Will be set to 0xea
    uint8_t pkt_size;    // Will be 0x08 to set the state
    uint8_t always_78h;  // Will be set to 0x78
    uint8_t cmd_type;    // will be 0x08 to set  the state
    das4q_map_t keycode;
    das4q_keymode_t mode;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t csum;
} das4q_set_cmd_t;

typedef struct __attribute__((__packed__)) das4q_active_cmd {
    uint8_t magic;       // Will be set to 0xea
    uint8_t pkt_size;    // Will be 0x0b for active.
    uint8_t always_78h;  // Will be set to 0x78
    uint8_t cmd_type;    // will be 0x04 for active
    das4q_map_t keycode;
    das4q_active_keymode_t mode;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t unk[3];
    uint8_t csum;
} das4q_active_cmd_t;

typedef struct das4q_priv {
    libusb_device_handle* handle;
} das4q_priv_t;

static bool verbose = true;

libusb_device_handle* get_device_by_vid_pid(uint16_t vid, uint16_t pid) {
    libusb_device_handle* handle;
    handle = libusb_open_device_with_vid_pid(NULL, vid, pid);
    if (handle == NULL) {
        perror(" Failed to open device.\n");
        return NULL;
    }

    int ret = libusb_set_auto_detach_kernel_driver(handle, 1);
    if (ret < 0) {
        perror(" Failed to set kernel auto detach\n");
        goto fail_kern;
    }

    // ret = libusb_kernel_driver_active(handle, 1);

    ret = libusb_claim_interface(handle, 1);
    if (ret < 0) {
        perror(" Failed to claim interface\n");
        goto fail_kern;
    }

    return handle;

fail_kern:
    libusb_close(handle);
    return NULL;
}

#define HID_GET_REPORT 0x01
#define HID_SET_REPORT 0x09

#define HID_REPORT_TYPE_FEATURE 0x03

int write_set_report(libusb_device_handle* handle, char* buff, int len) {
    for (int i = 0; i < len; i++) {
        printf("%02x ", (uint8_t)buff[i]);
    }
    printf("\n");
    int ret = libusb_control_transfer(
        handle,
        LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS |
            LIBUSB_RECIPIENT_INTERFACE,
        HID_SET_REPORT,
        HID_REPORT_TYPE_FEATURE << 8 | 0x01,  // Report ID 01
        1,                                    // Index 1
        buff, len, 3000);
    if (ret < 0) {
        printf("Got Error %s\n", libusb_error_name(ret));
    }
    return ret;
}

int read_get_report(libusb_device_handle* handle, char* obuff, int len) {
    memset(obuff, 0, len);

    int ret = 0;
    int total = 0;
    bool done = false;

    while (ret >= 0 && (total == 0 || total < len) && !done) {
        const char ebuff[8] = {0};
        char buff[8] = {0};

        ret = libusb_control_transfer(
            handle,
            LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS |
                LIBUSB_RECIPIENT_INTERFACE,
            HID_GET_REPORT, HID_REPORT_TYPE_FEATURE << 8 | 0x01, 1, buff, 8,
            3000);

        if (memcmp(buff, ebuff, 8) == 0) {
            done = true;
        }
        memcpy(obuff + total, buff, 8);
        total += ret;
    }
    if (ret >= 0) {
        return total;
    } else {
        return ret;
    }
}

uint8_t das4q_checksum_cmd(uint8_t* cmd) {
    // magic + length
    uint8_t len = cmd[1] + 2;
    uint8_t csum = 0;
    for (int i = 0; i < len; i++) {
        csum = csum ^ cmd[i];
    }
    return csum;
}

int das4q_send_cmd(das4q_handle handle, uint8_t* cmd) {
    // magic + length
    uint8_t len = cmd[1] + 2;
    das4q_priv_t* priv = handle;
    uint8_t usbcmd[8] = {0};
    int sent;
    int ret = 0;
    int tries = 0;
retry_cmd:
    for (int i = 0; i < len; i++) {
        printf("%02x ", (uint8_t)cmd[i]);
    }
    printf("\n");
    tries++;
    if (tries == 3) {
        return -EFAULT;
    }
    sent = 0;
    while (sent < len) {
        memset(usbcmd, 0, 8);
        usbcmd[0] = 0x01;
        if (len - sent < 7) {
            memcpy(usbcmd + 1, cmd + sent, len - sent);
        } else {
            memcpy(usbcmd + 1, cmd + sent, 7);
        }
        ret = write_set_report(priv->handle, usbcmd, 8);
        if (ret != 8) {
            goto retry_cmd;
        }
        sent += ret;
        sent -= 1;
    }
    return ret;
}

bool das4q_apply_changes(das4q_handle handle) {
    das4q_priv_t* priv = handle;
    uint8_t cmd1[] = "\x01\xea\x03\x78\x0a\x9b\x00\x00";
    int ret = write_set_report(priv->handle, cmd1, 8);
    if (ret != 8) {
        return false;
    }
    uint8_t unknown[128] = {0};
    ret = read_get_report(priv->handle, unknown, 128);
    if (ret < 0) {
        return false;
    }
    return true;
}

bool das4q_set_key_backlight(das4q_handle handle, das4q_map_t key,
                             das4q_setting_t setting,
                             das4q_active_setting_t active_setting) {
    das4q_priv_t* priv = handle;

    das4q_set_cmd_t cmd1 = {.magic = 0xea,
                            .pkt_size = 0x08,
                            .always_78h = 0x78,
                            .cmd_type = 0x08,
                            .keycode = key,
                            .mode = setting.mode,
                            .red = setting.red,
                            .green = setting.green,
                            .blue = setting.blue,
                            .csum = 0};

    das4q_active_cmd_t cmd2 = {.magic = 0xea,
                               .pkt_size = 0x0b,
                               .always_78h = 0x78,
                               .cmd_type = 0x04,
                               .keycode = key,
                               .mode = active_setting.mode,
                               .red = active_setting.red,
                               .green = active_setting.green,
                               .blue = active_setting.blue,
                               .unk = {0, 0, 0},
                               .csum = 0};
    switch (cmd2.mode) {
        case DAS4Q_ACTIVE_MODE_BREATHE:
            cmd2.unk[0] = 0x03;
            cmd2.unk[1] = 0xe8;
            cmd2.unk[2] = 0x03;
            break;
        case DAS4Q_ACTIVE_MODE_CYCLE:
            cmd2.unk[0] = 0x13;
            cmd2.unk[1] = 0x88;
            cmd2.unk[2] = 0x00;
            break;
        case DAS4Q_ACTIVE_MODE_SOLID:
            cmd2.unk[0] = 0x07;
            cmd2.unk[1] = 0xd0;
            cmd2.unk[2] = 0x00;
            break;
        case DAS4Q_ACTIVE_MODE_BLINK:
            cmd2.unk[0] = 0x01;
            cmd2.unk[1] = 0xf4;
            cmd2.unk[2] = 0x03;
            break;
        default:
            break;
    }

    cmd1.csum = das4q_checksum_cmd((uint8_t*)(&cmd1));
    cmd2.csum = das4q_checksum_cmd((uint8_t*)(&cmd2));

    int tries = 0;
retry:
    tries++;
    if (tries >= 3) {
        return false;
    }
    if (!das4q_send_cmd(handle, (uint8_t*)(&cmd1))) {
        return false;
    }

    if (!das4q_send_cmd(handle, (uint8_t*)(&cmd2))) {
        return false;
    }

    unsigned char unknown[128] = {0};
    {
        int ret = read_get_report(priv->handle, unknown, 128);
        uint8_t success_packet[] = {0xed, 0x03, 0x78, 0x00, 0x96, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00};

        if (ret != 16 || memcmp(unknown, success_packet, 16) != 0) {
            printf("Packet didn't match: ");
            for (int i = 0; i < ret; i++) {
                printf("0x%02x ", unknown[i]);
            }
            printf("\n");
            printf("                     ");
            for (int i = 0; i < ret; i++) {
                printf("0x%02x ", success_packet[i]);
            }
            printf("\n");
            goto retry;
        }
    }
    return true;
}

bool das4q_check_version(das4q_handle handle) {
    das4q_priv_t* priv = handle;
    char magic_string[] = "\x01\xea\x02\xb0\x58\x00\x00\x00";
    int ret = 0;
    unsigned char version_string[128] = {0};

retry:
    memset(version_string, 0, 128);
    ret = write_set_report(priv->handle, magic_string, 8);
    ret = read_get_report(priv->handle, version_string, 128);

    // We sent 0x01, 0xEA..
    // Maybe 0xED is the response?
    if (version_string[0] != 0xED) {  // Magic byte 1?
        printf("Wrong first byte\n");
        goto retry;
    }

    if (version_string[1] != 0x14) {  // Bytes after magic byte
        printf("Unexpected version length? %d\n", version_string[1]);
        return false;
    }

    // Not sure what this byte is... it was in the first magic packet?
    if (version_string[2] != 0xB0) {
        printf("Unexpected magic response? 0x%02x\n",
               (uint8_t)version_string[2]);
        return false;
    }

    if (version_string[3] != 0x00) {
        printf("Dunno...\n");
        return false;
    }

    if (memcmp(version_string + 4, "S2716V21/S2749V31m",
               strlen("S2716V21/S2749V31m")) != 0) {
        printf("Got unexpected version string: %s, expected %s",
               version_string + 4, "S2716V21/S2749V31m");
        return false;
    }

    printf("Version %s\n", version_string + 4);
    return true;
}

das4q_handle das4q_init_device(char* hiddev) {
    if (sizeof(das4q_map_t) != 1) {
        perror("Key Enum wrong size");
        return NULL;
    }
    if (sizeof(das4q_keymode_t) != 1) {
        perror("Mode Enum wrong size");
    }

    if (hiddev != NULL) {
        errno = -ENOTSUP;
        perror("named open not implemented yet\n");
        return NULL;
    }

    das4q_priv_t* priv = calloc(1, sizeof(das4q_priv_t));
    if (priv == NULL) {
        errno = -ENOMEM;
        perror("Failed to alloc private data\n");
        return NULL;
    }
    libusb_init(NULL);

    if (verbose) {
        const struct libusb_version* version = libusb_get_version();
        printf("Using libusb v%d.%d.%d.%d\n", version->major, version->minor,
               version->micro, version->nano);
    }

    if (hiddev == NULL) {
        // Should handle be an array?  How should we handle multiple
        // keyboards?
        priv->handle = get_device_by_vid_pid(0x24f0, 0x2037);
    }

    if (priv->handle == NULL) {
        // It might be better to register as a udev listener here
        // and watch for plug events... oh well.
        errno = -ENOENT;
        goto fatal;
    }
    if (das4q_check_version(priv)) {
        // Clears the backlight
        das4q_apply_changes(priv);
    }

    return priv;

fatal:
    free(priv);
    return NULL;
}

void das4q_close_device(das4q_handle handle) {
    das4q_priv_t* priv = handle;
    if (priv->handle) {
        libusb_close(priv->handle);
    }
    free(handle);
}
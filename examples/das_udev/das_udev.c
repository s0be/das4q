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
#include <argp.h>
#include <stdio.h>
#include <stdlib.h>

#include "cjson/cJSON.h"
#include "libdas4q.h"

das4q_setting_t *parse_setting(cJSON *node) {
    cJSON *mode = cJSON_GetObjectItem(node, "mode");
    cJSON *red = cJSON_GetObjectItem(node, "red");
    cJSON *green = cJSON_GetObjectItem(node, "green");
    cJSON *blue = cJSON_GetObjectItem(node, "blue");

    if (mode == NULL || red == NULL || green == NULL || blue == NULL) {
        printf("Failed parsing node, missing values:\n %s", cJSON_Print(node));
        return NULL;
    }

    if (!cJSON_IsNumber(mode) || !cJSON_IsNumber(red) ||
        !cJSON_IsNumber(green) || !cJSON_IsNumber(blue)) {
        printf("Numeric values expected:\n %s", cJSON_Print(node));
        return NULL;
    }

    das4q_setting_t *ret = calloc(1, sizeof(das4q_setting_t));
    // We could validate this, but nah.
    ret->mode = mode->valueint & 0xFF;
    ret->red = red->valueint & 0xFF;
    ret->green = green->valueint & 0xFF;
    ret->blue = blue->valueint & 0xFF;

    return ret;
}

void apply_config_file(char *config_file, das4q_handle handle) {
    FILE *fp = fopen(config_file, "r");
    if (fp == NULL) {
        printf("Failed to open %s\n", config_file);
        return;
    }

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *config_json = calloc(fsize, sizeof(char));
    fread(config_json, fsize, 1, fp);

    cJSON *cfg = cJSON_Parse(config_json);
    if (cfg != NULL) {
        das4q_setting_t *def_set = NULL;
        cJSON *def = cJSON_GetObjectItem(cfg, "default");
        if (def != NULL) {
            def_set = parse_setting(def);
        }
        if (def_set == NULL) {
            def_set = calloc(1, sizeof(das4q_setting_t));
            def_set->mode = 1;
            def_set->red = 0;
            def_set->green = 0;
            def_set->blue = 0;
        }
        das4q_setting_t *set_array[0x84] = {0};

        cJSON *conf_array = cJSON_GetObjectItem(cfg, "keys");
        if (conf_array != NULL) {
            cJSON *c = NULL;
            cJSON_ArrayForEach(c, conf_array) {
                cJSON *cnode = cJSON_GetObjectItem(c, "setting");
                cJSON *k = cJSON_GetObjectItem(c, "key");
                if (cnode == NULL || k == NULL || !cJSON_IsNumber(k)) {
                    printf("Something wrong: \n%s\n",
                           cJSON_PrintUnformatted(c));
                    continue;
                }
                set_array[k->valueint] = parse_setting(cnode);
            }
        }

        for (int i = 0; i < 0x84; i++) {
            das4q_setting_t *s = NULL;
            if (set_array[i] != NULL) {
                s = set_array[i];
            } else {
                s = def_set;
            }
            das4q_set_key_backlight(handle, i, *s);
        }

        free(def_set);
        cJSON_free(cfg);
    }
    free(config_json);
    fclose(fp);
}

const char *argp_program_version = "das_udev 0.01";
const char *argp_program_bug_address = "paerley@gmail.com";
static char doc[] =
    "Simple program to apply backlight settings to Das Keyboard 4Q";
static char args_doc[] = "";
static struct argp_option options[] = {
    {"config-file", 'c', "filename", 0, "Config file to apply"},
    {"red", 'r', "0", 0, "Default red amount"},
    {"green", 'g', "0", 0, "Default green amount"},
    {"blue", 'b', "0", 0, "Default blue amount"},
    {"mode", 'm', "1", 0,
     "Default mode:\n"
     "1 - Solid\n"
     "31 - Blinking"},
    {0}};

struct arguments {
    char *config_file;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    das4q_keymode_t mode;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
        case 'c':
            arguments->config_file = arg;
            break;
        case 'r':
            arguments->red = arg ? atoi(arg) : 0;
            break;
        case 'g':
            arguments->green = arg ? atoi(arg) : 0;
            break;
        case 'b':
            arguments->blue = arg ? atoi(arg) : 0;
            break;
        case 'm':
            arguments->mode = arg ? atoi(arg) : 1;
            break;
        case ARGP_KEY_ARG:
            return 0;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

int main(int argc, char *argv[]) {
    struct arguments arguments;

    arguments.config_file = NULL;
    arguments.red = 0;
    arguments.green = 0;
    arguments.blue = 0;
    arguments.mode = DAS4Q_MODE_SOLID;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    das4q_handle handle = das4q_init_device(NULL);
    if (handle == NULL) {
        printf("Failed to initialize das4q\n");
        exit(1);
    }

    if (arguments.config_file != NULL) {
        printf("Using settings from %s\n", arguments.config_file);
        apply_config_file(arguments.config_file, handle);
    } else {
        printf("Using Default RGB Color: [%i, %i, %i]\n", arguments.red,
               arguments.green, arguments.blue);
        printf("Using mode: 0x%x\n", arguments.mode);
        das4q_setting_t set = {.mode = arguments.mode,
                               .red = arguments.red,
                               .green = arguments.green,
                               .blue = arguments.blue};
        for (int i = 0; i < 0x84; i++) {
            das4q_set_key_backlight(handle, i, set);
        }
    }

    das4q_apply_changes(handle);

    das4q_close_device(handle);
    exit(0);
}

// Copyright 2023 Patrick Erley <paerley@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdbool.h>
#include <stdint.h>

#include "keymap.h"

typedef void *das4q_handle;

/*
 * Initializes the device at hiddev.
 *
 *  hiddev: /dev node to use.  If null, will search by vid/pid
 *
 *  returns: handle on success, Sets errno and returns NULL on error.
 */
das4q_handle das4q_init_device(char *hiddev);
void das4q_close_device(das4q_handle handle);

typedef enum __attribute__((__packed__)) das4q_keymode {
    DAS4Q_MODE_NONE = 0,
    DAS4Q_MODE_SOLID = 1,
    DAS4Q_MODE_BLINK = 0x1f
} das4q_keymode_t;

typedef struct das4q_setting {
    das4q_keymode_t mode;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} das4q_setting_t;

bool das4q_set_key_backlight(das4q_handle handle, das4q_map_t key,
                             das4q_setting_t setting);
bool das4q_apply_changes(das4q_handle handle);
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

typedef enum __attribute__((__packed__)) das4q_map {
    KET_L_CTRL = 0x00,
    KEY_L_SHIFT = 0x01,
    KEY_CAPSLOCK = 0x02,
    KEY_TAB = 0x03,
    KEY_TILDE = 0x04,
    KEY_ESCAPE = 0x05,
    KEY_L_SUPER = 0x06,
    // 0x07 Right half of left shift
    KEY_A = 0x08,
    KEY_Q = 0x09,
    KEY_1 = 0x0a,
    // 0x0B Gap between escape and f1
    KEY_L_ALT = 0x0c,
    KEY_Z = 0x0d,
    KEY_S = 0x0e,
    KEY_W = 0x0f,
    KEY_2 = 0x10,
    KEY_F1 = 0x11,
    // 0x12 Left end of spacebar?
    KEY_X = 0x13,
    KEY_D = 0x14,
    KEY_E = 0x15,
    KEY_3 = 0x16,
    KEY_F2 = 0x17,
    // 0x18 Mid-left of spacebar
    KEY_C = 0x19,
    KEY_F = 0x1a,
    KEY_R = 0x1b,
    KEY_4 = 0x1c,
    KEY_F3 = 0x1d,
    // 0x1e barely-left of spacebar
    KEY_V = 0x1f,
    KEY_G = 0x20,
    KEY_T = 0x21,
    KEY_5 = 0x22,
    KEY_F4 = 0x23,
    KEY_SPACE = 0x24,
    KEY_B = 0x25,
    KEY_H = 0x26,
    KEY_Y = 0x27,
    KEY_6 = 0x28,
    KEY_F5 = 0x29,
    // 0x2a barely-right of spacebar
    KEY_N = 0x2b,
    KEY_J = 0x2c,
    KEY_U = 0x2d,
    KEY_7 = 0x2e,
    KEY_F6 = 0x2f,
    // 0x30 mid-right of spacebar
    KEY_M = 0x31,
    KEY_K = 0x32,
    KEY_I = 0x33,
    KEY_8 = 0x34,
    KEY_F7 = 0x35,
    // 0x36 right end of spacebar
    KEY_COMMA = 0x37,
    KEY_L = 0x38,
    KEY_O = 0x39,
    KEY_9 = 0x3a,
    KEY_F8 = 0x3b,
    KEY_R_ALT = 0x3c,
    KEY_PERIOD = 0x3d,
    KEY_SEMICOLON = 0x3e,
    KEY_P = 0x3f,
    KEY_0 = 0x40,
    KEY_F9 = 0x41,
    KEY_R_SUPER = 0x42,
    KEY_SLASH = 0x43,
    KEY_APOSTROPHE = 0x44,
    KEY_L_BRACKET = 0x45,
    KEY_DASH_UNDERSCORE = 0x46,
    KEY_F10 = 0x47,
    KEY_META = 0x48,
    // 0x49 left end of right shift
    // 0x4a left end of enter key
    KEY_R_BRACKET = 0x4b,
    KEY_EQUALS_PLUS = 0x4c,
    KEY_F11 = 0x4d,
    KEY_R_CTRL = 0x4e,
    KEY_R_SHIFT = 0x4f,
    KEY_ENTER = 0x50,
    KEY_BACKSLASH = 0x51,
    KEY_BACKSPACE = 0x52,
    KEY_F12 = 0x53,
    KEY_ARROW_LEFT = 0x54,
    // 0x55 Above left arrow
    // 0x56 Below Delete
    KEY_DELETE = 0x57,
    KEY_INSERT = 0x58,
    KEY_PRINT_SCR = 0x59,
    KEY_ARROW_DOWN = 0x5a,
    KEY_ARROW_UP = 0x5b,
    // 0x5c Above arrow up/below end
    KEY_END = 0x5d,
    KEY_HOME = 0x5e,
    KEY_SCROLL_LOCK = 0x5f,
    KEY_ARROW_RIGHT = 0x60,
    // 0x61 Above right arrow
    // 0x62 Below Page down
    KEY_PAGE_DOWN = 0x63,
    KEY_PAGE_UP = 0x64,
    KEY_PAUSE = 0x65,
    KEY_NUM_0 = 0x66,
    KEY_NUM_1 = 0x67,
    KEY_NUM_4 = 0x68,
    KEY_NUM_7 = 0x69,
    KEY_NUMLOCK = 0x6a,
    // 0x6b ???? Above Numlock ?
    // 0x6c Right half of num pad 0
    KEY_NUM_2 = 0x6d,
    KEY_NUM_5 = 0x6e,
    KEY_NUM_8 = 0x6f,
    KEY_NUM_SLASH = 0x70,
    // 0x71 ???? Above Slash ?
    KEY_NUM_PERIOD = 0x72,
    KEY_NUM_3 = 0x73,
    KEY_NUM_6 = 0x74,
    KEY_NUM_9 = 0x75,
    KEY_NUM_STAR = 0x76,
    // 0x77 ???? Above Star ?
    // 0x78 ????
    // 0x79 ????
    KEY_NUM_ENTER = 0x7a,
    KEY_NUM_PLUS = 0x7b,
    KEY_NUM_MINUS = 0x7c,
    // 0x7d ????
    KEY_SLEEP = 0x7e,
    KEY_BACKLIGHT_CTRL = 0x7f,
    KEY_MEDIA_PLAY_PAUSE = 0x80,
    KEY_MEDIA_NEXT_TRACK = 0x81
    // 0x82 ????
    // 0x83 ????
    // 0x84 + causes errors
} das4q_map_t;
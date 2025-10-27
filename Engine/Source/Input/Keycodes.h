#pragma once

#include <Core.hpp>

#define ME_MAX_MOUSE_SIDEBTN_COUNT 6
#define ME_MAX_KEYCODE_COUNT 256

namespace ME::Input
{
    enum MouseCodes
    {
        ME_MOUSE_UNKNOWN = 0,

        ME_MOUSE_LEFTBUTTON = 1, ME_MOUSE_RIGHTBUTTON = 2, ME_MOUSE_MIDDLEBUTTON = 3,

        ME_MOUSE_SIDEBUTTON1 = 4, ME_MOUSE_SIDEBUTTON2 = 5, ME_MOUSE_SIDEBUTTON3 = 6,
        ME_MOUSE_SIDEBUTTON4 = 7, ME_MOUSE_SIDEBUTTON5 = 8, ME_MOUSE_SIDEBUTTON6 = 9,

    };

    enum Keycode
    {
        ME_UNKNOWN = 0,
        // Letters (1 - 26)
        ME_KEY_A = 1, ME_KEY_B = 2, ME_KEY_C = 3, ME_KEY_D = 4,
        ME_KEY_E = 5, ME_KEY_F = 6, ME_KEY_G = 7, ME_KEY_H = 8,
        ME_KEY_I = 9, ME_KEY_J = 10, ME_KEY_K = 11, ME_KEY_L = 12,
        ME_KEY_M = 13, ME_KEY_N = 14, ME_KEY_O = 15, ME_KEY_P = 16,
        ME_KEY_Q = 17, ME_KEY_R = 18, ME_KEY_S = 19, ME_KEY_T = 20,
        ME_KEY_U = 21, ME_KEY_V = 22, ME_KEY_W = 23, ME_KEY_X = 24,
        ME_KEY_Y = 25, ME_KEY_Z = 26,

        // Numbers (30 - 39)
        ME_KEY_1 = 30, ME_KEY_2 = 31, ME_KEY_3 = 32, ME_KEY_4 = 33, ME_KEY_5 = 34,
        ME_KEY_6 = 35, ME_KEY_7 = 36, ME_KEY_8 = 37, ME_KEY_9 = 38, ME_KEY_0 = 39,

        // Symbols (40 - 60)
        ME_KEY_MINUS = 40, ME_KEY_EQUAL = 41, ME_KEY_BACKSPACE = 42,
        ME_KEY_TAB = 43, ME_KEY_LEFTBRACKET = 44, ME_KEY_RIGHTBRACKET = 45,
        ME_KEY_SEMICOLON = 46, ME_KEY_APOSTROPHE = 47, ME_KEY_GRAVE = 48,
        ME_KEY_BACKSLASH = 49, ME_KEY_COMMA = 50, ME_KEY_PERIOD = 51,
        ME_KEY_SLASH = 52,

        // Special keys (60 - 80)
        ME_KEY_SPACE = 60, ME_KEY_ENTER = 61,
        ME_KEY_LEFTSHIFT = 62, ME_KEY_RIGHTSHIFT = 63,
        ME_KEY_LEFTCONTROL = 64, ME_KEY_RIGHTCONTROL = 65,
        ME_KEY_LEFTALT = 66, ME_KEY_RIGHTALT = 67,
        ME_KEY_LEFTCOMMAND = 68, ME_KEY_RIGHTCOMMAND = 69,
        ME_KEY_CAPSLOCK = 70, ME_KEY_NUMLOCK = 71,
        ME_KEY_ESCAPE = 72, ME_KEY_PRINTSCREEN = 73,
        ME_KEY_SCROLLLOCK = 74,


        // Functional keys (80 - 91)
        ME_KEY_F1 = 80, ME_KEY_F2 = 81,
        ME_KEY_F3 = 82, ME_KEY_F4 = 83,
        ME_KEY_F5 = 84, ME_KEY_F6 = 85,
        ME_KEY_F7 = 86, ME_KEY_F8 = 87,
        ME_KEY_F9 = 88, ME_KEY_F10 = 89,
        ME_KEY_F11 = 90, ME_KEY_F12 = 91,

        // Arrows (92 - 95)
        ME_KEY_UP = 92,
        ME_KEY_DOWN = 93,
        ME_KEY_LEFT = 94,
        ME_KEY_RIGHT = 95,

        // Control keys (96 - 105)
        ME_KEY_INSERT = 96,
        ME_KEY_HOME = 97,
        ME_KEY_PAGEUP = 98,
        ME_KEY_PAGEDOWN = 99,
        ME_KEY_DELETE = 100,
        ME_KEY_END = 101,
        ME_KEY_CLEAR = 102,

        // Numpad keys (105 - 130)
        ME_KEY_KP_1 = 105,
        ME_KEY_KP_2 = 106,
        ME_KEY_KP_3 = 107,
        ME_KEY_KP_4 = 108,
        ME_KEY_KP_5 = 109,
        ME_KEY_KP_6 = 110,
        ME_KEY_KP_7 = 111,
        ME_KEY_KP_8 = 112,
        ME_KEY_KP_9 = 113,
        ME_KEY_KP_0 = 114,
        ME_KEY_KP_DOT = 115,
        ME_KEY_KP_PLUS = 116,
        ME_KEY_KP_MINUS = 117,
        ME_KEY_KP_MULTIPLY = 118,
        ME_KEY_KP_DIVIDE = 119,
        ME_KEY_KP_ENTER = 120,

    };
}
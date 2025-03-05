#pragma once

#include "Core/CoreTypes.h"
#include "Core/Containers/String.h"

#define KEYCODE_TO_STR(keycode) TEXT(#keycode)

#define PE_MAX_KEYCODE_COUNT 256

enum Keycode
{
    PE_UNKNOWN = 0,
    // Letters (1 - 26)
    PE_KEY_A = 1,    PE_KEY_B = 2,    PE_KEY_C = 3,    PE_KEY_D = 4,
    PE_KEY_E = 5,    PE_KEY_F = 6,    PE_KEY_G = 7,    PE_KEY_H = 8,
    PE_KEY_I = 9,    PE_KEY_J = 10,   PE_KEY_K = 11,   PE_KEY_L = 12,
    PE_KEY_M = 13,   PE_KEY_N = 14,   PE_KEY_O = 15,   PE_KEY_P = 16,
    PE_KEY_Q = 17,   PE_KEY_R = 18,   PE_KEY_S = 19,   PE_KEY_T = 20,
    PE_KEY_U = 21,   PE_KEY_V = 22,   PE_KEY_W = 23,   PE_KEY_X = 24,
    PE_KEY_Y = 25,   PE_KEY_Z = 26,

    // Numbers (30 - 39)
    PE_KEY_1 = 30, PE_KEY_2 = 31, PE_KEY_3 = 32, PE_KEY_4 = 33, PE_KEY_5 = 34,
    PE_KEY_6 = 35, PE_KEY_7 = 36, PE_KEY_8 = 37, PE_KEY_9 = 38, PE_KEY_0 = 39,

    // Symbols (40 - 60)
    PE_KEY_MINUS                                = 40, PE_KEY_EQUAL                                = 41, PE_KEY_BACKSPACE                            = 42,
    PE_KEY_TAB                                  = 43, PE_KEY_LEFTBRACKET                          = 44, PE_KEY_RIGHTBRACKET                         = 45,
    PE_KEY_SEMICOLON                            = 46, PE_KEY_APOSTROPHE                           = 47, PE_KEY_GRAVE                                = 48,
    PE_KEY_BACKSLASH                            = 49, PE_KEY_COMMA                                = 50, PE_KEY_PERIOD                               = 51,
    PE_KEY_SLASH                                = 52,

    // Special keys (60 - 80)
    PE_KEY_SPACE                                = 60, PE_KEY_ENTER                                = 61,
    PE_KEY_LEFTSHIFT                            = 62, PE_KEY_RIGHTSHIFT                           = 63,
    PE_KEY_LEFTCONTROL                          = 64, PE_KEY_RIGHTCONTROL                         = 65,
    PE_KEY_LEFTALT                              = 66, PE_KEY_RIGHTALT                             = 67,
    PE_KEY_LEFTCOMMAND                          = 68, PE_KEY_RIGHTCOMMAND                         = 69,
    PE_KEY_CAPSLOCK                             = 70, PE_KEY_NUMLOCK                              = 71,
    PE_KEY_ESCAPE                               = 72, PE_KEY_PRINTSCREEN                          = 73,
    PE_KEY_SCROLLLOCK                           = 74,
    

    // Functional keys (80 - 91)
    PE_KEY_F1                                   = 80, PE_KEY_F2                                   = 81,
    PE_KEY_F3                                   = 82, PE_KEY_F4                                   = 83,
    PE_KEY_F5                                   = 84, PE_KEY_F6                                   = 85,
    PE_KEY_F7                                   = 86, PE_KEY_F8                                   = 87,
    PE_KEY_F9                                   = 88, PE_KEY_F10                                  = 89,
    PE_KEY_F11                                  = 90, PE_KEY_F12                                  = 91,

    // Arrows (92 - 95)
    PE_KEY_UP                                   = 92,
    PE_KEY_DOWN                                 = 93,
    PE_KEY_LEFT                                 = 94, 
    PE_KEY_RIGHT                                = 95,

    // Control keys (96 - 105)
    PE_KEY_INSERT                               = 96,
    PE_KEY_HOME                                 = 97,
    PE_KEY_PAGEUP                               = 98,
    PE_KEY_PAGEDOWN                             = 99,
    PE_KEY_DELETE                               = 100,
    PE_KEY_END                                  = 101,
    PE_KEY_CLEAR                                = 102,

    // Numpad keys (105 - 130)
    PE_KEY_KP_1                                 = 105,
    PE_KEY_KP_2                                 = 106,
    PE_KEY_KP_3                                 = 107,
    PE_KEY_KP_4                                 = 108,
    PE_KEY_KP_5                                 = 109,
    PE_KEY_KP_6                                 = 110,
    PE_KEY_KP_7                                 = 111,
    PE_KEY_KP_8                                 = 112,
    PE_KEY_KP_9                                 = 113,
    PE_KEY_KP_0                                 = 114,
    PE_KEY_KP_DOT                               = 115,
    PE_KEY_KP_PLUS                              = 116,
    PE_KEY_KP_MINUS                             = 117,
    PE_KEY_KP_MULTIPLY                          = 118,
    PE_KEY_KP_DIVIDE                            = 119,
    PE_KEY_KP_ENTER                             = 120,

};


#define ADD_KEYCODE_TOARR(keycode) KeycodeStrings[(SIZE_T)keycode] = KEYCODE_TO_STR(keycode)
#define INIT_TO_UNUSED(size) for (SIZE_T i = 0; i < size; i++) KeycodeStrings[i] = TEXT("Unused");

// const uchar* KeycodeStrings[256];
#pragma once

/**
 * Direct Input Keys
 * These are taken from the Wine Project which is under the
 * lesser general public license 2.1f
 * Original is here:
 * https://github.com/wine-mirror/wine/blob/master/include/dinput.h
 *
 * Original license to follow
 */
/*
 * Copyright (C) the Wine project
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1f of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#define DIK_ESCAPE          0x01
#define DIK_1               0x02
#define DIK_2               0x03
#define DIK_3               0x04
#define DIK_4               0x05
#define DIK_5               0x06
#define DIK_6               0x07
#define DIK_7               0x08
#define DIK_8               0x09
#define DIK_9               0x0A
#define DIK_0               0x0B
#define DIK_MINUS           0x0C    /* - on main keyboard */
#define DIK_EQUALS          0x0D
#define DIK_BACK            0x0E    /* backspace */
#define DIK_TAB             0x0F
#define DIK_Q               0x10
#define DIK_W               0x11
#define DIK_E               0x12
#define DIK_R               0x13
#define DIK_T               0x14
#define DIK_Y               0x15
#define DIK_U               0x16
#define DIK_I               0x17
#define DIK_O               0x18
#define DIK_P               0x19
#define DIK_LBRACKET        0x1A
#define DIK_RBRACKET        0x1B
#define DIK_RETURN          0x1C    /* Enter on main keyboard */
#define DIK_LCONTROL        0x1D
#define DIK_A               0x1E
#define DIK_S               0x1F
#define DIK_D               0x20
#define DIK_F               0x21
#define DIK_G               0x22
#define DIK_H               0x23
#define DIK_J               0x24
#define DIK_K               0x25
#define DIK_L               0x26
#define DIK_SEMICOLON       0x27
#define DIK_APOSTROPHE      0x28
#define DIK_GRAVE           0x29    /* accent grave */
#define DIK_LSHIFT          0x2A
#define DIK_BACKSLASH       0x2B
#define DIK_Z               0x2C
#define DIK_X               0x2D
#define DIK_C               0x2E
#define DIK_V               0x2F
#define DIK_B               0x30
#define DIK_N               0x31
#define DIK_M               0x32
#define DIK_COMMA           0x33
#define DIK_PERIOD          0x34    /* . on main keyboard */
#define DIK_SLASH           0x35    /* / on main keyboard */
#define DIK_RSHIFT          0x36
#define DIK_MULTIPLY        0x37    /* * on numeric keypad */
#define DIK_LMENU           0x38    /* left Alt */
#define DIK_SPACE           0x39
#define DIK_CAPITAL         0x3A
#define DIK_F1              0x3B
#define DIK_F2              0x3C
#define DIK_F3              0x3D
#define DIK_F4              0x3E
#define DIK_F5              0x3F
#define DIK_F6              0x40
#define DIK_F7              0x41
#define DIK_F8              0x42
#define DIK_F9              0x43
#define DIK_F10             0x44
#define DIK_NUMLOCK         0x45
#define DIK_SCROLL          0x46    /* Scroll Lock */
#define DIK_NUMPAD7         0x47
#define DIK_NUMPAD8         0x48
#define DIK_NUMPAD9         0x49
#define DIK_SUBTRACT        0x4A    /* - on numeric keypad */
#define DIK_NUMPAD4         0x4B
#define DIK_NUMPAD5         0x4C
#define DIK_NUMPAD6         0x4D
#define DIK_ADD             0x4E    /* + on numeric keypad */
#define DIK_NUMPAD1         0x4F
#define DIK_NUMPAD2         0x50
#define DIK_NUMPAD3         0x51
#define DIK_NUMPAD0         0x52
#define DIK_DECIMAL         0x53    /* . on numeric keypad */
#define DIK_OEM_102         0x56    /* < > | on UK/Germany keyboards */
#define DIK_F11             0x57
#define DIK_F12             0x58
#define DIK_F13             0x64    /*                     (NEC PC98) */
#define DIK_F14             0x65    /*                     (NEC PC98) */
#define DIK_F15             0x66    /*                     (NEC PC98) */
#define DIK_KANA            0x70    /* (Japanese keyboard)            */
#define DIK_ABNT_C1         0x73    /* / ? on Portugese (Brazilian) keyboards */
#define DIK_CONVERT         0x79    /* (Japanese keyboard)            */
#define DIK_NOCONVERT       0x7B    /* (Japanese keyboard)            */
#define DIK_YEN             0x7D    /* (Japanese keyboard)            */
#define DIK_ABNT_C2         0x7E    /* Numpad . on Portugese (Brazilian) keyboards */
#define DIK_NUMPADEQUALS    0x8D    /* = on numeric keypad (NEC PC98) */
#define DIK_CIRCUMFLEX      0x90    /* (Japanese keyboard)            */
#define DIK_AT              0x91    /*                     (NEC PC98) */
#define DIK_COLON           0x92    /*                     (NEC PC98) */
#define DIK_UNDERLINE       0x93    /*                     (NEC PC98) */
#define DIK_KANJI           0x94    /* (Japanese keyboard)            */
#define DIK_STOP            0x95    /*                     (NEC PC98) */
#define DIK_AX              0x96    /*                     (Japan AX) */
#define DIK_UNLABELED       0x97    /*                        (J3100) */
#define DIK_NEXTTRACK       0x99    /* Next Track */
#define DIK_NUMPADENTER     0x9C    /* Enter on numeric keypad */
#define DIK_RCONTROL        0x9D
#define DIK_MUTE           0xA0    /* Mute */
#define DIK_CALCULATOR      0xA1    /* Calculator */
#define DIK_PLAYPAUSE       0xA2    /* Play / Pause */
#define DIK_MEDIASTOP       0xA4    /* Media Stop */
#define DIK_VOLUMEDOWN      0xAE    /* Volume - */
#define DIK_VOLUMEUP        0xB0    /* Volume + */
#define DIK_WEBHOME         0xB2    /* Web home */
#define DIK_NUMPADCOMMA     0xB3    /* , on numeric keypad (NEC PC98) */
#define DIK_DIVIDE          0xB5    /* / on numeric keypad */
#define DIK_SYSRQ           0xB7
#define DIK_RMENU           0xB8    /* right Alt */
#define DIK_PAUSE           0xC5    /* Pause */
#define DIK_HOME            0xC7    /* Home on arrow keypad */
#define DIK_UP              0xC8    /* UpArrow on arrow keypad */
#define DIK_PRIOR           0xC9    /* PgUp on arrow keypad */
#define DIK_LEFT            0xCB    /* LeftArrow on arrow keypad */
#define DIK_RIGHT           0xCD    /* RightArrow on arrow keypad */
#define DIK_END             0xCF    /* End on arrow keypad */
#define DIK_DOWN            0xD0    /* DownArrow on arrow keypad */
#define DIK_NEXT            0xD1    /* PgDn on arrow keypad */
#define DIK_INSERT          0xD2    /* Insert on arrow keypad */
#define DIK_DELETE          0xD3    /* Delete on arrow keypad */
#define DIK_LWIN            0xDB    /* Left Windows key */
#define DIK_RWIN            0xDC    /* Right Windows key */
#define DIK_APPS            0xDD    /* AppMenu key */
#define DIK_POWER           0xDE
#define DIK_SLEEP           0xDF
#define DIK_WAKE            0xE3    /* System Wake */
#define DIK_WEBSEARCH       0xE5    /* Web Search */
#define DIK_WEBFAVORITES    0xE6    /* Web Favorites */
#define DIK_WEBREFRESH      0xE7    /* Web Refresh */
#define DIK_WEBSTOP         0xE8    /* Web Stop */
#define DIK_WEBFORWARD      0xE9    /* Web Forward */
#define DIK_WEBBACK         0xEA    /* Web Back */
#define DIK_MYCOMPUTER      0xEB    /* My Computer */
#define DIK_MAIL            0xEC    /* Mail */
#define DIK_MEDIASELECT     0xED    /* Media Select */

#define DIK_BACKSPACE       DIK_BACK            /* backspace */
#define DIK_NUMPADSTAR      DIK_MULTIPLY        /* * on numeric keypad */
#define DIK_LALT            DIK_LMENU           /* left Alt */
#define DIK_CAPSLOCK        DIK_CAPITAL         /* CapsLock */
#define DIK_NUMPADMINUS     DIK_SUBTRACT        /* - on numeric keypad */
#define DIK_NUMPADPLUS      DIK_ADD             /* + on numeric keypad */
#define DIK_NUMPADPERIOD    DIK_DECIMAL         /* . on numeric keypad */
#define DIK_NUMPADSLASH     DIK_DIVIDE          /* / on numeric keypad */
#define DIK_RALT            DIK_RMENU           /* right Alt */
#define DIK_UPARROW         DIK_UP              /* UpArrow on arrow keypad */
#define DIK_PGUP            DIK_PRIOR           /* PgUp on arrow keypad */
#define DIK_LEFTARROW       DIK_LEFT            /* LeftArrow on arrow keypad */
#define DIK_RIGHTARROW      DIK_RIGHT           /* RightArrow on arrow keypad */
#define DIK_DOWNARROW       DIK_DOWN            /* DownArrow on arrow keypad */
#define DIK_PGDN            DIK_NEXT            /* PgDn on arrow keypad */

/*
 * The following is our usage of the above wine sourced key codes
 */

#define MOUSE_OFFSET        0x100

const absl::flat_hash_map<std::string, std::uint32_t> NAMED_KEYS = {
	{ "DIK_ESCAPE", DIK_ESCAPE },
	{ "ESCAPE", DIK_ESCAPE },
	{ "DIK_1", DIK_1 },
	{ "1", DIK_1 },
	{ "DIK_2", DIK_2 },
	{ "2", DIK_2 },
	{ "DIK_3", DIK_3 },
	{ "3", DIK_3 },
	{ "DIK_4", DIK_4 },
	{ "4", DIK_4 },
	{ "DIK_5", DIK_5 },
	{ "5", DIK_5 },
	{ "DIK_6", DIK_6 },
	{ "6", DIK_6 },
	{ "DIK_7", DIK_7 },
	{ "7", DIK_7 },
	{ "DIK_8", DIK_8 },
	{ "8", DIK_8 },
	{ "DIK_9", DIK_9 },
	{ "9", DIK_9 },
	{ "DIK_0", DIK_0 },
	{ "0", DIK_0 },
	{ "DIK_MINUS", DIK_MINUS },
	{ "MINUS", DIK_MINUS },
	{ "DIK_EQUALS", DIK_EQUALS },
	{ "EQUALS", DIK_EQUALS },
	{ "DIK_BACK", DIK_BACK },
	{ "BACK", DIK_BACK },
	{ "DIK_TAB", DIK_TAB },
	{ "TAB", DIK_TAB },
	{ "DIK_Q", DIK_Q },
	{ "Q", DIK_Q },
	{ "DIK_W", DIK_W },
	{ "W", DIK_W },
	{ "DIK_E", DIK_E },
	{ "E", DIK_E },
	{ "DIK_R", DIK_R },
	{ "R", DIK_R },
	{ "DIK_T", DIK_T },
	{ "T", DIK_T },
	{ "DIK_Y", DIK_Y },
	{ "Y", DIK_Y },
	{ "DIK_U", DIK_U },
	{ "U", DIK_U },
	{ "DIK_I", DIK_I },
	{ "I", DIK_I },
	{ "DIK_O", DIK_O },
	{ "O", DIK_O },
	{ "DIK_P", DIK_P },
	{ "P", DIK_P },
	{ "DIK_LBRACKET", DIK_LBRACKET },
	{ "LBRACKET", DIK_LBRACKET },
	{ "DIK_RBRACKET", DIK_RBRACKET },
	{ "RBRACKET", DIK_RBRACKET },
	{ "DIK_RETURN", DIK_RETURN },
	{ "RETURN", DIK_RETURN },
	{ "DIK_LCONTROL", DIK_LCONTROL },
	{ "LCONTROL", DIK_LCONTROL },
	{ "DIK_A", DIK_A },
	{ "A", DIK_A },
	{ "DIK_S", DIK_S },
	{ "S", DIK_S },
	{ "DIK_D", DIK_D },
	{ "D", DIK_D },
	{ "DIK_F", DIK_F },
	{ "F", DIK_F },
	{ "DIK_G", DIK_G },
	{ "G", DIK_G },
	{ "DIK_H", DIK_H },
	{ "H", DIK_H },
	{ "DIK_J", DIK_J },
	{ "J", DIK_J },
	{ "DIK_K", DIK_K },
	{ "K", DIK_K },
	{ "DIK_L", DIK_L },
	{ "L", DIK_L },
	{ "DIK_SEMICOLON", DIK_SEMICOLON },
	{ "SEMICOLON", DIK_SEMICOLON },
	{ "DIK_APOSTROPHE", DIK_APOSTROPHE },
	{ "APOSTROPHE", DIK_APOSTROPHE },
	{ "DIK_GRAVE", DIK_GRAVE },
	{ "GRAVE", DIK_GRAVE },
	{ "DIK_LSHIFT", DIK_LSHIFT },
	{ "LSHIFT", DIK_LSHIFT },
	{ "DIK_BACKSLASH", DIK_BACKSLASH },
	{ "BACKSLASH", DIK_BACKSLASH },
	{ "DIK_Z", DIK_Z },
	{ "Z", DIK_Z },
	{ "DIK_X", DIK_X },
	{ "X", DIK_X },
	{ "DIK_C", DIK_C },
	{ "C", DIK_C },
	{ "DIK_V", DIK_V },
	{ "V", DIK_V },
	{ "DIK_B", DIK_B },
	{ "B", DIK_B },
	{ "DIK_N", DIK_N },
	{ "N", DIK_N },
	{ "DIK_M", DIK_M },
	{ "M", DIK_M },
	{ "DIK_COMMA", DIK_COMMA },
	{ "COMMA", DIK_COMMA },
	{ "DIK_PERIOD", DIK_PERIOD },
	{ "PERIOD", DIK_PERIOD },
	{ "DIK_SLASH", DIK_SLASH },
	{ "SLASH", DIK_SLASH },
	{ "DIK_RSHIFT", DIK_RSHIFT },
	{ "RSHIFT", DIK_RSHIFT },
	{ "DIK_MULTIPLY", DIK_MULTIPLY },
	{ "MULTIPLY", DIK_MULTIPLY },
	{ "DIK_LMENU", DIK_LMENU },
	{ "LMENU", DIK_LMENU },
	{ "DIK_SPACE", DIK_SPACE },
	{ "SPACE", DIK_SPACE },
	{ "DIK_CAPITAL", DIK_CAPITAL },
	{ "CAPITAL", DIK_CAPITAL },
	{ "DIK_F1", DIK_F1 },
	{ "F1", DIK_F1 },
	{ "DIK_F2", DIK_F2 },
	{ "F2", DIK_F2 },
	{ "DIK_F3", DIK_F3 },
	{ "F3", DIK_F3 },
	{ "DIK_F4", DIK_F4 },
	{ "F4", DIK_F4 },
	{ "DIK_F5", DIK_F5 },
	{ "F5", DIK_F5 },
	{ "DIK_F6", DIK_F6 },
	{ "F6", DIK_F6 },
	{ "DIK_F7", DIK_F7 },
	{ "F7", DIK_F7 },
	{ "DIK_F8", DIK_F8 },
	{ "F8", DIK_F8 },
	{ "DIK_F9", DIK_F9 },
	{ "F9", DIK_F9 },
	{ "DIK_F10", DIK_F10 },
	{ "F10", DIK_F10 },
	{ "DIK_NUMLOCK", DIK_NUMLOCK },
	{ "NUMLOCK", DIK_NUMLOCK },
	{ "DIK_SCROLL", DIK_SCROLL },
	{ "SCROLL", DIK_SCROLL },
	{ "DIK_NUMPAD7", DIK_NUMPAD7 },
	{ "NUMPAD7", DIK_NUMPAD7 },
	{ "DIK_NUMPAD8", DIK_NUMPAD8 },
	{ "NUMPAD8", DIK_NUMPAD8 },
	{ "DIK_NUMPAD9", DIK_NUMPAD9 },
	{ "NUMPAD9", DIK_NUMPAD9 },
	{ "DIK_SUBTRACT", DIK_SUBTRACT },
	{ "SUBTRACT", DIK_SUBTRACT },
	{ "DIK_NUMPAD4", DIK_NUMPAD4 },
	{ "NUMPAD4", DIK_NUMPAD4 },
	{ "DIK_NUMPAD5", DIK_NUMPAD5 },
	{ "NUMPAD5", DIK_NUMPAD5 },
	{ "DIK_NUMPAD6", DIK_NUMPAD6 },
	{ "NUMPAD6", DIK_NUMPAD6 },
	{ "DIK_ADD", DIK_ADD },
	{ "ADD", DIK_ADD },
	{ "DIK_NUMPAD1", DIK_NUMPAD1 },
	{ "NUMPAD1", DIK_NUMPAD1 },
	{ "DIK_NUMPAD2", DIK_NUMPAD2 },
	{ "NUMPAD2", DIK_NUMPAD2 },
	{ "DIK_NUMPAD3", DIK_NUMPAD3 },
	{ "NUMPAD3", DIK_NUMPAD3 },
	{ "DIK_NUMPAD0", DIK_NUMPAD0 },
	{ "NUMPAD0", DIK_NUMPAD0 },
	{ "DIK_DECIMAL", DIK_DECIMAL },
	{ "DECIMAL", DIK_DECIMAL },
	{ "DIK_OEM_102", DIK_OEM_102 },
	{ "OEM_102", DIK_OEM_102 },
	{ "DIK_F11", DIK_F11 },
	{ "F11", DIK_F11 },
	{ "DIK_F12", DIK_F12 },
	{ "F12", DIK_F12 },
	{ "DIK_F13", DIK_F13 },
	{ "F13", DIK_F13 },
	{ "DIK_F14", DIK_F14 },
	{ "F14", DIK_F14 },
	{ "DIK_F15", DIK_F15 },
	{ "F15", DIK_F15 },
	{ "DIK_KANA", DIK_KANA },
	{ "KANA", DIK_KANA },
	{ "DIK_ABNT_C1", DIK_ABNT_C1 },
	{ "ABNT_C1", DIK_ABNT_C1 },
	{ "DIK_CONVERT", DIK_CONVERT },
	{ "CONVERT", DIK_CONVERT },
	{ "DIK_NOCONVERT", DIK_NOCONVERT },
	{ "NOCONVERT", DIK_NOCONVERT },
	{ "DIK_YEN", DIK_YEN },
	{ "YEN", DIK_YEN },
	{ "DIK_ABNT_C2", DIK_ABNT_C2 },
	{ "ABNT_C2", DIK_ABNT_C2 },
	{ "DIK_NUMPADEQUALS", DIK_NUMPADEQUALS },
	{ "NUMPADEQUALS", DIK_NUMPADEQUALS },
	{ "DIK_CIRCUMFLEX", DIK_CIRCUMFLEX },
	{ "CIRCUMFLEX", DIK_CIRCUMFLEX },
	{ "DIK_AT", DIK_AT },
	{ "AT", DIK_AT },
	{ "DIK_COLON", DIK_COLON },
	{ "COLON", DIK_COLON },
	{ "DIK_UNDERLINE", DIK_UNDERLINE },
	{ "UNDERLINE", DIK_UNDERLINE },
	{ "DIK_KANJI", DIK_KANJI },
	{ "KANJI", DIK_KANJI },
	{ "DIK_STOP", DIK_STOP },
	{ "STOP", DIK_STOP },
	{ "DIK_AX", DIK_AX },
	{ "AX", DIK_AX },
	{ "DIK_UNLABELED", DIK_UNLABELED },
	{ "UNLABELED", DIK_UNLABELED },
	{ "DIK_NEXTTRACK", DIK_NEXTTRACK },
	{ "NEXTTRACK", DIK_NEXTTRACK },
	{ "DIK_NUMPADENTER", DIK_NUMPADENTER },
	{ "NUMPADENTER", DIK_NUMPADENTER },
	{ "DIK_RCONTROL", DIK_RCONTROL },
	{ "RCONTROL", DIK_RCONTROL },
	{ "DIK_MUTE", DIK_MUTE },
	{ "MUTE", DIK_MUTE },
	{ "DIK_CALCULATOR", DIK_CALCULATOR },
	{ "CALCULATOR", DIK_CALCULATOR },
	{ "DIK_PLAYPAUSE", DIK_PLAYPAUSE },
	{ "PLAYPAUSE", DIK_PLAYPAUSE },
	{ "DIK_MEDIASTOP", DIK_MEDIASTOP },
	{ "MEDIASTOP", DIK_MEDIASTOP },
	{ "DIK_VOLUMEDOWN", DIK_VOLUMEDOWN },
	{ "VOLUMEDOWN", DIK_VOLUMEDOWN },
	{ "DIK_VOLUMEUP", DIK_VOLUMEUP },
	{ "VOLUMEUP", DIK_VOLUMEUP },
	{ "DIK_WEBHOME", DIK_WEBHOME },
	{ "WEBHOME", DIK_WEBHOME },
	{ "DIK_NUMPADCOMMA", DIK_NUMPADCOMMA },
	{ "NUMPADCOMMA", DIK_NUMPADCOMMA },
	{ "DIK_DIVIDE", DIK_DIVIDE },
	{ "DIVIDE", DIK_DIVIDE },
	{ "DIK_SYSRQ", DIK_SYSRQ },
	{ "SYSRQ", DIK_SYSRQ },
	{ "DIK_RMENU", DIK_RMENU },
	{ "RMENU", DIK_RMENU },
	{ "DIK_PAUSE", DIK_PAUSE },
	{ "PAUSE", DIK_PAUSE },
	{ "DIK_HOME", DIK_HOME },
	{ "HOME", DIK_HOME },
	{ "DIK_UP", DIK_UP },
	{ "UP", DIK_UP },
	{ "DIK_PRIOR", DIK_PRIOR },
	{ "PRIOR", DIK_PRIOR },
	{ "DIK_LEFT", DIK_LEFT },
	{ "LEFT", DIK_LEFT },
	{ "DIK_RIGHT", DIK_RIGHT },
	{ "RIGHT", DIK_RIGHT },
	{ "DIK_END", DIK_END },
	{ "END", DIK_END },
	{ "DIK_DOWN", DIK_DOWN },
	{ "DOWN", DIK_DOWN },
	{ "DIK_NEXT", DIK_NEXT },
	{ "NEXT", DIK_NEXT },
	{ "DIK_INSERT", DIK_INSERT },
	{ "INSERT", DIK_INSERT },
	{ "DIK_DELETE", DIK_DELETE },
	{ "DELETE", DIK_DELETE },
	{ "DIK_LWIN", DIK_LWIN },
	{ "LWIN", DIK_LWIN },
	{ "DIK_RWIN", DIK_RWIN },
	{ "RWIN", DIK_RWIN },
	{ "DIK_APPS", DIK_APPS },
	{ "APPS", DIK_APPS },
	{ "DIK_POWER", DIK_POWER },
	{ "POWER", DIK_POWER },
	{ "DIK_SLEEP", DIK_SLEEP },
	{ "SLEEP", DIK_SLEEP },
	{ "DIK_WAKE", DIK_WAKE },
	{ "WAKE", DIK_WAKE },
	{ "DIK_WEBSEARCH", DIK_WEBSEARCH },
	{ "WEBSEARCH", DIK_WEBSEARCH },
	{ "DIK_WEBFAVORITES", DIK_WEBFAVORITES },
	{ "WEBFAVORITES", DIK_WEBFAVORITES },
	{ "DIK_WEBREFRESH", DIK_WEBREFRESH },
	{ "WEBREFRESH", DIK_WEBREFRESH },
	{ "DIK_WEBSTOP", DIK_WEBSTOP },
	{ "WEBSTOP", DIK_WEBSTOP },
	{ "DIK_WEBFORWARD", DIK_WEBFORWARD },
	{ "WEBFORWARD", DIK_WEBFORWARD },
	{ "DIK_WEBBACK", DIK_WEBBACK },
	{ "WEBBACK", DIK_WEBBACK },
	{ "DIK_MYCOMPUTER", DIK_MYCOMPUTER },
	{ "MYCOMPUTER", DIK_MYCOMPUTER },
	{ "DIK_MAIL", DIK_MAIL },
	{ "MAIL", DIK_MAIL },
	{ "DIK_MEDIASELECT", DIK_MEDIASELECT },
	{ "MEDIASELECT", DIK_MEDIASELECT },
	{ "DIK_BACKSPACE", DIK_BACKSPACE },
	{ "BACKSPACE", DIK_BACKSPACE },
	{ "DIK_NUMPADSTAR", DIK_NUMPADSTAR },
	{ "NUMPADSTAR", DIK_NUMPADSTAR },
	{ "DIK_LALT", DIK_LALT },
	{ "LALT", DIK_LALT },
	{ "DIK_CAPSLOCK", DIK_CAPSLOCK },
	{ "CAPSLOCK", DIK_CAPSLOCK },
	{ "DIK_NUMPADMINUS", DIK_NUMPADMINUS },
	{ "NUMPADMINUS", DIK_NUMPADMINUS },
	{ "DIK_NUMPADPLUS", DIK_NUMPADPLUS },
	{ "NUMPADPLUS", DIK_NUMPADPLUS },
	{ "DIK_NUMPADPERIOD", DIK_NUMPADPERIOD },
	{ "NUMPADPERIOD", DIK_NUMPADPERIOD },
	{ "DIK_NUMPADSLASH", DIK_NUMPADSLASH },
	{ "NUMPADSLASH", DIK_NUMPADSLASH },
	{ "DIK_RALT", DIK_RALT },
	{ "RALT", DIK_RALT },
	{ "DIK_UPARROW", DIK_UPARROW },
	{ "UPARROW", DIK_UPARROW },
	{ "DIK_PGUP", DIK_PGUP },
	{ "PGUP", DIK_PGUP },
	{ "DIK_LEFTARROW", DIK_LEFTARROW },
	{ "LEFTARROW", DIK_LEFTARROW },
	{ "DIK_RIGHTARROW", DIK_RIGHTARROW },
	{ "RIGHTARROW", DIK_RIGHTARROW },
	{ "DIK_DOWNARROW", DIK_DOWNARROW },
	{ "DOWNARROW", DIK_DOWNARROW },
	{ "DIK_PGDN", DIK_PGDN },
	{ "PGDN", DIK_PGDN },
	{ "LMB", 0x00 + MOUSE_OFFSET },
	{ "LMOUSEBUTTON", 0x00 + MOUSE_OFFSET },
	{ "LMOUSE", 0x00 + MOUSE_OFFSET },
	{ "LCLICK", 0x00 + MOUSE_OFFSET },
	{ "RMB", 0x01 + MOUSE_OFFSET },
	{ "RTMOUSEBUTTON", 0x01 + MOUSE_OFFSET },
	{ "RMOUSE", 0x01 + MOUSE_OFFSET },
	{ "RCLICK", 0x01 + MOUSE_OFFSET },

	{ "MB_MID", 0x02 + MOUSE_OFFSET },
	{ "MB_X1", 0x03 + MOUSE_OFFSET },
	{ "MB_X2", 0x04 + MOUSE_OFFSET },

	{ "MOUSE3", 0x02 + MOUSE_OFFSET },
	{ "MOUSE4", 0x03 + MOUSE_OFFSET },
	{ "MOUSE5", 0x04 + MOUSE_OFFSET }, 
};

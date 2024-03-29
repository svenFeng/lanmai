#pragma once

#include "err.h"
#include <cassert>
#include <functional>
#include <libevdev/libevdev.h>
#include <map>
#include <string>
#include <variant>
#include <vector>

#define LANMAI_VERSION "0.1.2"

using uint = unsigned int;

struct Defer {
    std::function<void()> f;
    Defer(std::function<void()> f) : f(f) {}
    ~Defer() { f(); }
};

// uinput and udev
std::vector<std::string> get_devices(const char* dt);
inline std::vector<std::string> get_kbd_devices() { return get_devices("ID_INPUT_KEYBOARD"); }

void print_all_kbd_devices();
void print_all_keys();

inline const std::map<std::string, uint> TABLE = {
    {"RESERVED", 0},
    {"ESC", 1},
    {"1", 2},
    {"2", 3},
    {"3", 4},
    {"4", 5},
    {"5", 6},
    {"6", 7},
    {"7", 8},
    {"8", 9},
    {"9", 10},
    {"0", 11},
    {"MINUS", 12},
    {"EQUAL", 13},
    {"BACKSPACE", 14},
    {"TAB", 15},
    {"Q", 16},
    {"W", 17},
    {"E", 18},
    {"R", 19},
    {"T", 20},
    {"Y", 21},
    {"U", 22},
    {"I", 23},
    {"O", 24},
    {"P", 25},
    {"LEFTBRACE", 26},
    {"RIGHTBRACE", 27},
    {"ENTER", 28},
    {"LEFTCTRL", 29},
    {"A", 30},
    {"S", 31},
    {"D", 32},
    {"F", 33},
    {"G", 34},
    {"H", 35},
    {"J", 36},
    {"K", 37},
    {"L", 38},
    {"SEMICOLON", 39},
    {"APOSTROPHE", 40},
    {"GRAVE", 41},
    {"LEFTSHIFT", 42},
    {"BACKSLASH", 43},
    {"Z", 44},
    {"X", 45},
    {"C", 46},
    {"V", 47},
    {"B", 48},
    {"N", 49},
    {"M", 50},
    {"COMMA", 51},
    {"DOT", 52},
    {"SLASH", 53},
    {"RIGHTSHIFT", 54},
    {"KPASTERISK", 55},
    {"LEFTALT", 56},
    {"SPACE", 57},
    {"CAPSLOCK", 58},
    {"F1", 59},
    {"F2", 60},
    {"F3", 61},
    {"F4", 62},
    {"F5", 63},
    {"F6", 64},
    {"F7", 65},
    {"F8", 66},
    {"F9", 67},
    {"F10", 68},
    {"NUMLOCK", 69},
    {"SCROLLLOCK", 70},
    {"KP7", 71},
    {"KP8", 72},
    {"KP9", 73},
    {"KPMINUS", 74},
    {"KP4", 75},
    {"KP5", 76},
    {"KP6", 77},
    {"KPPLUS", 78},
    {"KP1", 79},
    {"KP2", 80},
    {"KP3", 81},
    {"KP0", 82},
    {"KPDOT", 83},
    {"ZENKAKUHANKAKU", 85},
    {"102ND", 86},
    {"F11", 87},
    {"F12", 88},
    {"RO", 89},
    {"KATAKANA", 90},
    {"HIRAGANA", 91},
    {"HENKAN", 92},
    {"KATAKANAHIRAGANA", 93},
    {"MUHENKAN", 94},
    {"KPJPCOMMA", 95},
    {"KPENTER", 96},
    {"RIGHTCTRL", 97},
    {"KPSLASH", 98},
    {"SYSRQ", 99},
    {"RIGHTALT", 100},
    {"LINEFEED", 101},
    {"HOME", 102},
    {"UP", 103},
    {"PAGEUP", 104},
    {"LEFT", 105},
    {"RIGHT", 106},
    {"END", 107},
    {"DOWN", 108},
    {"PAGEDOWN", 109},
    {"INSERT", 110},
    {"DELETE", 111},
    {"MACRO", 112},
    {"MUTE", 113},
    {"VOLUMEDOWN", 114},
    {"VOLUMEUP", 115},
    {"POWER", 116},
    {"KPEQUAL", 117},
    {"KPPLUSMINUS", 118},
    {"PAUSE", 119},
    {"SCALE", 120},
    {"KPCOMMA", 121},
    {"HANGEUL", 122},
    {"HANJA", 123},
    {"YEN", 124},
    {"LEFTMETA", 125},
    {"RIGHTMETA", 126},
    {"COMPOSE", 127},
    {"STOP", 128},
    {"AGAIN", 129},
    {"PROPS", 130},
    {"UNDO", 131},
    {"FRONT", 132},
    {"COPY", 133},
    {"OPEN", 134},
    {"PASTE", 135},
    {"FIND", 136},
    {"CUT", 137},
    {"HELP", 138},
    {"MENU", 139},
    {"CALC", 140},
    {"SETUP", 141},
    {"SLEEP", 142},
    {"WAKEUP", 143},
    {"FILE", 144},
    {"SENDFILE", 145},
    {"DELETEFILE", 146},
    {"XFER", 147},
    {"PROG1", 148},
    {"PROG2", 149},
    {"WWW", 150},
    {"MSDOS", 151},
    {"COFFEE", 152},
    {"ROTATE_DISPLAY", 153},
    {"CYCLEWINDOWS", 154},
    {"MAIL", 155},
    {"BOOKMARKS", 156},
    {"COMPUTER", 157},
    {"BACK", 158},
    {"FORWARD", 159},
    {"CLOSECD", 160},
    {"EJECTCD", 161},
    {"EJECTCLOSECD", 162},
    {"NEXTSONG", 163},
    {"PLAYPAUSE", 164},
    {"PREVIOUSSONG", 165},
    {"STOPCD", 166},
    {"RECORD", 167},
    {"REWIND", 168},
    {"PHONE", 169},
    {"ISO", 170},
    {"CONFIG", 171},
    {"HOMEPAGE", 172},
    {"REFRESH", 173},
    {"EXIT", 174},
    {"MOVE", 175},
    {"EDIT", 176},
    {"SCROLLUP", 177},
    {"SCROLLDOWN", 178},
    {"KPLEFTPAREN", 179},
    {"KPRIGHTPAREN", 180},
    {"NEW", 181},
    {"REDO", 182},
    {"F13", 183},
    {"F14", 184},
    {"F15", 185},
    {"F16", 186},
    {"F17", 187},
    {"F18", 188},
    {"F19", 189},
    {"F20", 190},
    {"F21", 191},
    {"F22", 192},
    {"F23", 193},
    {"F24", 194},
    {"PLAYCD", 200},
    {"PAUSECD", 201},
    {"PROG3", 202},
    {"PROG4", 203},
    {"DASHBOARD", 204},
    {"SUSPEND", 205},
    {"CLOSE", 206},
    {"PLAY", 207},
    {"FASTFORWARD", 208},
    {"BASSBOOST", 209},
    {"PRINT", 210},
    {"HP", 211},
    {"CAMERA", 212},
    {"SOUND", 213},
    {"QUESTION", 214},
    {"EMAIL", 215},
    {"CHAT", 216},
    {"SEARCH", 217},
    {"CONNECT", 218},
    {"FINANCE", 219},
    {"SPORT", 220},
    {"SHOP", 221},
    {"ALTERASE", 222},
    {"CANCEL", 223},
    {"BRIGHTNESSDOWN", 224},
    {"BRIGHTNESSUP", 225},
    {"MEDIA", 226},
    {"SWITCHVIDEOMODE", 227},
    {"KBDILLUMTOGGLE", 228},
    {"KBDILLUMDOWN", 229},
    {"KBDILLUMUP", 230},
    {"SEND", 231},
    {"REPLY", 232},
    {"FORWARDMAIL", 233},
    {"SAVE", 234},
    {"DOCUMENTS", 235},
    {"BATTERY", 236},
    {"BLUETOOTH", 237},
    {"WLAN", 238},
    {"UWB", 239},
    {"UNKNOWN", 240},
    {"VIDEO_NEXT", 241},
    {"VIDEO_PREV", 242},
    {"BRIGHTNESS_CYCLE", 243},
    {"BRIGHTNESS_AUTO", 244},
    {"DISPLAY_OFF", 245},
    {"WWAN", 246},
    {"RFKILL", 247},
    {"MICMUTE", 248},
};

inline const std::map<uint, std::string> TABLE_REV = {
    {0, "RESERVED"},
    {1, "ESC"},
    {2, "1"},
    {3, "2"},
    {4, "3"},
    {5, "4"},
    {6, "5"},
    {7, "6"},
    {8, "7"},
    {9, "8"},
    {10, "9"},
    {11, "0"},
    {12, "MINUS"},
    {13, "EQUAL"},
    {14, "BACKSPACE"},
    {15, "TAB"},
    {16, "Q"},
    {17, "W"},
    {18, "E"},
    {19, "R"},
    {20, "T"},
    {21, "Y"},
    {22, "U"},
    {23, "I"},
    {24, "O"},
    {25, "P"},
    {26, "LEFTBRACE"},
    {27, "RIGHTBRACE"},
    {28, "ENTER"},
    {29, "LEFTCTRL"},
    {30, "A"},
    {31, "S"},
    {32, "D"},
    {33, "F"},
    {34, "G"},
    {35, "H"},
    {36, "J"},
    {37, "K"},
    {38, "L"},
    {39, "SEMICOLON"},
    {40, "APOSTROPHE"},
    {41, "GRAVE"},
    {42, "LEFTSHIFT"},
    {43, "BACKSLASH"},
    {44, "Z"},
    {45, "X"},
    {46, "C"},
    {47, "V"},
    {48, "B"},
    {49, "N"},
    {50, "M"},
    {51, "COMMA"},
    {52, "DOT"},
    {53, "SLASH"},
    {54, "RIGHTSHIFT"},
    {55, "KPASTERISK"},
    {56, "LEFTALT"},
    {57, "SPACE"},
    {58, "CAPSLOCK"},
    {59, "F1"},
    {60, "F2"},
    {61, "F3"},
    {62, "F4"},
    {63, "F5"},
    {64, "F6"},
    {65, "F7"},
    {66, "F8"},
    {67, "F9"},
    {68, "F10"},
    {69, "NUMLOCK"},
    {70, "SCROLLLOCK"},
    {71, "KP7"},
    {72, "KP8"},
    {73, "KP9"},
    {74, "KPMINUS"},
    {75, "KP4"},
    {76, "KP5"},
    {77, "KP6"},
    {78, "KPPLUS"},
    {79, "KP1"},
    {80, "KP2"},
    {81, "KP3"},
    {82, "KP0"},
    {83, "KPDOT"},
    {85, "ZENKAKUHANKAKU"},
    {86, "102ND"},
    {87, "F11"},
    {88, "F12"},
    {89, "RO"},
    {90, "KATAKANA"},
    {91, "HIRAGANA"},
    {92, "HENKAN"},
    {93, "KATAKANAHIRAGANA"},
    {94, "MUHENKAN"},
    {95, "KPJPCOMMA"},
    {96, "KPENTER"},
    {97, "RIGHTCTRL"},
    {98, "KPSLASH"},
    {99, "SYSRQ"},
    {100, "RIGHTALT"},
    {101, "LINEFEED"},
    {102, "HOME"},
    {103, "UP"},
    {104, "PAGEUP"},
    {105, "LEFT"},
    {106, "RIGHT"},
    {107, "END"},
    {108, "DOWN"},
    {109, "PAGEDOWN"},
    {110, "INSERT"},
    {111, "DELETE"},
    {112, "MACRO"},
    {113, "MUTE"},
    {114, "VOLUMEDOWN"},
    {115, "VOLUMEUP"},
    {116, "POWER"},
    {117, "KPEQUAL"},
    {118, "KPPLUSMINUS"},
    {119, "PAUSE"},
    {120, "SCALE"},
    {121, "KPCOMMA"},
    {122, "HANGEUL"},
    {123, "HANJA"},
    {124, "YEN"},
    {125, "LEFTMETA"},
    {126, "RIGHTMETA"},
    {127, "COMPOSE"},
    {128, "STOP"},
    {129, "AGAIN"},
    {130, "PROPS"},
    {131, "UNDO"},
    {132, "FRONT"},
    {133, "COPY"},
    {134, "OPEN"},
    {135, "PASTE"},
    {136, "FIND"},
    {137, "CUT"},
    {138, "HELP"},
    {139, "MENU"},
    {140, "CALC"},
    {141, "SETUP"},
    {142, "SLEEP"},
    {143, "WAKEUP"},
    {144, "FILE"},
    {145, "SENDFILE"},
    {146, "DELETEFILE"},
    {147, "XFER"},
    {148, "PROG1"},
    {149, "PROG2"},
    {150, "WWW"},
    {151, "MSDOS"},
    {152, "COFFEE"},
    {153, "ROTATE_DISPLAY"},
    {154, "CYCLEWINDOWS"},
    {155, "MAIL"},
    {156, "BOOKMARKS"},
    {157, "COMPUTER"},
    {158, "BACK"},
    {159, "FORWARD"},
    {160, "CLOSECD"},
    {161, "EJECTCD"},
    {162, "EJECTCLOSECD"},
    {163, "NEXTSONG"},
    {164, "PLAYPAUSE"},
    {165, "PREVIOUSSONG"},
    {166, "STOPCD"},
    {167, "RECORD"},
    {168, "REWIND"},
    {169, "PHONE"},
    {170, "ISO"},
    {171, "CONFIG"},
    {172, "HOMEPAGE"},
    {173, "REFRESH"},
    {174, "EXIT"},
    {175, "MOVE"},
    {176, "EDIT"},
    {177, "SCROLLUP"},
    {178, "SCROLLDOWN"},
    {179, "KPLEFTPAREN"},
    {180, "KPRIGHTPAREN"},
    {181, "NEW"},
    {182, "REDO"},
    {183, "F13"},
    {184, "F14"},
    {185, "F15"},
    {186, "F16"},
    {187, "F17"},
    {188, "F18"},
    {189, "F19"},
    {190, "F20"},
    {191, "F21"},
    {192, "F22"},
    {193, "F23"},
    {194, "F24"},
    {200, "PLAYCD"},
    {201, "PAUSECD"},
    {202, "PROG3"},
    {203, "PROG4"},
    {204, "DASHBOARD"},
    {205, "SUSPEND"},
    {206, "CLOSE"},
    {207, "PLAY"},
    {208, "FASTFORWARD"},
    {209, "BASSBOOST"},
    {210, "PRINT"},
    {211, "HP"},
    {212, "CAMERA"},
    {213, "SOUND"},
    {214, "QUESTION"},
    {215, "EMAIL"},
    {216, "CHAT"},
    {217, "SEARCH"},
    {218, "CONNECT"},
    {219, "FINANCE"},
    {220, "SPORT"},
    {221, "SHOP"},
    {222, "ALTERASE"},
    {223, "CANCEL"},
    {224, "BRIGHTNESSDOWN"},
    {225, "BRIGHTNESSUP"},
    {226, "MEDIA"},
    {227, "SWITCHVIDEOMODE"},
    {228, "KBDILLUMTOGGLE"},
    {229, "KBDILLUMDOWN"},
    {230, "KBDILLUMUP"},
    {231, "SEND"},
    {232, "REPLY"},
    {233, "FORWARDMAIL"},
    {234, "SAVE"},
    {235, "DOCUMENTS"},
    {236, "BATTERY"},
    {237, "BLUETOOTH"},
    {238, "WLAN"},
    {239, "UWB"},
    {240, "UNKNOWN"},
    {241, "VIDEO_NEXT"},
    {242, "VIDEO_PREV"},
    {243, "BRIGHTNESS_CYCLE"},
    {244, "BRIGHTNESS_AUTO"},
    {245, "DISPLAY_OFF"},
    {246, "WWAN"},
    {247, "RFKILL"},
    {248, "MICMUTE"},
};

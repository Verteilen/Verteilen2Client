#pragma once
#include <iostream>
#include <string>

//#define APP_USE_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
#define APP_USE_VULKAN_DEBUG_REPORT
#define AKTION_LOG_LEVEL 5
#define AKTION_LOG_FETAL(Message, ...) if (AKTION_LOG_LEVEL >= 1) printf((std::string("\x1B[31m[%s] ") + Message + "\033[0m\n").c_str(), "FETAL", __VA_ARGS__)
#define AKTION_LOG_ERROR(Message, ...) if (AKTION_LOG_LEVEL >= 2) printf((std::string("\x1B[91m[%s] ") + Message + "\033[0m\n").c_str(), "ERROR", __VA_ARGS__)
#define AKTION_LOG_WARN(Message, ...)  if (AKTION_LOG_LEVEL >= 3) printf((std::string("\x1B[33m[%s] ") + Message + "\033[0m\n").c_str(), "WARN", __VA_ARGS__)
#define AKTION_LOG_INFO(Message, ...)  if (AKTION_LOG_LEVEL >= 4) printf((std::string("\x1B[96m[%s] ") + Message + "\033[0m\n").c_str(), "INFO", __VA_ARGS__)
#define AKTION_LOG_DEBUG(Message, ...) if (AKTION_LOG_LEVEL >= 5) printf((std::string("\x1B[32m[%s] ") + Message + "\033[0m\n").c_str(), "DEBUG", __VA_ARGS__)
#define AKTION_LOG(Message, ...) printf((std::string("[%s]: ") + Message + "\n").c_str(), "USER", __VA_ARGS__)
#else
#define AKTION_LOG_FETAL(Message, ...)
#define AKTION_LOG_ERROR(Message, ...)
#define AKTION_LOG_WARN(Message, ...)
#define AKTION_LOG_INFO(Message, ...)
#define AKTION_LOG_DEBUG(Message, ...)
#define AKTION_LOG(Message, ...)
#endif

#define APPNAME "AktionDesigner"
#define WIN_WIDTH 1920
#define WIN_HEIGHT 1080
// 0 Window, 1 Maximized, 2 Fullscreen
#define WIN_USE_FULLSCREEN 1

#define MAXDISPLAY 12
#define MIN_SCALE 0.5f
#define MAX_SCALE 5.0f
#define IM_MIN(A, B)            (((A) < (B)) ? (A) : (B))
#define IM_MAX(A, B)            (((A) >= (B)) ? (A) : (B))
#define IM_CLAMP(V, MN, MX)     ((V) < (MN) ? (MN) : (V) > (MX) ? (MX) : (V))

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define THEMECOUNT 3
#define IMGUI_DEFINE_MATH_OPERATORS

const float gui_clear_color[4] = { 0.45f, 0.55f, 0.60f, 1.00f };

#define PIN_ICON_SIZE 24

// Preference files
#define THEME_FILE "theme.bin"

// Project files
#define SETTING_FILE "setting.bin"

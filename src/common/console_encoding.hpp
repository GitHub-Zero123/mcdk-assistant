#pragma once

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace mcdk::app {

inline bool init_console_encoding() {
#ifdef _WIN32
    return SetConsoleOutputCP(CP_UTF8) != 0 && SetConsoleCP(CP_UTF8) != 0;
#else
    return true;
#endif
}

} // namespace mcdk::app


#pragma once

#include "Config/Config.hpp"

#ifdef SPDLOG
#include <spdlog/spdlog.h>
#endif

class Logger {
public:
    // support all spdlog overloads

    // single argument overloads
    template<typename T>
    static void info(const T* msg) {
#ifdef SPDLOG
        spdlog::info(msg);
#else
        printf("%s\n", msg);
#endif
    }
        
    template<typename T>
    static void error(const T* msg) {
#ifdef SPDLOG
        spdlog::error(msg);
#else
        fprintf(stderr, "%s\n", msg);
#endif
    }

    template<typename T>
    static void warn(const T* msg) {
#ifdef SPDLOG
        spdlog::warn(msg);
#else
        printf("%s\n", msg);
#endif
    }

    template<typename T>
    static void debug(const T* msg) {
#ifdef SPDLOG
        spdlog::debug(msg);
#else
        printf("%s\n", msg);
#endif
    }


    // variable arguments oveloads (only for spdlog)
    // if spdlog is not defined, only the first argument will be printed
    template<typename... Args>
    static void info(const char* fmt, const Args&... args) {
#ifdef SPDLOG
        spdlog::info(fmt, args...);
#else
        printf(fmt);
        printf("\n");
#endif
    }

    template<typename... Args>
    static void error(const char* fmt, const Args&... args) {
#ifdef SPDLOG
        spdlog::error(fmt, args...);
#else
        fprintf(stderr, fmt, args...);
        fprintf(stderr, "\n");
#endif
    }

    template<typename... Args>
    static void warn(const char* fmt, const Args&... args) {
#ifdef SPDLOG
        spdlog::warn(fmt, args...);
#else
        printf(fmt, args...);
        printf("\n");
#endif
    }

    template<typename... Args>
    static void debug(const char* fmt, const Args&... args) {
#ifdef SPDLOG
        spdlog::debug(fmt, args...);
#else
        printf(fmt, args...);
        printf("\n");
#endif
    }
    
    
};
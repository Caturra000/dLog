#ifndef __DLOG_LOG_H__
#define __DLOG_LOG_H__
#include <bits/stdc++.h>
#include "resolve.h"
#include "stream.h"
#include "sched.h"
#include "chrono.h"
#include "tid.h"
namespace dlog {

class LogBase {
public:
    static Wthread& init() {
        static Wthread wthread;
        return wthread;
    }

    template <typename ...Ts>
    static void debug(Ts &&...msg);

    template <typename ...Ts>
    static void info(Ts &&...msg);

    template <typename ...Ts>
    static void warn(Ts &&...msg);

    template <typename ...Ts>
    static void error(Ts &&...msg);

    template <typename ...Ts>
    static void wtf(Ts &&...msg);

private:
    enum LogLevel {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        WTF,

        LOG_LEVEL_LIMIT
    };

    template <LogLevel LEVEL>
    static constexpr char levelFormat() noexcept {
        static_assert(LEVEL >= 0 && LEVEL < LOG_LEVEL_LIMIT, "check log level config.");
        return "DIWE?"[LEVEL];
    }

    template <typename ...Ts>
    static void log(Ts &&...msg);

    template <LogLevel Level, typename ...Ts>
    static void logFormat(Ts &&...msg);

};

using Log = LogBase;

template <typename ...Ts>
inline void LogBase::debug(Ts &&...msg) {
    logFormat<LogLevel::DEBUG>(std::forward<Ts>(msg)...);
}

template <typename ...Ts>
inline void LogBase::info(Ts &&...msg) {
    logFormat<LogLevel::INFO>(std::forward<Ts>(msg)...);
}

template <typename ...Ts>
inline void LogBase::warn(Ts &&...msg) {
    logFormat<LogLevel::WARN>(std::forward<Ts>(msg)...);
}

template <typename ...Ts>
inline void LogBase::error(Ts &&...msg) {
    logFormat<LogLevel::ERROR>(std::forward<Ts>(msg)...);
}

template <typename ...Ts>
inline void LogBase::wtf(Ts &&...msg) {
    logFormat<LogLevel::WTF>(std::forward<Ts>(msg)...);
}

template <typename T> inline constexpr size_t bufcnt(T &&) { return StreamTraits<T>::size; }
template <typename T, typename ...Ts> inline constexpr size_t bufcnt(T &&, Ts &&...others) {
    return StreamTraits<T>::size + bufcnt(std::forward<Ts>(others)...);
}

template <typename T> inline constexpr size_t strcnt(T&&) { return 0; }
template <size_t N> inline constexpr size_t strcnt(const char (&)[N]) { return 1; }
template <typename T, typename ...Ts> inline constexpr size_t strcnt(T &&t, Ts &&...ts) {
    return strcnt(std::forward<T>(t)) + strcnt(std::forward<Ts>(ts)...); 
}

template <typename T> inline constexpr size_t iovcnt(T&&) { return 1; }
template <size_t N> inline constexpr size_t iovcnt(const std::array<IoVector, N>&) { return N; }
template <size_t N> inline constexpr size_t iovcnt(std::array<IoVector, N>&) { return N; }
template <size_t N> inline constexpr size_t iovcnt(std::array<IoVector, N>&&) { return N; }
template <typename T, typename ...Ts> inline constexpr size_t iovcnt(T &&t, Ts &&...ts) {
    return iovcnt(std::forward<T>(t)) + iovcnt(std::forward<Ts>(ts)...); 
}


template <typename ...Ts>
inline void LogBase::log(Ts &&...msg) {
    char tmp[bufcnt(msg...)];
    const char *tmpref[strcnt(msg...)]; // an array stores char_ptr
    IoVector ioves[iovcnt(msg...)];
    ResolveArgs args {
        .local = tmp,
        .cur = 0,
        .ioves = ioves,
        .count = 0,
        .total = 0
    };
    Resolver::resolve(args, std::forward<Ts>(msg)...);
    Scheduler::log(args);
}

template <LogBase::LogLevel LEVEL, typename ...Ts>
inline void LogBase::logFormat(Ts &&...msg) {
    log(Chrono::format(Chrono::now()), Tid::format(), levelFormat<LEVEL>(), std::forward<Ts>(msg)...);
}

} // dlog
#endif
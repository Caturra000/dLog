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
    static void info(Ts &&...msg);

    template <typename ...Ts>
    static void debug(Ts &&...msg);

private:
    enum LogLevel {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        UNKNOWN
    };

    // print to file
    static constexpr char printLevel(LogLevel level) noexcept {
        return "DIWE?"[level];
    }

    template <typename ...Ts>
    static void log(Ts &&...msg);

};

using Log = LogBase;

template <typename ...Ts>
inline void LogBase::info(Ts &&...msg) {
    auto dateTime = Chrono::format(Chrono::now());
    auto tid = Tid::getIoV();
    log(dateTime[0], dateTime[1], tid, printLevel(INFO),  std::forward<Ts>(msg)...);
}

template <typename ...Ts>
inline void LogBase::debug(Ts &&...msg) {
    auto dateTime = Chrono::format(Chrono::now());
    auto tid = Tid::getIoV();
    log(dateTime[0], dateTime[1], tid, printLevel(DEBUG),  std::forward<Ts>(msg)...);
}

template <typename ...Ts>
inline void LogBase::log(Ts &&...msg) {
    char tmp[bufcnt(std::forward<Ts>(msg)...)];
    const char *tmpref[strcnt(std::forward<Ts>(msg)...)]; // an array stores char_ptr
    IoVector ioves[sizeof...(msg)];
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

} // dlog
#endif
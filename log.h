#ifndef __DLOG_LOG_H__
#define __DLOG_LOG_H__
#include <bits/stdc++.h>
#include "resolve.h"
#include "stream.h"
#include "sched.h"
namespace dlog {

struct LogBase {

    template <typename ...Ts>
    static void info(Ts &&...msg);

};

using Log = LogBase;

template <typename ...Ts>
inline void LogBase::info(Ts &&...msg) {
    char tmp[bufcnt(std::forward<Ts>(msg)...)];
    const char *tmpref[strcnt(std::forward<Ts>(msg)...)]; // an array stores char_ptr
    IoVector ioves[sizeof...(msg)];
    // TODO use std::array
    ResolveArgs args {
        .local = tmp,
        .cur = 0,
        .ioves = ioves,
        .count = 0
    };
    Resolver::resolve(args, std::forward<Ts>(msg)...);
    Scheduler::log(args);
}

} // dlog
#endif
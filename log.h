#ifndef __DLOG_LOG_H__
#define __DLOG_LOG_H__
#include <bits/stdc++.h>
#include "resolve.h"
#include "stream.h"
#include "sched.h"
namespace dlog {

struct LogBase {

    static Wthread& init() {
        static Wthread wthread;
        return wthread;
    }

    template <typename ...Ts>
    static void info(Ts &&...msg);

    template <typename ...Ts>
    static void test(Ts &&...msg);

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
        .count = 0,
        .total = 0
    };
    Resolver::resolve(args, std::forward<Ts>(msg)...);
    Scheduler::log(args);
}

template <typename ...Ts>
inline void LogBase::test(Ts &&...msg) {
    char tmp[bufcnt(std::forward<Ts>(msg)...)];
    const char *tmpref[strcnt(std::forward<Ts>(msg)...)]; // an array stores char_ptr
    IoVector ioves[sizeof...(msg)];
    // TODO use std::array
    ResolveArgs args {
        .local = tmp,
        .cur = 0,
        .ioves = ioves,
        .count = 0,
        .total = 0
    };
    Resolver::resolve(args, std::forward<Ts>(msg)...);
    for(int i = 0; i < args.count; ++i) {
        for(int j = 0; j < args.ioves[i].len; ++j) {
            std::cerr << args.ioves[i].base[j];
        }
        std::cerr << ' ';
    }
    std::cerr << std::endl;

    char ibuf[0xffff]{};
    
    Resolver::vec2buf(args, ibuf);
    std::cerr << ibuf;

}

} // dlog
#endif
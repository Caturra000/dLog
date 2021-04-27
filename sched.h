#ifndef __DLOG_SCHED_H__
#define __DLOG_SCHED_H__
#include <bits/stdc++.h>
#include "shared.h"
#include "stream.h"
#include "resolve.h"
namespace dlog {

std::mutex rmtx, wmtx;
std::condition_variable rcond, wcond;
using namespace std::literals::chrono_literals;
using Timestamp = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;
std::once_flag cflag;
Timestamp checkpoint;

struct Scheduler {

    // brief: start a write-log thread
    // usage: Scheduler::start().detach()
    // TODO stop it manually
    static std::thread start() {
        std::call_once(cflag, [] { checkpoint = std::chrono::system_clock::now(); });
        return std::thread {[] {
            while(true) {
                {
                    std::unique_lock<std::mutex> lk{wmtx};
                    wcond.wait(lk, [] { return wcur != 0; });
                    // write to file
                    // debug
                    for(int i = 0; i < wcur; ++i) std::cerr << buf[widx][i];
                    wcur = 0;
                }
                rcond.notify_one();
                // if(stop) break;
            }
        }};
    }

    static void log(ResolveArgs &args) {
        {
            std::lock_guard<std::mutex> lk{rmtx};
            if(rcur + Resolver::calspace(args) > sizeof(buf[0]) || (std::chrono::system_clock::now() - checkpoint) > 500ms) {
                {
                    std::unique_lock<std::mutex> _{wmtx};
                    rcond.wait(_, [] { return wcur == 0; });
                    ridx ^= 1; widx ^= 1;
                    wcur = rcur; rcur = 0;
                }
                wcond.notify_one();
                checkpoint = std::chrono::system_clock::now();
            }
            auto rbuf = buf[ridx];
            Resolver::vec2buf(args, rbuf);
            rcur += Resolver::calspace(args);
        }
        wcond.notify_one();
    }
};


} // dlog
#endif
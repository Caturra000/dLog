#ifndef __DLOG_SCHED_H__
#define __DLOG_SCHED_H__
#include <bits/stdc++.h>
#include "shared.h"
#include "stream.h"
#include "resolve.h"
namespace dlog {

std::mutex rmtx, smtx; // read_mutex swap_mutex
std::condition_variable cond;
using namespace std::literals::chrono_literals;
bool sflag = true; // swap finished

struct Scheduler {

    // brief: start a write-log thread
    // usage: Scheduler::start().detach()
    // TODO stop it manually
    static std::thread start() {
        return std::thread {[] {

            int cur, idx;
            auto swap = [&] {
                ridx ^= 1;
                widx ^= 1;
                wcur = rcur;
                rcur = 0;

                sflag = true; // swap finished

                cur = wcur;
                wcur = 0;

                idx = widx;
                
            };


            while(true) {
                {
                    std::unique_lock<std::mutex> lk{smtx};
                    auto request = cond.wait_for(lk, 10ms, [] { return !sflag; });
                    
                    if(request) {
                        swap();
                    } else {  
                        // corner case, timeout
                        if(rmtx.try_lock()) {
                            std::lock_guard<std::mutex> _{rmtx, std::adopt_lock};
                            swap();
                        }
                    }
                    
                }
                // debug IO
                for(int i = 0; i < cur; ++i) std::cerr << buf[idx][i];
                // if(stop) break;
            }
        }};
    }

    static void log(ResolveArgs &args) {
        std::lock_guard<std::mutex> lk{rmtx};
        if(rcur + Resolver::calspace(args) >= sizeof(buf[0])) {
            {
                std::unique_lock<std::mutex> _{smtx};
                sflag = false;
            }
            while(!sflag) cond.notify_one(); // dont wait until IO complete
        }
        auto rbuf = buf[ridx];
        Resolver::vec2buf(args, rbuf + rcur);
        rcur += Resolver::calspace(args);
    }
};


} // dlog
#endif
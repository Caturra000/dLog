#ifndef __DLOG_SCHED_H__
#define __DLOG_SCHED_H__
#include <bits/stdc++.h>
#include "shared.h"
#include "stream.h"
#include "resolve.h"
#include "fs.h"
namespace dlog {

std::mutex rmtx, smtx; // read_mutex swap_mutex
std::condition_variable cond;
using namespace std::literals::chrono_literals;
bool sflag = true; // swap finished

// a writer thread
class Wthread {
public:
    Wthread();
    ~Wthread();

private:
    std::thread writer;
    File file;
    void writeFunc();
}; // wthread;

// interact with wthread
struct Scheduler {

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


inline Wthread::Wthread()
    : file("test.log"), // hard code
      writer {[this] { writeFunc(); }} {}

inline Wthread::~Wthread() {
    if(writer.joinable()) {
        writer.join();
    }
}

inline void Wthread::writeFunc() {
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
            } else if(rmtx.try_lock()) { // use try, no dead lock
                // corner case, timeout
                std::lock_guard<std::mutex> _{rmtx, std::adopt_lock};
                swap();
            } else {
                continue;
            }
            
        }
        file.append(buf[idx], cur);
    }
}

} // dlog
#endif
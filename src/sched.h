#ifndef __DLOG_SCHED_H__
#define __DLOG_SCHED_H__
#include <bits/stdc++.h>
#include "config.h"
#include "stream.h"
#include "resolve.h"
#include "fs.h"
#include "io.h"
#include "chrono.h"
namespace dlog {

// shared mutable variables
// protected by namespace

constexpr size_t DLOG_BUFSIZE = 1<<20;
char buf[2][DLOG_BUFSIZE];
size_t ridx = 0, rcur = 0;
size_t widx = 1, wcur = 0; 

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
    static std::string generateFileName();
}; // wthread;

// interact with wthread
struct Scheduler {
    static void log(ResolveContext &args) {
        std::lock_guard<std::mutex> lk{rmtx};
        if(rcur + Resolver::calspace(args) >= sizeof(buf[0])) {
            {
                std::unique_lock<std::mutex> _{smtx};
                sflag = false;
            }
            while(!sflag) cond.notify_one();
        }
        auto rbuf = buf[ridx];
        Resolver::vec2buf(args, rbuf + rcur);
        rcur += Resolver::calspace(args);
    }
};

/// impl

inline Wthread::Wthread()
    : file(generateFileName()),
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
        if(file.updatable(cur)) {
            file.update(generateFileName());
        }
        file.append(buf[idx], cur);
    }
}

inline std::string Wthread::generateFileName() {
    std::array<IoVector, 2> dateTime = Chrono::format(Chrono::now());
    std::string add = std::string(dateTime[0].base) + '-' + std::string(dateTime[1].base);
    std::for_each(add.begin(), add.end(), [](char &ch) {
        if(ch == ' ' || ch == ':') ch = '-';
    });

    return std::string(staticConfig.log_dir) + '/'
          + staticConfig.log_filename
          + '.' + add
          + staticConfig.log_filename_extension;
}

} // dlog
#endif
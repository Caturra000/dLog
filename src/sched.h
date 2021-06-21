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



// shared for writer thread (backend) and scheduler (frontend)
// use singleton instead of global variables as a inline field
struct Shared {

    // buffers

    constexpr static size_t DLOG_BUFSIZE = 1<<20;

    char buf[2][DLOG_BUFSIZE];
    size_t ridx = 0, rcur = 0;
    size_t widx = 1, wcur = 0;

    // concurrency

    std::mutex rmtx, smtx; // read_mutex swap_mutex
    std::condition_variable cond;
    bool sflag = true; // swap finished

    static Shared& singleton() {
        static Shared instance;
        return instance;
    }
};

// a writer thread
class Wthread {
public:
    Wthread();
    ~Wthread();
    void kill() { kflag.store(true); }
private:
    File file;
    std::atomic<bool> kflag;
    std::thread writer;
    void writeFunc();
    static std::string generateFileName();
}; // wthread;

// interact with wthread
template <typename ResolverImpl>
struct SchedulerBase {
    static void apply(ResolveContext &args) {
        auto &s = Shared::singleton();
        std::lock_guard<std::mutex> lk{s.rmtx};
        if(s.rcur + ResolverImpl::estimate(args) >= sizeof(s.buf[0])) {
            {
                std::unique_lock<std::mutex> _{s.smtx};
                s.sflag = false;
            }
            while(!s.sflag) s.cond.notify_one();
        }
        auto rbuf = s.buf[s.ridx];
        s.rcur += ResolverImpl::put(args, rbuf + s.rcur);
    }
};

/// impl

inline Wthread::Wthread()
    : file(generateFileName()),
      kflag(false),
      writer {[this] { writeFunc(); }} {}

inline Wthread::~Wthread() {
    kill();
    if(writer.joinable()) {
        writer.join();
    }
}

inline void Wthread::writeFunc() {
    using namespace std::literals::chrono_literals;
    int cur, idx;
    auto &s = Shared::singleton();
    auto swap = [&cur, &idx](size_t &ridx, size_t &rcur,
                             size_t &widx, size_t &wcur) {
        ridx ^= 1;
        widx ^= 1;
        wcur = rcur;
        rcur = 0;
        cur = wcur;
        wcur = 0;
        idx = widx;
    };
    while(!kflag.load()) {
        {
            std::unique_lock<std::mutex> lk{s.smtx};
            auto request = s.cond.wait_for(lk, 10ms, [&] { return !s.sflag; });
            if(request) {
                swap(s.ridx, s.rcur, s.widx, s.wcur);
                s.sflag = true; // swap finished
            } else if(s.rmtx.try_lock()) { // use try, no dead lock
                // corner case, timeout
                std::lock_guard<std::mutex> _{s.rmtx, std::adopt_lock};
                swap(s.ridx, s.rcur, s.widx, s.wcur);
                s.sflag = true;
            } else {
                continue;
            }
        }
        if(file.updatable(cur)) {
            file.update(generateFileName());
        }
        file.append(s.buf[idx], cur);
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
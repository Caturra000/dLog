#ifndef __DLOG_TID_H__
#define __DLOG_TID_H__
#include <unistd.h>
#include <bits/stdc++.h>
#include "stream.h"
#include "io.h"
namespace dlog {

class Tid {
public:
    static const Tid& get() {
        static const thread_local Tid instance;
        return instance;
    }

    static IoVector getIoV() {
        auto &instance = get();
        return { .base = instance.buf, .len = instance.len };
    }

    pid_t getTid() const { return tid; }

private:
    Tid(): tid(::gettid()), len(Stream::parseLength(tid)) {
        Stream::parse(buf, tid, len);
    }

    pid_t tid;
    size_t len;
    char buf[33]{};
};

} // dlog
#endif
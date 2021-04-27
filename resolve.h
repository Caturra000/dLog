#ifndef __DLOG_RESOLVE_H__
#define __DLOG_RESOLVE_H__
#include <bits/stdc++.h>
#include "stream.h"
namespace dlog {

struct IoVector {
    const char *base; // base address
    size_t len; // [base, base + len)
};

struct ResolveArgs {
    char *local; // local stack buffer
    size_t cur; // current index of local
    IoVector *ioves;
    size_t count; // count of iovectors
    size_t total; // total length
};

struct Resolver {
    template <typename T>
    static void resolve(ResolveArgs &args, T &&msg) {
        resolveDispatch(args, msg);
    }

    template <typename T, typename ...Ts>
    static void resolve(ResolveArgs &args, T&&msg, Ts &&...others) {
        resolveDispatch(args, msg);
        resolve(args, std::forward<Ts>(others)...);
    }

    static size_t calspace(ResolveArgs &args) {
        return args.total + args.count;
    }

    static void vec2buf(ResolveArgs &args, char *buf) {
        IoVector *ioves = args.ioves;
        for(int i = 0, j = 0; i < args.count; ++i) {
            std::memcpy(buf + j, ioves[i].base, ioves[i].len);
            j += ioves[i].len;
            if(i == args.count-1) buf[j] = '\n';
            else buf[j] = ' ';
            ++j;
        }
    }

private:

    template <typename T>
    static void resolveDispatch(ResolveArgs &args, T &&msg) {
        size_t len = Stream::parseLength(msg);
        const char *buf = resolveIovBase(args, msg);
        parseIfNeed(buf, msg, len);
        bool isLocal = (buf == args.local + args.cur);
        if(isLocal) args.cur += len;
        args.ioves[args.count].base = buf;
        args.ioves[args.count++].len = len;
        args.total += len;
    }

    template <typename T>
    static const char* resolveIovBase(ResolveArgs &args, T &&) {
        return args.local + args.cur;
    }

    template <size_t N>
    static const char* resolveIovBase(ResolveArgs &args, const char(&str)[N]) {
        return str;
    }

    template <typename T>
    static void parseIfNeed(const char *buf, T &&msg, size_t length) {
        // safe
        Stream::parse(const_cast<char*>(buf), msg, length);
    }

    template <size_t N>
    static void parseIfNeed(const char *buf, const char (&msg)[N], size_t length) {}

};

} // dlog
#endif
#ifndef __DLOG_RESOLVE_H__
#define __DLOG_RESOLVE_H__
#include <bits/stdc++.h>
#include "stream.h"
#include "io.h"
namespace dlog {

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
        resolveDispatch(args, std::forward<T>(msg));
    }

    template <typename T, typename ...Ts>
    static void resolve(ResolveArgs &args, T&&msg, Ts &&...others) {
        resolveDispatch(args, std::forward<T>(msg));
        resolve(args, std::forward<Ts>(others)...);
    }

    static size_t calspace(ResolveArgs &args) {
        return args.total + args.count;
    }

    static void vec2buf(ResolveArgs &args, char *buf) {
        IoVector *ioves = args.ioves;
        size_t offset = 0;
        for(size_t i = 0; i < args.count; ++i) {
            std::memcpy(buf + offset, ioves[i].base, ioves[i].len);
            offset += ioves[i].len;
            if(i == args.count-1) buf[offset] = '\n';
            else buf[offset] = ' ';
            ++offset;
        }
    }

private:

    template <typename T>
    static void resolveDispatch(ResolveArgs &args, T &&msg) {
        size_t len = Stream::parseLength(std::forward<T>(msg));
        const char *ptr = resolveIovBase(args, msg);
        parseIfNeed(ptr, msg, len);
        bool isRuntime = (ptr == args.local + args.cur);
        if(isRuntime) args.cur += len;
        args.ioves[args.count].base = ptr;
        args.ioves[args.count].len = len;
        args.count++;
        args.total += len;
    }

    static void resolveDispatch(ResolveArgs &args, IoVector iov) {
        args.ioves[args.count].base = iov.base;
        args.ioves[args.count].len = iov.len;
        args.count++;
        args.total += iov.len;
    }

    static void resolveDispatch(ResolveArgs &args, IoVector &&iov) {
        resolveDispatch(args, iov); // lref
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
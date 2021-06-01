#ifndef __DLOG_RESOLVE_H__
#define __DLOG_RESOLVE_H__
#include <bits/stdc++.h>
#include "stream.h"
#include "io.h"
#include "mstr.h"
namespace dlog {

struct ResolveContext {
    char *local; // local stack buffer
    size_t cur; // current index of local
    IoVector *ioves;
    size_t count; // count of iovectors
    size_t total; // total length

    char* currentLocal() { return local + cur; }

    // notice: message should be parsed & transferred to local
    void updateLocal(size_t len) {
        updateExternal(local + cur, len);
        cur += len;
    }

    // external source, just update IO vectors
    void updateExternal(const char *buf, size_t len) {
        ioves[count].base = buf;
        ioves[count].len = len;
        count++;
        total += len;
    }
};

struct Resolver {
    template <typename T> static void resolve(ResolveContext &ctx, T &&msg);
    template <typename T, typename ...Ts> static void resolve(ResolveContext &ctx, T&&msg, Ts &&...others);

    static size_t calspace(ResolveContext &ctx) { return ctx.total + ctx.count; }
    static void put(ResolveContext &ctx, char *buf);

private:
    template <typename T> static void resolveDispatch(ResolveContext &ctx, T &&msg);
    template <size_t N> static void resolveDispatch(ResolveContext &ctx, const char (&msg)[N]);
    static void resolveDispatch(ResolveContext &ctx, int msg);
    static void resolveDispatch(ResolveContext &ctx, size_t msg);
    static void resolveDispatch(ResolveContext &ctx, IoVector iov);
    template <size_t N> static void resolveDispatch(ResolveContext &ctx, std::array<IoVector, N> &ioves);
    template <size_t N> static void resolveDispatch(ResolveContext &ctx, std::array<IoVector, N> &&ioves);
};

/// impl

template <typename T>
inline void Resolver::resolve(ResolveContext &ctx, T &&msg) {
    resolveDispatch(ctx, std::forward<T>(msg));
}

template <typename T, typename ...Ts>
inline void Resolver::resolve(ResolveContext &ctx, T&&msg, Ts &&...others) {
    resolveDispatch(ctx, std::forward<T>(msg));
    resolve(ctx, std::forward<Ts>(others)...);
}

inline void Resolver::put(ResolveContext &ctx, char *buf) {
    IoVector *ioves = ctx.ioves;
    size_t offset = 0;
    for(size_t i = 0; i < ctx.count; ++i) {
        std::memcpy(buf + offset, ioves[i].base, ioves[i].len);
        offset += ioves[i].len;
        if(i == ctx.count-1) buf[offset] = '\n';
        else buf[offset] = ' ';
        ++offset;
    }
}

template <typename T>
inline void Resolver::resolveDispatch(ResolveContext &ctx, T &&msg) {
    size_t len = Stream::parseLength(std::forward<T>(msg));
    char *buf = ctx.currentLocal();
    Stream::parse(buf, std::forward<T>(msg), len);
    ctx.updateLocal(len);
}

template <size_t N>
inline void Resolver::resolveDispatch(ResolveContext &ctx, const char (&msg)[N]) {
    static_assert(N >= 1, "N must be positive.");
    size_t len = N-1;
    ctx.updateExternal(msg, len);
}

inline void Resolver::resolveDispatch(ResolveContext &ctx, int msg) {
    constexpr static size_t limit = 10000;
    using Cache = meta::NumericMetaStringsSequence<limit>;
    if(msg > 0 && msg < limit) {
        resolveDispatch(ctx, IoVector{Cache::bufs[msg], Cache::len[msg]});
    } else {
        resolveDispatch<>(ctx, msg);
    }
}

inline void Resolver::resolveDispatch(ResolveContext &ctx, size_t msg) {
    constexpr static size_t limit = 10000;
    using Cache = meta::NumericMetaStringsSequence<limit>;
    if(msg < limit) {
        resolveDispatch(ctx, IoVector{Cache::bufs[msg], Cache::len[msg]});
    } else {
        resolveDispatch<>(ctx, msg);
    }
}

inline void Resolver::resolveDispatch(ResolveContext &ctx, IoVector iov) {
    ctx.updateExternal(iov.base, iov.len);
}

template <size_t N>
inline void Resolver::resolveDispatch(ResolveContext &ctx, std::array<IoVector, N> &ioves) {
    for(auto &iov : ioves) resolveDispatch(ctx, iov);
}

template <size_t N>
inline void Resolver::resolveDispatch(ResolveContext &ctx, std::array<IoVector, N> &&ioves) {
    resolveDispatch(ctx, ioves);
}

} // dlog
#endif
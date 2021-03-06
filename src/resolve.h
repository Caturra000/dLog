#ifndef __DLOG_RESOLVE_H__
#define __DLOG_RESOLVE_H__
#include <bits/stdc++.h>
#include "stream.h"
#include "io.h"
#include "mstr.h"
#include "mixin.h"
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

template <typename StreamImpl>
struct NonPutResolverBase {
    template <typename T> static void resolve(ResolveContext &ctx, T &&msg);
    template <typename T, typename ...Ts> static void resolve(ResolveContext &ctx, T&&msg, Ts &&...others);

private:
    template <typename T> static void resolveDispatch(ResolveContext &ctx, T &&msg);
    template <size_t N> static void resolveDispatch(ResolveContext &ctx, const char (&msg)[N]);
    static void resolveDispatch(ResolveContext &ctx, int msg);
    static void resolveDispatch(ResolveContext &ctx, size_t msg);
    static void resolveDispatch(ResolveContext &ctx, IoVector iov);
    template <size_t N> static void resolveDispatch(ResolveContext &ctx, std::array<IoVector, N> &ioves);
    template <size_t N> static void resolveDispatch(ResolveContext &ctx, std::array<IoVector, N> &&ioves);
};

/// extend

namespace policy {

// CRTP interface
template <typename Derived>
struct PutInterface {
    static size_t estimate(ResolveContext &ctx);
    static size_t put(ResolveContext &ctx, char *buf);
};

// default put policy
struct Whitespace: public PutInterface<Whitespace> {
    static size_t estimateImpl(ResolveContext &ctx) { return ctx.total + ctx.count; }
    static size_t putIov(char *buf, IoVector &iov, size_t nth);
    static size_t putGap(char *buf, size_t nth);
    static size_t putLine(char *buf);
};

} // policy

template <typename StreamImpl>
struct ResolverBase: public NonPutResolverBase<StreamImpl>, public policy::Whitespace {};

template <typename StreamImpl, typename ...Policies>
struct ResolverExtend;

template <typename StreamImpl>
struct ResolverExtend<StreamImpl>: public ResolverBase<StreamImpl> {};

template <typename StreamImpl>
struct ResolverExtend<StreamImpl, policy::Whitespace>: public ResolverBase<StreamImpl> {};

template <typename StreamImpl, typename ...Policies>
struct ResolverExtend: public NonPutResolverBase<StreamImpl> {
    using Policy = meta::Mixin<Policies...>;
    using Base = NonPutResolverBase<StreamImpl>;

    template <typename P = Policy>
    static auto estimate(ResolveContext &ctx)
        -> decltype(P::estimate(ctx)) { return P::estimate(ctx); }

    template <typename B = Base, typename = B>
    static auto estimate(ResolveContext &ctx)
        -> decltype(B::estimate(ctx)) { return B::estimate(ctx); }

    template <typename P = Policy>
    static auto put(ResolveContext &ctx, char *buf)
        -> decltype(P::put(ctx, buf)) { return P::put(ctx, buf); }

    template <typename B = Base, typename = B>
    static auto put(ResolveContext &ctx, char *buf)
        -> decltype(B::put(ctx, buf)) { return B::put(ctx, buf); }
};

/// impl

template <typename StreamImpl>
template <typename T>
inline void NonPutResolverBase<StreamImpl>::resolve(ResolveContext &ctx, T &&msg) {
    resolveDispatch(ctx, std::forward<T>(msg));
}

template <typename StreamImpl>
template <typename T, typename ...Ts>
inline void NonPutResolverBase<StreamImpl>::resolve(ResolveContext &ctx, T&&msg, Ts &&...others) {
    resolveDispatch(ctx, std::forward<T>(msg));
    resolve(ctx, std::forward<Ts>(others)...);
}

template <typename StreamImpl>
template <typename T>
inline void NonPutResolverBase<StreamImpl>::resolveDispatch(ResolveContext &ctx, T &&msg) {
    size_t len = StreamImpl::parseLength(std::forward<T>(msg));
    char *buf = ctx.currentLocal();
    StreamImpl::parse(buf, std::forward<T>(msg), len);
    ctx.updateLocal(len);
}

template <typename StreamImpl>
template <size_t N>
inline void NonPutResolverBase<StreamImpl>::resolveDispatch(ResolveContext &ctx, const char (&msg)[N]) {
    static_assert(N >= 1, "N must be positive.");
    size_t len = N-1;
    ctx.updateExternal(msg, len);
}

template <typename StreamImpl>
inline void NonPutResolverBase<StreamImpl>::resolveDispatch(ResolveContext &ctx, int msg) {
    constexpr static size_t limit = 10000;
    using Cache = meta::NumericMetaStringsSequence<limit>;
    if(msg > 0 && msg < limit) {
        resolveDispatch(ctx, IoVector{Cache::bufs[msg], Cache::len[msg]});
    } else {
        resolveDispatch<>(ctx, msg);
    }
}

template <typename StreamImpl>
inline void NonPutResolverBase<StreamImpl>::resolveDispatch(ResolveContext &ctx, size_t msg) {
    constexpr static size_t limit = 10000;
    using Cache = meta::NumericMetaStringsSequence<limit>;
    if(msg < limit) {
        resolveDispatch(ctx, IoVector{Cache::bufs[msg], Cache::len[msg]});
    } else {
        resolveDispatch<>(ctx, msg);
    }
}

template <typename StreamImpl>
inline void NonPutResolverBase<StreamImpl>::resolveDispatch(ResolveContext &ctx, IoVector iov) {
    ctx.updateExternal(iov.base, iov.len);
}

template <typename StreamImpl>
template <size_t N>
inline void NonPutResolverBase<StreamImpl>::resolveDispatch(ResolveContext &ctx, std::array<IoVector, N> &ioves) {
    for(auto &iov : ioves) resolveDispatch(ctx, iov);
}

template <typename StreamImpl>
template <size_t N>
inline void NonPutResolverBase<StreamImpl>::resolveDispatch(ResolveContext &ctx, std::array<IoVector, N> &&ioves) {
    resolveDispatch(ctx, ioves);
}

namespace policy {

template <typename Derived>
inline size_t PutInterface<Derived>::estimate(ResolveContext &ctx) {
    return Derived::estimateImpl(ctx);
}

template <typename Derived>
inline size_t PutInterface<Derived>::put(ResolveContext &ctx, char *buf) {
    char *base = buf;
    IoVector *ioves = ctx.ioves;
    size_t offset;
    for(size_t i = 0; i < ctx.count; ++i) {
        offset = Derived::putIov(buf, ioves[i], i);
        buf += offset;
        offset = (i+1!=ctx.count ? Derived::putGap(buf, i) : Derived::putLine(buf));
        buf += offset;
    }
    return buf - base;
}

inline size_t Whitespace::putIov(char *buf, IoVector &iov, size_t nth) {
    (void)nth;
    std::memcpy(buf, iov.base, iov.len);
    return iov.len;
}
inline size_t Whitespace::putGap(char *buf, size_t nth) {
    (void)nth;
    *buf = ' ';
    return 1;
}

inline size_t Whitespace::putLine(char *buf) {
    *buf = '\n';
    return 1;
}

} // policy

} // dlog
#endif
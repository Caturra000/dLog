#ifndef __DLOG_UTILS_H__
#define __DLOG_UTILS_H__
#include <bits/stdc++.h>
#include "resolve.h"
namespace dlog {

template <size_t Omit = 0>
struct NoWhitespacePolicy {
    static size_t estimate(ResolveContext &ctx);
    static size_t put(ResolveContext &ctx, char *buf);
};

/// impl

template <size_t Omit>
inline size_t NoWhitespacePolicy<Omit>::estimate(ResolveContext &ctx) {
    return ctx.total + (Omit+1 >= ctx.count ? ctx.count : Omit);
}

template <size_t Omit>
inline size_t NoWhitespacePolicy<Omit>::put(ResolveContext &ctx, char *buf) {
    size_t omitted = 0;
    IoVector *ioves = ctx.ioves;
    size_t offset = 0;
    for(size_t i = 0; i < ctx.count; ++i) {
        std::memcpy(buf + offset, ioves[i].base, ioves[i].len);
        offset += ioves[i].len;
        if(i == ctx.count-1) {
            buf[offset] = '\n';
            ++offset;
        }
        else if(omitted < Omit) {
            buf[offset] = ' ';
            ++offset;
            ++omitted;
        }
    }
    return offset;
}

} // dlog
#endif
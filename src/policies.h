#ifndef __DLOG_POLICIES_H__
#define __DLOG_POLICIES_H__
#include <bits/stdc++.h>
#include "resolve.h"
#include "simhash.h" // TODO chatty policy
namespace dlog {
namespace policy {

template <size_t Omit = 0>
struct NoWhitespace {
    static size_t estimate(ResolveContext &ctx);
    static size_t put(ResolveContext &ctx, char *buf);
};

struct ColorfulWhitespace {
    static size_t estimate(ResolveContext &ctx);
    static size_t put(ResolveContext &ctx, char *buf);
};

template <size_t Omit = 0>
struct ColorfulNoWhitespace {
    static size_t estimate(ResolveContext &ctx);
    static size_t put(ResolveContext &ctx, char *buf);
};

// see example below
struct Specialization {
    template <typename T>
    struct ExtraStream;

    template <typename T>
    static auto parse(char *buf, const T &msg, size_t length)
        -> decltype(ExtraStream<T>::parse(0, msg, 0)) {
        return ExtraStream<T>::parse(buf, msg, length);
    }

    template <typename T>
    static auto parseLength(const T &msg)
        -> decltype(ExtraStream<T>::parseLength(msg)) {
        return ExtraStream<T>::parseLength(msg);
    }
};

/// impl

template <size_t Omit>
inline size_t NoWhitespace<Omit>::estimate(ResolveContext &ctx) {
    return ctx.total + (Omit+1 >= ctx.count ? ctx.count : Omit);
}

template <size_t Omit>
inline size_t NoWhitespace<Omit>::put(ResolveContext &ctx, char *buf) {
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

inline size_t ColorfulWhitespace::estimate(ResolveContext &ctx) {
    constexpr static char before[] = "\033[31m";
    constexpr static char after[] = "\033[0m";
    return ctx.total + ctx.count*(1+ sizeof(before) + sizeof(after) - 2);
}

inline size_t ColorfulWhitespace::put(ResolveContext &ctx, char *buf) {
    constexpr static char before[][6] = {"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m"};
    constexpr static char after[] = "\033[0m";
    constexpr static size_t roll = sizeof(before) / sizeof(before[0]);
    IoVector *ioves = ctx.ioves;
    size_t offset = 0;
    for(size_t i = 0; i < ctx.count; ++i) {
        std::memcpy(buf + offset, before[i % roll], sizeof(before[0]) - 1);
        offset += sizeof(before[0]) - 1;
        std::memcpy(buf + offset, ioves[i].base, ioves[i].len);
        offset += ioves[i].len;
        std::memcpy(buf + offset, after, sizeof(after) - 1);
        offset += sizeof(after) - 1;
        if(i == ctx.count-1) buf[offset] = '\n';
        else buf[offset] = ' ';
        ++offset;
    }
    return offset;
}

template <size_t Omit>
inline size_t ColorfulNoWhitespace<Omit>::estimate(ResolveContext &ctx) {
    constexpr static char before[] = "\033[31m";
    constexpr static char after[] = "\033[0m";
    return ctx.total + (Omit+1 >= ctx.count ? ctx.count : Omit) + ctx.count * (sizeof(before) + sizeof(after) - 2);
}

template <size_t Omit>
inline size_t ColorfulNoWhitespace<Omit>::put(ResolveContext &ctx, char *buf) {
    constexpr static char before[][6] = {"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m"};
    constexpr static char after[] = "\033[0m";
    constexpr static size_t roll = sizeof(before) / sizeof(before[0]);
    size_t omitted = 0;
    IoVector *ioves = ctx.ioves;
    size_t offset = 0;
    for(size_t i = 0; i < ctx.count; ++i) {
        std::memcpy(buf + offset, before[i % roll], sizeof(before[0]) - 1);
        offset += sizeof(before[0]) - 1;
        std::memcpy(buf + offset, ioves[i].base, ioves[i].len);
        offset += ioves[i].len;
        std::memcpy(buf + offset, after, sizeof(after) - 1);
        offset += sizeof(after) - 1;
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

///////// Specialization example
//
// struct Point {
//     int x, y, z;
// };
//
// usage:  Log::debug( Point{1, 2, 3} );
// output: [1,2,3]
//
// namespace dlog {
//     template <>
//     struct Specialization::ExtraStream<Point> {
//         static void parse(char *buf, const Point &p, size_t length) {
//             int vs[] = {p.x, p.y, p.z};
//             size_t cur = 0;
//             buf[cur++] = '[';
//             for(size_t i = 0, len; i < 3; ++i) {
//                 len = StreamBase::parseLength(vs[i]);
//                 StreamBase::parse(&buf[cur], vs[i], len);
//                 cur += len;
//                 if(i != 2) buf[cur++] = ',';
//                 else buf[cur++] = ']';
//             }
//         }
//
//         static size_t parseLength(const Point &p) {
//             int vs[] = {p.x, p.y, p.z};
//             size_t len = 4;
//             for(auto v :vs) {
//                 len += StreamBase::parseLength(v);
//             }
//             return len;
//         }
//     };
// }

} // policy
} // dlog
#endif
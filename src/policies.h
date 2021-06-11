#ifndef __DLOG_POLICIES_H__
#define __DLOG_POLICIES_H__
#include <bits/stdc++.h>
#include "resolve.h"
#include "simhash.h" // TODO chatty policy
namespace dlog {
namespace policy {

template <size_t Omit = 0>
struct NoWhitespace: public PutInterface<NoWhitespace<Omit>> {
    static size_t estimateImpl(ResolveContext &ctx);
    static size_t putIov(char *buf, IoVector &iov, size_t nth);
    static size_t putGap(char *buf, size_t nth);
    static size_t putLine(char *buf);
};

// ColorfulDecorator is just a decorator
// use Colorful as policy
template <typename Decorated> // Decorated put-policy
struct ColorfulDecorator: protected Decorated {
    static size_t estimateImpl(ResolveContext &ctx);
    static size_t putIov(char *buf, IoVector &iov, size_t nth);
    static size_t putGap(char *buf, size_t nth);
    static size_t putLine(char *buf);
};

template <typename Decorated>
struct Colorful: public PutInterface< ColorfulDecorator<Decorated> > {};

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
inline size_t NoWhitespace<Omit>::estimateImpl(ResolveContext &ctx) {
    return ctx.total + (Omit+1 >= ctx.count ? ctx.count : Omit);
}

template <size_t Omit>
inline size_t NoWhitespace<Omit>::putIov(char *buf, IoVector &iov, size_t nth) {
    return Whitespace::putIov(buf, iov, nth);
}

template <size_t Omit>
inline size_t NoWhitespace<Omit>::putGap(char *buf, size_t nth) {
    return nth >= Omit ? 0 : Whitespace::putGap(buf, nth);
}

template <size_t Omit>
inline size_t NoWhitespace<Omit>::putLine(char *buf) {
    return Whitespace::putLine(buf);
}

template <typename Decorated>
inline size_t ColorfulDecorator<Decorated>::estimateImpl(ResolveContext &ctx) {
    constexpr static char before[] = "\033[31m";
    constexpr static char after[] = "\033[0m";
    return Decorated::estimateImpl(ctx) + ctx.count*(sizeof(before) + sizeof(after) - 2);
}

template <typename Decorated>
inline size_t ColorfulDecorator<Decorated>::putIov(char *buf, IoVector &iov, size_t nth) {
    constexpr static char before[][6] = {"\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m", "\033[36m"};
    constexpr static char after[] = "\033[0m";
    constexpr static size_t roll = sizeof(before) / sizeof(before[0]);

    size_t offset;
    char *base = buf;

    std::memcpy(buf, before[nth % roll], sizeof(before[0]) - 1);
    offset = sizeof(before[0]) - 1;
    buf += offset;
    offset = Decorated::putIov(buf, iov, nth);
    buf += offset;
    std::memcpy(buf, after, sizeof(after) - 1);
    buf += sizeof(after) - 1;

    return buf - base;
}

template <typename Decorated>
inline size_t ColorfulDecorator<Decorated>::putGap(char *buf, size_t nth) {
    return Decorated::putGap(buf, nth);
}

template <typename Decorated>
inline size_t ColorfulDecorator<Decorated>::putLine(char *buf) {
    return Decorated::putLine(buf);
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
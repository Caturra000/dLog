#ifndef __DLOG_TAGS_H__
#define __DLOG_TAGS_H__
#include <bits/stdc++.h>
#include "chrono.h"
#include "io.h"
#include "tid.h"
#include "level.h"
#include "msort.h"
namespace dlog {

struct DateTimeTag {
    static std::array<IoVector, 2> format() {
        return Chrono::format(Chrono::now());
    }
};

struct ThreadIdTag {
    static IoVector format() {
        return Tid::format();
    }
};

template <LogLevel LEVEL>
struct LogLevelTag {
    static char format() {
        return levelFormat<LEVEL>();
    }
};

namespace meta {

template <typename V>
struct Elem;

template <>
struct Elem<DateTimeTag>: Key<DateTimeTag>, Value<1> {};

template <LogLevel LEVEL>
struct Elem<LogLevelTag<LEVEL>>: Key<LogLevelTag<LEVEL>>, Value<2> {};

template <>
struct Elem<ThreadIdTag>: Key<ThreadIdTag>, Value<3> {};

} // meta
} // dlog
#endif
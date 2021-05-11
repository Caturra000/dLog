#ifndef __DLOG_TAGS_H__
#define __DLOG_TAGS_H__
#include <bits/stdc++.h>
#include "chrono.h"
#include "io.h"
#include "tid.h"
#include "sort.h"
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


template <typename T>
struct TagResolver {
    static decltype(auto) format() {
        return T::format();
    }
};

// template <typename ...Ts>
// struct TagsResolver;

// template <typename ...Ts>
// struct TagsResolver<std::tuple<Ts...>> {
//     static std::array<IoVector, sizeof...(Ts)> format() {
//         return { TagResolver<Ts>::format()... };
//     }
// };


template <typename V>
struct Elem;

template <>
struct Elem<DateTimeTag>: Key<DateTimeTag>, Value<1> {};

template <>
struct Elem<ThreadIdTag>: Key<ThreadIdTag>, Value<2> {};

} // dlog
#endif
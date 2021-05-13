#ifndef __DLOG_TAGS_H__
#define __DLOG_TAGS_H__
#include <bits/stdc++.h>
#include "chrono.h"
#include "io.h"
#include "tid.h"
#include "level.h"
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

template <LogLevel LEVEL>
struct LogLevelTag {
    static char format() {
        return levelFormat<LEVEL>();
    }
};

struct LogLevelTagPlaceHolder {};

/// tuple helper

template <size_t I, typename T, typename ...Ts>
struct TupleResolver;

template <typename T, typename ...Ts>
struct TupleResolver<0, std::tuple<T, Ts...>> {
    using type = T;
};

template <size_t I, typename T, typename ...Ts>
struct TupleResolver<I, std::tuple<T, Ts...>> {
    using type = typename TupleResolver<I-1, std::tuple<Ts...>>::type;
};




template <size_t I, typename ...Ts>
struct TagResolver;

// return static method 'format' of i_th type of tuple
template <size_t I, typename ...Ts>
struct TagResolver<I, std::tuple<Ts...>> {
    using type = typename TupleResolver<I, std::tuple<Ts...>>::type;
    static decltype(auto) format() {
        return type::format();
    }
};

template <typename ...Ts>
struct TagsResolver;

// TODO: simple interface
template <typename ...Ts, typename ...PlaceHolders, typename PlaceHolder>
struct TagsResolver<std::tuple<Ts...>, PlaceHolder, std::tuple<PlaceHolders...> > {
    static decltype(auto) format() {
        constexpr auto pos = Find<PlaceHolder, PlaceHolders...>::value; // 0based
        using Nth = typename TagResolver<pos, std::tuple<Ts...>>::type;
        return Nth::format();
    }
};

template <typename V>
struct Elem;

template <>
struct Elem<DateTimeTag>: Key<DateTimeTag>, Value<1> {};

template <LogLevel LEVEL>
struct Elem<LogLevelTag<LEVEL>>: Key<LogLevelTag<LEVEL>>, Value<2> {};

template <>
struct Elem<ThreadIdTag>: Key<ThreadIdTag>, Value<3> {};

template <>
struct Elem<LogLevelTagPlaceHolder>: Key<LogLevelTagPlaceHolder>, Value<1024> {};


} // dlog
#endif
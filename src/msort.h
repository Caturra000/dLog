#ifndef __DLOG_MSORT_H__
#define __DLOG_MSORT_H__
#include <bits/stdc++.h>
namespace dlog {
namespace meta {

/// interface

template <typename ...Ts>
struct Sort;

template <typename T, typename U, typename ...Ts>
struct Find;

template <ssize_t L, ssize_t R, typename ...Ts>
struct Select;

template <typename ...Ts>
struct Min;

template <typename T, typename ...Ts>
struct MinValue;

template <typename T, typename ...Ts>
struct MinType;

template <typename ...Ts>
struct Concat;

template <typename V>
struct Elem;
template <typename K>
struct Key;
template <ssize_t V>
struct Value;

/// sub-interface

template <typename T, typename ...Ts>
struct Sort1;

template <ssize_t MinPos, ssize_t MaxIdx, typename T, typename ...Ts>
struct Sort2;

template <ssize_t Cur, typename T, typename U, typename ...Ts>
struct Find1;

template <ssize_t Start, ssize_t End, ssize_t Cur, typename T, typename ...Ts>
struct Select1;

template <ssize_t N, typename T, typename ...Ts>
struct Select2;

/// impl

/// KV

template <ssize_t V>
struct Value {
    static constexpr ssize_t value = V;
};
template <typename K>
struct Key {
    using type = K;
};
template <typename V>
struct Elem;


// DEMO: sort comp register
// template <>
// struct Elem<int>: Key<int>, Value<1> {};
// template <>
// struct Elem<double>: Key<double>, Value<2> {};
// template <>
// struct Elem<char>: Key<char>, Value<3> {};
// template <>
// struct Elem<float>: Key<float>, Value<4> {};
// template <>
// struct Elem<unsigned>: Key<unsigned>, Value<5> {};

/// Concat

template <typename ...Ts>
struct Concat;

// input: Concat< std::tuple<L....>, std::tuple<R....> >
// output: type -> std::tuple<L..., R...>
template <typename ...L, typename ...R>
struct Concat<std::tuple<L...>, std::tuple<R...>> {
    using type = std::tuple<L..., R...>;
};
// std::tuple<T> => type->std::tuple<T>
template <typename T>
struct Concat<std::tuple<T>> {
    using type = std::tuple<T>;
};

/// Min

template <typename ...Ts>
struct Min {
    constexpr static ssize_t value = MinValue<Ts...>::value;
    using type = typename MinType<Ts...>::type;
};

template <typename T, typename ...Ts>
struct MinValue {
    constexpr static ssize_t value = std::min(Elem<T>::value, MinValue<Ts...>::value);
};
template <typename T>
struct MinValue<T> {
    constexpr static ssize_t value = Elem<T>::value;
};

template <typename T, typename ...Ts>
struct MinType {
    using type = std::conditional_t<
        /*if  */Elem<T>::value <= MinValue<Ts...>::value, 
        /*then*/T,
        /*else*/typename MinType<Ts...>::type>;
};
template <typename T>
struct MinType<T> {
    using type = T;
};

/// Select

template <ssize_t L, ssize_t R, typename ...Ts>
struct Select {
    using type = typename Select1<L, R, 0, Ts...>::type;
    using check = std::enable_if_t<L <= R && L >= 0 && R <= sizeof...(Ts)>;
};

// not equal
template <ssize_t Start, ssize_t End, ssize_t Cur, typename T, typename ...Ts>
struct Select1 {
    using type = typename Select1<Start, End, Cur+1, Ts...>::type;
};

// start == cur
template <ssize_t Start, ssize_t End, typename T, typename ...Ts>
struct Select1<Start, End, Start, T, Ts...> {
    using type = typename Select2<End-Start+1, T, Ts...>::type; // MOCK
};

template <ssize_t N, typename T, typename ...Ts>
struct Select2 {
    using type = typename Concat<std::tuple<T>, typename Select2<N-1, Ts...>::type >::type;
};
template <typename T, typename ...Ts>
struct Select2<0, T, Ts...>;
template <typename T, typename ...Ts>
struct Select2<1, T, Ts...> {
    using type = std::tuple<T>;
};
template <typename T>
struct Select2<1, T> {
    using type = std::tuple<T>;
};

/// Find

template <typename T, typename U, typename ...Ts>
struct Find {
    constexpr static ssize_t value = Find1<0, T, U, Ts...>::value;
};

template <ssize_t Cur, typename T, typename U, typename ...Ts>
struct Find1 {
    constexpr static ssize_t value = Find1<Cur+1, T, Ts...>::value;
};
template <ssize_t Cur, typename T, typename ...Ts>
struct Find1<Cur, T, T, Ts...> {
    constexpr static ssize_t value = Cur;
};

/// Sort

template <typename ...Ts>
struct Sort {
    using type = typename Sort1<std::tuple<Ts...>>::type;
};

template <typename T, typename ...Ts>
struct Sort1;

template <typename T>
struct Sort1<std::tuple<T>> {
    using type = std::tuple<T>;
};

template <typename T, typename ...Ts>
struct Sort1<std::tuple<T, Ts...>> {
    using type = typename Sort2<
        Find<typename Min<T, Ts...>::type, T, Ts...>::value,
        sizeof...(Ts),
        T, Ts...
    >::type;
};

// minpos mid
// no std::tuple
template <ssize_t MinPos, ssize_t MaxIdx, typename T, typename ...Ts>
struct Sort2 {
    using check = std::enable_if_t<MinPos+1 <= MaxIdx && MinPos-1 >= 0>;
    using type = typename Concat<
        typename Select<MinPos, MinPos, T, Ts...>::type,
        typename Sort1<
            typename Concat<
                typename Select<0, MinPos-1, T, Ts...>::type,
                typename Select<MinPos+1, MaxIdx, T, Ts...>::type
            >::type
        >::type
    >::type;
};

// minpos == 0
template <ssize_t MaxIdx, typename T, typename ...Ts>
struct Sort2<0, MaxIdx, T, Ts...> {
    using type = typename Concat<
        std::tuple<T>,
        typename Sort1<typename Select<1, MaxIdx, T, Ts...>::type>::type
    >::type;
};

// minpos == end
template <ssize_t MaxIdx, typename T, typename ...Ts>
struct Sort2<MaxIdx, MaxIdx, T, Ts...> {
    using type = typename Concat<
        typename Select<MaxIdx, MaxIdx, T, Ts...>::type,
        typename Sort1<typename Select<0, MaxIdx-1, T, Ts...>::type>::type
    >::type;
};

} // meta
} // dlog
#endif
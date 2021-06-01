#ifndef __DLOG__MSTR_H__
#define __DLOG__MSTR_H__
#include <bits/stdc++.h>
namespace dlog {
namespace meta {

// example: MetaString<'a', 'b', 'c'>::buf = "abc" ('\0')
template <char ...Chars>
struct MetaString {
    constexpr static char buf[sizeof...(Chars)] = { Chars... };
};

template <char ...Chars> constexpr char MetaString<Chars...>::buf[sizeof...(Chars)];

template <typename ...Ts>
struct Concat;

template <char ...L, char ...R>
struct Concat<MetaString<L...>, MetaString<R...>> {
    using type = MetaString<L..., R...>;
};

template <char ...Chars>
struct Concat<MetaString<Chars...>> {
    using type = MetaString<Chars...>;
};

// example: ContinuousMetaString<3, 'a'>::type = MetaString<'a', 'a', 'a'>
template <size_t N, char C>
struct ContinuousMetaString {
    using type = typename Concat<
        MetaString<C>,
        typename ContinuousMetaString<N-1, C>::type
    >::type;
};

template <char C>
struct ContinuousMetaString<1, C> {
    using type = MetaString<C, '\0'>;
};

template <char C>
struct ContinuousMetaString<0, C>;

// example: Whitespace<5>::buf = "     "
template <size_t N>
using Whitespace = typename ContinuousMetaString<N, ' '>::type;


/// numeric

constexpr inline int abs(int I) { return I < 0 ? abs(-I) : I; }
constexpr inline size_t length(int I) { return I < 0 ? 1 + length(-I) : I < 10 ? 1 : 1 + length(I / 10); }

template<size_t N, int I, char... Chars>
struct NumericMetaStringBuilder {
    using type = typename NumericMetaStringBuilder<N - 1, I / 10, '0' + abs(I) % 10, Chars...>::type;
};
template<int I, char... Chars>
struct NumericMetaStringBuilder<2, I, Chars...> {
    using type = MetaString<I < 0 ? '-' : '0' + I / 10, '0' + abs(I) % 10, Chars...>;
};
template<int I, char... Chars>
struct NumericMetaStringBuilder<1, I, Chars...> {
    using type = MetaString<'0' + I, Chars...>;
};

// example: Numeric<123>::type = MetaString<'1', '2', '3', '\0'>
template<int I>
struct Numeric {
    using type = typename NumericMetaStringBuilder<length(I), I, '\0'>::type;
};

template <int I>
using NumericMetaString = typename Numeric<I>::type;

template <typename>
struct NumericMetaStrings;

template <size_t ...Is>
struct NumericMetaStrings<std::index_sequence<Is...>> {
    constexpr const static char *bufs[] = { NumericMetaString<Is>::buf... };
    constexpr const static size_t len[] = { length(Is)... };
};

template <size_t ...Is> constexpr const char *NumericMetaStrings<std::index_sequence<Is...>>::bufs[];
template <size_t ...Is> constexpr const size_t NumericMetaStrings<std::index_sequence<Is...>>::len[];

// example: NumericMetaStringsSequence<3>::bufs[] = {"0", "1", "2"};
template <size_t N>
struct NumericMetaStringsSequence {
    constexpr static auto bufs = NumericMetaStrings<std::make_index_sequence<N>>::bufs;
    constexpr static auto len = NumericMetaStrings<std::make_index_sequence<N>>::len;
};

} // meta
} // dlog
#endif
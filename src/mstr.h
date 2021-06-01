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
template <size_t N, char C, bool Eof = true>
struct Continuous {
    using type = typename Concat<
        MetaString<C>,
        typename Continuous<N-1, C, Eof>::type
    >::type;
};

template <char C>
struct Continuous<1, C> {
    using type = MetaString<C, '\0'>;
};

template <char C>
struct Continuous<1, C, false> {
    using type = MetaString<C>;
};

template <char C>
struct Continuous<0, C> {
    using type = MetaString<'\0'>;
};

template <char C>
struct Continuous<0, C, false> {
    using type = MetaString<>;
};

template <size_t N, char C, bool Eof = true>
using ContinuousMetaString = typename Continuous<N, C, Eof>::type;

// example: Whitespace<5>::buf = "     "
template <size_t N>
using Whitespace = ContinuousMetaString<N, ' '>;


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

// example: LeadingZeroNumeric<123, 5>::type = MetaString<'0', '0', '1', '2', '3', '\0'>
template <int I, size_t P, char Z = '0'> // P(prefix) >= length(I)
struct LeadingZeroNumeric {
    constexpr static size_t len = length(I);
    using check = std::enable_if_t<P >= len && I >= 0>;
    using ztype = ContinuousMetaString<P-len, Z, false>;
    using ntype = NumericMetaString<I>;
    using type = typename Concat<ztype, ntype>::type; // MetaString
};

template <int I, size_t P, char Z = '0'>
using LeadingZeroNumericMetaString = typename LeadingZeroNumeric<I, P, Z>::type;

} // meta
} // dlog
#endif
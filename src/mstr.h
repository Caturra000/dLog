#ifndef __DLOG__MSTR_H__
#define __DLOG__MSTR_H__
#include <bits/stdc++.h>
#include "mseq.h"
namespace dlog {
namespace meta {

// example: MetaString<'a', 'b', 'c'>::buf = "abc" ('\0')
template <char ...Chars>
struct MetaString {
    constexpr static char buf[sizeof...(Chars)] = { Chars... };
};

template <char ...Chars> constexpr char MetaString<Chars...>::buf[sizeof...(Chars)];

template <typename MStr, size_t P>
struct MetaStringArray;

template <char ...Chars, size_t N>
struct MetaStringArray<MetaString<Chars...>, N> {
    constexpr static const char buf[][N] = { Chars... };
    using type = decltype(buf); // buf type
};

template <char ...Chars, size_t N>
constexpr const char MetaStringArray<MetaString<Chars...>, N>::buf[][N];

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

template <size_t ...Is>
struct LeadingZeroNumericArrayBuilderImplSequence;

template <size_t P, size_t I, size_t ...Is>
struct LeadingZeroNumericArrayBuilderImplSequence<P, I, Is...> {
    using type = typename Concat<
        LeadingZeroNumericMetaString<I, P>,
        typename LeadingZeroNumericArrayBuilderImplSequence<P, Is...>::type
    >::type;
};

template <size_t P, size_t I>
struct LeadingZeroNumericArrayBuilderImplSequence<P, I> {
    using type = LeadingZeroNumericMetaString<I, P>;
};

template <typename, size_t>
struct LeadingZeroNumericArrayBuilderImpl;

template <size_t P, size_t ...Is>
struct LeadingZeroNumericArrayBuilderImpl<std::index_sequence<Is...>, P> {
    // type -> MetaString
    using type = typename LeadingZeroNumericArrayBuilderImplSequence<P, Is...>::type;
};

// solve ftemplate-depth limit
template <size_t Begin, size_t End, size_t P, bool Flag>
struct LeadingZeroNumericArrayBuilderRange {
    using type = typename LeadingZeroNumericArrayBuilderImpl<typename MakeSequence<Begin, End>::type, P>::type;
};

template <size_t Begin, size_t End, size_t P>
struct LeadingZeroNumericArrayBuilderRange<Begin, End, P, false> {
    using type = typename Concat<
        typename LeadingZeroNumericArrayBuilderRange<Begin, Begin+500, P, true>::type,
        typename LeadingZeroNumericArrayBuilderRange<Begin+500, End, P, (End-Begin<=1000)>::type
    >::type;
};

template <size_t N, size_t P>
struct LeadingZeroNumericArrayBuilder {
    using check = std::enable_if_t<N && P >= length(N-1)>;
    // type -> MetaString
    using type = typename LeadingZeroNumericArrayBuilderRange<0, N, P, (N<=500)>::type;
};

// example: LeadingZeroNumericArray<12, 3>::buf = { "000", "001", "002", ... "010", "011" } (const char[12][4])
template <size_t N, size_t P>
using LeadingZeroNumericArray = MetaStringArray<typename LeadingZeroNumericArrayBuilder<N, P>::type ,P+1>;

} // meta
} // dlog
#endif
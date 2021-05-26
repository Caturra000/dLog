#ifndef __DLOG__MSTR_H__
#define __DLOG__MSTR_H__
#include <bits/stdc++.h>
namespace dlog {

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

} // dlog
#endif
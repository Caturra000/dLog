#ifndef __DLOG_MSEQ_H__
#define __DLOG_MSEQ_H__
#include <bits/stdc++.h>
namespace dlog {
namespace meta {

template <size_t, typename>
struct OffsetSequence;

template <size_t Offset, size_t ...Is>
struct OffsetSequence<Offset, std::index_sequence<Is...>> {
    using type = std::index_sequence<Is+Offset...>;
};

// example: MakeSequence<12, 15>::type = std::index_sequence<12, 13, 14>
template <size_t Begin, size_t End>
struct MakeSequence {
    using type = typename OffsetSequence<Begin, std::make_index_sequence<End - Begin>>::type;
};

} // meta
} // dlog
#endif
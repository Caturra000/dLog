#ifndef __DLOG_FILENAME_H__
#define __DLOG_FILENAME_H__
#include <bits/stdc++.h>
#include "io.h"
namespace dlog {

template <size_t N>
inline constexpr size_t lastPathComponentPos(const char (&path)[N]) {
    static_assert(N > 1, "file path length N should be greater than 1.");
    for(size_t i = N-1; i; --i) {
        if(path[i] == '/') return i;
    }
    return 0;
}

template <size_t N>
inline constexpr size_t lastPathComponentSize(const char (&path)[N]) {
    auto pos = lastPathComponentPos(path);
    return N - pos - 1;
}

template <size_t N>
inline constexpr IoVector filename(const char (&fullPath)[N]) {
    auto pos = lastPathComponentPos(fullPath);
    return {fullPath + pos, lastPathComponentSize(fullPath)};
}

} // dlog
#endif
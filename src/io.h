#ifndef __DLOG_IO_H__
#define __DLOG_IO_H__
#include <bits/stdc++.h>
namespace dlog {

struct IoVector {
    const char *base; // base address
    size_t len; // [base, base + len)
};

} // dlog
#endif
#ifndef __DLOG_SHARED_H__
#define __DLOG_SHARED_H__
#include <bits/stdc++.h>
namespace dlog {

// shared mutable variables
// protected by namespace

constexpr size_t DLOG_BUFSIZE = 1<<20;
char buf[2][DLOG_BUFSIZE];
size_t ridx = 0, rcur = 0;
size_t widx = 1, wcur = 0; 

} // dlog
#endif
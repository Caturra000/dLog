#ifndef __DLOG_SHARED_H__
#define __DLOG_SHARED_H__
#include <bits/stdc++.h>
namespace dlog {

// shared mutable variables
// protected by namespace

constexpr size_t DLOG_BUFSIZE = 233;
char buf[2][DLOG_BUFSIZE];
int ridx = 0, rcur = 0;
int widx = 1, wcur = 0;

} // dlog
#endif
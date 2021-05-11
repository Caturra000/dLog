#ifndef __DLOG_LEVEL_H__
#define __DLOG_LEVEL_H__
#include <bits/stdc++.h>
namespace dlog {

enum LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    WTF,

    LOG_LEVEL_LIMIT
};

template <LogLevel LEVEL>
static constexpr char levelFormat() noexcept {
    static_assert(LEVEL >= 0 && LEVEL < LOG_LEVEL_LIMIT, "check log level config.");
    return "DIWE?"[LEVEL];
}

} // dlog
#endif
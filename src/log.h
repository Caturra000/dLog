#ifndef __DLOG_LOG_H__
#define __DLOG_LOG_H__
#include <bits/stdc++.h>
#include "config.h"
#include "io.h"
#include "resolve.h"
#include "sched.h"
#include "level.h"
#include "tags.h"
#include "msort.h"
#include "mstr.h"
#include "macro.h"
#include "filename.h"
namespace dlog {

template <typename ...Tags>
class LogBase;

using Log = LogBase<DateTimeTag, ThreadIdTag>;

template <typename ...Tags>
class LogBase {
public:
    static void init() { worker(); }
    static void done() { worker().kill(); }
    static Wthread& worker();

    template <typename ...Ts>
    static void debug(Ts &&...msg);

    template <typename ...Ts>
    static void info(Ts &&...msg);

    template <typename ...Ts>
    static void warn(Ts &&...msg);

    template <typename ...Ts>
    static void error(Ts &&...msg);

    template <typename ...Ts>
    static void wtf(Ts &&...msg);

private:
    template <LogLevel Level, typename ...Ts>
    static void logFormat(Ts &&...msg);

};

template <typename ...Tags>
struct LogBaseFacade;

template <typename ...Tags>
struct LogBaseFacade<std::tuple<Tags...>> {
    template <typename ...Ts>
    static void log(Ts &&...msg);
};

struct LogBaseImpl {
    template <typename ...Ts>
    static void log(Ts &&...msg);
};

/// impl

template <typename ...Tags>
inline Wthread& LogBase<Tags...>::worker() {
    static Wthread wthread;
    return wthread;
}

template <typename ...Tags>
template <typename ...Ts>
inline void LogBase<Tags...>::debug(Ts &&...msg) {
    if /*constexpr*/ (staticConfig.debugOn) logFormat<LogLevel::DEBUG>(std::forward<Ts>(msg)...);
}

template <typename ...Tags>
template <typename ...Ts>
inline void LogBase<Tags...>::info(Ts &&...msg) {
    if(staticConfig.infoOn) logFormat<LogLevel::INFO>(std::forward<Ts>(msg)...);
}

template <typename ...Tags>
template <typename ...Ts>
inline void LogBase<Tags...>::warn(Ts &&...msg) {
    if(staticConfig.warnOn) logFormat<LogLevel::WARN>(std::forward<Ts>(msg)...);
}

template <typename ...Tags>
template <typename ...Ts>
inline void LogBase<Tags...>::error(Ts &&...msg) {
    if(staticConfig.errorOn) logFormat<LogLevel::ERROR>(std::forward<Ts>(msg)...);
}

template <typename ...Tags>
template <typename ...Ts>
inline void LogBase<Tags...>::wtf(Ts &&...msg) {
    if(staticConfig.wtfOn) logFormat<LogLevel::WTF>(std::forward<Ts>(msg)...);
}

template <typename T> inline constexpr size_t bufcnt(T &&) { return StreamTraits<T>::size; }
template <typename T, typename ...Ts> inline constexpr size_t bufcnt(T &&, Ts &&...others) {
    return StreamTraits<T>::size + bufcnt(std::forward<Ts>(others)...);
}

template <typename T> inline constexpr size_t strcnt(T&&) { return 0; }
template <size_t N> inline constexpr size_t strcnt(const char (&)[N]) { return 1; }
template <typename T, typename ...Ts> inline constexpr size_t strcnt(T &&t, Ts &&...ts) {
    return strcnt(std::forward<T>(t)) + strcnt(std::forward<Ts>(ts)...);
}

template <typename T> inline constexpr size_t iovcnt(T&&) { return 1; }
template <size_t N> inline constexpr size_t iovcnt(const std::array<IoVector, N>&) { return N; }
template <size_t N> inline constexpr size_t iovcnt(std::array<IoVector, N>&) { return N; }
template <size_t N> inline constexpr size_t iovcnt(std::array<IoVector, N>&&) { return N; }
template <typename T, typename ...Ts> inline constexpr size_t iovcnt(T &&t, Ts &&...ts) {
    return iovcnt(std::forward<T>(t)) + iovcnt(std::forward<Ts>(ts)...);
}

template <typename ...Tags>
template <LogLevel LEVEL, typename ...Ts>
inline void LogBase<Tags...>::logFormat(Ts &&...msg) {
    using SortedTagsTuple = typename meta::Sort<LogLevelTag<LEVEL>, Tags...>::type;
    LogBaseFacade<SortedTagsTuple>::log(std::forward<Ts>(msg)...);
}

template <typename ...Tags>
template <typename ...Ts>
inline void LogBaseFacade<std::tuple<Tags...>>::log(Ts &&...msg) {
    LogBaseImpl::log(Tags::format()..., std::forward<Ts>(msg)...);
}

template <typename ...Ts>
inline void LogBaseImpl::log(Ts &&...msg) {
    char tmp[bufcnt(msg...)];
    const char *tmpref[strcnt(msg...)]; // an array stores char_ptr
    IoVector ioves[iovcnt(msg...)];
    ResolveContext args {
        .local = tmp,
        .cur = 0,
        .ioves = ioves,
        .count = 0,
        .total = 0
    };
    Resolver::resolve(args, std::forward<Ts>(msg)...);
    Scheduler::apply(args);
}

} // dlog
#endif
#ifndef __DLOG_STREAM_H__
#define __DLOG_STREAM_H__
#include <bits/stdc++.h>
#include "mixin.h"
namespace dlog {

template <size_t N = 1024> struct StreamTraitsBase { static constexpr size_t size = N; };
template <typename T> struct StreamTraits: public StreamTraitsBase<sizeof(T) + 10> {};
template <> struct StreamTraits<int>: public StreamTraitsBase<12> {}; // std::log10(INT_MAX)+1 + 1
template <> struct StreamTraits<long>: public StreamTraitsBase<21> {};
template <> struct StreamTraits<long long>: public StreamTraitsBase<21> {};
template <> struct StreamTraits<double>: public StreamTraitsBase<42> {};
template <> struct StreamTraits<char>: public StreamTraitsBase<2> {};
template <> struct StreamTraits<char*>: public StreamTraitsBase<> {};
template <> struct StreamTraits<std::string>: public StreamTraitsBase<> {};
template <size_t N> struct StreamTraits<const char[N]>: public StreamTraitsBase<N> {};

struct StreamBase {
    template <typename T>
    using VoidIfInt = std::enable_if_t<std::is_integral<T>::value>;
    template <typename T>
    using SizeTypeIfInt = std::enable_if_t<std::is_integral<T>::value, size_t>;

    template <typename T> static VoidIfInt<T> parse(char *buf, T msg, size_t length);
    template <size_t N> static void parse(char *buf, const char (&msg)[N], size_t length);
    static void parse(char *buf, const char *msg, size_t length);
    static void parse(char *buf, double msg, size_t length);
    static void parse(char *buf, char msg, size_t length);
    static void parse(char *buf, const std::string &str, size_t length);

    template <typename T> static constexpr SizeTypeIfInt<T> parseLength(T val);
    template <size_t N> static constexpr size_t parseLength(const char (&str)[N]) { return N-1; }
    static size_t parseLength(const char *str) { return std::strlen(str); }
    static size_t parseLength(double val);
    static constexpr size_t parseLength(char ch) { return 1; }
    static size_t parseLength(const std::string &str) { return str.length(); }
};

/// ext

template <typename ...Policies>
struct StreamExtend: public StreamBase {
    using Policy = meta::Mixin<Policies...>;

    template <typename ...Args, typename B = StreamBase> // "B = StreamBase" makes SFINAE possible
    static auto parse(Args &&...args)
        -> decltype(B::parse(std::forward<Args>(args)...)) { // dont use decltype(auto), cannot SFINAE
        return B::parse(std::forward<Args>(args)...);
    }

    template <typename ...Args, typename P = Policy, typename = P> // lower priority
    static auto parse(Args &&...args)
        -> decltype(P::parse(std::forward<Args>(args)...)) {
        return P::parse(std::forward<Args>(args)...);
    }

    template <typename ...Args, typename B = StreamBase>
    static auto parseLength(Args &&...args)
        -> decltype(B::parseLength(std::forward<Args>(args)...)) {
        return B::parseLength(std::forward<Args>(args)...);
    }

    template <typename ...Args, typename P = Policy, typename = P>
    static auto parseLength(Args &&...args)
        -> decltype(P::parseLength(std::forward<Args>(args)...)) {
        return P::parseLength(std::forward<Args>(args)...);
    }
};

/// impl

template <typename T>
inline StreamBase::VoidIfInt<T>
StreamBase::parse(char *buf, T msg, size_t length) {
    int cur = length-1;
    if(msg < 0) {
        buf[0] = '-';
        msg = -msg;
    } else if(msg == 0) {
        buf[0] = '0';
        return;
    }
    while(msg) {
        char v = (msg % 10) + '0';
        buf[cur--] = v;
        msg /= 10;
    }
}

template <size_t N>
inline void StreamBase::parse(char *buf, const char (&msg)[N], size_t length) {
    parse(buf, (char*)msg, length);
}

inline void StreamBase::parse(char *buf, const char *msg, size_t length) {
    std::memcpy(buf, msg, length);
}

inline void StreamBase::parse(char *buf, double msg, size_t length) {
    if(msg > 0) {
        long ival = msg;
        size_t ilen = parseLength(ival);
        parse(buf, ival, ilen);
        buf += ilen;
        msg -= ival;
        length -= ilen + 1;
        int cur = 0;
        buf[cur++] = '.';
        constexpr double EPS = 1e-12;
        int limit = 5, v;
        do {
            msg *= 10;
            v = msg;
            buf[cur++] = v | 48;
            msg -= v;
        } while(msg > EPS && limit--);
    } else {
        *buf++ = '-';
        parse(buf, -msg, length-1);
    }
}

inline void StreamBase::parse(char *buf, char msg, size_t) {
    buf[0] = msg;
}

inline void StreamBase::parse(char *buf, const std::string &str, size_t length) {
    parse(buf, str.c_str(), length);
}


template <typename T>
inline constexpr StreamBase::SizeTypeIfInt<T>
StreamBase::parseLength(T val) {
    if(val >= 0) {
        if(val == 0)
            return 1;
        if(val >= 1 && val <= 9)
            return 1;
        if(val >= 10 && val <= 99)
            return 2;
        if(val >= 100 && val <= 999)
            return 3;
        if(val >= 1000 && val <= 9999)
            return 4;
        if(val >= 10000 && val <= 99999)
            return 5;
        if(val >= 100000 && val <= 999999)
            return 6;
        if(val >= 1000000 && val <= 9999999)
            return 7;
        if(val >= 10000000 && val <= 99999999)
            return 8;
        if(val >= 100000000 && val <= 999999999)
            return 9;
        if(val >= 1000000000 && val <= 9999999999L)
            return 10;
        return 10 + parseLength(val / 10000000000);
    }
    return 1 + parseLength(-val);
}

inline size_t StreamBase::parseLength(double val) {
    if(val > 0) {
        long ival = val;
        size_t len = parseLength(ival) + 1; // '.'
        val -= ival;
        constexpr double EPS = 1e-12;
        int limit = 5;
        do {
            val *= 10;
            ival = val;
            val -= ival;
            len++;
        } while(val > EPS && limit--);
        return len;
    }
    return 1 + parseLength(-val);
}

} // dlog
#endif
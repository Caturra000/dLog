#include <bits/stdc++.h>
#include "table.h"
// test-version

namespace dlog {

// shared mutable variables
// protected by namespace

constexpr size_t DLOG_BUFSIZE = 233;
char buf[2][DLOG_BUFSIZE];
int ridx = 0, rcur = 0;
int widx = 1, wcur = 0;

struct LogBase {

    // Scheduler
    // Stream
    
    template <typename T>
    static void d(T &&msg);

    template <typename T, typename ...Ts>
    static void d(T &&msg, Ts &&...others);

    template <typename T>
    static void i(T &&msg);

    template <typename T, typename ...Ts>
    static void i(T &&msg, Ts &&...others);

    template <typename ...Ts>
    static void info(Ts &&...msg);

};


template <size_t N = 1024> struct StreamTraitsBase { static constexpr size_t size = N; };
template <typename T> struct StreamTraits: public StreamTraitsBase<> {};
template <> struct StreamTraits<int>: public StreamTraitsBase<12> {}; // std::log10(INT_MAX)+1 + 1
template <> struct StreamTraits<long>: public StreamTraitsBase<21> {};
template <> struct StreamTraits<long long>: public StreamTraitsBase<21> {};
template <> struct StreamTraits<double>: public StreamTraitsBase<42> {};
template <> struct StreamTraits<char>: public StreamTraitsBase<2> {};
template <size_t N> struct StreamTraits<const char(&)[N]>: public StreamTraitsBase<N> {};
template <size_t N> struct StreamTraits<const char[N]>: public StreamTraitsBase<N> {};
// use rvalue and perfect forward to support const char[]
template <typename T> inline constexpr size_t bufcnt(T &&) { return StreamTraits<T>::size; }
template <typename T, typename ...Ts> inline constexpr size_t bufcnt(T &&, Ts &&...others) {
    return StreamTraits<T>::size + bufcnt(std::forward<Ts>(others)...);
}

template <typename T> inline constexpr size_t strcnt(T&&) { return 0; }
template <size_t N> inline constexpr size_t strcnt(const char (&)[N]) { return 1; }
template <typename T, typename ...Ts> inline constexpr size_t strcnt(T &&t, Ts &&...ts) {
    return strcnt(std::forward<T>(t)) + strcnt(std::forward<Ts>(ts)...); 
}


template <typename T>
struct ExtraStream;

struct Stream {
    template <typename T,
    typename = std::enable_if_t<std::is_integral<T>::value>>
    static void parse(char *buf, T msg, size_t length);

    static void parse(char *buf, const char *msg, size_t length);

    template <size_t N>
    static void parse(char *buf, const char (&msg)[N], size_t length);

    static void parse(char *buf, double msg, size_t length);

    static void parse(char *buf, char msg, size_t length);

    static void parse(char *buf, const std::string &str, size_t length);

    template <typename T,
    typename = std::enable_if_t<std::is_integral<T>::value>>
    static size_t parseLength(T val);

    template <size_t N>
    static size_t parseLength(const char (&str)[N]) { return N-1; }

    static size_t parseLength(const char *str) { return strlen(str); }

    static size_t parseLength(double val);

    static constexpr size_t parseLength(char ch) { return 1; }

    static size_t parseLength(const std::string &str) { return str.length(); }

    template <typename T>
    static void parse(T &whatever) {
        // 这里通过ExtraStream<>来扩容，且不用为基础类也分出一堆的Stream struct
        ExtraStream<T>::parse(whatever);
    }
};

std::mutex rmtx, wmtx;
std::condition_variable rcond, wcond;
using namespace std::literals::chrono_literals;
using Timestamp = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;
std::once_flag cflag;
Timestamp checkpoint;

// 确定该用哪个buffer，不用从每个Stream.from()接口都传入固定的buf
// 并且提供阻塞（当r/w失衡时）
struct Scheduler {

    

    // brief: start a write-log thread
    // usage: Scheduler::start().detach()
    // TODO stop it manually
    static std::thread start() {
        std::call_once(cflag, [] { checkpoint = std::chrono::system_clock::now(); });
        return std::thread {[]() mutable {
            while(true) {
                {
                    std::unique_lock<std::mutex> lk{wmtx};
                    wcond.wait(lk, [] { return wcur != 0; });
                    // write to file
                    // fake
                    for(int i = 0; i < wcur; ++i) std::cerr << buf[widx][i];
                    // ...
                    // 是否需要主动swap？
                    // try to lock rmtx // avoid deadlock
                    // widx ^= 1; ridx ^= 1; ...
                    wcur = 0;
                }
                rcond.notify_one();
                // if(stop) break;
            }
        }};
    }

    // return a current read buffer for user
    static char* current() {
        return buf[ridx];
    }

    // called by looper thread
    template <typename T>
    static void log(const T &msg,size_t length, char ch = ' ') {
        // parse T locally without lock
        // ...
        char tmp[StreamTraits<T>::size];
        Stream::parse(tmp, msg, length);

        {
            std::lock_guard<std::mutex> lk{rmtx};
            // auto rbuf = current();
            if(rcur + length > sizeof(buf[0]) + 2 /*' ' or \n */ || (std::chrono::system_clock::now() - checkpoint) > 500ms) {
                {
                    std::unique_lock<std::mutex> _{wmtx};
                    rcond.wait(_, [] { return wcur == 0; });
                    ridx ^= 1; widx ^= 1;
                    wcur = rcur; rcur = 0;
                }
                wcond.notify_one();
                checkpoint = std::chrono::system_clock::now();
            }
            
            auto rbuf = current();
            // write to rbuf
            for(int i = 0; i < length; ++i) rbuf[rcur++] = tmp[i];
            if(ch) rbuf[rcur++] = ch;
        }
        wcond.notify_one();
    }
    
};




using Log = LogBase;


// hard code
template <typename T>
inline void LogBase::d(T &&msg) {
    Stream::parse(Scheduler::current() + rcur, msg, Stream::parseLength(msg));
    rcur += Stream::parseLength(msg);
    Stream::parse(Scheduler::current() + rcur, '\n', Stream::parseLength('\n'));
    rcur += 1;
}

template <typename T, typename ...Ts>
inline void LogBase::d(T &&msg, Ts &&...others) {
    Stream::parse(Scheduler::current() + rcur, msg, Stream::parseLength(msg));
    rcur += Stream::parseLength(msg);
    Stream::parse(Scheduler::current() + rcur, ' ', Stream::parseLength(' '));
    rcur++;
    d(std::forward<Ts>(others)...);
}

template <typename T>
inline void LogBase::i(T &&msg) {
    Scheduler::log(msg, Stream::parseLength(msg), '\n');
}

template <typename T, typename ...Ts>
inline void LogBase::i(T &&msg, Ts &&...others) {
    Scheduler::log(msg, Stream::parseLength(msg));
    i(std::forward<Ts>(others)...);
}



struct Iov {
    const char *base;
    size_t len;
};

struct Wrap {
    char *tmp;
    const char **tmpref;
    size_t cur;
    size_t refcur;
    // Iov *ioves;
};

template <typename T>
inline void processDispatch(Wrap &wrap, T &&msg, char ch) {
    size_t length = Stream::parseLength(msg);
    char *buf = wrap.tmp + wrap.cur;
    Stream::parse(buf, msg, length);
    if(ch) Stream::parse(buf + length, ch, 1);
    wrap.cur += length + (ch != 0);
}

// template <size_t N>
// inline void processDispatch(Wrap &wrap, const char (&msg)[N], char ch) {
//     size_t length = Stream::parseLength(msg);
//     char *buf = wrap.tmp + wrap.cur;
    
//     iov

//     if(ch) Stream::parse(buf + length, ch, 1);
//     wrap.cur += length + (ch != 0);
// }


template <typename T>
inline void process(Wrap &wrap, T &&msg) {
    processDispatch(wrap, msg, '\n');
}

template <typename T, typename ...Ts>
inline void process(Wrap &wrap, T &&msg, Ts &&...others) {
    processDispatch(wrap, msg, ' ');
    process(wrap, std::forward<Ts>(others)...);
}






template <typename ...Ts>
inline void LogBase::info(Ts &&...msg) {
    char tmp[bufcnt(std::forward<Ts>(msg)...)];
    const char *tmpref[strcnt(std::forward<Ts>(msg)...)]; // an array stores char_ptr
    // Iov ioves[sizeof...(msg)];
    // TODO use std::array
    size_t cur = 0, refcur = 0;
    Wrap wrap {
        .tmp = tmp,
        .tmpref = tmpref, // unused
        .cur = cur,
        .refcur = refcur, // unused
        // .ioves = ioves
    };
    process(wrap, std::forward<Ts>(msg)...);
}














template <typename T,typename>
inline void Stream::parse(char *buf, T msg, size_t length) {
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

inline void Stream::parse(char *buf, const char *msg, size_t length) {
    std::memcpy(buf, msg, length);
}

template <size_t N>
inline void Stream::parse(char *buf, const char (&msg)[N], size_t length) {
    // TODO no copy
    parse(buf, (char*)msg, length);
}


inline void Stream::parse(char *buf, double msg, size_t length) {
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

inline void Stream::parse(char *buf, char msg, size_t) {
    buf[0] = msg;
}

inline void Stream::parse(char *buf, const std::string &str, size_t length) {
    parse(buf, str.c_str(), length);
}


template <typename T,typename>
inline size_t Stream::parseLength(T val) {
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
        return 10; // TODO long
    }
    return 1 + parseLength(-val);
}

inline size_t Stream::parseLength(double val) {
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


int main() {
    using namespace dlog;
    //auto t = Scheduler::start();
    Log::info(1, -223ll, "789", std::string("12.345"), 78.1, 78.0, 78.123445, 78.999, 0.0001);
    Log::info(1, 3, 4, 6, -3, 12.345, "are you ok?", -34.23);
    Log::info(-0.102);
    Log::info(123);
    Log::info(1, -223ll);
    
    //t.join();
    return 0;
}
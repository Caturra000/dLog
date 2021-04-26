#include <bits/stdc++.h>

// test-version

namespace dlog {

// shared mutable variables
// protected by namespace

constexpr size_t DLOG_BUFSIZE = 1<<18;
char buf[2][DLOG_BUFSIZE];
int ridx = 0, rcur = 0;
int widx = 1, wcur = 0;

struct LogBase {

    // Scheduler
    // Stream
    
    template <typename T>
    static void d(T &&msg) {
        out(msg, buf[ridx]);
        out("\n", buf[ridx]);
    }
    template <typename T, typename ...Ts>
    static void d(T &&msg, Ts &&...others) {
        out(msg, buf[ridx]);
        out(" ", buf[ridx]);
        d(std::forward<Ts>(others)...);
    }
    template <typename T,
    typename = std::enable_if_t<std::is_integral<T>::value>>
    static void out(T content, char *buf) {
        if(content < 0) {
            buf[rcur++] = '-';
            content = -content;
        } else if(content == 0) {
            buf[rcur++] = '0';
            return;
        }
        // int begin = rcur;
        char buf2[64], i{};
        while(content) {
            char v = (content%10)+'0';
            //buf[rcur++] = v;
            buf2[i++] = v;
            content /= 10;
        }
        // int end = rcur-1;
        // while(begin < end) {
        //     std::swap(buf[begin++], buf[end--]);
        // }
        --i;
        while(~i) buf[rcur++] = buf2[i--];
    }
    static void out(const char ch, char *buf) {
        buf[rcur++] = ch;
    }
    template <size_t N>
    static void out(const char (&from)[N], char *buf) {
        out(from, N-1, buf);
    }
    static void out(const std::string &str, char *buf) {
        out(str.c_str(), str.length(), buf);
    }
    static void out(const char *from, size_t N, char *buf) {
        std::memcpy(buf + rcur, from, sizeof(N));
        rcur += N;
    }
    static void out(double content, char *buf) {
        if(content < 0) {
            buf[rcur++] = '-';
            content = -content;
        }
        long i = content;
        out(i, buf);
        content -= i;
        constexpr double EPS = 1e-12;
        buf[rcur++] = '.';
        int limit = 5, v;
        do {
            content *= 10;
            v = content;
            buf[rcur++] = v | 48;
            content -= v;
        } while(content > EPS && limit--);
    }
    
    static void assign() {
    }
};
// 确定该用哪个buffer，不用从每个Stream.from()接口都传入固定的buf
// 并且提供阻塞（当r/w失衡时）
struct Scheduler {

    static std::mutex rmtx, wmtx;
    static std::condition_variable rcond, wcond;

    // brief: start a write-log thread
    // usage: Scheduler::start().detach()
    // TODO stop it manually
    static std::thread start() {
        return std::thread {[&] {
            while(true) {
                {
                    std::unique_lock<std::mutex> lk{wmtx};
                    wcond.wait(lk, [&] { return wcur != 0; });
                    // write to file
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
    static void log(/*T msg&, */size_t length) {
        // parse T locally without lock
        // ...
        char tmp[1<<10 /*StreamTraitis<T>::size*/];
        // Stream::serialize(tmp, msg, length);

        {
            std::lock_guard<std::mutex> lk{rmtx};
            // auto rbuf = current();
            if(length > sizeof(buf[0]) - rcur + 2 /*' ' or \n */ /*|| timeout*/) {
                {
                    std::unique_lock<std::mutex> _{wmtx};
                    rcond.wait(_, [&] { return wcur == 0; });
                    ridx ^= 1; widx ^= 1;
                    wcur = rcur; rcur = 0;
                }
                wcond.notify_one();
            }
            
            auto rbuf = current();
            // write to rbuf
            for(int i = 0; i < length; ++i) rbuf[rcur++] = tmp[i];
            // if(endOfLine) rbuf[rcur++] = '\n';
        }
        wcond.notify_one();
    }
    
};

template <typename T>
struct ExtraStream;

struct Stream {
    template <typename T,
    typename = std::enable_if_t<std::is_integral<T>::value>>
    static void serialize() {
    }
    template <typename T>
    static void serialize(T &whatever) {
        // 这里通过ExtraStream<>来扩容，且不用为基础类也分出一堆的Stream struct
        ExtraStream<T>::serialize(whatever);
    }
};
using Log = LogBase;



} // dlog


int main() {
    using namespace dlog;
    
    return 0;
}
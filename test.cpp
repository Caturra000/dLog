#include <bits/stdc++.h>

// test-version


char buf[2][1<<20];
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

    char* current() {
        if(rcur != sizeof(buf[0])) {
            return buf[ridx];
        }

        return buf[ridx];
        
    }
    
};

template <typename T>
struct ExtraStream;

struct Stream {
    template <typename T,
    typename = std::enable_if_t<std::is_integral<T>::value>>
    static void from() {
    }
    template <typename T>
    static void from(T &whatever) {
        // 这里通过ExtraStream<>来扩容，且不用为基础类也分出一堆的Stream struct
        ExtraStream<T>::from(whatever);
    }
};
using Log = LogBase;
int main() {
    Log::d(1, -223ll, "789", std::string("12.345"), 78.1, 78.0, 78.123445, 78.999, 0.0001);
    std::cout << buf[ridx];
    return 0;
}

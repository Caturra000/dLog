// portable single header version
// autogen by: https://github.com/Viatorus/quom
// dlog version: 45925975e69957b453ef5a5a203129a36745885b

#ifndef __DLOG_HPP__
#define __DLOG_HPP__
#ifndef __DLOG_MSORT_H__
#define __DLOG_MSORT_H__
#include <bits/stdc++.h>
namespace dlog {
namespace meta {

/// interface

template <typename ...Ts>
struct Sort;

template <typename T, typename U, typename ...Ts>
struct Find;

template <ssize_t L, ssize_t R, typename ...Ts>
struct Select;

template <typename ...Ts>
struct Min;

template <typename T, typename ...Ts>
struct MinValue;

template <typename T, typename ...Ts>
struct MinType;

template <typename ...Ts>
struct Concat;

template <typename V>
struct Elem;
template <typename K>
struct Key;
template <ssize_t V>
struct Value;

/// sub-interface

template <typename T, typename ...Ts>
struct Sort1;

template <ssize_t MinPos, ssize_t MaxIdx, typename T, typename ...Ts>
struct Sort2;

template <ssize_t Cur, typename T, typename U, typename ...Ts>
struct Find1;

template <ssize_t Start, ssize_t End, ssize_t Cur, typename T, typename ...Ts>
struct Select1;

template <ssize_t N, typename T, typename ...Ts>
struct Select2;

/// impl

/// KV

template <ssize_t V>
struct Value {
    static constexpr ssize_t value = V;
};
template <typename K>
struct Key {
    using type = K;
};
template <typename V>
struct Elem;

// DEMO: sort comp register
// template <>
// struct Elem<int>: Key<int>, Value<1> {};
// template <>
// struct Elem<double>: Key<double>, Value<2> {};
// template <>
// struct Elem<char>: Key<char>, Value<3> {};
// template <>
// struct Elem<float>: Key<float>, Value<4> {};
// template <>
// struct Elem<unsigned>: Key<unsigned>, Value<5> {};

/// Concat

template <typename ...Ts>
struct Concat;

// input: Concat< std::tuple<L....>, std::tuple<R....> >
// output: type -> std::tuple<L..., R...>
template <typename ...L, typename ...R>
struct Concat<std::tuple<L...>, std::tuple<R...>> {
    using type = std::tuple<L..., R...>;
};
// std::tuple<T> => type->std::tuple<T>
template <typename T>
struct Concat<std::tuple<T>> {
    using type = std::tuple<T>;
};

/// Min

template <typename ...Ts>
struct Min {
    constexpr static ssize_t value = MinValue<Ts...>::value;
    using type = typename MinType<Ts...>::type;
};

template <typename T, typename ...Ts>
struct MinValue {
    constexpr static ssize_t value = std::min(Elem<T>::value, MinValue<Ts...>::value);
};
template <typename T>
struct MinValue<T> {
    constexpr static ssize_t value = Elem<T>::value;
};

template <typename T, typename ...Ts>
struct MinType {
    using type = std::conditional_t<
        /*if  */Elem<T>::value <= MinValue<Ts...>::value, 
        /*then*/T,
        /*else*/typename MinType<Ts...>::type>;
};
template <typename T>
struct MinType<T> {
    using type = T;
};

/// Select

template <ssize_t L, ssize_t R, typename ...Ts>
struct Select {
    using type = typename Select1<L, R, 0, Ts...>::type;
    using check = std::enable_if_t<L <= R && L >= 0 && R <= sizeof...(Ts)>;
};

// not equal
template <ssize_t Start, ssize_t End, ssize_t Cur, typename T, typename ...Ts>
struct Select1 {
    using type = typename Select1<Start, End, Cur+1, Ts...>::type;
};

// start == cur
template <ssize_t Start, ssize_t End, typename T, typename ...Ts>
struct Select1<Start, End, Start, T, Ts...> {
    using type = typename Select2<End-Start+1, T, Ts...>::type; // MOCK
};

template <ssize_t N, typename T, typename ...Ts>
struct Select2 {
    using type = typename Concat<std::tuple<T>, typename Select2<N-1, Ts...>::type >::type;
};
template <typename T, typename ...Ts>
struct Select2<0, T, Ts...>;
template <typename T, typename ...Ts>
struct Select2<1, T, Ts...> {
    using type = std::tuple<T>;
};
template <typename T>
struct Select2<1, T> {
    using type = std::tuple<T>;
};

/// Find

template <typename T, typename U, typename ...Ts>
struct Find {
    constexpr static ssize_t value = Find1<0, T, U, Ts...>::value;
};

template <ssize_t Cur, typename T, typename U, typename ...Ts>
struct Find1 {
    constexpr static ssize_t value = Find1<Cur+1, T, Ts...>::value;
};
template <ssize_t Cur, typename T, typename ...Ts>
struct Find1<Cur, T, T, Ts...> {
    constexpr static ssize_t value = Cur;
};

/// Sort

template <typename ...Ts>
struct Sort {
    using type = typename Sort1<std::tuple<Ts...>>::type;
};

template <typename T, typename ...Ts>
struct Sort1;

template <typename T>
struct Sort1<std::tuple<T>> {
    using type = std::tuple<T>;
};

template <typename T, typename ...Ts>
struct Sort1<std::tuple<T, Ts...>> {
    using type = typename Sort2<
        Find<typename Min<T, Ts...>::type, T, Ts...>::value,
        sizeof...(Ts),
        T, Ts...
    >::type;
};

// minpos mid
// no std::tuple
template <ssize_t MinPos, ssize_t MaxIdx, typename T, typename ...Ts>
struct Sort2 {
    using check = std::enable_if_t<MinPos+1 <= MaxIdx && MinPos-1 >= 0>;
    using type = typename Concat<
        typename Select<MinPos, MinPos, T, Ts...>::type,
        typename Sort1<
            typename Concat<
                typename Select<0, MinPos-1, T, Ts...>::type,
                typename Select<MinPos+1, MaxIdx, T, Ts...>::type
            >::type
        >::type
    >::type;
};

// minpos == 0
template <ssize_t MaxIdx, typename T, typename ...Ts>
struct Sort2<0, MaxIdx, T, Ts...> {
    using type = typename Concat<
        std::tuple<T>,
        typename Sort1<typename Select<1, MaxIdx, T, Ts...>::type>::type
    >::type;
};

// minpos == end
template <ssize_t MaxIdx, typename T, typename ...Ts>
struct Sort2<MaxIdx, MaxIdx, T, Ts...> {
    using type = typename Concat<
        typename Select<MaxIdx, MaxIdx, T, Ts...>::type,
        typename Sort1<typename Select<0, MaxIdx-1, T, Ts...>::type>::type
    >::type;
};

} // meta
} // dlog
#endif
#ifndef __DLOG_MSEQ_H__
#define __DLOG_MSEQ_H__
#include <bits/stdc++.h>
namespace dlog {
namespace meta {

template <size_t, typename>
struct OffsetSequence;

template <size_t Offset, size_t ...Is>
struct OffsetSequence<Offset, std::index_sequence<Is...>> {
    using type = std::index_sequence<Is+Offset...>;
};

// example: MakeSequence<12, 15>::type = std::index_sequence<12, 13, 14>
template <size_t Begin, size_t End>
struct MakeSequence {
    using type = typename OffsetSequence<Begin, std::make_index_sequence<End - Begin>>::type;
};

} // meta
} // dlog
#endif
#ifndef __DLOG__MSTR_H__
#define __DLOG__MSTR_H__
#include <bits/stdc++.h>

namespace dlog {
namespace meta {

// example: MetaString<'a', 'b', 'c'>::buf = "abc" ('\0')
template <char ...Chars>
struct MetaString {
    constexpr static char buf[sizeof...(Chars)] = { Chars... };
};

template <char ...Chars> constexpr char MetaString<Chars...>::buf[sizeof...(Chars)];

template <typename MStr, size_t P>
struct MetaStringArray;

template <char ...Chars, size_t N>
struct MetaStringArray<MetaString<Chars...>, N> {
    constexpr static const char buf[][N] = { Chars... };
    using type = decltype(buf); // buf type
};

template <char ...Chars, size_t N>
constexpr const char MetaStringArray<MetaString<Chars...>, N>::buf[][N];

template <typename ...Ts>
struct Concat;

template <char ...L, char ...R>
struct Concat<MetaString<L...>, MetaString<R...>> {
    using type = MetaString<L..., R...>;
};

template <char ...Chars>
struct Concat<MetaString<Chars...>> {
    using type = MetaString<Chars...>;
};

// example: ContinuousMetaString<3, 'a'>::type = MetaString<'a', 'a', 'a'>
template <size_t N, char C, bool Eof = true>
struct Continuous {
    using type = typename Concat<
        MetaString<C>,
        typename Continuous<N-1, C, Eof>::type
    >::type;
};

template <char C>
struct Continuous<1, C> {
    using type = MetaString<C, '\0'>;
};

template <char C>
struct Continuous<1, C, false> {
    using type = MetaString<C>;
};

template <char C>
struct Continuous<0, C> {
    using type = MetaString<'\0'>;
};

template <char C>
struct Continuous<0, C, false> {
    using type = MetaString<>;
};

template <size_t N, char C, bool Eof = true>
using ContinuousMetaString = typename Continuous<N, C, Eof>::type;

// example: Whitespace<5>::buf = "     "
template <size_t N>
using Whitespace = ContinuousMetaString<N, ' '>;

/// numeric

constexpr inline int abs(int I) { return I < 0 ? abs(-I) : I; }
constexpr inline size_t length(int I) { return I < 0 ? 1 + length(-I) : I < 10 ? 1 : 1 + length(I / 10); }

template<size_t N, int I, char... Chars>
struct NumericMetaStringBuilder {
    using type = typename NumericMetaStringBuilder<N - 1, I / 10, '0' + abs(I) % 10, Chars...>::type;
};
template<int I, char... Chars>
struct NumericMetaStringBuilder<2, I, Chars...> {
    using type = MetaString<I < 0 ? '-' : '0' + I / 10, '0' + abs(I) % 10, Chars...>;
};
template<int I, char... Chars>
struct NumericMetaStringBuilder<1, I, Chars...> {
    using type = MetaString<'0' + I, Chars...>;
};

// example: Numeric<123>::type = MetaString<'1', '2', '3', '\0'>
template<int I>
struct Numeric {
    using type = typename NumericMetaStringBuilder<length(I), I, '\0'>::type;
};

template <int I>
using NumericMetaString = typename Numeric<I>::type;

template <typename>
struct NumericMetaStrings;

template <size_t ...Is>
struct NumericMetaStrings<std::index_sequence<Is...>> {
    constexpr const static char *bufs[] = { NumericMetaString<Is>::buf... };
    constexpr const static size_t len[] = { length(Is)... };
};

template <size_t ...Is> constexpr const char *NumericMetaStrings<std::index_sequence<Is...>>::bufs[];
template <size_t ...Is> constexpr const size_t NumericMetaStrings<std::index_sequence<Is...>>::len[];

// example: NumericMetaStringsSequence<3>::bufs[] = {"0", "1", "2"};
template <size_t N>
struct NumericMetaStringsSequence {
    constexpr static auto bufs = NumericMetaStrings<std::make_index_sequence<N>>::bufs;
    constexpr static auto len = NumericMetaStrings<std::make_index_sequence<N>>::len;
};

// example: LeadingZeroNumeric<123, 5>::type = MetaString<'0', '0', '1', '2', '3', '\0'>
template <int I, size_t P, char Z = '0'> // P(prefix) >= length(I)
struct LeadingZeroNumeric {
    constexpr static size_t len = length(I);
    using check = std::enable_if_t<P >= len && I >= 0>;
    using ztype = ContinuousMetaString<P-len, Z, false>;
    using ntype = NumericMetaString<I>;
    using type = typename Concat<ztype, ntype>::type; // MetaString
};

template <int I, size_t P, char Z = '0'>
using LeadingZeroNumericMetaString = typename LeadingZeroNumeric<I, P, Z>::type;

template <size_t ...Is>
struct LeadingZeroNumericArrayBuilderImplSequence;

template <size_t P, size_t I, size_t ...Is>
struct LeadingZeroNumericArrayBuilderImplSequence<P, I, Is...> {
    using type = typename Concat<
        LeadingZeroNumericMetaString<I, P>,
        typename LeadingZeroNumericArrayBuilderImplSequence<P, Is...>::type
    >::type;
};

template <size_t P, size_t I>
struct LeadingZeroNumericArrayBuilderImplSequence<P, I> {
    using type = LeadingZeroNumericMetaString<I, P>;
};

template <typename, size_t>
struct LeadingZeroNumericArrayBuilderImpl;

template <size_t P, size_t ...Is>
struct LeadingZeroNumericArrayBuilderImpl<std::index_sequence<Is...>, P> {
    // type -> MetaString
    using type = typename LeadingZeroNumericArrayBuilderImplSequence<P, Is...>::type;
};

// solve ftemplate-depth limit
template <size_t Begin, size_t End, size_t P, bool Flag>
struct LeadingZeroNumericArrayBuilderRange {
    using type = typename LeadingZeroNumericArrayBuilderImpl<typename MakeSequence<Begin, End>::type, P>::type;
};

template <size_t Begin, size_t End, size_t P>
struct LeadingZeroNumericArrayBuilderRange<Begin, End, P, false> {
    using type = typename Concat<
        typename LeadingZeroNumericArrayBuilderRange<Begin, Begin+500, P, true>::type,
        typename LeadingZeroNumericArrayBuilderRange<Begin+500, End, P, (1000 >= (End-Begin))>::type
    >::type;
};

template <size_t N, size_t P>
struct LeadingZeroNumericArrayBuilder {
    using check = std::enable_if_t<N && P >= length(N-1)>;
    // type -> MetaString
    using type = typename LeadingZeroNumericArrayBuilderRange<0, N, P, 500>=N>::type;
};

// example: LeadingZeroNumericArray<12, 3>::buf = { "000", "001", "002", ... "010", "011" } (const char[12][4])
template <size_t N, size_t P>
using LeadingZeroNumericArray = MetaStringArray<typename LeadingZeroNumericArrayBuilder<N, P>::type ,P+1>;

} // meta
} // dlog
#endif
#ifndef __DLOG_CHRONO_H__
#define __DLOG_CHRONO_H__
#include <bits/stdc++.h>
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

namespace dlog {

// ref: http://howardhinnant.github.io/date_algorithms.html
// UTC +0
struct Chrono {
    struct DaysDuration {
        using Rep = int;
        using Period = std::ratio_multiply<std::chrono::hours::period, std::ratio<24>>;
        using Type = std::chrono::duration<Rep, Period>; // for std::chrono::duration
    };

    struct Date {
        DaysDuration::Rep y;
        DaysDuration::Rep M;
        DaysDuration::Rep d;
        DaysDuration::Rep& year() { return y; }
        DaysDuration::Rep& month() { return M; }
        DaysDuration::Rep& day() { return d; }
    };

    struct Time {
        DaysDuration::Rep h;
        DaysDuration::Rep m;
        DaysDuration::Rep s;
        DaysDuration::Rep S;
        DaysDuration::Rep& hour() { return h; }
        DaysDuration::Rep& minute() { return m; }
        DaysDuration::Rep& second() { return s; }
        DaysDuration::Rep& millisecond() { return S; }
    };

    struct DateTime: public Date, public Time {
        constexpr DateTime(Date date, Time time): Date(date), Time(time) {}
    };

    static constexpr Date getDate(std::chrono::system_clock::time_point point) noexcept {
        return getDate(point.time_since_epoch());
    }

    // duration: since epoch
    static constexpr Date getDate(std::chrono::system_clock::duration duration) noexcept {
        return getDate(std::chrono::duration_cast<DaysDuration::Type>(duration).count());
    }

    static constexpr Date getDate(DaysDuration::Rep count) noexcept {
        return calDate(count);
    }

    static Time getTime(std::chrono::system_clock::time_point point) {
        return getTime(point.time_since_epoch());
    }

    static Time getTime(std::chrono::system_clock::duration duration) {
        return calTime(duration);
    }

    static DateTime getDateTime(std::chrono::system_clock::time_point point) {
        return { getDate(point), getTime(point) };
    }

    static DateTime getDateTime(std::chrono::system_clock::duration duration) {
        return { getDate(duration), getTime(duration) };
    }

    // utils

    static std::chrono::system_clock::time_point now() {
        return std::chrono::system_clock::now();
    }

    // truncate: yyyy-MM-dd hh:mm:ss -> yyyy-MM-dd 00:00:00
    static constexpr auto thisDay(std::chrono::system_clock::time_point point) noexcept -> decltype(point) {
        constexpr std::chrono::system_clock::time_point epoch {};
        return epoch + std::chrono::duration_cast<Chrono::DaysDuration::Type>(point.time_since_epoch());
    }

    // only for debug
    static std::string formatDebug(std::chrono::system_clock::time_point point);
    // fast format
    static std::array<IoVector, 2> format(std::chrono::system_clock::time_point point);

private:
    // Returns year/month/day triple in civil calendar
    // Preconditions:  z is number of days since 1970-01-01 and is in the range:
    //                   [numeric_limits<Int>::min(), numeric_limits<Int>::max()-719468].
    template <class Int> static constexpr Date calDate(Int z) noexcept;
    static Time calTime(std::chrono::system_clock::duration duration);
};

/// impl

inline std::string Chrono::formatDebug(std::chrono::system_clock::time_point point) {
    auto dateTime = getDateTime(point);
    return std::to_string(dateTime.year()) + '-' + std::to_string(dateTime.month()) + '-' + std::to_string(dateTime.day()) + ' '
            + std::to_string(dateTime.hour()) + ':' + std::to_string(dateTime.minute()) + ':' + std::to_string(dateTime.second()) 
                + std::to_string(dateTime.hour()) + ':' + std::to_string(dateTime.minute()) + ':' + std::to_string(dateTime.second()) 
            + std::to_string(dateTime.hour()) + ':' + std::to_string(dateTime.minute()) + ':' + std::to_string(dateTime.second()) 
            + '.' + std::to_string(dateTime.millisecond());
}

inline std::array<IoVector, 2> Chrono::format(std::chrono::system_clock::time_point point) {
    using namespace std::chrono;
    using namespace std::chrono_literals;
    using Digits100  = meta::LeadingZeroNumericArray<100, 2>;
    using Digits1000 = meta::LeadingZeroNumericArray<1000, 3>;
    using Digits2050 = meta::LeadingZeroNumericArray<2050, 4>;
    constexpr static Digits100::type  &digits100  = Digits100::buf;
    constexpr static Digits1000::type &digits1000 = Digits1000::buf;
    constexpr static Digits2050::type &digits2050 = Digits2050::buf;
    static thread_local char dateRecord[] = "1970-01-01";
    static thread_local char timeRecord[] = "00:00:00.000";
    static thread_local system_clock::time_point last {};

    auto duration = point - last;
    auto dateTime = getDateTime(point);
    if(duration >= 24h) {
        last = thisDay(point);
        // update date
        std::memcpy(dateRecord, digits2050[dateTime.year()], 4);
        std::memcpy(dateRecord + 5, digits100[dateTime.month()], 2);
        std::memcpy(dateRecord + 8, digits100[dateTime.day()], 2);
    }

    // update time
    std::memcpy(timeRecord, digits100[dateTime.hour()], 2);
    std::memcpy(timeRecord + 3, digits100[dateTime.minute()], 2);
    std::memcpy(timeRecord + 6, digits100[dateTime.second()], 2);
    std::memcpy(timeRecord + 9, digits1000[dateTime.millisecond()], 3);

    return {
        IoVector {
            .base = dateRecord,
            .len = sizeof(dateRecord)-1
        },
        IoVector {
            .base = timeRecord,
            .len = sizeof(timeRecord)-1
        }
    };
}

template <class Int>
inline constexpr Chrono::Date Chrono::calDate(Int z) noexcept {
    static_assert(std::numeric_limits<unsigned>::digits >= 18,
             "This algorithm has not been ported to a 16 bit unsigned integer");
    static_assert(std::numeric_limits<Int>::digits >= 20,
             "This algorithm has not been ported to a 16 bit signed integer");
    z += 719468;
    const Int era = (z >= 0 ? z : z - 146096) / 146097;
    const unsigned doe = static_cast<unsigned>(z - era * 146097);          // [0, 146096]
    const unsigned yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;  // [0, 399]
    const Int y = static_cast<Int>(yoe) + era * 400;
    const unsigned doy = doe - (365*yoe + yoe/4 - yoe/100);                // [0, 365]
    const unsigned mp = (5*doy + 2)/153;                                   // [0, 11]
    const unsigned d = doy - (153*mp+2)/5 + 1;                             // [1, 31]
    const unsigned m = mp + (mp < 10 ? 3 : -9);                            // [1, 12]
    return { y + (m <= 2), static_cast<Int>(m), static_cast<Int>(d) };
}

inline Chrono::Time Chrono::calTime(std::chrono::system_clock::duration duration) {
    using namespace std::chrono;
    auto prefix = duration_cast<DaysDuration::Type>(duration);
    duration -= prefix;
    auto h = duration_cast<hours>(duration);
    duration -= h;
    auto m = duration_cast<minutes>(duration);
    duration -= m;
    auto s = duration_cast<seconds>(duration);
    duration -= s;
    auto S = duration_cast<milliseconds>(duration);
    return {
        static_cast<DaysDuration::Rep>(h.count()),
        static_cast<DaysDuration::Rep>(m.count()),
        static_cast<DaysDuration::Rep>(s.count()),
        static_cast<DaysDuration::Rep>(S.count())
    };
}

} // dlog
#endif
#ifndef __DLOG_LOG_H__
#define __DLOG_LOG_H__
#include <bits/stdc++.h>
#ifndef __DLOG_CONFIG_H__
#define __DLOG_CONFIG_H__
#include <bits/stdc++.h>
namespace dlog {

struct StaticConfig {
    const char *log_dir;
    const char *log_filename;
    const char *log_filename_extension;
    size_t fileMaxSize;
    bool debugOn;
    bool infoOn;
    bool warnOn;
    bool errorOn;
    bool wtfOn;
    std::chrono::milliseconds fileRollingInterval;
    size_t msg_align;
    constexpr StaticConfig(const char *log_dir, const char *log_filename, const char *log_filename_extension,
        size_t fileMaxSize,
        bool debugOn, bool infoOn, bool warnOn, bool errorOn, bool wtfOn,
        std::chrono::milliseconds fileRollingInterval,
        size_t msg_align)
        : log_dir(log_dir),
          log_filename(log_filename),
          log_filename_extension(log_filename_extension),
          fileMaxSize(fileMaxSize),
          debugOn(debugOn),
          infoOn(infoOn),
          warnOn(warnOn),
          errorOn(errorOn),
          wtfOn(wtfOn),
          fileRollingInterval(fileRollingInterval),
          msg_align(msg_align) {}
};
// constexpr const StaticConfig &staticConfig

/// impl

namespace conf { // internal impl, don't use outside

using namespace std::chrono_literals;

enum log_level { DEBUG, INFO, WARN, ERROR, WTF };

// a script-like config builder
struct config {
    enum class type {
        CONFIG,
        LOG_PATH,
        LOG_FILTER,
        MAX_FILE_SIZE,
        ROLLING,
        MSG_ALIGN,
    } _type;

    const char *_path {"log_default.log"};

    const char *_log_dir {"."};
    const char *_log_filename {"log_default"};
    const char *_log_filename_extension {".log"};
    size_t _fileMaxSize {};
    size_t _logOpenFlags[5] {};
    std::chrono::milliseconds _fileRollingInterval {24h};
    size_t _msg_align {20};

    constexpr config(enum type type)
        : _type(type) {}
    constexpr config(std::initializer_list<config> children)
        : _type(type::CONFIG) {
            for(const auto &l: children) {
                switch(l._type) {
                    case type::CONFIG:
                        copy(l);
                    break;
                    case type::LOG_PATH:
                        _log_dir = l._log_dir;
                        _log_filename = l._log_filename;
                        _log_filename_extension = l._log_filename_extension;
                    break;
                    case type::LOG_FILTER:
                        for(size_t i = 0; i < sizeof(_logOpenFlags)/sizeof(decltype(_logOpenFlags[0])); ++i) {
                            _logOpenFlags[i] = l._logOpenFlags[i];
                        }
                    break;
                    case type::MAX_FILE_SIZE:
                        _fileMaxSize = l._fileMaxSize;
                    break;
                    case type::ROLLING:
                        _fileRollingInterval = l._fileRollingInterval;
                    break;
                    case type::MSG_ALIGN:
                        _msg_align = l._msg_align;
                    break;
                    default:
                    break;
                }
            }
        }

    constexpr void copy(const config &rhs) {
        _type = rhs._type;
        _log_dir = rhs._log_dir;
        _log_filename = rhs._log_filename;
        _log_filename_extension = rhs._log_filename_extension;
        _fileMaxSize = rhs._fileMaxSize;
        _fileRollingInterval = rhs._fileRollingInterval;
        for(size_t i = 0; i < sizeof(_logOpenFlags)/sizeof(decltype(_logOpenFlags[0])); ++i) {
            _logOpenFlags[i] = rhs._logOpenFlags[i];
        }
    }

    constexpr operator StaticConfig() {
        return StaticConfig(
            _log_dir, _log_filename, _log_filename_extension,
            _fileMaxSize,
            _logOpenFlags[0], _logOpenFlags[1], _logOpenFlags[2], _logOpenFlags[3], _logOpenFlags[4],
            _fileRollingInterval,
            _msg_align
        );
    }
};

struct log_path: public config {
    enum class path_type {
        LOG_PATH,
        LOG_DIR,
        LOG_FILENAME,
        LOG_FILENAME_EXTENSION
    } _path_type;

    explicit constexpr log_path(enum path_type path_type)
        : config(type::LOG_PATH),
          _path_type(path_type) {}
    explicit constexpr log_path(std::initializer_list<log_path> children)
        : config(type::LOG_PATH),
          _path_type(path_type::LOG_PATH) {
        for(const auto &l: children) {
            switch(l._path_type) {
                case path_type::LOG_DIR:
                    _log_dir = l._log_dir;
                break;
                case path_type::LOG_FILENAME:
                    _log_filename = l._log_filename;
                break;
                case path_type::LOG_FILENAME_EXTENSION:
                    _log_filename_extension = l._log_filename_extension;
                break;
                default:
                break;
            }
        }
    }
};

struct log_dir: public log_path {
    explicit constexpr log_dir(const char* dir)
        : log_path(path_type::LOG_DIR) { _log_dir = dir; }
};

struct log_filename: public log_path {
    explicit constexpr log_filename(const char* filename)
        : log_path(path_type::LOG_FILENAME) { _log_filename = filename; }
};

struct log_filename_extension: public log_path {
    explicit constexpr log_filename_extension(const char* extension)
        : log_path(path_type::LOG_FILENAME_EXTENSION) { _log_filename_extension = extension; }
};

struct log_filter: public config {
    explicit constexpr log_filter(std::initializer_list<log_level> levels)
        : config(type::LOG_FILTER) {
        for(const auto &level : levels) {
            _logOpenFlags[level] = true;
        }
    }
};

struct msg_align: public config {
    explicit constexpr msg_align(size_t align)
        : config(type::MSG_ALIGN) { _msg_align = align; }
};

// need -Wno-literal-suffix
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-suffix"
inline constexpr unsigned long long operator""B(unsigned long long x) { return x; }
inline constexpr unsigned long long operator""KB(unsigned long long x) { return 1024B * x; }
inline constexpr unsigned long long operator""MB(unsigned long long x) { return 1024KB * x; }
inline constexpr unsigned long long operator""GB(unsigned long long x) { return 1024MB * x; }
#pragma GCC diagnostic pop

struct file_max_size: public config {
    explicit constexpr file_max_size(size_t fileMaxSize)
        : config(type::MAX_FILE_SIZE) { _fileMaxSize = fileMaxSize; }
};

struct file_rolling_interval: public config {
    explicit constexpr file_rolling_interval(std::chrono::milliseconds fileRollingInterval)
        : config(type::ROLLING) { _fileRollingInterval = fileRollingInterval; }
};

constexpr static StaticConfig globalConfigBoot {
#ifdef DLOG_CONF_PATH
#include DLOG_CONF_PATH
#else
#include "dlog.conf"
#endif
};

} // conf

constexpr const static StaticConfig &staticConfig = conf::globalConfigBoot;

} // dlog
#endif

#ifndef __DLOG_RESOLVE_H__
#define __DLOG_RESOLVE_H__
#include <bits/stdc++.h>
#ifndef __DLOG_STREAM_H__
#define __DLOG_STREAM_H__
#include <bits/stdc++.h>
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

template <typename T>
struct ExtraStream;

struct Stream {
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

    template <typename T>
    static auto parse(char *buf, T &&whatever, size_t length)
        -> decltype(ExtraStream<T>::parse(0, std::forward<T>(whatever), 0)) {
        ExtraStream<T>::parse(buf, std::forward<T>(whatever), length);
    }

    template <typename T>
    static auto parseLength(T &&whatever)
        -> decltype(ExtraStream<T>::parseLength(std::forward<T>(whatever))) {
        return ExtraStream<T>::parseLength(std::forward<T>(whatever));
    }
};

/// impl

template <typename T>
inline Stream::VoidIfInt<T>
Stream::parse(char *buf, T msg, size_t length) {
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
inline void Stream::parse(char *buf, const char (&msg)[N], size_t length) {
    parse(buf, (char*)msg, length);
}

inline void Stream::parse(char *buf, const char *msg, size_t length) {
    std::memcpy(buf, msg, length);
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

template <typename T>
inline constexpr Stream::SizeTypeIfInt<T>
Stream::parseLength(T val) {
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
#endif

namespace dlog {

struct ResolveContext {
    char *local; // local stack buffer
    size_t cur; // current index of local
    IoVector *ioves;
    size_t count; // count of iovectors
    size_t total; // total length

    char* currentLocal() { return local + cur; }

    // notice: message should be parsed & transferred to local
    void updateLocal(size_t len) {
        updateExternal(local + cur, len);
        cur += len;
    }

    // external source, just update IO vectors
    void updateExternal(const char *buf, size_t len) {
        ioves[count].base = buf;
        ioves[count].len = len;
        count++;
        total += len;
    }
};

struct Resolver {
    template <typename T> static void resolve(ResolveContext &ctx, T &&msg);
    template <typename T, typename ...Ts> static void resolve(ResolveContext &ctx, T&&msg, Ts &&...others);

    static size_t calspace(ResolveContext &ctx) { return ctx.total + ctx.count; }
    static void put(ResolveContext &ctx, char *buf);

private:
    template <typename T> static void resolveDispatch(ResolveContext &ctx, T &&msg);
    template <size_t N> static void resolveDispatch(ResolveContext &ctx, const char (&msg)[N]);
    static void resolveDispatch(ResolveContext &ctx, int msg);
    static void resolveDispatch(ResolveContext &ctx, size_t msg);
    static void resolveDispatch(ResolveContext &ctx, IoVector iov);
    template <size_t N> static void resolveDispatch(ResolveContext &ctx, std::array<IoVector, N> &ioves);
    template <size_t N> static void resolveDispatch(ResolveContext &ctx, std::array<IoVector, N> &&ioves);
};

/// impl

template <typename T>
inline void Resolver::resolve(ResolveContext &ctx, T &&msg) {
    resolveDispatch(ctx, std::forward<T>(msg));
}

template <typename T, typename ...Ts>
inline void Resolver::resolve(ResolveContext &ctx, T&&msg, Ts &&...others) {
    resolveDispatch(ctx, std::forward<T>(msg));
    resolve(ctx, std::forward<Ts>(others)...);
}

inline void Resolver::put(ResolveContext &ctx, char *buf) {
    IoVector *ioves = ctx.ioves;
    size_t offset = 0;
    for(size_t i = 0; i < ctx.count; ++i) {
        std::memcpy(buf + offset, ioves[i].base, ioves[i].len);
        offset += ioves[i].len;
        if(i == ctx.count-1) buf[offset] = '\n';
        else buf[offset] = ' ';
        ++offset;
    }
}

template <typename T>
inline void Resolver::resolveDispatch(ResolveContext &ctx, T &&msg) {
    size_t len = Stream::parseLength(std::forward<T>(msg));
    char *buf = ctx.currentLocal();
    Stream::parse(buf, std::forward<T>(msg), len);
    ctx.updateLocal(len);
}

template <size_t N>
inline void Resolver::resolveDispatch(ResolveContext &ctx, const char (&msg)[N]) {
    static_assert(N >= 1, "N must be positive.");
    size_t len = N-1;
    ctx.updateExternal(msg, len);
}

inline void Resolver::resolveDispatch(ResolveContext &ctx, int msg) {
    constexpr static size_t limit = 10000;
    using Cache = meta::NumericMetaStringsSequence<limit>;
    if(msg > 0 && msg < limit) {
        resolveDispatch(ctx, IoVector{Cache::bufs[msg], Cache::len[msg]});
    } else {
        resolveDispatch<>(ctx, msg);
    }
}

inline void Resolver::resolveDispatch(ResolveContext &ctx, size_t msg) {
    constexpr static size_t limit = 10000;
    using Cache = meta::NumericMetaStringsSequence<limit>;
    if(msg < limit) {
        resolveDispatch(ctx, IoVector{Cache::bufs[msg], Cache::len[msg]});
    } else {
        resolveDispatch<>(ctx, msg);
    }
}

inline void Resolver::resolveDispatch(ResolveContext &ctx, IoVector iov) {
    ctx.updateExternal(iov.base, iov.len);
}

template <size_t N>
inline void Resolver::resolveDispatch(ResolveContext &ctx, std::array<IoVector, N> &ioves) {
    for(auto &iov : ioves) resolveDispatch(ctx, iov);
}

template <size_t N>
inline void Resolver::resolveDispatch(ResolveContext &ctx, std::array<IoVector, N> &&ioves) {
    resolveDispatch(ctx, ioves);
}

} // dlog
#endif
#ifndef __DLOG_SCHED_H__
#define __DLOG_SCHED_H__
#include <bits/stdc++.h>

#ifndef __DLOG_FS_H__
#define __DLOG_FS_H__
#include <unistd.h>
#include <fcntl.h>
#include <bits/stdc++.h>

namespace dlog {

// a very simple file util class, used for append-write
class File {
public:
    explicit File(std::string path)
        : _path(std::move(path)),
          _timePoint(std::chrono::system_clock::now()),
          _written(0),
          _fd(::open(_path.c_str(), OPEN_FLAG, OPEN_MODE)) { // blocking
        // if(fd < 0) throw
    }
    // unsafe
    explicit File(int fd): _fd(fd) {}
    ~File() { if(_fd != INVALID_FD) ::close(_fd); } // throw...

    // support rvalue

    File(File &&rhs): _fd(rhs._fd) { rhs._fd = INVALID_FD; }
    File& operator=(File &&that) {
        File(static_cast<File&&>(that)).swap(*this);
        return *this;
    }

    // basic operations

    int fd() { return _fd; }
    void detach() { _fd = INVALID_FD; }
    void swap(File &that);
    void append(const char *buf, size_t count);

    // feature

    bool updatable(size_t count);
    void update(std::string newPath);

private:
    constexpr static int INVALID_FD = -1;
    constexpr static int OPEN_FLAG = O_RDWR | O_CREAT | O_APPEND | O_CLOEXEC;
    constexpr static int OPEN_MODE = 0644;
    std::string _path;
    std::chrono::system_clock::time_point _timePoint;
    size_t _written;
    int _fd;
    
};

/// impl

inline void File::swap(File &that) {
    std::swap(this->_path, that._path);
    std::swap(this->_timePoint, that._timePoint);
    std::swap(this->_written, that._written);
    std::swap(this->_fd, that._fd);
}

inline void File::append(const char *buf, size_t count) {
    size_t written = 0;
    while(written != count) {
        size_t remain = count - written;
        size_t n = ::write(_fd, buf + written, remain);
        if(n < 0) ; // throw...
        written += n;
    }
    _written += written;
}

inline bool File::updatable(size_t count) {
    if(_written + count >= staticConfig.fileMaxSize) {
        return true;
    }
    auto current = std::chrono::system_clock::now();
    if(current - _timePoint >= staticConfig.fileRollingInterval) {
        return true;
    }
    return false;
}

inline void File::update(std::string newPath) {
    File roll(std::move(newPath));
    roll.swap(*this);
}

} // dlog
#endif

namespace dlog {

// shared for writer thread (backend) and scheduler (frontend)
// use singleton instead of global variables as a inline field
struct Shared {

    // buffers

    constexpr static size_t DLOG_BUFSIZE = 1<<20;

    char buf[2][DLOG_BUFSIZE];
    size_t ridx = 0, rcur = 0;
    size_t widx = 1, wcur = 0;

    // concurrency

    std::mutex rmtx, smtx; // read_mutex swap_mutex
    std::condition_variable cond;
    bool sflag = true; // swap finished

    static Shared& singleton() {
        static Shared instance;
        return instance;
    }
};

// a writer thread
class Wthread {
public:
    Wthread();
    ~Wthread();
    void kill() { kflag.store(true); }
private:
    File file;
    std::atomic<bool> kflag;
    std::thread writer;
    void writeFunc();
    static std::string generateFileName();
}; // wthread;

// interact with wthread
struct Scheduler {
    static void apply(ResolveContext &args) {
        auto &s = Shared::singleton();
        std::lock_guard<std::mutex> lk{s.rmtx};
        if(s.rcur + Resolver::calspace(args) >= sizeof(s.buf[0])) {
            {
                std::unique_lock<std::mutex> _{s.smtx};
                s.sflag = false;
            }
            while(!s.sflag) s.cond.notify_one();
        }
        auto rbuf = s.buf[s.ridx];
        Resolver::put(args, rbuf + s.rcur);
        s.rcur += Resolver::calspace(args);
    }
};

/// impl

inline Wthread::Wthread()
    : file(generateFileName()),
      kflag(false),
      writer {[this] { writeFunc(); }} {}

inline Wthread::~Wthread() {
    kill();
    if(writer.joinable()) {
        writer.join();
    }
}

inline void Wthread::writeFunc() {
    using namespace std::literals::chrono_literals;
    int cur, idx;
    auto &s = Shared::singleton();
    auto swap = [&] {
        s.ridx ^= 1;
        s.widx ^= 1;
        s.wcur = s.rcur;
        s.rcur = 0;
        s.sflag = true; // swap finished
        cur = s.wcur;
        s.wcur = 0;
        idx = s.widx;
    };
    while(!kflag.load()) {
        {
            std::unique_lock<std::mutex> lk{s.smtx};
            auto request = s.cond.wait_for(lk, 10ms, [&] { return !s.sflag; });
            if(request) {
                swap();
            } else if(s.rmtx.try_lock()) { // use try, no dead lock
                // corner case, timeout
                std::lock_guard<std::mutex> _{s.rmtx, std::adopt_lock};
                swap();
            } else {
                continue;
            }
        }
        if(file.updatable(cur)) {
            file.update(generateFileName());
        }
        file.append(s.buf[idx], cur);
    }
}

inline std::string Wthread::generateFileName() {
    std::array<IoVector, 2> dateTime = Chrono::format(Chrono::now());
    std::string add = std::string(dateTime[0].base) + '-' + std::string(dateTime[1].base);
    std::for_each(add.begin(), add.end(), [](char &ch) {
        if(ch == ' ' || ch == ':') ch = '-';
    });

    return std::string(staticConfig.log_dir) + '/'
          + staticConfig.log_filename
          + '.' + add
          + staticConfig.log_filename_extension;
}

} // dlog
#endif
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
#ifndef __DLOG_TAGS_H__
#define __DLOG_TAGS_H__
#include <bits/stdc++.h>

#ifndef __DLOG_TID_H__
#define __DLOG_TID_H__
#include <unistd.h>
#include <bits/stdc++.h>

namespace dlog {

class Tid {
public:
    static const Tid& get() {
        static const thread_local Tid instance;
        return instance;
    }

    static IoVector format() {
        auto &instance = get();
        return { .base = instance.buf, .len = instance.len };
    }

    pid_t getTid() const { return tid; }

private:
    Tid(): tid(::gettid()), len(Stream::parseLength(tid)) {
        Stream::parse(buf, tid, len);
    }

    pid_t tid;
    size_t len;
    char buf[33]{};
};

} // dlog
#endif

namespace dlog {

struct DateTimeTag {
    static std::array<IoVector, 2> format() {
        return Chrono::format(Chrono::now());
    }
};

struct ThreadIdTag {
    static IoVector format() {
        return Tid::format();
    }
};

template <LogLevel LEVEL>
struct LogLevelTag {
    static char format() {
        return levelFormat<LEVEL>();
    }
};

namespace meta {

template <typename V>
struct Elem;

template <>
struct Elem<DateTimeTag>: Key<DateTimeTag>, Value<1> {};

template <LogLevel LEVEL>
struct Elem<LogLevelTag<LEVEL>>: Key<LogLevelTag<LEVEL>>, Value<2> {};

template <>
struct Elem<ThreadIdTag>: Key<ThreadIdTag>, Value<3> {};

} // meta
} // dlog
#endif

#ifndef __DLOG_MACRO_H__
#define __DLOG_MACRO_H__

#define DLOG_DEBUG(...) Log::debug(dlog::filename(__FILE__), __LINE__, __VA_ARGS__)
#define DLOG_INFO(...) Log::info(dlog::filename(__FILE__), __LINE__, __VA_ARGS__)
#define DLOG_WARN(...) Log::warn(dlog::filename(__FILE__), __LINE__, __VA_ARGS__)
#define DLOG_ERROR(...) Log::error(dlog::filename(__FILE__), __LINE__, __VA_ARGS__)
#define DLOG_WTF(...) Log::wtf(dlog::filename(__FILE__), __LINE__, __VA_ARGS__)

#define DLOG_DEBUG_ALIGN(...) Log::debug(dlog::filename(__FILE__), __LINE__, \
    dlog::meta::Whitespace<(size_t)std::max(ssize_t(1), ssize_t(dlog::staticConfig.msg_align) \
    - ssize_t(dlog::lastPathComponentSize(__FILE__)+dlog::Stream::parseLength(__LINE__)))>::buf,__VA_ARGS__) // line+2

#define DLOG_INFO_ALIGN(...) Log::info(dlog::filename(__FILE__), __LINE__, \
    dlog::meta::Whitespace<(size_t)std::max(ssize_t(1), ssize_t(dlog::staticConfig.msg_align) \
    - ssize_t(dlog::lastPathComponentSize(__FILE__)+dlog::Stream::parseLength(__LINE__)))>::buf,__VA_ARGS__)

#define DLOG_WARN_ALIGN(...) Log::warn(dlog::filename(__FILE__), __LINE__, \
    dlog::meta::Whitespace<(size_t)std::max(ssize_t(1), ssize_t(dlog::staticConfig.msg_align) \
    - ssize_t(dlog::lastPathComponentSize(__FILE__)+dlog::Stream::parseLength(__LINE__)))>::buf,__VA_ARGS__)

#define DLOG_ERROR_ALIGN(...) Log::error(dlog::filename(__FILE__), __LINE__, \
    dlog::meta::Whitespace<(size_t)std::max(ssize_t(1), ssize_t(dlog::staticConfig.msg_align) \
    - ssize_t(dlog::lastPathComponentSize(__FILE__)+dlog::Stream::parseLength(__LINE__)))>::buf,__VA_ARGS__)

#define DLOG_WTF_ALIGN(...) Log::wtf(dlog::filename(__FILE__), __LINE__, \
    dlog::meta::Whitespace<(size_t)std::max(ssize_t(1), ssize_t(dlog::staticConfig.msg_align) \
    - ssize_t(dlog::lastPathComponentSize(__FILE__)+dlog::Stream::parseLength(__LINE__)))>::buf,__VA_ARGS__)

namespace dlog {}
#endif
#ifndef __DLOG_FILENAME_H__
#define __DLOG_FILENAME_H__
#include <bits/stdc++.h>

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
#endif
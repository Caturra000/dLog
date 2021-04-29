#ifndef __DLOG_CHRONO_H__
#define __DLOG_CHRONO_H__
#include <bits/stdc++.h>
namespace dlog {

// ref: http://howardhinnant.github.io/date_algorithms.html
// UTC +0
struct Chrono {

    struct DaysDuration {
        using Rep = int;
        using Period = std::chrono::duration<Rep,
            std::ratio_multiply<std::chrono::hours::period, std::ratio<24>>>;
    };

    struct Date {
        DaysDuration::Rep y;
        DaysDuration::Rep M;
        DaysDuration::Rep d;
    };

    struct Time {
        DaysDuration::Rep h;
        DaysDuration::Rep m;
        DaysDuration::Rep s;
    };

    struct DateTime {
        Date date;
        Time time;
        DaysDuration::Rep& year() { return date.y; }
        DaysDuration::Rep& month() { return date.M; }
        DaysDuration::Rep& day() { return date.d; }
        DaysDuration::Rep& hour() { return time.h; }
        DaysDuration::Rep& minute() { return time.m; }
        DaysDuration::Rep& second() { return time.s; }
    };

    static constexpr Date getDate(std::chrono::system_clock::time_point point) noexcept {
        return getDate(point.time_since_epoch());
    }

    // duration: since epoch
    static constexpr Date getDate(std::chrono::system_clock::duration duration) noexcept {
        return getDate(std::chrono::duration_cast<DaysDuration::Period>(duration).count());
    }

    // FIX rep
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

private:

    template <class Int>
    static constexpr Date calDate(Int z) noexcept;

    static Time calTime(std::chrono::system_clock::duration duration);
};

// Returns year/month/day triple in civil calendar
// Preconditions:  z is number of days since 1970-01-01 and is in the range:
//                   [numeric_limits<Int>::min(), numeric_limits<Int>::max()-719468].
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
    auto prefix = duration_cast<DaysDuration::Period>(duration);
    duration -= prefix;
    auto h = duration_cast<hours>(duration);
    duration -= h;
    auto m = duration_cast<minutes>(duration);
    duration -= m;
    auto s = duration_cast<seconds>(duration);
    return {
        static_cast<DaysDuration::Rep>(h.count()),
        static_cast<DaysDuration::Rep>(m.count()),
        static_cast<DaysDuration::Rep>(s.count())
    };
}

} // dlog
#endif
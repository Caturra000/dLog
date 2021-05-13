#ifndef __DLOG_CHRONO_H__
#define __DLOG_CHRONO_H__
#include <bits/stdc++.h>
#include "io.h"
#include "digits.h"
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
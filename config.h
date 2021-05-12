#ifndef __DLOG_CONFIG_H__
#define __DLOG_CONFIG_H__
#include <bits/stdc++.h>
namespace dlog {

struct StaticConfig {
    const char *path;
    size_t fileMaxSize;
    bool debugOn;
    bool infoOn;
    bool warnOn;
    bool errorOn;
    bool wtfOn;
    std::chrono::milliseconds fileRollingInterval;
    constexpr StaticConfig(const char *path, size_t fileMaxSize,
        bool debugOn, bool infoOn, bool warnOn, bool errorOn, bool wtfOn,
        std::chrono::milliseconds fileRollingInterval)
        : path(path),
          fileMaxSize(fileMaxSize),
          debugOn(debugOn),
          infoOn(infoOn),
          warnOn(warnOn),
          errorOn(errorOn),
          wtfOn(wtfOn),
          fileRollingInterval(fileRollingInterval) {}
};
// constexpr const StaticConfig &staticConfig





namespace conf { // internal, dont use outside

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
    } _type;

    const char *_path {"log_default.log"};
    size_t _fileMaxSize {};
    size_t _logOpenFlags[5] {};
    std::chrono::milliseconds _fileRollingInterval {24h};

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
                        _path = l._path;
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
                    default:
                    break;
                }
            }
        }

    constexpr void copy(const config &rhs) {
        _type = rhs._type;
        _path = rhs._path;
        _fileMaxSize = rhs._fileMaxSize;
        _fileRollingInterval = rhs._fileRollingInterval;
        for(size_t i = 0; i < sizeof(_logOpenFlags)/sizeof(decltype(_logOpenFlags[0])); ++i) {
            _logOpenFlags[i] = rhs._logOpenFlags[i];
        }
    }

    constexpr operator StaticConfig() {
        return StaticConfig(
            _path, _fileMaxSize,
            _logOpenFlags[0], _logOpenFlags[1], _logOpenFlags[2], _logOpenFlags[3], _logOpenFlags[4],
            _fileRollingInterval
        );
    }
};

struct log_path: public config {
    explicit constexpr log_path(const char *path)
        : config(type::LOG_PATH) { _path = path; }
};

struct log_filter: public config {
    explicit constexpr log_filter(std::initializer_list<log_level> levels)
        : config(type::LOG_FILTER) {
        for(const auto &level : levels) {
            _logOpenFlags[level] = true;
        }
    }
};

// need -Wno-literal-suffix
inline constexpr unsigned long long operator""B(unsigned long long x) { return x; }
inline constexpr unsigned long long operator""KB(unsigned long long x) { return 1024B * x; }
inline constexpr unsigned long long operator""MB(unsigned long long x) { return 1024KB * x; }
inline constexpr unsigned long long operator""GB(unsigned long long x) { return 1024MB * x; }

struct file_max_size: public config {
    explicit constexpr file_max_size(size_t fileMaxSize)
        : config(type::MAX_FILE_SIZE) { _fileMaxSize = fileMaxSize; }
};


struct file_rolling_interval: public config {
    explicit constexpr file_rolling_interval(std::chrono::milliseconds fileRollingInterval)
        : config(type::ROLLING) { _fileRollingInterval = fileRollingInterval; }
};

constexpr StaticConfig globalConfigBoot {
#include "dlog.conf"
};

} // conf




constexpr const StaticConfig &staticConfig = conf::globalConfigBoot;

} // dlog
#endif

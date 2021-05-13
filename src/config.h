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
    constexpr StaticConfig(const char *log_dir, const char *log_filename, const char *log_filename_extension,
        size_t fileMaxSize,
        bool debugOn, bool infoOn, bool warnOn, bool errorOn, bool wtfOn,
        std::chrono::milliseconds fileRollingInterval)
        : log_dir(log_dir),
          log_filename(log_filename),
          log_filename_extension(log_filename_extension),
          fileMaxSize(fileMaxSize),
          debugOn(debugOn),
          infoOn(infoOn),
          warnOn(warnOn),
          errorOn(errorOn),
          wtfOn(wtfOn),
          fileRollingInterval(fileRollingInterval) {}
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
    } _type;

    const char *_path {"log_default.log"};

    const char *_log_dir {"."};
    const char *_log_filename {"log_default"};
    const char *_log_filename_extension {".log"};
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
            _fileRollingInterval
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

constexpr static StaticConfig globalConfigBoot {
#include "../dlog.conf"
};

} // conf




constexpr const static StaticConfig &staticConfig = conf::globalConfigBoot;

} // dlog
#endif
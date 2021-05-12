#ifndef __DLOG_FS_H__
#define __DLOG_FS_H__
#include <unistd.h>
#include <fcntl.h>
#include <bits/stdc++.h>
#include "config.h"
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
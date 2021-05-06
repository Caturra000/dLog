#ifndef __DLOG_FS_H__
#define __DLOG_FS_H__
#include <unistd.h>
#include <fcntl.h>
#include <bits/stdc++.h>
namespace dlog {

// a very simple file util class, used for append-write
class File {
public:
    explicit File(const char *path): _fd(::open(path, OPEN_FLAG, OPEN_MODE)) { // blocking
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

    int fd() { return _fd; }
    void detach() { _fd = INVALID_FD; }
    void append(const char *buf, size_t count) { ::write(_fd, buf, count); }
    void swap(File &that) { std::swap(this->_fd, that._fd); }

private:
    constexpr static int INVALID_FD = -1;
    constexpr static int OPEN_FLAG = O_RDWR | O_CREAT | O_APPEND | O_CLOEXEC;
    constexpr static int OPEN_MODE = 0644;
    int _fd;
};

} // dlog
#endif
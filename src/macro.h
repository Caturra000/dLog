#ifndef __DLOG_MACRO_H__
#define __DLOG_MACRO_H__

#define DLOG_DEBUG(...) Log::debug(filename(__FILE__), __LINE__, __VA_ARGS__)
#define DLOG_INFO(...) Log::info(filename(__FILE__), __LINE__, __VA_ARGS__)
#define DLOG_WARN(...) Log::warn(filename(__FILE__), __LINE__, __VA_ARGS__)
#define DLOG_ERROR(...) Log::error(filename(__FILE__), __LINE__, __VA_ARGS__)
#define DLOG_WTF(...) Log::wtf(filename(__FILE__), __LINE__, __VA_ARGS__)

#define DLOG_DEBUG_ALIGN(...) Log::debug(filename(__FILE__), __LINE__, \
    Whitespace<(size_t)std::max(ssize_t(1), ssize_t(staticConfig.msg_align) \
    - ssize_t(lastPathComponentSize(__FILE__)+Stream::parseLength(__LINE__)))>::buf,__VA_ARGS__) // line+2

#define DLOG_INFO_ALIGN(...) Log::info(filename(__FILE__), __LINE__, \
    Whitespace<(size_t)std::max(ssize_t(1), ssize_t(staticConfig.msg_align) \
    - ssize_t(lastPathComponentSize(__FILE__)+Stream::parseLength(__LINE__)))>::buf,__VA_ARGS__)

#define DLOG_WARN_ALIGN(...) Log::warn(filename(__FILE__), __LINE__, \
    Whitespace<(size_t)std::max(ssize_t(1), ssize_t(staticConfig.msg_align) \
    - ssize_t(lastPathComponentSize(__FILE__)+Stream::parseLength(__LINE__)))>::buf,__VA_ARGS__)

#define DLOG_ERROR_ALIGN(...) Log::error(filename(__FILE__), __LINE__, \
    Whitespace<(size_t)std::max(ssize_t(1), ssize_t(staticConfig.msg_align) \
    - ssize_t(lastPathComponentSize(__FILE__)+Stream::parseLength(__LINE__)))>::buf,__VA_ARGS__)

#define DLOG_WTF_ALIGN(...) Log::wtf(filename(__FILE__), __LINE__, \
    Whitespace<(size_t)std::max(ssize_t(1), ssize_t(staticConfig.msg_align) \
    - ssize_t(lastPathComponentSize(__FILE__)+Stream::parseLength(__LINE__)))>::buf,__VA_ARGS__)

namespace dlog {}
#endif
#ifndef __DLOG_MACRO_H__
#define __DLOG_MACRO_H__

#define DLOG_DEBUG(...) Log::debug(__FILE__, __LINE__, __VA_ARGS__)
#define DLOG_INFO(...) Log::info(__FILE__, __LINE__, __VA_ARGS__)
#define DLOG_WARN(...) Log::warn(__FILE__, __LINE__, __VA_ARGS__)
#define DLOG_ERROR(...) Log::error(__FILE__, __LINE__, __VA_ARGS__)
#define DLOG_WTF(...) Log::wtf(__FILE__, __LINE__, __VA_ARGS__)

#define DLOG_DEBUG_ALIGN(...) Log::debug(__FILE__, __LINE__, \
    Whitespace<(size_t)std::max(ssize_t(1), ssize_t(staticConfig.msg_align) \
    - ssize_t(sizeof(__FILE__)+Stream::parseLength(__LINE__)))>::buf,__VA_ARGS__) // line+2

#define DLOG_INFO_ALIGN(...) Log::info(__FILE__, __LINE__, \
    Whitespace<(size_t)std::max(ssize_t(1), ssize_t(staticConfig.msg_align) \
    - ssize_t(sizeof(__FILE__)+Stream::parseLength(__LINE__)))>::buf,__VA_ARGS__)

#define DLOG_WARN_ALIGN(...) Log::warn(__FILE__, __LINE__, \
    Whitespace<(size_t)std::max(ssize_t(1), ssize_t(staticConfig.msg_align) \
    - ssize_t(sizeof(__FILE__)+Stream::parseLength(__LINE__)))>::buf,__VA_ARGS__)

#define DLOG_ERROR_ALIGN(...) Log::error(__FILE__, __LINE__, \
    Whitespace<(size_t)std::max(ssize_t(1), ssize_t(staticConfig.msg_align) \
    - ssize_t(sizeof(__FILE__)+Stream::parseLength(__LINE__)))>::buf,__VA_ARGS__)

#define DLOG_WTF_ALIGN(...) Log::wtf(__FILE__, __LINE__, \
    Whitespace<(size_t)std::max(ssize_t(1), ssize_t(staticConfig.msg_align) \
    - ssize_t(sizeof(__FILE__)+Stream::parseLength(__LINE__)))>::buf,__VA_ARGS__)

namespace dlog {}
#endif
#ifndef __DLOG_MACRO_H__
#define __DLOG_MACRO_H__

#define DLOG_DEBUG(...) Log::debug(dlog::filename(__FILE__), __LINE__, __VA_ARGS__)
#define DLOG_INFO(...) Log::info(dlog::filename(__FILE__), __LINE__, __VA_ARGS__)
#define DLOG_WARN(...) Log::warn(dlog::filename(__FILE__), __LINE__, __VA_ARGS__)
#define DLOG_ERROR(...) Log::error(dlog::filename(__FILE__), __LINE__, __VA_ARGS__)
#define DLOG_WTF(...) Log::wtf(dlog::filename(__FILE__), __LINE__, __VA_ARGS__)

#define DLOG_DEBUG_ALIGN(...) Log::debug(dlog::filename(__FILE__), __LINE__, \
    dlog::meta::Whitespace<(size_t)std::max(ssize_t(1), ssize_t(dlog::staticConfig.msg_align) \
    - ssize_t(dlog::lastPathComponentSize(__FILE__)+dlog::StreamBase::parseLength(__LINE__)))>::buf,__VA_ARGS__) // line+2

#define DLOG_INFO_ALIGN(...) Log::info(dlog::filename(__FILE__), __LINE__, \
    dlog::meta::Whitespace<(size_t)std::max(ssize_t(1), ssize_t(dlog::staticConfig.msg_align) \
    - ssize_t(dlog::lastPathComponentSize(__FILE__)+dlog::StreamBase::parseLength(__LINE__)))>::buf,__VA_ARGS__)

#define DLOG_WARN_ALIGN(...) Log::warn(dlog::filename(__FILE__), __LINE__, \
    dlog::meta::Whitespace<(size_t)std::max(ssize_t(1), ssize_t(dlog::staticConfig.msg_align) \
    - ssize_t(dlog::lastPathComponentSize(__FILE__)+dlog::StreamBase::parseLength(__LINE__)))>::buf,__VA_ARGS__)

#define DLOG_ERROR_ALIGN(...) Log::error(dlog::filename(__FILE__), __LINE__, \
    dlog::meta::Whitespace<(size_t)std::max(ssize_t(1), ssize_t(dlog::staticConfig.msg_align) \
    - ssize_t(dlog::lastPathComponentSize(__FILE__)+dlog::StreamBase::parseLength(__LINE__)))>::buf,__VA_ARGS__)

#define DLOG_WTF_ALIGN(...) Log::wtf(dlog::filename(__FILE__), __LINE__, \
    dlog::meta::Whitespace<(size_t)std::max(ssize_t(1), ssize_t(dlog::staticConfig.msg_align) \
    - ssize_t(dlog::lastPathComponentSize(__FILE__)+dlog::StreamBase::parseLength(__LINE__)))>::buf,__VA_ARGS__)

namespace dlog {}
#endif
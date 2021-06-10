#ifndef __DLOG_MIXIN_H__
#define __DLOG_MIXIN_H__
namespace dlog {
namespace meta {

template <typename ...Ts>
struct Mixin: public Ts... {};

} // meta
} // dlog
#endif
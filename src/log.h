#ifndef __DLOG_LOG_H__
#define __DLOG_LOG_H__
#include <bits/stdc++.h>
#include "logbase.h"
#include "tags.h"
#include "policies.h"
namespace dlog {

struct VoidExtend {};

// StreamBase or StreamExtend<...>
using Stream = StreamExtend<policy::Specialization>;

// used in LogBaseImpl
// ResolverBase<Stream> or ResolverExtend<Stream, ...>
using Resolver = ResolverExtend<Stream, policy::Whitespace>;

// no extend
using Scheduler = SchedulerBase<Resolver>;

// prefix tags
template <typename ...Tags>
using LogExtend = LogBase<Resolver, Scheduler, Tags...>;

using Log = LogExtend<DateTimeTag, ThreadIdTag>;

} // dlog
#endif
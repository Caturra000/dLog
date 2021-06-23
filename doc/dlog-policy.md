# dLog Policy

## Policies

`dlog`提供一定的policy来定制输出方案和支持类型，后续会提供更多支持（如fd定制、schedule定制）

目前已经提供：

- `policy::Whitespace`
- `policy::NoWhitespace`
- `policy::Colorful`
- `policy::Less`
- `policy::Chatty`
- `policy::Specialization`

这些在`src/policies.h`里有完整实现



## Whitespace / NoWhitespace

`Whitespace`是默认使用的方案：传入`log`的每个参数都会使用空格隔离

`NoWhitespace<N>`则相反，传入`log`的参数是紧接着输出的，`N`是忽略项数，前`N`个参数依然按照空格隔开



## Colorful

`Colorful`提供代码高亮的输出

可以组合其它`policy`使用

比如`policy::Colorful<policy::Whitespace>`

也提供装饰器类`policy::ColorfulDecorator`



## Less

`Less<N>`允许每条日志只输出前`N`个参数

同样可以组合使用，如`policy::Less<policy::NoWhitespace<4>, 20>`

也提供装饰器类`policy::LessDecorator`，可以`policy::Colorful< policy::LessDecorator<policy::NoWhitespace<4>, 20> >`



## Chatty

`Chatty`用于过滤相同或相似的日志（每线程间判断）

目前实现比较简单，用的低配版`simhash`实现

也提供装饰器类`policy::ChattyDecorator`

使用方法同`Less / Colorful`



## Specialization

`Specialization`支持任意类型输出到`log`，且是非侵入式的，需要为传入参数的类型提供特化实现

使用方式类似`std::hash`

栗子如下

```C++
struct Point {
    int x, y, z;
};

// usage:  Log::debug( Point{1, 2, 3} );
// output: [1,2,3]

namespace dlog {
namespace policies {
    template <>
    struct Specialization::ExtraStream<Point> {
        static void parse(char *buf, const Point &p, size_t length) {
            int vs[] = {p.x, p.y, p.z};
            size_t cur = 0;
            buf[cur++] = '[';
            for(size_t i = 0, len; i < 3; ++i) {
                len = StreamBase::parseLength(vs[i]);
                StreamBase::parse(&buf[cur], vs[i], len);
                cur += len;
                if(i != 2) buf[cur++] = ',';
                else buf[cur++] = ']';
            }
        }

        static size_t parseLength(const Point &p) {
            int vs[] = {p.x, p.y, p.z};
            size_t len = 4;
            for(auto v :vs) {
                len += StreamBase::parseLength(v);
            }
            return len;
        }
    };
}
}
```



## 不使用特化实现输出支持

特化版本主要是写着费劲，因此提供第二种途径

`dlog::StreamExtend`支持多个`policy`

默认情况为`Stream = StreamExtend<policy::Specialization>`

你可以定制`Stream = StreamExtend<policy::Specialization, JsonSupport, ...>`来支持`Json`（或其它任何类型）

只需要`JsonSupport`（名字无所谓）实现`parse(char*, const Json&, size_t)`和`parseLength(const Json&)`接口（见`src/stream.h`）

N个输出类支持M个类型也是允许的，只要函数签名不冲突



## 使用policy

默认的`log.h`已经提供好开箱即用的`Log`

因此，如果需要使用上述的`policy`，不应该`#include "log.h"`，而是`logbase.h`

只需参考`log.h`如何调用即可

示例

```C++
#include "src/logbase.h"
#include "src/tags.h"
#include "src/policies.h"

namespace dlog {

using Stream = StreamExtend<policy::Specialization>;
using Resolver = ResolverExtend<Stream, policy::Chatty<policy::ColorfulDecorator<policy::Whitespace>>>; // here
using Scheduler = SchedulerBase<Resolver>;
template <typename ...Tags>
using LogExtend = LogBase<Resolver, Scheduler, Tags...>;
using Log = LogExtend<DateTimeTag, ThreadIdTag>; // here

} // dlog
```

（TODO `dlog.hpp`单文件需要自己写个宏，或者删掉`using`自己写，有空我简化下流程）



## 定制自己的policy

可参考`policies.h`

你需要熟悉`CRTP`（奇异递归模板模式）技巧

`Resolver`部分已提供接口类`PutInterface`简化实现流程

`Stream`部分可直接参考`Specialization`的实现
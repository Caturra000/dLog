# dLog Design

## 设计模型

`dlog`的设计模型非常的直接简单

```
          +---------+     +-----------+     +----------+     +-----------+
Input --> |   Log   | --> |  LogBase  | --> | Resolver | --> | Scheduler | --> Output
          +---------+     +-----------+     +----------+     +-----------+
                                                 |^
                                                 ||
                                                 v|
                                            +----------+
                                            |  Stream  |
                                            +----------+
```

- `Log`面向用户，决定所有policy，也决定了日志的Tag
- `LogBase`处理前端接收的参数，以及处理Tag，其实`Log`也只是`LogBase`的实例化
- `Resolver`解析参数类型，面向iov，也充当拦截器，符合条件的类型不会经过`Stream`
- `Stream`辅助`Resolver`确定类型的序列化，与`Resolver`不同，它是面向对象的
- `Scheduler`实现异步模型
- 上述所有类型均为`class template`形式

## 一些抽象

接下来解释各种乱七八糟的名词

- `policy`：策略模式，dlog核心类均是policy-based的设计（大概就是低配版`STL`），目前能用于扩展的有`Resolver`和`Stream`，分别为定制输出和支持不同类型，举例的话就是非侵入式地支持一个五颜六色(highlight)的日志用来输出`json`
- `Tag`：一种适配器接口，只要使用特化适配了`format()`接口，均可应用于log的前缀输出，目前的date-time / thread-id / log-level等等信息都是通过`Tag`适配器实现的，并且支持编译时类型排序确定前后输出的优先级
- `Iov`：全称`IoVector`，对，名字就是抄的linux内核的`iovec`，干的事情也一样。它是一个很简单的类，描述了某个对象的地址和它的长度，`Iov`保证它描述的对象是生命周期安全的，因此可以传递给异步的`Scheduler`，同时，它也是优化性能的关键

## utils

实现过程写了一些小工具，模块比较独立，稍微改一下就能复用于其它项目

- `chrono.h`：适用于`std::chrono`的日期时间库，算法实现日期时间的输出
  - 给出`std::chrono::time_point`，得到一个日期时间如1926-08-17 12:34:56.789
  - 写日志库的时候发现标准库连像样的date-time都没有，就写了这个
  - 如果尝试单独使用，需要去除`IoVector`（可以换成`std::string`）和`meta`（直接打表）
- `trace.h`：用于输出调用堆栈的工具
  - 依赖于`gcc`的`execinfo.h`，简单封装为modern C++口味
  - 虽然我也没用过（
- `m*.h`：一系列的元编程操作
  - `msort.h`：编译时排序
  - `mixin.h`：就是`Mixin`
  - `mstr.h`：功能多了去了（同时也是编译速度感人的源头）
    - `MetaString<'a', 'b', 'c'>::buf = "abc"`
    - `ContinuousMetaString<3, 'a'>::type = MetaString<'a', 'a', 'a'>`
    - `Whitespace<5>::buf = "     "`
    - `Numeric<123>::type = MetaString<'1', '2', '3', '\0'>`
    - `NumericMetaStringsSequence<3>::bufs[] = {"0", "1", "2"};`
    - `LeadingZeroNumeric<123, 5>::type = MetaString<'0', '0', '1', '2', '3', '\0'>`
    - `LeadingZeroNumericArray<12, 3>::buf = { "000", "001", "002", ... "010", "011" }`


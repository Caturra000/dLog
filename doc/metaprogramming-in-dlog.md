# Metaprogramming in dLog



## 前言

我在尝试使用元编程来简化项目的开发流程和提高运行时性能，也希望能一定程度上证明元编程不是一种纯粹拿来炫技的玩具

这篇文章会大概整理一下`dlog`里元编程相关的内容，包括应用和实现

（限于本咸鱼的水平比较菜，以及日志库本身也很简单，所以并没多少内容）



## 元编程用在哪里

主要是三个方面

- 给类型排序，模板中不同类型的执行顺序直接由编译时声明的优先级确定
- 缓存，需要解析的数字不必再次计算了，同时也不希望用打表到代码里的原始方式来完成
- 格式化，一些固定的format操作能编译时解决就不必放到运行时来处理了



## 排序

先从一个小细节开始

这是`Log`的入口，通过`Log::info`可以生成一条如下的日志消息

```
2021-06-22 05:14:34.845 I 1350 /test.cpp 10           998 244 353
^ Date     ^ Time       ^ ^
					     LogLevel (I = info)
					       ThreadId
```

不考虑后面的内容，前缀输出顺序为

1. 日期
2. 时间
3. 日志等级
4. 线程ID

它的接口调用流程如下

```C++
// ===================================================
// file: log.h

// 这个Log别名表示：在每条日志信息上，都输出线程ID和日期时间
// 即使换成LogExtend<ThreadIdTag, DateTimeTag>也是一样的输出
using Log = LogExtend<DateTimeTag, ThreadIdTag>;

// ===================================================
// file: logbase.h

// 调用Log::info
template <typename ResolverImpl, typename SchedulerImpl, typename ...Tags>
template <typename ...Ts>
inline void LogBase<ResolverImpl, SchedulerImpl, Tags...>::info(Ts &&...msg) {
    if(staticConfig.infoOn) logFormat<LogLevel::INFO>(std::forward<Ts>(msg)...);
}

// 此时Tags有DateTimeTag ThreadIdTag, LEVEL为LogLevel::INFO
template <typename ResolverImpl, typename SchedulerImpl, typename ...Tags>
template <LogLevel LEVEL, typename ...Ts>
inline void LogBase<ResolverImpl, SchedulerImpl, Tags...>::logFormat(Ts &&...msg) {
    // 通过适配器，使得LogLevel也成为一种Tag类型，放入Tags中
    using SortedTagsTuple = typename meta::Sort<LogLevelTag<LEVEL>, Tags...>::type;
    LogBaseFacade<ResolverImpl, SchedulerImpl, SortedTagsTuple>::log(std::forward<Ts>(msg)...);
}

// ...
```

经过`meta::Sort`接口，会生成`std::tuple<DateTimeTag, LogLevelTag<LogLevel::INFO>, ThreadIdTag>`

这些`Tag`类型都有约定的`format()`接口，最终会依照顺序输出到日志后端缓冲

你可以认为，`meta::Sort`实现的是比较另类的执行顺序上的多态：`LogLevel`可以插入到`DateTimeTag`和`ThreadIdTag`的中间

其实它们之间的顺序依赖于一个优先级`Elem`

```C++
// file: tags.h

template <typename V>
struct Elem;

template <>
struct Elem<DateTimeTag>: Key<DateTimeTag>, Value<1> {};

template <LogLevel LEVEL>
struct Elem<LogLevelTag<LEVEL>>: Key<LogLevelTag<LEVEL>>, Value<2> {};

template <>
struct Elem<ThreadIdTag>: Key<ThreadIdTag>, Value<3> {};
```

因此，要确定`Log`接口的`Tag`输出顺序，只需特化一个`Elem`，声明它的优先级`Value`即可

这样的实现方式有什么好处呢

- 直观：可以帮助快速确认日志的格式，看优先级总比翻一堆代码要方便
- 易于扩展：如果要自定义一个`Tag`，根本不用考虑`Log`里面的模板是什么东西，往后面加上去就行了（facebook/folly似乎也有这种做法）
- 控制内部实现：在`LogBase`里，`LogLevel`其实是内部实现的`Tag`，但是可以通过`Elem`轻易调整它的优先级，再也不用梳理藏得特别深的代码流程了
- 解决本来就是`template`自身的问题：对于可变参数模板来说，如`Args...`，要加一个新的`Arg`，能怎么办？要么`Args..., Arg`，要么`Arg, Args...`，不用一定的技巧没法插入到`Args...`中间



## 缓存

这个接口很简单，我这里简单做了个`Numeric`的值缓存

```C++
// file: resolve.h
template <typename StreamImpl>
inline void NonPutResolverBase<StreamImpl>::resolveDispatch(ResolveContext &ctx, size_t msg) {
    constexpr static size_t limit = 10000;
    // 轻易地生成10000个cache
    using Cache = meta::NumericMetaStringsSequence<limit>;
    if(msg < limit) {
        resolveDispatch(ctx, IoVector{Cache::bufs[msg], Cache::len[msg]});
    } else {
        resolveDispatch<>(ctx, msg);
    }
}

// 如果被cache命中，只是简单的更新ctx
template <typename StreamImpl>
template <size_t N>
inline void NonPutResolverBase<StreamImpl>::resolveDispatch(ResolveContext &ctx, const char (&msg)[N]) {
    static_assert(N >= 1, "N must be positive.");
    size_t len = N-1;
    ctx.updateExternal(msg, len);
}

// 如果没有被cache命中（走else分支），只能依靠stream来计算msg了
template <typename StreamImpl>
template <typename T>
inline void NonPutResolverBase<StreamImpl>::resolveDispatch(ResolveContext &ctx, T &&msg) {
    size_t len = StreamImpl::parseLength(std::forward<T>(msg));
    char *buf = ctx.currentLocal();
    // 一个相对耗时的操作
    StreamImpl::parse(buf, std::forward<T>(msg), len);
    ctx.updateLocal(len);
}
```

这样如果需要`resolve(ctx, size_t(2333))`，从`Cache`中直接拿到字符串字面值`"2333\0"`返回即可

想要增大点缓存的话，只需改动`limit`的值即可，连代码生成器都省了



## 格式化

格式化其实也是类似缓存的思路，最频繁使用的就是`date-time`的格式化处理

区别的话，就是元编程实现要复杂..一点

```C++
// file: chrono.h
// 传入time_point会格式化为形如["1970-01-01", "00:00:00.000"]字面值的大小为2的数组
inline std::array<IoVector, 2> Chrono::format(std::chrono::system_clock::time_point point) {
    using namespace std::chrono;
    using namespace std::chrono_literals;
    // Digits100固定宽度为2，范围00-99，可以对小时、分钟、月、日进行格式化
    // Digits1000固定宽度为3，可以对毫秒格式化
    // Digits2050范围0000-2049，处理年部分
    using Digits100  = meta::LeadingZeroNumericArray<100, 2>;
    using Digits1000 = meta::LeadingZeroNumericArray<1000, 3>;
    using Digits2050 = meta::LeadingZeroNumericArray<2050, 4>;
    constexpr static Digits100::type  &digits100  = Digits100::buf;
    constexpr static Digits1000::type &digits1000 = Digits1000::buf;
    constexpr static Digits2050::type &digits2050 = Digits2050::buf;
    static thread_local char dateRecord[] = "1970-01-01";
    static thread_local char timeRecord[] = "00:00:00.000";
    static thread_local system_clock::time_point last {};

    // 具体的格式化操作，可以看到就是简单的memcpy
    auto duration = point - last;
    auto dateTime = getDateTime(point);
    if(duration >= 24h) {
        last = thisDay(point);
        // update date
        std::memcpy(dateRecord, digits2050[dateTime.year()], 4);
        std::memcpy(dateRecord + 5, digits100[dateTime.month()], 2);
        std::memcpy(dateRecord + 8, digits100[dateTime.day()], 2);
    }

    // update time
    std::memcpy(timeRecord, digits100[dateTime.hour()], 2);
    std::memcpy(timeRecord + 3, digits100[dateTime.minute()], 2);
    std::memcpy(timeRecord + 6, digits100[dateTime.second()], 2);
    std::memcpy(timeRecord + 9, digits1000[dateTime.millisecond()], 3);

    return {
        IoVector {
            .base = dateRecord,
            .len = sizeof(dateRecord)-1
        },
        IoVector {
            .base = timeRecord,
            .len = sizeof(timeRecord)-1
        }
    };
}
```

当然一些比较简单的可以直接用`constexpr`硬算，如`LogLevel`

```C++
// file: level.h
enum LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR,
    WTF,

    LOG_LEVEL_LIMIT
};

// 分别返回字符 D / I / W / E / ?
template <LogLevel LEVEL>
static constexpr char levelFormat() noexcept {
    static_assert(LEVEL >= 0 && LEVEL < LOG_LEVEL_LIMIT, "check log level config.");
    return "DIWE?"[LEVEL];
}
```

还有处理空格对齐的部分，我们可以知道像线程ID、日期时间这些都是固定长度的，但是因为`filename`是不等长的，所以会导致一些可读性的问题

一个对比的例子

```
// 淦输出的是啥
2021-06-22 05:14:34.845 W 1350 /test.cpp 10 998 244 353
2021-06-22 05:14:34.845 E 11200 /test123.cpp 155 998 244 353

// 一目了然
2021-06-22 05:14:34.845 W 1350 /test.cpp 10          998 244 353
2021-06-22 05:14:34.845 E 11200 /test123.cpp 155     998 244 353
```

要对齐的话，一个思路就是编译时推算出前面这些日志前缀的长度

```C++
#define DLOG_INFO_ALIGN(...) Log::info(dlog::filename(__FILE__), __LINE__, \
    dlog::meta::Whitespace<(size_t)std::max(ssize_t(1), ssize_t(dlog::staticConfig.msg_align) \
    - ssize_t(dlog::lastPathComponentSize(__FILE__)+dlog::StreamBase::parseLength(__LINE__)))>::buf,__VA_ARGS__)

```

这里生成空格的类型为`meta::Whitespace<N>`，由于里面`<>`的函数都是`constexpr`返回的`size_t`

每一种长度`N`，对应一个独立的类型`meta::Whitespace<N>`，里面有一个长度为`N`的`buf`字面值



## 排序实现

见链接

[实现编译时排序 – Caturra's blog](http://www.caturra.cc/2021/05/15/逐渐变态实现编译时排序/)



## 缓存实现

首先需要一个表示字符串字面值的类

```C++
// example: MetaString<'a', 'b', 'c'>::buf = "abc" ('\0')
template <char ...Chars>
struct MetaString {
    constexpr static char buf[sizeof...(Chars)] = { Chars... };
};
```

接着对每个数位进行简单运算

```C++
constexpr inline int abs(int I) { return I < 0 ? abs(-I) : I; }
constexpr inline size_t length(int I) { return I < 0 ? 1 + length(-I) : I < 10 ? 1 : 1 + length(I / 10); }

template<size_t N, int I, char... Chars>
struct NumericMetaStringBuilder {
    using type = typename NumericMetaStringBuilder<N - 1, I / 10, '0' + abs(I) % 10, Chars...>::type;
};
template<int I, char... Chars>
struct NumericMetaStringBuilder<2, I, Chars...> {
    using type = MetaString<I < 0 ? '-' : '0' + I / 10, '0' + abs(I) % 10, Chars...>;
};
template<int I, char... Chars>
struct NumericMetaStringBuilder<1, I, Chars...> {
    using type = MetaString<'0' + I, Chars...>;
};
```

然后就能对单个值进行cache

```C++
// example: Numeric<123>::type = MetaString<'1', '2', '3', '\0'>
template<int I>
struct Numeric {
    using type = typename NumericMetaStringBuilder<length(I), I, '\0'>::type;
};

template <int I>
using NumericMetaString = typename Numeric<I>::type;
```

最后构造出从`0`到`N`的cache字符串字面值数组

这里需要`index_sequence`辅助一下

```C++
template <typename>
struct NumericMetaStrings;

template <size_t ...Is>
struct NumericMetaStrings<std::index_sequence<Is...>> {
    constexpr const static char *bufs[] = { NumericMetaString<Is>::buf... };
    constexpr const static size_t len[] = { length(Is)... };
};

// example: NumericMetaStringsSequence<3>::bufs[] = {"0", "1", "2"};
template <size_t N>
struct NumericMetaStringsSequence {
    constexpr static auto bufs = NumericMetaStrings<std::make_index_sequence<N>>::bufs;
    constexpr static auto len = NumericMetaStrings<std::make_index_sequence<N>>::len;
};
```



## 格式化实现 - 对齐

同样基于`MetaString`，先加一个非常有用的拼接辅助类

```C++
template <typename ...Ts>
struct Concat;

template <char ...L, char ...R>
struct Concat<MetaString<L...>, MetaString<R...>> {
    using type = MetaString<L..., R...>;
};

template <char ...Chars>
struct Concat<MetaString<Chars...>> {
    using type = MetaString<Chars...>;
};
```

接着写一个抽象点的`Continuous`，一路concat下去即可

```C++
// example: ContinuousMetaString<3, 'a'>::type = MetaString<'a', 'a', 'a'>
template <size_t N, char C, bool Eof = true>
struct Continuous {
    using type = typename Concat<
        MetaString<C>,
        typename Continuous<N-1, C, Eof>::type
    >::type;
};

template <char C>
struct Continuous<1, C> {
    using type = MetaString<C, '\0'>;
};

template <char C>
struct Continuous<1, C, false> {
    using type = MetaString<C>;
};

template <char C>
struct Continuous<0, C> {
    using type = MetaString<'\0'>;
};

template <char C>
struct Continuous<0, C, false> {
    using type = MetaString<>;
};

template <size_t N, char C, bool Eof = true>
using ContinuousMetaString = typename Continuous<N, C, Eof>::type;
```

既然抽象的都完成了，那简单写个特殊情况即可

```C++
// example: Whitespace<5>::buf = "     "
template <size_t N>
using Whitespace = ContinuousMetaString<N, ' '>;
```

完成

## 格式化实现 - chrono

该实现是基于前面已有的`Numeric`和`Continuous`进行加强，过程比较。。。随意

可以在这里看到，元编程组合起来使用是非常强大的，意思就是拿连续的`0`和剩余的`numeric`进行拼接，同样返回`MetaString`

PS. 这里`typename P`应该写为总的长度，而不是前缀prefix，早期命名没留意，不改了

```C++
// example: LeadingZeroNumeric<123, 5>::type = MetaString<'0', '0', '1', '2', '3', '\0'>
template <int I, size_t P, char Z = '0'> // P(prefix) >= length(I)
struct LeadingZeroNumeric {
    constexpr static size_t len = length(I);
    using check = std::enable_if_t<P >= len && I >= 0>;
    using ztype = ContinuousMetaString<P-len, Z, false>;
    using ntype = NumericMetaString<I>;
    using type = typename Concat<ztype, ntype>::type; // MetaString
};

template <int I, size_t P, char Z = '0'>
using LeadingZeroNumericMetaString = typename LeadingZeroNumeric<I, P, Z>::type;
```

前面的是单个值的字符串字面值，我们同样需要把它构造为数组形式，表示`0-N`的`LeadingZero`

一个思路是，通过划分数组的方式来构造得到（这种方式不会引起数组退化为指针）

这里`buf[][N]`可以直接划分每`N`个为一组

```C++
template <typename MStr, size_t P>
struct MetaStringArray;

template <char ...Chars, size_t N>
struct MetaStringArray<MetaString<Chars...>, N> {
    constexpr static const char buf[][N] = { Chars... };
    using type = decltype(buf); // buf type
};

template <char ...Chars, size_t N>
constexpr const char MetaStringArray<MetaString<Chars...>, N>::buf[][N];
```

从这里开始，请不要在乎，不明觉厉的命名方式

需要注意的一点是，模板递归深度默认是900，而目前的写法很容易超出限制，

因此我想到的一个简单又不用二分的处理方式是，每500个template放在一起处理，因此引入中间层`LeadingZeroNumericArrayBuilderRange`

（我个人比较建议把代码顺序倒过来看）

```C++
template <size_t ...Is>
struct LeadingZeroNumericArrayBuilderImplSequence;

template <size_t P, size_t I, size_t ...Is>
struct LeadingZeroNumericArrayBuilderImplSequence<P, I, Is...> {
    using type = typename Concat<
        LeadingZeroNumericMetaString<I, P>,
        typename LeadingZeroNumericArrayBuilderImplSequence<P, Is...>::type
    >::type;
};

// 别问为什么把P I反过来
template <size_t P, size_t I>
struct LeadingZeroNumericArrayBuilderImplSequence<P, I> {
    using type = LeadingZeroNumericMetaString<I, P>;
};

template <typename, size_t>
struct LeadingZeroNumericArrayBuilderImpl;

template <size_t P, size_t ...Is>
struct LeadingZeroNumericArrayBuilderImpl<std::index_sequence<Is...>, P> {
    // type -> MetaString
    using type = typename LeadingZeroNumericArrayBuilderImplSequence<P, Is...>::type;
};

// solve ftemplate-depth limit
template <size_t Begin, size_t End, size_t P, bool Flag>
struct LeadingZeroNumericArrayBuilderRange {
    using type = typename LeadingZeroNumericArrayBuilderImpl<typename MakeSequence<Begin, End>::type, P>::type;
};

template <size_t Begin, size_t End, size_t P>
struct LeadingZeroNumericArrayBuilderRange<Begin, End, P, false> {
    using type = typename Concat<
        typename LeadingZeroNumericArrayBuilderRange<Begin, Begin+500, P, true>::type,
        typename LeadingZeroNumericArrayBuilderRange<Begin+500, End, P, (End-Begin<=1000)>::type
    >::type;
};

template <size_t N, size_t P>
struct LeadingZeroNumericArrayBuilder {
    using check = std::enable_if_t<N && P >= length(N-1)>;
    // type -> MetaString
    using type = typename LeadingZeroNumericArrayBuilderRange<0, N, P, (N<=500)>::type;
};

// example: LeadingZeroNumericArray<12, 3>::buf = { "000", "001", "002", ... "010", "011" } (const char[12][4])
template <size_t N, size_t P>
using LeadingZeroNumericArray = MetaStringArray<typename LeadingZeroNumericArrayBuilder<N, P>::type ,P+1>;

```
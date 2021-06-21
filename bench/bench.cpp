#include <bits/stdc++.h>

// note: you should manually install libraries (spdlog / glog...)

// dlog
#include "dlog/dlog.hpp"

// spdlog
#include "spdlog/logger.h"
#include "spdlog/async.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

// glog
#include "glog/logging.h"

template <typename F, typename ...Args>
auto bench(F &&f, Args &&...args) {
    using namespace std::chrono;
    auto start = system_clock::now();
    std::forward<F>(f)(std::forward<Args>(args)...);
    auto end = system_clock::now();
    auto delta = duration_cast<milliseconds>(end - start);
    return delta;
}

template <typename F, typename ...Args>
auto benchMultiThreads(int threads, F &&f, Args &&...args) {
    using namespace std::chrono;
    auto start = system_clock::now();
    std::vector<std::thread> tasks;
    while(threads--) {
        tasks.emplace_back(std::forward<F>(f), std::forward<Args>(args)...);
    }
    for(auto &&task : tasks) task.join();
    auto end = system_clock::now();
    auto delta = duration_cast<milliseconds>(end - start);
    return delta;
}

auto reportInit() {
    std::cout << std::setw(22) << "test"
              << std::setw(15) << "cost(ms)"
              << std::setw(17) << "total(msg)"
              << std::setw(23) << "consume(msg/sec)"
              << std::fixed << std::setprecision(2) << std::endl;
}

auto report(const std::string &test, std::chrono::milliseconds cost,
            int count/*per thread*/, int threads) {
    const std::string wrapped = "[" + test + "]";
    std::cout << std::setw(22) << wrapped
              << std::setw(15) << cost.count()
              << std::setw(17) << count * threads
              << std::setw(23) << double(count) / (1e-3 * cost.count()) * threads
              << std::endl;
}

int main() {
    constexpr int count = 1e6;
    reportInit();

    // spdlog
    // https://github.com/gabime/spdlog/blob/v1.x/bench/async_bench.cpp
    {
        auto spdlogBasicSingle = spdlog::basic_logger_st("basic_st/backtrace-on", "out/basic_st.log", false);
        auto spdlogCommon = [count](std::shared_ptr<spdlog::logger> log) {
            for(int i = 0; i < count; ++i) {
                log->info("Hello, world!");
            }
        };

        auto spdlogTestSingle = bench(spdlogCommon, spdlogBasicSingle);
        report("spdlog-basic st 1", spdlogTestSingle, count, 1);

        auto spdlogBasicMulti = spdlog::basic_logger_mt("basic_mt/backtrace-on", "out/basic_mt.log", false);
        auto spdlogTestMulti2 = benchMultiThreads(2, spdlogCommon, spdlogBasicMulti);
        report("spdlog-basic mt 2", spdlogTestMulti2, count, 2);
        auto spdlogTestMulti4 = benchMultiThreads(4, spdlogCommon, spdlogBasicMulti);
        report("spdlog-basic mt 4", spdlogTestMulti4, count, 4);
        auto spdlogTestMulti8 = benchMultiThreads(8, spdlogCommon, spdlogBasicMulti);
        report("spdlog-basic mt 8", spdlogTestMulti8, count, 8);


        auto threadPool = std::make_shared<spdlog::details::thread_pool>(500000 / 2, 1); // queue size limit: 500000
        auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("out/spdlog_async_block.log", false);
        auto spdlogAsyncBlock = std::make_shared<spdlog::async_logger>
                ("async_logger", std::move(fileSink), std::move(threadPool), spdlog::async_overflow_policy::block);

        auto spdlogTestMulti2Async = benchMultiThreads(2, spdlogCommon, spdlogAsyncBlock);
        report("spdlog-async mt 2 b", spdlogTestMulti2Async, count, 2);
        auto spdlogTestMulti4Async = benchMultiThreads(4, spdlogCommon, spdlogAsyncBlock);
        report("spdlog-async mt 4 b", spdlogTestMulti4Async, count, 4);
        auto spdlogTestMulti8Async = benchMultiThreads(8, spdlogCommon, spdlogAsyncBlock);
        report("spdlog-async mt 8 b", spdlogTestMulti8Async, count, 8);

        threadPool = std::make_shared<spdlog::details::thread_pool>(500000 / 2, 1);
        fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("out/spdlog_async_discard.log", false);
        auto spdlogAsyncDiscard = std::make_shared<spdlog::async_logger>
                ("async_discard", std::move(fileSink), std::move(threadPool), spdlog::async_overflow_policy::overrun_oldest);

        auto spdlogTestMulti2Discard = benchMultiThreads(2, spdlogCommon, spdlogAsyncDiscard);
        report("spdlog-async mt 2 d", spdlogTestMulti2Discard, count, 2);
        auto spdlogTestMulti4Discard = benchMultiThreads(4, spdlogCommon, spdlogAsyncDiscard);
        report("spdlog-async mt 4 d", spdlogTestMulti4Discard, count, 4);
        auto spdlogTestMulti8Discard = benchMultiThreads(8, spdlogCommon, spdlogAsyncDiscard);
        report("spdlog-async mt 8 d", spdlogTestMulti8Discard, count, 8);
    }

    // glog
    {
        google::InitGoogleLogging("glog");
        FLAGS_log_dir = "out";
        auto glogCommon = [count] {
            for(int i = 0; i < count; ++i) {
                LOG(INFO) << "Hello, world!";
            }
        };
        auto glogTestSingle = bench(glogCommon);
        report("glog st 1", glogTestSingle, count, 1);
        auto glogTestMulti2 = benchMultiThreads(2, glogCommon);
        report("glog mt 2", glogTestMulti2, count, 2);
        auto glogTestMulti4 = benchMultiThreads(2, glogCommon);
        report("glog mt 4", glogTestMulti4, count, 4);
        auto glogTestMulti8 = benchMultiThreads(8, glogCommon);
        report("glog mt 8", glogTestMulti8, count, 8);
    }

    // dlog
    {
        dlog::Log::init();
        auto dlogCommon = [count] {
            using namespace dlog;
            for(int i = 0; i < count; ++i) {
                DLOG_DEBUG_ALIGN("Hello, world!");
            }
        };
        auto dlogTestSingle = bench(dlogCommon);
        report("dlog-macro st 1", dlogTestSingle, count, 1);
        auto dlogTestMulti2 = benchMultiThreads(2, dlogCommon);
        report("dlog-macro mt 2", dlogTestMulti2, count, 2);
        auto dlogTestMulti4 = benchMultiThreads(4, dlogCommon);
        report("dlog-macro mt 4", dlogTestMulti4, count, 4);
        auto dlogTestMulti8 = benchMultiThreads(8, dlogCommon);
        report("dlog-macro mt 8", dlogTestMulti8, count, 8);
    }

    return 0;
}
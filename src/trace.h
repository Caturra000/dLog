#ifndef __DLOG_TRACE_H__
#define __DLOG_TRACE_H__
#include <execinfo.h>
#include <bits/stdc++.h>
namespace dlog {

// ref: http://www.gnu.org/software/libc/manual/html_node/Backtraces.html

// original C-style version
/* Obtain a backtrace and print it to stdout. */
// void
// print_trace (void)
// {
//   void *array[10];
//   char **strings;
//   int size, i;
//   size = backtrace (array, 10);
//   strings = backtrace_symbols (array, size);
//   if (strings != NULL)
//   {
//     printf ("Obtained %d stack frames.\n", size);
//     for (i = 0; i < size; i++)
//       printf ("%s\n", strings[i]);
//   }
//   free (strings);
// }


// static, low-cost version
// no copy-overhead
template <size_t Limit = 10>
inline std::pair<std::unique_ptr<char*[]>, int> getBacktraceStatic() {
    void *array[Limit];
    int size = ::backtrace(array, Limit);
    std::unique_ptr<char*[]> owner {::backtrace_symbols(array, size)};
    return {std::move(owner), size};
}

// dynamic version
// ease to use
inline std::vector<std::string> getBacktrace(size_t limit = 10) {
    std::vector<void*> array(limit);
    int size = ::backtrace(array.data(), limit);
    std::unique_ptr<char*[]> owner {::backtrace_symbols(array.data(), size)};
    auto strings = owner.get();
    std::vector<std::string> bt;
    for(int i = 0; i < size; ++i) {
        bt.emplace_back(strings[i]);
    }
    return bt;
}

// usage
// int main() {
//     auto bt = getBacktraceStatic();
//     auto strings = bt.first.get();
//     auto size = bt.second;
//     if (strings != NULL) {
//         printf ("Obtained %d stack frames.\n", size);
//         for (int i = 0; i < size; i++)
//             printf ("%s\n", strings[i]);
//     }
//     auto bt2 = getBacktrace();
//     std::cout << "Obtained " << bt2.size() << " stack frames." << std::endl;
//     for(auto &&b : bt2) {
//         std::cout << b << std::endl;
//     }
// }

} // dlog
#endif
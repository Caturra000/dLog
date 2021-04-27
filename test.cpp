#include <bits/stdc++.h>
#include "log.h"

// test-version


int main() {
    using namespace dlog;
    //auto t = Scheduler::start();
    Log::test(1, -223ll, "789", std::string("12.345"), 78.1, 78.0, 78.123445, 78.999, 0.0001);
    Log::test(1, 3, 4, 6, -3, 12.345, "are you ok?", -34.23);
    Log::test(-0.102);
    Log::test(123);
    Log::test(1, -223ll);
    //t.join();
    return 0;
}
#include <bits/stdc++.h>
#include "log.h"

// test-version


int main() {
    using namespace dlog;
    Log::init();
    // Log::info(1, -223ll, "789", "12.345", 78.1, 78.0, 78.123445, 78.999, 0.0001);
    // Log::info(1, -223ll, "789", "12.345", 78.1, 78.0, 78.123445, 78.999, 0.0001);
    // Log::info(1, -223ll, "789", "12.345", 78.1, 78.0, 78.123445, 78.999, 0.0001);
    // Log::info(1, 3, 4, 6, -3, 12.345, "are you ok?", -34.23);
    // Log::info(-0.102);
    // Log::info(123);
    // Log::info(1, -223ll);
    std::thread { [] { Log::info(1, -223ll, "123", "12.345", 78.1, 78.0, 78.123445, 78.999, 0.0001); }}.detach();
    std::thread { [] { Log::info(1, -223ll, "456", "12.345", 78.1, 78.0, 78.123445, 78.999, 0.0001); }}.detach();
    std::thread { [] { Log::info(1, -223ll, "789", "12.345", 78.1, 78.0, 78.123445, 78.999, 0.0001); }}.detach();
    std::thread { [] { std::this_thread::sleep_for(50ms); Log::info(1, 3, 4, 6, -3, 12.345, "are you ok?", -34.23); }}.detach();
    std::thread { [] { Log::info(-0.102); }}.detach();
    std::thread { [] { Log::info(123); }}.detach();
    std::thread { [] { Log::info(1, -223ll); }}.detach();
    return 0;
}
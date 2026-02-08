#pragma once
#include <cstdint>
#include <chrono>


inline uint64_t now_ms_monotonic() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        steady_clock::now().time_since_epoch()
    ).count();
    
}
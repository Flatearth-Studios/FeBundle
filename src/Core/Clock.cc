#include "FeBundle/Core/Clock.hpp"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <ctime>
#endif

namespace febundle {

    void Clock::Start() {
        this->startTime = getAbsoluteTime();
    }

    void Clock::Update() {
        if (startTime != 0.0) {
            this->elapsed = getAbsoluteTime() - startTime;
        }
    }

    void Clock::Stop() {
        this->startTime = 0.0;
    }

    float64 Clock::NowTime() {
        return getAbsoluteTime();
    }

    float64 Clock::getAbsoluteTime() {
#if defined(_WIN32)
        static LARGE_INTEGER freq = [] {
            LARGE_INTEGER f;
            QueryPerformanceFrequency(&f);
            return f;
            }();

        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        return static_cast<double>(counter.QuadPart) / static_cast<double>(freq.QuadPart);
#else
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        return static_cast<double>(now.tv_sec) + static_cast<double>(now.tv_nsec) * 1e-9;
#endif
    }

} // namespace febundle

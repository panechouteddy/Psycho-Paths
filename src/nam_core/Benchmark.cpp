#include "pch.h"
#include "Benchmark.h"

namespace nam
{
    UnMap<std::string, std::chrono::steady_clock::time_point> Benchmark::startTimes;

    void Benchmark::Start(const std::string& tag)
    {
        startTimes[tag] = std::chrono::steady_clock::now();
    }

    void Benchmark::End(const std::string& tag)
    {
        auto it = startTimes.find(tag);
        if (it == startTimes.end()) {
            _ASSERT(false && "[Benchmark] Tag inconnu");
            return;
        }

        auto elapsed = std::chrono::steady_clock::now() - it->second;
        i64 us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

        std::string unit;
        if (us < 1'000) {
            unit = " micros";
        }
        else if (us < 1'000'000) {
            us = us / 1'000;
            unit = " ms";
        }
        else {
            i64 s = us / 1'000'000;
            unit = " s";
        }

        Print("[Benchmark] ", tag.c_str(), " : ", us, unit.c_str());

        startTimes.erase(it);
    }
}

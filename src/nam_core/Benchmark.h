#pragma once
#include <chrono>
#include <unordered_map>
#include <string>
#include <iostream>

namespace nam
{
    class Benchmark {
    public:
        static void Start(const std::string& tag);
        static void End(const std::string& tag);

    private:
        static UnMap<std::string, std::chrono::steady_clock::time_point> startTimes;
    };
}

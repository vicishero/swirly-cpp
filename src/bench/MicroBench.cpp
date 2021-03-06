/*
 * The Restful Matching-Engine.
 * Copyright (C) 2013, 2018 Swirly Cloud Limited.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program; if
 * not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "MicroBench.hxx"

#include <map>
#include <regex>

#include <sys/syscall.h>
#include <unistd.h>

using namespace std;
using namespace swirly;

namespace swirly {
inline namespace tool {
namespace {
std::map<std::string, BenchmarkBase*> suite;
} // namespace
void register_benchmark(const std::string& name, BenchmarkBase& b)
{
    suite[name] = &b;
}
} // namespace tool
} // namespace swirly

int main(int argc, char* argv[])
{
    int ret = 1;
    try {
        print_header();

        if (argc > 1) {
            // Run specific test-cases according to regex arguments.
            for (int i{1}; i < argc; ++i) {
                const regex pat{argv[i]};
                for (auto& [name, b] : suite) {
                    if (regex_match(name, pat)) {
                        b->run();
                    }
                }
            }
        } else {
            // Run all test-cases.
            for (auto& [name, b] : suite) {
                b->run();
            }
        }
        ret = 0;
    } catch (const exception& e) {
        cerr << "exception: " << e.what();
    }
    return ret;
}

SWIRLY_BENCHMARK(clock_gettime_real)
{
    return []() {
        timespec ts;
        ::clock_gettime(CLOCK_REALTIME, &ts);
    };
}

SWIRLY_BENCHMARK(sys_clock_gettime_real)
{
    return []() {
        timespec ts;
        syscall(SYS_clock_gettime, CLOCK_REALTIME, &ts);
    };
}

SWIRLY_BENCHMARK(clock_gettime_mono)
{
    return []() {
        timespec ts;
        ::clock_gettime(CLOCK_MONOTONIC, &ts);
    };
}

SWIRLY_BENCHMARK(sys_clock_gettime_mono)
{
    return []() {
        timespec ts;
        syscall(SYS_clock_gettime, CLOCK_MONOTONIC, &ts);
    };
}

SWIRLY_BENCHMARK(cycl_time_set)
{
    return []() { CyclTime::now(); };
}

SWIRLY_BENCHMARK(cycl_time_set_and_get)
{
    return []() {
        auto now = CyclTime::now();
        clobber_memory();
        auto t = now.mono_time();
        do_not_optimise(t);
    };
}

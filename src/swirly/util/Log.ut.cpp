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
#include "Log.hpp"

#include <swirly/util/Finally.hpp>

#include <boost/test/unit_test.hpp>

#include <cstring>

using namespace std;
using namespace swirly;

namespace {

template <typename T, typename U>
struct Foo {
    T first;
    U second;
};

ostream& operator<<(ostream& os, const Foo<int, int>& val)
{
    return os << '(' << val.first << ',' << val.second << ')';
}

int last_level{};
string last_msg{};

void test_logger(int level, string_view msg)
{
    last_level = level;
    last_msg.assign(msg.data(), msg.size());
}

} // namespace

BOOST_AUTO_TEST_SUITE(LogSuite)

BOOST_AUTO_TEST_CASE(LogLabelCase)
{
    BOOST_TEST(strcmp(log_label(-1), "CRIT") == 0);
    BOOST_TEST(strcmp(log_label(Log::Crit), "CRIT") == 0);
    BOOST_TEST(strcmp(log_label(Log::Error), "ERROR") == 0);
    BOOST_TEST(strcmp(log_label(Log::Warning), "WARNING") == 0);
    BOOST_TEST(strcmp(log_label(Log::Notice), "NOTICE") == 0);
    BOOST_TEST(strcmp(log_label(Log::Info), "INFO") == 0);
    BOOST_TEST(strcmp(log_label(Log::Debug), "DEBUG") == 0);
    BOOST_TEST(strcmp(log_label(99), "DEBUG") == 0);
}

BOOST_AUTO_TEST_CASE(LogMacroCase)
{
    auto prev_level = set_log_level(Log::Info);
    auto prev_logger = set_logger(test_logger);
    // clang-format off
    const auto finally = make_finally([prev_level, prev_logger]() noexcept {
        set_log_level(prev_level);
        set_logger(prev_logger);
    });
    // clang-format on

    SWIRLY_LOG(Log::Info) << "test1: " << Foo<int, int>{10, 20};
    BOOST_TEST(last_level == Log::Info);
    BOOST_TEST(last_msg == "test1: (10,20)");

    SWIRLY_CRIT << "test2: " << Foo<int, int>{10, 20};
    BOOST_TEST(last_level == Log::Crit);
    BOOST_TEST(last_msg == "test2: (10,20)");

    SWIRLY_ERROR << "test3: " << Foo<int, int>{10, 20};
    BOOST_TEST(last_level == Log::Error);
    BOOST_TEST(last_msg == "test3: (10,20)");

    SWIRLY_WARNING << "test4: " << Foo<int, int>{10, 20};
    BOOST_TEST(last_level == Log::Warning);
    BOOST_TEST(last_msg == "test4: (10,20)");

    SWIRLY_NOTICE << "test5: " << Foo<int, int>{10, 20};
    BOOST_TEST(last_level == Log::Notice);
    BOOST_TEST(last_msg == "test5: (10,20)");

    SWIRLY_INFO << "test6: " << Foo<int, int>{10, 20};
    BOOST_TEST(last_level == Log::Info);
    BOOST_TEST(last_msg == "test6: (10,20)");

    // This should not be logged.
    SWIRLY_DEBUG << "test7: " << Foo<int, int>{10, 20};
    BOOST_TEST(last_level == Log::Info);
    BOOST_TEST(last_msg == "test6: (10,20)");
}

BOOST_AUTO_TEST_SUITE_END()

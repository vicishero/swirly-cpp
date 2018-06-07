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
#include "VarSub.hpp"

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace swirly;

namespace {

template <typename FnT>
string apply_copy(FnT fn, string s)
{
    fn(s);
    return s;
}

string get_var(const string& name)
{
    string val;
    if (name == "FOO") {
        val = "101";
    } else if (name == "BAR") {
        val = "202";
    } else if (name == "BAZ") {
        val = "FOO";
    } else if (name == "QUX") {
        val = "BAR";
    } else if (name == "FOOBAR") {
        val = "${FOO}${BAR}";
    }
    return val;
}

} // namespace

BOOST_AUTO_TEST_SUITE(VarSubSuite)

BOOST_AUTO_TEST_CASE(VarSubBasicCase)
{
    VarSub fn{get_var};

    BOOST_TEST(apply_copy(fn, "${FOO}") == "101");
    BOOST_TEST(apply_copy(fn, "${BAR}") == "202");
    BOOST_TEST(apply_copy(fn, "<${FOO}>") == "<101>");
    BOOST_TEST(apply_copy(fn, "<${FOO} ${BAR}>") == "<101 202>");
}

BOOST_AUTO_TEST_CASE(VarSubEmptyCase)
{
    VarSub fn{get_var};

    BOOST_TEST(apply_copy(fn, "${}").empty());
    BOOST_TEST(apply_copy(fn, "${123}").empty());
    BOOST_TEST(apply_copy(fn, "${EMPTY}").empty());
}

BOOST_AUTO_TEST_CASE(VarSubEscapeCase)
{
    VarSub fn{get_var};

    BOOST_TEST(apply_copy(fn, "\\\\") == "\\");
    BOOST_TEST(apply_copy(fn, "\\\\>") == "\\>");
    BOOST_TEST(apply_copy(fn, "<\\\\") == "<\\");
    BOOST_TEST(apply_copy(fn, "\\${FOO}") == "${FOO}");
    BOOST_TEST(apply_copy(fn, "$\\{FOO}") == "${FOO}");
    BOOST_TEST(apply_copy(fn, "${\\FOO}") == "101");
    BOOST_TEST(apply_copy(fn, "${FOO\\}") == "${FOO}");
    BOOST_TEST(apply_copy(fn, "${FOO}\\") == "101\\");
}

BOOST_AUTO_TEST_CASE(VarSubPartialCase)
{
    VarSub fn{get_var};

    BOOST_TEST(apply_copy(fn, "$") == "$");
    BOOST_TEST(apply_copy(fn, "{") == "{");
    BOOST_TEST(apply_copy(fn, "}") == "}");
    BOOST_TEST(apply_copy(fn, "$FOO") == "$FOO");
    BOOST_TEST(apply_copy(fn, "{FOO") == "{FOO");
    BOOST_TEST(apply_copy(fn, "${FOO") == "${FOO");
    BOOST_TEST(apply_copy(fn, "FOO}") == "FOO}");
    BOOST_TEST(apply_copy(fn, "$${FOO}") == "$101");
}

BOOST_AUTO_TEST_CASE(VarSubNestedCase)
{
    VarSub fn{get_var};

    BOOST_TEST(apply_copy(fn, "${FOOBAR}") == "101202");
    BOOST_TEST(apply_copy(fn, "${${BAZ}}") == "101");
    BOOST_TEST(apply_copy(fn, "${${BAZ}BAR}") == "101202");
    BOOST_TEST(apply_copy(fn, "${FOO${QUX}}") == "101202");
    BOOST_TEST(apply_copy(fn, "${${BAZ}${QUX}}") == "101202");
}

BOOST_AUTO_TEST_CASE(VarSubLoopCase)
{
    VarSub fn{[](const string& name) {
        string val;
        if (name == "FOO") {
            val = "${BAR}";
        } else if (name == "BAR") {
            val = "${BAZ}";
        } else if (name == "BAZ") {
            val = "${QUX}";
        } else if (name == "QUX") {
            val = "${FOO}";
        }
        return val;
    }};

    BOOST_TEST(apply_copy(fn, "${FOO}").empty());
    BOOST_TEST(apply_copy(fn, "${FOO}${FOO}").empty());
    BOOST_TEST(apply_copy(fn, "${FOO${FOO}}").empty());
    BOOST_TEST(apply_copy(fn, "<${FOO}>") == "<>");
    BOOST_TEST(apply_copy(fn, "<${FOO${FOO}}>") == "<>");
    BOOST_TEST(apply_copy(fn, "<${FOO} ${FOO}>") == "< >");
}

BOOST_AUTO_TEST_SUITE_END()

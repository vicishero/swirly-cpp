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
#ifndef SWIRLY_UTIL_COMPARE_HPP
#define SWIRLY_UTIL_COMPARE_HPP

#include <type_traits>

namespace swirly {
inline namespace util {

template <typename ValueT>
struct Comparable {
    friend constexpr bool operator==(const ValueT& lhs, const ValueT& rhs) noexcept
    {
        return lhs.compare(rhs) == 0;
    }

    friend constexpr bool operator!=(const ValueT& lhs, const ValueT& rhs) noexcept
    {
        return lhs.compare(rhs) != 0;
    }

    friend constexpr bool operator<(const ValueT& lhs, const ValueT& rhs) noexcept
    {
        return lhs.compare(rhs) < 0;
    }

    friend constexpr bool operator<=(const ValueT& lhs, const ValueT& rhs) noexcept
    {
        return lhs.compare(rhs) <= 0;
    }

    friend constexpr bool operator>(const ValueT& lhs, const ValueT& rhs) noexcept
    {
        return lhs.compare(rhs) > 0;
    }

    friend constexpr bool operator>=(const ValueT& lhs, const ValueT& rhs) noexcept
    {
        return lhs.compare(rhs) >= 0;
    }

  protected:
    constexpr Comparable() noexcept = default;
    ~Comparable() = default;

    // Copy.
    constexpr Comparable(const Comparable&) noexcept = default;
    Comparable& operator=(const Comparable&) noexcept = default;

    // Move.
    constexpr Comparable(Comparable&&) noexcept = default;
    Comparable& operator=(Comparable&&) noexcept = default;
};

template <typename ValueT>
constexpr int compare(ValueT lhs, ValueT rhs) noexcept
{
    int i{};
    if (lhs < rhs) {
        i = -1;
    } else if (lhs > rhs) {
        i = 1;
    } else {
        i = 0;
    }
    return i;
}

} // namespace util
} // namespace swirly

#endif // SWIRLY_UTIL_COMPARE_HPP

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
#ifndef SWIRLY_FIN_MARKETID_HPP
#define SWIRLY_FIN_MARKETID_HPP

#include <swirly/util/Date.hpp>

namespace swirly {
inline namespace fin {

constexpr Id64 to_market_id(Id32 instr_id) noexcept
{
    return Id64{instr_id.count() << 16};
}

constexpr Id64 to_market_id(Id32 instr_id, JDay settl_day) noexcept
{
    std::int64_t id{instr_id.count() << 16};
    if (settl_day != 0_jd) {
        id |= jd_to_tjd(settl_day) & 0xffff;
    }
    return Id64{id};
}
static_assert(to_market_id(12_id32, 0_jd) == 786432_id64);
// 2440000 is the epoch for truncated Julian day.
static_assert(to_market_id(12_id32, 2440000_jd + 1_jd) == 786433_id64);

constexpr Id64 to_market_id(Id32 instr_id, IsoDate settl_date) noexcept
{
    std::int64_t id{instr_id.count() << 16};
    if (settl_date != 0_ymd) {
        id |= jd_to_tjd(iso_to_jd(settl_date)) & 0xffff;
    }
    return Id64{id};
}
static_assert(to_market_id(12_id32, 0_ymd) == 786432_id64);

template <typename ValueT>
struct MarketIdTraits {
    using Id = Id64;
    static Id id(const ValueT& value) noexcept { return value.market_id(); }
};

} // namespace fin
} // namespace swirly

#endif // SWIRLY_FIN_MARKETID_HPP

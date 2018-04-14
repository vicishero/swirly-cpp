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
#include "Instr.hpp"

#include <swirly/fin/Conv.hpp>

#include <swirly/util/Stream.hpp>

namespace swirly {
inline namespace fin {
using namespace std;

static_assert(sizeof(Instr) <= 4 * 64, "no greater than specified cache-lines");

Instr::Instr(Id32 id, Symbol symbol, string_view display, Symbol baseAsset, Symbol termCcy,
             Symbol broker, int lotNumer, int lotDenom, int tickNumer, int tickDenom, int pipDp,
             Lots minLots, Lots maxLots) noexcept
: id_{id}
, symbol_{symbol}
, display_{display}
, baseAsset_{baseAsset}
, termCcy_{termCcy}
, broker_{broker}
, lotNumer_{lotNumer}
, lotDenom_{lotDenom}
, qtyInc_{fractToReal(lotNumer, lotDenom)}
, tickNumer_{tickNumer}
, tickDenom_{tickDenom}
, priceInc_{fractToReal(tickNumer, tickDenom)}
, pipDp_{pipDp}
, qtyDp_{realToDp(qtyInc_)}
, priceDp_{realToDp(priceInc_)}
, minLots_{minLots}
, maxLots_{maxLots}
{
}

Instr::~Instr() = default;

Instr::Instr(const Instr&) = default;

Instr::Instr(Instr&&) = default;

void Instr::toDsv(ostream& os, char delim) const
{
    OStreamJoiner osj{os, delim};
    osj << id_        //
        << symbol_    //
        << display_   //
        << baseAsset_ //
        << termCcy_   //
        << broker_    //
        << lotNumer_  //
        << lotDenom_  //
        << tickNumer_ //
        << tickDenom_ //
        << pipDp_     //
        << minLots_   //
        << maxLots_;
}

void Instr::toJson(ostream& os) const
{
    os << "{\"symbol\":\"" << symbol_         //
       << "\",\"display\":\"" << display_     //
       << "\",\"baseAsset\":\"" << baseAsset_ //
       << "\",\"termCcy\":\"" << termCcy_     //
       << "\",\"broker\":\"" << broker_       //
       << "\",\"lotNumer\":" << lotNumer_     //
       << ",\"lotDenom\":" << lotDenom_       //
       << ",\"tickNumer\":" << tickNumer_     //
       << ",\"tickDenom\":" << tickDenom_     //
       << ",\"pipDp\":" << pipDp_             //
       << ",\"minLots\":" << minLots_         //
       << ",\"maxLots\":" << maxLots_         //
       << "}";
}

} // namespace fin
} // namespace swirly

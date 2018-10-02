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
#ifndef SWIRLYD_RESTIMPL_HXX
#define SWIRLYD_RESTIMPL_HXX

#include "EntitySet.hxx"
#include "Page.hxx"

#include <swirly/fin/IntTypes.hpp>

#include <swirly/util/Array.hpp>
#include <swirly/util/Date.hpp>
#include <swirly/util/Symbol.hpp>
#include <swirly/util/Time.hpp>

namespace swirly {
inline namespace lob {
class LobApp;
} // namespace lob

class RestImpl {
  public:
    explicit RestImpl(LobApp& lob_app)
    : lob_app_(lob_app)
    {
    }
    ~RestImpl();

    // Copy.
    RestImpl(const RestImpl&) = delete;
    RestImpl& operator=(const RestImpl&) = delete;

    // Move.
    RestImpl(RestImpl&&) = delete;
    RestImpl& operator=(RestImpl&&) = delete;

    void get_ref_data(WallTime now, EntitySet es, std::ostream& out) const;

    void get_asset(WallTime now, std::ostream& out) const;

    void get_asset(WallTime now, Symbol symbol, std::ostream& out) const;

    void get_instr(WallTime now, std::ostream& out) const;

    void get_instr(WallTime now, Symbol symbol, std::ostream& out) const;

    void get_sess(WallTime now, Symbol accnt, EntitySet es, Page page, std::ostream& out) const;

    void get_market(WallTime now, std::ostream& out) const;

    void get_market(WallTime now, Symbol instr, std::ostream& out) const;

    void get_market(WallTime now, Symbol instr, IsoDate settl_date, std::ostream& out) const;

    void get_order(WallTime now, Symbol accnt, std::ostream& out) const;

    void get_order(WallTime now, Symbol accnt, Symbol instr, std::ostream& out) const;

    void get_order(WallTime now, Symbol accnt, Symbol instr, IsoDate settl_date,
                   std::ostream& out) const;

    void get_order(WallTime now, Symbol accnt, Symbol instr, IsoDate settl_date, Id64 id,
                   std::ostream& out) const;

    void get_exec(WallTime now, Symbol accnt, Page page, std::ostream& out) const;

    void get_trade(WallTime now, Symbol accnt, std::ostream& out) const;

    void get_trade(WallTime now, Symbol accnt, Symbol instr, std::ostream& out) const;

    void get_trade(WallTime now, Symbol accnt, Symbol instr, IsoDate settl_date,
                   std::ostream& out) const;

    void get_trade(WallTime now, Symbol accnt, Symbol instr, IsoDate settl_date, Id64 id,
                   std::ostream& out) const;

    void get_posn(WallTime now, Symbol accnt, std::ostream& out) const;

    void get_posn(WallTime now, Symbol accnt, Symbol instr, std::ostream& out) const;

    void get_posn(WallTime now, Symbol accnt, Symbol instr, IsoDate settl_date,
                  std::ostream& out) const;

    void post_market(WallTime now, Symbol instr, IsoDate settl_date, MarketState state,
                     std::ostream& out);

    void put_market(WallTime now, Symbol instr, IsoDate settl_date, MarketState state,
                    std::ostream& out);

    void post_order(WallTime now, Symbol accnt, Symbol instr, IsoDate settl_date,
                    std::string_view ref, Side side, Lots lots, Ticks ticks, Lots min_lots,
                    std::ostream& out);

    void put_order(WallTime now, Symbol accnt, Symbol instr, IsoDate settl_date,
                   ArrayView<Id64> ids, Lots lots, std::ostream& out);

    void post_trade(WallTime now, Symbol accnt, Symbol instr, IsoDate settl_date,
                    std::string_view ref, Side side, Lots lots, Ticks ticks, LiqInd liq_ind,
                    Symbol cpty, std::ostream& out);

    void delete_trade(WallTime now, Symbol accnt, Symbol instr, IsoDate settl_date,
                      ArrayView<Id64> ids);

  private:
    LobApp& lob_app_;
};

} // namespace swirly

#endif // SWIRLYD_RESTIMPL_HXX

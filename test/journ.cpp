/*
 *  Copyright (C) 2013 Mark Aylett <mark.aylett@gmail.com>
 *
 *  This file is part of Doobry written by Mark Aylett.
 *
 *  Doobry is free software; you can redistribute it and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Doobry is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program; if
 *  not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA.
 */
#include "journ.hpp"

using namespace dbr;
using namespace std;

DbrIden
Journ::alloc_id() noexcept
{
    return id_++;
}

DbrBool
Journ::begin_trans() noexcept
{
    return 1;
}

DbrBool
Journ::commit_trans() noexcept
{
    return 1;
}

DbrBool
Journ::rollback_trans() noexcept
{
    return 1;
}

DbrBool
Journ::insert_order(Order order) noexcept
{
    return 1;
}

DbrBool
Journ::update_order(DbrIden id, int rev, int status, DbrLots resd, DbrLots exec,
                    DbrLots lots, DbrMillis now) noexcept
{
    return 1;
}

DbrBool
Journ::archive_order(DbrIden id, DbrMillis now) noexcept
{
    return 1;
}

DbrBool
Journ::insert_trade(Trade trade) noexcept
{
    return 1;
}

DbrBool
Journ::archive_trade(DbrIden id, DbrMillis now) noexcept
{
    return 1;
}
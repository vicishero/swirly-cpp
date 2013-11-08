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
#include "factory.hpp"

#include <dbrpp/pool.hpp>

#include <dbr/conv.h>

using namespace dbr;
using namespace std;

shared_ptr<DbrRec>
create_trader(Pool& pool, DbrIden id, const char* mnem, const char* display, const char* email)
{
    auto deleter = [&pool](DbrRec* rec) {
        pool.free_rec(rec);
    };
    std::shared_ptr<DbrRec> rec(pool.alloc_rec(), deleter);
    dbr_rec_init(rec.get());

    rec->type = DBR_TRADER;
    rec->id = id;
    strncpy(rec->mnem, mnem, DBR_MNEM_MAX);
    strncpy(rec->display, display, DBR_DISPLAY_MAX);

    // Body.
    strncpy(rec->trader.email, email, DBR_EMAIL_MAX);
    return rec;
}

shared_ptr<DbrRec>
create_accnt(Pool& pool, DbrIden id, const char* mnem, const char* display, const char* email)
{
    auto deleter = [&pool](DbrRec* rec) {
        pool.free_rec(rec);
    };
    std::shared_ptr<DbrRec> rec(pool.alloc_rec(), deleter);
    dbr_rec_init(rec.get());

    rec->type = DBR_ACCNT;
    rec->id = id;
    strncpy(rec->mnem, mnem, DBR_MNEM_MAX);
    strncpy(rec->display, display, DBR_DISPLAY_MAX);

    // Body.
    strncpy(rec->accnt.email, email, DBR_EMAIL_MAX);
    return rec;
}

shared_ptr<DbrRec>
create_contr(Pool& pool, DbrIden id, const char* mnem, const char* display, const char* asset_type,
             const char* asset, const char* ccy, int tick_numer, int tick_denom, int lot_numer,
             int lot_denom, int pip_dp, DbrLots min_lots, DbrLots max_lots)
{
    auto deleter = [&pool](DbrRec* rec) {
        pool.free_rec(rec);
    };
    std::shared_ptr<DbrRec> rec(pool.alloc_rec(), deleter);
    dbr_rec_init(rec.get());

    rec->type = DBR_CONTR;
    rec->id = id;
    strncpy(rec->mnem, mnem, DBR_MNEM_MAX);
    strncpy(rec->display, display, DBR_DISPLAY_MAX);

    // Body.
    strncpy(rec->contr.asset_type, asset_type, DBR_MNEM_MAX);
    strncpy(rec->contr.asset, asset, DBR_MNEM_MAX);
    strncpy(rec->contr.ccy, ccy, DBR_MNEM_MAX);
    rec->contr.tick_numer = tick_numer;
    rec->contr.tick_denom = tick_denom;
    rec->contr.price_inc = dbr_fract_to_real(tick_numer, tick_denom);
    rec->contr.lot_numer = lot_numer;
    rec->contr.lot_denom = lot_denom;
    rec->contr.qty_inc = dbr_fract_to_real(lot_numer, lot_denom);
    rec->contr.price_dp = dbr_real_to_dp(rec->contr.price_inc);
    rec->contr.pip_dp = pip_dp;
    rec->contr.qty_dp = dbr_real_to_dp(rec->contr.qty_inc);
    rec->contr.min_lots = min_lots;
    rec->contr.max_lots = max_lots;
    return rec;
}

shared_ptr<DbrOrder>
create_order(Pool& pool, DbrIden id, DbrRec& trader, DbrRec& accnt, DbrRec& contr,
             DbrDate settl_date, const char* ref, int action, DbrTicks ticks, DbrLots lots,
             DbrLots min, DbrFlags flags, DbrMillis now)
{
    auto deleter = [&pool](DbrOrder* order) {
        pool.free_order(order);
    };
    std::shared_ptr<DbrOrder> order(pool.alloc_order(), deleter);
    dbr_order_init(order.get());

    order->id = id;
    order->level = NULL;
    order->rev = 0;
    order->status = 0;
    order->trader.rec = &trader;
    order->accnt.rec = &accnt;
    order->contr.rec = &contr;
    order->settl_date = settl_date;
    if (ref)
        strncpy(order->ref, ref, DBR_REF_MAX);
    else
        order->ref[0] = '\0';

    order->action = action;
    order->ticks = ticks;
    order->resd = lots;
    order->exec = 0;
    order->lots = lots;
    order->min = min;
    order->flags = flags;
    order->created = now;
    order->modified = now;

    return order;
}

shared_ptr<DbrOrder>
create_order(Pool& pool, DbrIden id, DbrIden tid, DbrIden aid, DbrIden cid,
             DbrDate settl_date, const char* ref, int action, DbrTicks ticks, DbrLots lots,
             DbrLots min, DbrFlags flags, DbrMillis now)
{
    auto deleter = [&pool](DbrOrder* order) {
        pool.free_order(order);
    };
    std::shared_ptr<DbrOrder> order(pool.alloc_order(), deleter);
    dbr_order_init(order.get());

    order->id = id;
    order->level = NULL;
    order->rev = 0;
    order->status = 0;
    order->trader.id_only = tid;
    order->accnt.id_only = aid;
    order->contr.id_only = cid;
    order->settl_date = settl_date;
    if (ref)
        strncpy(order->ref, ref, DBR_REF_MAX);
    else
        order->ref[0] = '\0';

    order->action = action;
    order->ticks = ticks;
    order->resd = lots;
    order->exec = 0;
    order->lots = lots;
    order->min = min;
    order->flags = flags;
    order->created = now;
    order->modified = now;

    return order;
}

shared_ptr<DbrExec>
create_trade(Pool& pool, DbrIden id, DbrIden order, int rev, DbrIden tid, DbrIden aid,
             DbrIden cid, DbrDate settl_date, const char* ref, int action, DbrTicks ticks,
             DbrLots resd, DbrLots exec, DbrLots lots, DbrIden match, DbrIden cpty, int role,
             DbrMillis now)
{
    auto deleter = [&pool](DbrExec* exec) {
        pool.free_exec(exec);
    };
    std::shared_ptr<DbrExec> ptr(pool.alloc_exec(), deleter);
    dbr_exec_init(ptr.get());

    ptr->id = id;
    ptr->order = order;
    ptr->rev = rev;
    ptr->trader.id_only = tid;
    ptr->accnt.id_only = aid;
    ptr->contr.id_only = cid;
    ptr->settl_date = settl_date;
    if (ref)
        strncpy(ptr->ref, ref, DBR_REF_MAX);
    else
        ptr->ref[0] = '\0';
    ptr->action = action;
    ptr->ticks = ticks;
    ptr->resd = resd;
    ptr->exec = exec;
    ptr->lots = lots;
    ptr->match = match;
    ptr->cpty.id_only = cpty;
    ptr->role = role;
    ptr->created = now;
    ptr->modified = now;

    return ptr;
}

shared_ptr<DbrMemb>
create_memb(Pool& pool, DbrIden tid, DbrIden aid)
{
    auto deleter = [&pool](DbrMemb* memb) {
        pool.free_memb(memb);
    };
    std::shared_ptr<DbrMemb> memb(pool.alloc_memb(), deleter);
    dbr_memb_init(memb.get());

    memb->trader.id_only = tid;
    memb->accnt.id_only = aid;

    return memb;
}

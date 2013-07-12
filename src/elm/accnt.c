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
#include "accnt.h"

#include "pool.h"

#include <dbr/conv.h>
#include <dbr/sess.h>

#include <ash/err.h>

#include <assert.h>
#include <stdlib.h>

static void
trade(DbrAccntSess sess, struct DbrOrder* order, struct DbrTrade* trade, struct DbrPosn* posn)
{
}

static const struct DbrAccntSessVtbl SESS_VTBL = {
    .trade = trade
};

static struct DbrIAccntSess sess_noop = {
    .vtbl = &SESS_VTBL
};

static void
free_membs(struct ElmAccnt* accnt)
{
    assert(accnt);
    struct DbrRbNode* node;
    while ((node = accnt->membs.root)) {
        struct DbrMemb* memb = dbr_accnt_memb_entry(node);
        ash_tree_remove(&accnt->membs, node);
        elm_pool_free_memb(accnt->pool, memb);
    }
}

static void
free_trades(struct ElmAccnt* accnt)
{
    assert(accnt);
    struct DbrRbNode* node;
    while ((node = accnt->trades.root)) {
        struct DbrTrade* trade = dbr_accnt_trade_entry(node);
        ash_tree_remove(&accnt->trades, node);
        elm_pool_free_trade(accnt->pool, trade);
    }
}

static void
free_posns(struct ElmAccnt* accnt)
{
    assert(accnt);
    struct DbrRbNode* node;
    while ((node = accnt->posns.root)) {
        struct DbrPosn* posn = dbr_accnt_posn_entry(node);
        ash_tree_remove(&accnt->posns, node);
        elm_pool_free_posn(accnt->pool, posn);
    }
}

DBR_EXTERN struct ElmAccnt*
elm_accnt_lazy(struct DbrRec* arec, struct ElmPool* pool)
{
    assert(arec);
    assert(arec->type == DBR_ACCNT);
    struct ElmAccnt* accnt = arec->accnt.state;
    if (dbr_unlikely(!accnt)) {
        accnt = malloc(sizeof(struct ElmAccnt));
        if (dbr_unlikely(!accnt)) {
            ash_err_set(DBR_ENOMEM, "out of memory");
            return NULL;
        }
        accnt->id = arec->id;
        accnt->pool = pool;
        ash_tree_init(&accnt->membs);
        ash_tree_init(&accnt->trades);
        ash_tree_init(&accnt->posns);
        accnt->sess = &sess_noop;

        arec->accnt.state = accnt;
    }
    return accnt;
}

DBR_EXTERN void
elm_accnt_term(struct DbrRec* arec)
{
    assert(arec);
    assert(arec->type == DBR_ACCNT);
    struct ElmAccnt* accnt = arec->accnt.state;
    if (accnt) {
        arec->accnt.state = NULL;
        free_posns(accnt);
        free_trades(accnt);
        free_membs(accnt);
        free(accnt);
    }
}

DBR_EXTERN struct DbrPosn*
elm_accnt_posn(struct DbrRec* arec, struct DbrRec* irec, DbrDate settl_date, struct ElmPool* pool)
{
    assert(arec);
    assert(arec->type == DBR_ACCNT);

    assert(irec);
    assert(irec->type == DBR_INSTR);

    // Synthentic id from instrument and settlment date.
    const DbrIden id = irec->id * 100000000L + settl_date;

    struct ElmAccnt* accnt = elm_accnt_lazy(arec, pool);
    if (dbr_unlikely(!accnt))
        return NULL;

    struct DbrPosn* posn;
	struct DbrRbNode* node = ash_tree_pfind(&accnt->posns, id);
    if (!node || node->key != id) {
        if (!(posn = elm_pool_alloc_posn(accnt->pool, id)))
            return NULL;

        posn->id = id;
        posn->accnt.rec = arec;
        posn->instr.rec = irec;
        posn->settl_date = settl_date;
        posn->buy_licks = 0;
        posn->buy_lots = 0;
        posn->sell_licks = 0;
        posn->sell_lots = 0;

        DBR_DEBUG1F("insert posn: accnt=%.16s, instr=%.16s, settl_date=%d",
                    arec->mnem, irec->mnem, settl_date);

        struct DbrRbNode* parent = node;
        ash_tree_pinsert(&accnt->posns, &posn->accnt_node_, parent);
    } else
        posn = dbr_accnt_posn_entry(node);
    return posn;
}

DBR_EXTERN void
elm_accnt_set_sess(struct ElmAccnt* accnt, DbrAccntSess sess)
{
    accnt->sess = sess ? sess : &sess_noop;
}

DBR_API DbrIden
dbr_accnt_id(DbrAccnt accnt)
{
    return elm_accnt_id(accnt);
}

// AccntTrade

DBR_API struct DbrRbNode*
dbr_accnt_find_trade_id(DbrAccnt accnt, DbrIden id)
{
    return elm_accnt_find_trade_id(accnt, id);
}

DBR_API struct DbrRbNode*
dbr_accnt_first_trade(DbrAccnt accnt)
{
    return elm_accnt_first_trade(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_last_trade(DbrAccnt accnt)
{
    return elm_accnt_last_trade(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_end_trade(DbrAccnt accnt)
{
    return elm_accnt_end_trade(accnt);
}

DBR_API DbrBool
dbr_accnt_empty_trade(DbrAccnt accnt)
{
    return elm_accnt_empty_trade(accnt);
}

// AccntPosn.

DBR_API struct DbrRbNode*
dbr_accnt_find_posn_id(DbrAccnt accnt, DbrIden id)
{
    return elm_accnt_find_posn_id(accnt, id);
}

DBR_API struct DbrRbNode*
dbr_accnt_first_posn(DbrAccnt accnt)
{
    return elm_accnt_first_posn(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_last_posn(DbrAccnt accnt)
{
    return elm_accnt_last_posn(accnt);
}

DBR_API struct DbrRbNode*
dbr_accnt_end_posn(DbrAccnt accnt)
{
    return elm_accnt_end_posn(accnt);
}

DBR_API DbrBool
dbr_accnt_empty_posn(DbrAccnt accnt)
{
    return elm_accnt_empty_posn(accnt);
}

DBR_API void
dbr_accnt_set_sess(DbrAccnt accnt, DbrAccntSess sess)
{
    elm_accnt_set_sess(accnt, sess);
}

DBR_API DbrAccntSess
dbr_accnt_sess(DbrAccnt accnt)
{
    return elm_accnt_sess(accnt);
}

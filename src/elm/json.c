/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
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
#include <dbr/json.h>

#include "format.h"

#include <dbr/types.h>
#include <dbr/util.h>

#include <assert.h>
#include <string.h>

DBR_API size_t
dbr_json_accnt_len(const struct DbrRec* rec)
{
    assert(rec->type == DBR_ENTITY_ACCNT);
    enum {
        ACCNT_SIZE =
        sizeof("{\"mnem\":\"\","
               "\"display\":\"\","
               "\"email\":\"\"}") - 1
    };

    return ACCNT_SIZE
        + strnlen(rec->mnem, DBR_MNEM_MAX)
        + strnlen(rec->display, DBR_DISPLAY_MAX)
        + strnlen(rec->accnt.email, DBR_EMAIL_MAX);
}

DBR_API char*
dbr_json_write_accnt(char* buf, const struct DbrRec* rec)
{
    assert(rec->type == DBR_ENTITY_ACCNT);
    static const char ACCNT_FORMAT[] =
        "{\"mnem\":\"%m\","
        "\"display\":\"%s\","
        "\"email\":\"%s\"}";

    return dbr_format(buf, ACCNT_FORMAT,
                      rec->mnem,
                      DBR_DISPLAY_MAX, rec->display,
                      DBR_EMAIL_MAX, rec->accnt.email);
}

DBR_API size_t
dbr_json_contr_len(const struct DbrRec* rec)
{
    assert(rec->type == DBR_ENTITY_CONTR);
    enum {
        CONTR_SIZE =
        sizeof("{\"mnem\":\"\","
               "\"display\":\"\","
               "\"asset_type\":\"\","
               "\"asset\":\"\","
               "\"ccy\":\"\","
               "\"tick_numer\":,"
               "\"tick_denom\":,"
               "\"lot_numer\":,"
               "\"lot_denom\":,"
               "\"price_dp\":,"
               "\"pip_dp\":,"
               "\"qty_dp\":,"
               "\"min_lots\":,"
               "\"max_lots\":}") - 1
    };

    return CONTR_SIZE
        + strnlen(rec->mnem, DBR_MNEM_MAX)
        + strnlen(rec->display, DBR_DISPLAY_MAX)
        + strnlen(rec->contr.asset_type, DBR_MNEM_MAX)
        + strnlen(rec->contr.asset, DBR_MNEM_MAX)
        + strnlen(rec->contr.ccy, DBR_MNEM_MAX)
        + dbr_int_len(rec->contr.tick_numer)
        + dbr_int_len(rec->contr.tick_denom)
        + dbr_int_len(rec->contr.lot_numer)
        + dbr_int_len(rec->contr.lot_denom)
        + dbr_int_len(rec->contr.price_dp)
        + dbr_int_len(rec->contr.pip_dp)
        + dbr_int_len(rec->contr.qty_dp)
        + dbr_long_len(rec->contr.min_lots)
        + dbr_long_len(rec->contr.max_lots);
}

DBR_API char*
dbr_json_write_contr(char* buf, const struct DbrRec* rec)
{
    assert(rec->type == DBR_ENTITY_CONTR);
    static const char CONTR_FORMAT[] =
        "{\"mnem\":\"%m\","
        "\"display\":\"%s\","
        "\"asset_type\":\"%m\","
        "\"asset\":\"%m\","
        "\"ccy\":\"%m\","
        "\"tick_numer\":%d,"
        "\"tick_denom\":%d,"
        "\"lot_numer\":%d,"
        "\"lot_denom\":%d,"
        "\"price_dp\":%d,"
        "\"pip_dp\":%d,"
        "\"qty_dp\":%d,"
        "\"min_lots\":%l,"
        "\"max_lots\":%l}";

    return dbr_format(buf, CONTR_FORMAT,
                      rec->mnem,
                      DBR_DISPLAY_MAX, rec->display,
                      rec->contr.asset_type,
                      rec->contr.asset,
                      rec->contr.ccy,
                      rec->contr.tick_numer,
                      rec->contr.tick_denom,
                      rec->contr.lot_numer,
                      rec->contr.lot_denom,
                      rec->contr.price_dp,
                      rec->contr.pip_dp,
                      rec->contr.qty_dp,
                      rec->contr.min_lots,
                      rec->contr.max_lots);
}

DBR_API size_t
dbr_json_memb_len(const struct DbrMemb* memb)
{
    enum {
        MEMB_SIZE =
        sizeof("{\"user\":\"\","
               "\"group\":\"\"}") - 1
    };

    return MEMB_SIZE
        + strnlen(memb->user.rec->mnem, DBR_MNEM_MAX)
        + strnlen(memb->group.rec->mnem, DBR_MNEM_MAX);
}

DBR_API char*
dbr_json_write_memb(char* buf, const struct DbrMemb* memb)
{
    static const char MEMB_FORMAT[] =
        "{\"user\":\"%m\","
        "\"group\":\"%m\"}";

    return dbr_format(buf, MEMB_FORMAT,
                      memb->user.rec->mnem,
                      memb->group.rec->mnem);
}

DBR_API size_t
dbr_json_order_len(const struct DbrOrder* order)
{
    enum {
        ORDER_SIZE =
        sizeof("{\"id\":,"
               "\"user\":\"\","
               "\"group\":\"\","
               "\"contr\":\"\","
               "\"settl_day\":,"
               "\"ref\":\"\","
               "\"state\":\"\","
               "\"action\":\"\","
               "\"ticks\":,"
               "\"lots\":,"
               "\"resd\":,"
               "\"exec\":,"
               "\"last_ticks\":,"
               "\"last_lots\":,"
               "\"created\":,"
               "\"modified\":}") - 1
    };

    return ORDER_SIZE
        + dbr_long_len(order->id)
        + strnlen(order->c.user.rec->mnem, DBR_MNEM_MAX)
        + strnlen(order->c.group.rec->mnem, DBR_MNEM_MAX)
        + strnlen(order->c.contr.rec->mnem, DBR_MNEM_MAX)
        + dbr_int_len(order->c.settl_day)
        + strnlen(order->c.ref, DBR_REF_MAX)
        + elm_state_len(order->c.state)
        + elm_action_len(order->c.action)
        + dbr_long_len(order->c.ticks)
        + dbr_long_len(order->c.lots)
        + dbr_long_len(order->c.resd)
        + dbr_long_len(order->c.exec)
        + dbr_long_len(order->c.last_ticks)
        + dbr_long_len(order->c.last_lots)
        + dbr_long_len(order->created)
        + dbr_long_len(order->modified);
}

DBR_API char*
dbr_json_write_order(char* buf, const struct DbrOrder* order)
{
    static const char ORDER_FORMAT[] =
        "{\"id\":%l,"
        "\"user\":\"%m\","
        "\"group\":\"%m\","
        "\"contr\":\"%m\","
        "\"settl_day\":%d,"
        "\"ref\":\"%s\","
        "\"state\":\"%S\","
        "\"action\":\"%A\","
        "\"ticks\":%l,"
        "\"lots\":%l,"
        "\"resd\":%l,"
        "\"exec\":%l,"
        "\"last_ticks\":%l,"
        "\"last_lots\":%l,"
        "\"created\":%l,"
        "\"modified\":%l}";

    return dbr_format(buf, ORDER_FORMAT,
                      order->id,
                      order->c.user.rec->mnem,
                      order->c.group.rec->mnem,
                      order->c.contr.rec->mnem,
                      order->c.settl_day,
                      DBR_REF_MAX, order->c.ref,
                      order->c.state,
                      order->c.action,
                      order->c.ticks,
                      order->c.lots,
                      order->c.resd,
                      order->c.exec,
                      order->c.last_ticks,
                      order->c.last_lots,
                      order->created,
                      order->modified);
}

DBR_API size_t
dbr_json_exec_len(const struct DbrExec* exec)
{
    enum {
        EXEC_SIZE =
        sizeof("{\"id\":,"
               "{\"order\":,"
               "\"user\":\"\","
               "\"group\":\"\","
               "\"contr\":\"\","
               "\"settl_day\":,"
               "\"ref\":\"\","
               "\"state\":\"\","
               "\"action\":\"\","
               "\"ticks\":,"
               "\"lots\":,"
               "\"resd\":,"
               "\"exec\":,"
               "\"last_ticks\":,"
               "\"last_lots\":,"
               "\"match\":,"
               "\"role\":\"\","
               "\"cpty\":\"\","
               "\"created\":}") - 1
    };
    return EXEC_SIZE
        + dbr_long_len(exec->id)
        + dbr_long_len(exec->order)
        + strnlen(exec->c.user.rec->mnem, DBR_MNEM_MAX)
        + strnlen(exec->c.group.rec->mnem, DBR_MNEM_MAX)
        + strnlen(exec->c.contr.rec->mnem, DBR_MNEM_MAX)
        + dbr_int_len(exec->c.settl_day)
        + strnlen(exec->c.ref, DBR_REF_MAX)
        + elm_state_len(exec->c.state)
        + elm_action_len(exec->c.action)
        + dbr_long_len(exec->c.ticks)
        + dbr_long_len(exec->c.lots)
        + dbr_long_len(exec->c.resd)
        + dbr_long_len(exec->c.exec)
        + dbr_long_len(exec->c.last_ticks)
        + dbr_long_len(exec->c.last_lots)
        + dbr_long_len(exec->match)
        + elm_role_len(exec->role)
        + strnlen(exec->cpty.rec->mnem, DBR_MNEM_MAX)
        + dbr_long_len(exec->created);
}

DBR_API char*
dbr_json_write_exec(char* buf, const struct DbrExec* exec)
{
    static const char EXEC_FORMAT[] =
        "{\"id\":%l,"
        "\"order\":%l,"
        "\"user\":\"%m\","
        "\"group\":\"%m\","
        "\"contr\":\"%m\","
        "\"settl_day\":%d,"
        "\"ref\":\"%s\","
        "\"state\":\"%S\","
        "\"action\":\"%A\","
        "\"ticks\":%l,"
        "\"lots\":%l,"
        "\"resd\":%l,"
        "\"exec\":%l,"
        "\"last_ticks\":%l,"
        "\"last_lots\":%l,"
        "\"match\":%l,"
        "\"role\":\"%R\","
        "\"cpty\":\"%m\","
        "\"created\":%l}";

    return dbr_format(buf, EXEC_FORMAT,
                      exec->id,
                      exec->order,
                      exec->c.user.rec->mnem,
                      exec->c.group.rec->mnem,
                      exec->c.contr.rec->mnem,
                      exec->c.settl_day,
                      DBR_REF_MAX, exec->c.ref,
                      exec->c.state,
                      exec->c.action,
                      exec->c.ticks,
                      exec->c.lots,
                      exec->c.resd,
                      exec->c.exec,
                      exec->c.last_ticks,
                      exec->c.last_lots,
                      exec->match,
                      exec->role,
                      exec->cpty,
                      exec->created);
}

DBR_API size_t
dbr_json_posn_len(const struct DbrPosn* posn)
{
    return 0;
}

DBR_API char*
dbr_json_write_posn(char* buf, const struct DbrPosn* posn)
{
    return NULL;
}

DBR_API size_t
dbr_json_view_len(const struct DbrView* view)
{
    return 0;
}

DBR_API char*
dbr_json_write_view(char* buf, const struct DbrView* view)
{
    return NULL;
}
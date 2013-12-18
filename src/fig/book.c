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
#include <dbr/book.h>

#include <dbr/err.h>

#include <stdlib.h>

DBR_API void
dbr_book_init(struct DbrBook* book, struct DbrRec* crec, DbrDate settl_date, DbrPool pool)
{
    book->crec = crec;
    book->settl_date = settl_date;
    dbr_side_init(&book->bid_side, pool);
    dbr_side_init(&book->ask_side, pool);
    dbr_rbnode_init(&book->serv_node_);
}

DBR_API void
dbr_book_term(struct DbrBook* book)
{
    dbr_side_term(&book->bid_side);
    dbr_side_term(&book->ask_side);
}

DBR_API struct DbrBest*
dbr_book_best(struct DbrBook* book, struct DbrBest* best)
{
    struct DbrSide* side = &book->bid_side;
    struct DbrRbNode* it = dbr_side_first_level(side);
    if (it != DBR_SIDE_END_LEVEL) {
        struct DbrLevel* level = dbr_side_level_entry(it);
        best->bid_ticks = level->ticks;
        best->bid_lots = level->lots;
        best->bid_count = level->count;
    } else {
        best->bid_ticks = 0;
        best->bid_lots = 0;
        best->bid_count = 0;
    }

    side = &book->ask_side;
    it = dbr_side_first_level(side);
    if (it != DBR_SIDE_END_LEVEL) {
        struct DbrLevel* level = dbr_side_level_entry(it);
        best->ask_ticks = level->ticks;
        best->ask_lots = level->lots;
        best->ask_count = level->count;
    } else {
        best->ask_ticks = 0;
        best->ask_lots = 0;
        best->ask_count = 0;
    }
    return best;
}

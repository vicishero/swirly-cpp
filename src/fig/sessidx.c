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
#include "sessidx.h"

#include <dbr/err.h>
#include <dbr/pool.h>
#include <dbr/sess.h>
#include <dbr/slnode.h>
#include <dbr/types.h>

#include <string.h>

static inline struct DbrSess*
sess_entry_uuid(struct DbrSlNode* node)
{
    return dbr_implof(struct DbrSess, uuid_node_, node);
}

static inline void
free_sess_list(struct DbrSlNode* node, DbrPool pool)
{
    while (node) {
        struct DbrSess* sess = sess_entry_uuid(node);
        node = node->next;
        dbr_sess_term(sess);
        dbr_pool_free_sess(pool, sess);
    }
}

static inline DbrBool
equal_uuid(const struct DbrSess* sess, const DbrUuid uuid)
{
    return __builtin_memcmp(sess->uuid, uuid, 16) == 0;
}

static inline size_t
hash_uuid(const DbrUuid uuid)
{
    size_t h = 0;
    for (int i = 0; i < 16; ++uuid, ++i)
        h = 31 * h + *uuid;
    return h;
}

DBR_EXTERN void
fig_sessidx_init(struct FigSessIdx* sessidx, DbrPool pool)
{
    sessidx->pool = pool;
    // Zero buckets.
    memset(sessidx->buckets, 0, sizeof(sessidx->buckets));
}

DBR_EXTERN void
fig_sessidx_term(struct FigSessIdx* sessidx)
{
    // Free each bucket.
    for (size_t i = 0; i < FIG_SESSIDX_BUCKETS; ++i)
        free_sess_list(sessidx->buckets[i].uuids.first, sessidx->pool);
}

DBR_EXTERN struct DbrSess*
fig_sessidx_lazy(struct FigSessIdx* sessidx, const DbrUuid uuid)
{
    assert(uuid);

    struct DbrSess* sess;

    const size_t bucket = hash_uuid(uuid) % FIG_SESSIDX_BUCKETS;
    for (struct DbrSlNode* node = dbr_stack_first(&sessidx->buckets[bucket].uuids);
         node; node = node->next) {
        sess = sess_entry_uuid(node);
        if (equal_uuid(sess, uuid))
            goto done;
    }
    if (!(sess = dbr_pool_alloc_sess(sessidx->pool)))
        goto done;

    dbr_sess_init(sess, uuid, sessidx->pool);
    dbr_stack_push(&sessidx->buckets[bucket].uuids, &sess->uuid_node_);
 done:
    return sess;
}

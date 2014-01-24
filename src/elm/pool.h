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
#ifndef ELM_POOL_H
#define ELM_POOL_H

#include <dbr/log.h>
#include <dbr/slnode.h>
#include <dbr/types.h>

struct ElmSmallBlock;
struct ElmLargeBlock;

struct ElmPool {
    void* addr;
    size_t used;
    size_t capacity;
    // Free lists.
    struct ElmSmallNode* first_small;
    struct ElmLargeNode* first_large;
    long allocs;
    unsigned long checksum;
};

struct ElmSmallNode {
    union {
        struct ElmSmallNode* next;
        // Small data structures.
        struct DbrLevel level;
        struct DbrMatch match;
        struct DbrMemb memb;
        struct DbrPosn posn;
        struct DbrSess sess;
    };
};

struct ElmLargeNode {
    union {
        struct ElmLargeNode* next;
        // Large data structures.
        struct DbrRec rec;
        struct DbrOrder order;
        struct DbrExec exec;
        struct DbrView view;
        struct DbrBook book;
    };
};

// Error fields are set on failure.

DBR_EXTERN DbrBool
elm_pool_init(struct ElmPool* pool, size_t capacity);

// Assumes that pointer is not null.

DBR_EXTERN void
elm_pool_term(struct ElmPool* pool);

DBR_EXTERN void
elm_pool_free_small(struct ElmPool* pool, struct ElmSmallNode* node);

DBR_EXTERN void
elm_pool_free_large(struct ElmPool* pool, struct ElmLargeNode* node);

DBR_EXTERN struct ElmSmallNode*
elm_pool_alloc_small(struct ElmPool* pool);

DBR_EXTERN struct ElmLargeNode*
elm_pool_alloc_large(struct ElmPool* pool);

static inline struct DbrRec*
elm_pool_alloc_rec(struct ElmPool* pool)
{
    struct ElmLargeNode* node = elm_pool_alloc_large(pool);
    return node ? &node->rec : NULL;
}

static inline void
elm_pool_free_rec(struct ElmPool* pool, struct DbrRec* rec)
{
    struct ElmLargeNode* node = (struct ElmLargeNode*)rec;
    elm_pool_free_large(pool, node);
}

static inline struct DbrOrder*
elm_pool_alloc_order(struct ElmPool* pool)
{
    struct ElmLargeNode* node = elm_pool_alloc_large(pool);
    return node ? &node->order : NULL;
}

static inline void
elm_pool_free_order(struct ElmPool* pool, struct DbrOrder* order)
{
    struct ElmLargeNode* node = (struct ElmLargeNode*)order;
    elm_pool_free_large(pool, node);
}

static inline struct DbrLevel*
elm_pool_alloc_level(struct ElmPool* pool)
{
    struct ElmSmallNode* node = elm_pool_alloc_small(pool);
    return node ? &node->level : NULL;
}

static inline void
elm_pool_free_level(struct ElmPool* pool, struct DbrLevel* level)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)level;
    elm_pool_free_small(pool, node);
}

static inline struct DbrExec*
elm_pool_alloc_exec(struct ElmPool* pool)
{
    struct ElmLargeNode* node = elm_pool_alloc_large(pool);
    return node ? &node->exec : NULL;
}

static inline void
elm_pool_free_exec(struct ElmPool* pool, struct DbrExec* exec)
{
    struct ElmLargeNode* node = (struct ElmLargeNode*)exec;
    elm_pool_free_large(pool, node);
}

static inline struct DbrMatch*
elm_pool_alloc_match(struct ElmPool* pool)
{
    struct ElmSmallNode* node = elm_pool_alloc_small(pool);
    return node ? &node->match : NULL;
}

static inline void
elm_pool_free_match(struct ElmPool* pool, struct DbrMatch* match)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)match;
    elm_pool_free_small(pool, node);
}

static inline struct DbrMemb*
elm_pool_alloc_memb(struct ElmPool* pool)
{
    struct ElmSmallNode* node = elm_pool_alloc_small(pool);
    return node ? &node->memb : NULL;
}

static inline void
elm_pool_free_memb(struct ElmPool* pool, struct DbrMemb* memb)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)memb;
    elm_pool_free_small(pool, node);
}

static inline struct DbrPosn*
elm_pool_alloc_posn(struct ElmPool* pool)
{
    struct ElmSmallNode* node = elm_pool_alloc_small(pool);
    return node ? &node->posn : NULL;
}

static inline void
elm_pool_free_posn(struct ElmPool* pool, struct DbrPosn* posn)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)posn;
    elm_pool_free_small(pool, node);
}

static inline struct DbrView*
elm_pool_alloc_view(struct ElmPool* pool)
{
    struct ElmLargeNode* node = elm_pool_alloc_large(pool);
    return node ? &node->view : NULL;
}

static inline void
elm_pool_free_view(struct ElmPool* pool, struct DbrView* view)
{
    struct ElmLargeNode* node = (struct ElmLargeNode*)view;
    elm_pool_free_large(pool, node);
}

static inline struct DbrBook*
elm_pool_alloc_book(struct ElmPool* pool)
{
    struct ElmLargeNode* node = elm_pool_alloc_large(pool);
    return node ? &node->book : NULL;
}

static inline void
elm_pool_free_book(struct ElmPool* pool, struct DbrBook* book)
{
    struct ElmLargeNode* node = (struct ElmLargeNode*)book;
    elm_pool_free_large(pool, node);
}

static inline struct DbrSess*
elm_pool_alloc_sess(struct ElmPool* pool)
{
    struct ElmSmallNode* node = elm_pool_alloc_small(pool);
    return node ? &node->sess : NULL;
}

static inline void
elm_pool_free_sess(struct ElmPool* pool, struct DbrSess* sess)
{
    struct ElmSmallNode* node = (struct ElmSmallNode*)sess;
    elm_pool_free_small(pool, node);
}

#endif // ELM_POOL_H

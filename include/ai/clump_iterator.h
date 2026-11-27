#ifndef AI_CLUMP_ITERATOR_H
#define AI_CLUMP_ITERATOR_H

#include "ai/clump.h"
#include "ai/clump_datum.h"
#include "ai/actor_datum.h"
#include "ai/prop_datum.h"
#include "memory/tls_globals.h"
#include "memory/data.h"
#include "ai/clump_data.h"
#include "ai/prop_ref_datum.h"
#include "ai/prop_ref_data.h"

static __forceinline void clump_actor_iterator_new(clump_actor_iterator *iterator, clump_datum *clump)
{
    iterator->next_actor_index = clump->first_actor_index;
}

static __forceinline void clump_actor_iterator_new(clump_actor_iterator *iterator, long clump_index)
{
    clump_actor_iterator_new(iterator, datum_get<clump_datum>(clump_data, clump_index));
}

static __forceinline actor_datum *clump_actor_iterator_next(clump_actor_iterator *iterator)
{
    actor_datum *result = 0;
    if (iterator->next_actor_index != NONE) {
        result = ((actor_datum *)((char *)actor_data->data + ((iterator->next_actor_index) & 0xffff) * sizeof(actor_datum)));
        iterator->actor_index = iterator->next_actor_index;
        iterator->next_actor_index = result->next_clump_actor_index;
    }
    return result;
}

// Cursor over a clump's linked list of props (March clump.obj 82f84310/82f84368).
struct clump_prop_iterator
{
    long index;
    long next_index;
};

typedef char clump_prop_iterator_size[(sizeof(clump_prop_iterator) == 8) ? 1 : -1];

static __forceinline void clump_prop_iterator_new(clump_prop_iterator *iterator, clump_datum *clump)
{
    iterator->next_index = clump->first_prop_index;
}

static __forceinline void clump_prop_iterator_new(clump_prop_iterator *iterator, long clump_index)
{
    clump_prop_iterator_new(iterator, datum_get<clump_datum>(clump_data, clump_index));
}

static __forceinline prop_datum *clump_prop_iterator_next(clump_prop_iterator *iterator)
{
    prop_datum *result = 0;
    if (iterator->next_index != NONE) {
        result = ((prop_datum *)((char *)prop_data->data + ((iterator->next_index) & 0xffff) * sizeof(prop_datum)));
        iterator->index = iterator->next_index;
        iterator->next_index = result->next_prop;
    }
    return result;
}

// Cursor over a prop's linked list of prop references (March clump.obj 82f84618/82f84670).
struct clump_prop_ref_iterator
{
    long index;
    long next_index;
};

typedef char clump_prop_ref_iterator_size[(sizeof(clump_prop_ref_iterator) == 8) ? 1 : -1];

static __forceinline void clump_prop_ref_iterator_new(clump_prop_ref_iterator *iterator, long prop_index)
{
    iterator->next_index = datum_get<prop_datum>(prop_data, prop_index)->first_prop_ref_index;
}

static __forceinline prop_ref_datum *clump_prop_ref_iterator_next(clump_prop_ref_iterator *iterator)
{
    prop_ref_datum *result = 0;
    if (iterator->next_index != NONE) {
        result = datum_get<prop_ref_datum>(prop_ref_data, iterator->next_index);
        iterator->index = iterator->next_index;
        iterator->next_index = result->prop_next_ref_index;
    }
    return result;
}

#endif // AI_CLUMP_ITERATOR_H

#ifndef AI_ACTOR_PROP_REF_ITERATOR_H
#define AI_ACTOR_PROP_REF_ITERATOR_H

#include "ai/actor_datum.h"
#include "ai/prop_ref_datum.h"
#include "memory/tls_globals.h"
#include "memory/data.h"

// Cursor over an actor's linked list of prop references.
struct actor_prop_ref_iterator
{
    long index;
    long next_index;
};

typedef char actor_prop_ref_iterator_size[(sizeof(actor_prop_ref_iterator) == 8) ? 1 : -1];

// March clump.obj 82f84540/82f84598: new sets next_index; next updates index only on success.
// match: 0x5123b8, 0x513fc8: the data-array argument preserves the existing actor lookup boundary and base reuse.
static __forceinline actor_datum *actor_prop_ref_iterator_actor_get(s_data_array *data, long actor_index)
{
    return datum_get<actor_datum>(data, actor_index);
}

// match: 0x513fc8: keep the data-array lookup boundary shared with the caller's prop reference reads.
static __forceinline prop_ref_datum *actor_prop_ref_iterator_ref_get(s_data_array *data, long prop_ref_index)
{
    return datum_get<prop_ref_datum>(data, prop_ref_index);
}

static __forceinline void actor_prop_ref_iterator_new(actor_prop_ref_iterator *iterator, long actor_index)
{
    iterator->next_index = actor_prop_ref_iterator_actor_get(tls_actor_data(), actor_index)->first_prop_ref_index;
}

static __forceinline prop_ref_datum *actor_prop_ref_iterator_next(actor_prop_ref_iterator *iterator)
{
    prop_ref_datum *result = 0;
    if (iterator->next_index != -1)
    {
        result = actor_prop_ref_iterator_ref_get(tls_prop_ref_data(), iterator->next_index);
        iterator->index = iterator->next_index;
        iterator->next_index = result->next_prop_ref_index;
    }
    return result;
}

#endif // AI_ACTOR_PROP_REF_ITERATOR_H

#ifndef AI_ACTOR_NEARBY_ITERATOR_TYPES_H
#define AI_ACTOR_NEARBY_ITERATOR_TYPES_H

#include <stddef.h>
#include "memory/data_types.h"
#include "ai/clump.h"
#include "math/real_math_types.h"

struct clump_datum;

// Layout read by actors.cpp's nearby iterator routines; the initial 16 bytes are unused there.
struct actor_nearby_actors_iterator
{
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    struct clump_iterator
    {
        clump_datum *datum;
        s_data_iterator iterator;
        long index;
    } clump_iter;
    clump_actor_iterator actor_iter;
    short filter;
    short team;
    float radius;
    float radius_squared;
    long clump_index;
    real_point3d point;
    long actor_index;
    float distance_squared;
};

typedef char actor_nearby_iterator_size[(sizeof(void *) != 4 || ((sizeof(actor_nearby_actors_iterator) == 0x50))) ? 1 : -1];
typedef char actor_nearby_iterator_actor_offset[(sizeof(void *) != 4 || ((offsetof(actor_nearby_actors_iterator, actor_index) == 0x48))) ? 1 : -1];

#endif

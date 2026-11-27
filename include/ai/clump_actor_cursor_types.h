#ifndef AI_CLUMP_ACTOR_CURSOR_TYPES_H
#define AI_CLUMP_ACTOR_CURSOR_TYPES_H

#include "memory/data_types.h"

struct squad_datum;

struct clump_actor_cursor {
    void *datum;
    s_data_iterator iterator;
    long index;
};

extern "C" squad_datum *squad_iterator_next(clump_actor_cursor *iterator);

extern "C" void *clump_actor_cursor_next(clump_actor_cursor *cursor);

#endif // AI_CLUMP_ACTOR_CURSOR_TYPES_H

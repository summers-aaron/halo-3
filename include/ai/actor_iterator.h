#ifndef AI_ACTOR_ITERATOR_H
#define AI_ACTOR_ITERATOR_H

#include "memory/data.h"

struct actor_datum;

// Actor iteration extends the shared data iterator with filtering and current actor index.
struct actor_iterator : c_data_iterator<actor_datum>
{
    bool active_only;
    long index;
};

typedef char actor_iterator_size[(sizeof(void *) != 4 || sizeof(actor_iterator) == 0x18) ? 1 : -1];

#endif // AI_ACTOR_ITERATOR_H

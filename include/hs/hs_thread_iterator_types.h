#ifndef HS_HS_THREAD_ITERATOR_TYPES_H
#define HS_HS_THREAD_ITERATOR_TYPES_H

#include "memory/data_types.h"

struct s_hs_thread_iterator {
    long index;
    bool active_only;
    bool include_remote;
    bool finished;
};

#endif // HS_HS_THREAD_ITERATOR_TYPES_H

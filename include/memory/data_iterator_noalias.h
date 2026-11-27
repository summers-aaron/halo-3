#ifndef MEMORY_DATA_ITERATOR_NOALIAS_H
#define MEMORY_DATA_ITERATOR_NOALIAS_H

struct s_data_iterator;

// match: 0x1f8620: noalias keeps the iterator word live in r30 across the call; without it looping_sound_find_sound reloads it with an extra lwz+mr.
__declspec(noalias) void *data_iterator_next(s_data_iterator *iterator);

#endif // MEMORY_DATA_ITERATOR_NOALIAS_H

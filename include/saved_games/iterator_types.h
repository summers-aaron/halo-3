#ifndef SAVED_GAMES_ITERATOR_TYPES_H
#define SAVED_GAMES_ITERATOR_TYPES_H

#include "memory/data_types.h"

struct s_datum;

struct s_iterator
{
    s_datum *datum;
    s_data_iterator iterator;
};

#endif // SAVED_GAMES_ITERATOR_TYPES_H

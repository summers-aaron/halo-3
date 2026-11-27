#ifndef GAME_PLAYER_WITH_UNIT_ITERATOR_H
#define GAME_PLAYER_WITH_UNIT_ITERATOR_H

#include "memory/data_types.h"
#include "game/player_data.h"

struct player_datum;

// March players.obj: begin 829a8670 delegates to the data iterator at +0;
// next 829a86b0 skips datums without a unit. No header path is attested.
class c_player_with_unit_iterator
{
public:
    __forceinline void begin()
    {
        iterator.data = player_data;
        iterator.absolute_index = -1;
        iterator.index = -1;
    }
    bool next();
    player_datum *get_datum() { return datum; }
    long get_index() { return iterator.index; }
    short get_absolute_index() { return (short)(iterator.index & 0xffff); }

    player_datum *datum;
    s_data_iterator iterator;
};

typedef char player_with_unit_iterator_size[(sizeof(void *) != 4 || ((sizeof(c_player_with_unit_iterator) == 0x10) )) ? 1 : -1];

#endif // GAME_PLAYER_WITH_UNIT_ITERATOR_H

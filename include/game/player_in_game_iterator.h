#ifndef GAME_PLAYER_IN_GAME_ITERATOR_H
#define GAME_PLAYER_IN_GAME_ITERATOR_H

#include "memory/data.h"
#include "game/player_data.h"

struct player_datum;
// The players TLS slot, with the extern "C" linker name memory/tls_globals.h declares for every TLS slot.

// March players.obj: begin 829a84e8 delegates to c_data_iterator<player_datum> at +0;
// get_datum/get_index/get_absolute_index are non-const (829a85c8/829a85f8/829a8630).
// No method asserts a header path; this leaf keeps the existing players.h includers independent.
class c_player_in_game_iterator {
public:
    // Always inlined; starts iteration over the player data.
    __forceinline void begin() { m_iterator.begin(player_data); }
    bool next();
    player_datum *get_datum() { return m_iterator.m_datum; }
    long get_index() { return m_iterator.m_iterator.index; }
    short get_absolute_index() { return m_iterator.get_absolute_index(); }

    c_data_iterator<player_datum> m_iterator;
};

typedef char player_in_game_iterator_size[(sizeof(void *) != 4 || ((sizeof(c_player_in_game_iterator) == 0x10) )) ? 1 : -1];

#endif // GAME_PLAYER_IN_GAME_ITERATOR_H

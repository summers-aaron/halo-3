#ifndef GAME_PLAYER_ITERATOR_H
#define GAME_PLAYER_ITERATOR_H

struct player_iterator
{
    long player_index;
    long controller_index;
};

typedef char player_iterator_size[(sizeof(player_iterator) == 8) ? 1 : -1];

#endif // GAME_PLAYER_ITERATOR_H

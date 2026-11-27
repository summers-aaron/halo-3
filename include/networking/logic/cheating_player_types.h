#ifndef NETWORKING_LOGIC_CHEATING_PLAYER_TYPES_H
#define NETWORKING_LOGIC_CHEATING_PLAYER_TYPES_H

#include "memory/data_types.h"
#include "shell/shell.h"

struct cheating_player
{
    union
    {
        unsigned __int64 identifier;
        e_controller_index controller_index;
    };
    unsigned __int64 xuid;
    wchar_t name[16];
    unsigned long cheats;
};

typedef char cheating_player_size[(sizeof(cheating_player) == 56) ? 1 : -1];

#endif // NETWORKING_LOGIC_CHEATING_PLAYER_TYPES_H

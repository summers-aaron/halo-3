#ifndef INTERFACE_C_SIGNED_IN_USERS_WATCHER_H
#define INTERFACE_C_SIGNED_IN_USERS_WATCHER_H

#include "game/player_identifier_types.h"

struct c_signed_in_users_watcher
{
    long signin_state;
    s_player_identifier users[4];
    bool have_users_changed();
};

typedef char signed_in_users_watcher_size[(sizeof(void *) != 4 || sizeof(c_signed_in_users_watcher) == 0x24) ? 1 : -1];

#endif // INTERFACE_C_SIGNED_IN_USERS_WATCHER_H

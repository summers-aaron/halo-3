#ifndef ITEMS_MULTIPLAYER_WEAPON_TRACKER_H
#define ITEMS_MULTIPLAYER_WEAPON_TRACKER_H

#include <stddef.h>

struct s_multiplayer_weapon_tracker {
    long weapon_index;
    short id;
    short pad6;
    long carrier_unit_index;
    long carrier_player_index;
};

typedef char mp_weapon_tracker_size[(sizeof(s_multiplayer_weapon_tracker) == 0x10) ? 1 : -1];
typedef char mp_weapon_tracker_carrier_unit_offset[(offsetof(s_multiplayer_weapon_tracker, carrier_unit_index) == 8) ? 1 : -1];
typedef char mp_weapon_tracker_carrier_player_offset[(offsetof(s_multiplayer_weapon_tracker, carrier_player_index) == 0xc) ? 1 : -1];

#endif // ITEMS_MULTIPLAYER_WEAPON_TRACKER_H

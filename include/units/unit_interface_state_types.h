#ifndef UNITS_UNIT_INTERFACE_STATE_TYPES_H
#define UNITS_UNIT_INTERFACE_STATE_TYPES_H

struct unit_interface_state
{
    // Zeroed by the builder and never read; the first two words hold no live field in either build.
    long spare[2];
    bool primary_weapon;
    bool dual_weapons;
    bool seat_flag;
    char padb;
};

#endif // UNITS_UNIT_INTERFACE_STATE_TYPES_H

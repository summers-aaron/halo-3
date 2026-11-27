#ifndef UNITS_UNIT_WEAPON_PICKUP_RESULT_H
#define UNITS_UNIT_WEAPON_PICKUP_RESULT_H
#include "cseries/cseries.h"

struct unit_weapon_pickup_result
{
    bool primary[2];
    bool backpack[2];
};

typedef char unit_weapon_pickup_result_size[(sizeof(unit_weapon_pickup_result) == 4) ? 1 : -1];

#endif

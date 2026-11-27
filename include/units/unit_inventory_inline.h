#ifndef UNITS_UNIT_INVENTORY_INLINE_H
#define UNITS_UNIT_INVENTORY_INLINE_H

#include "objects/object_header.h"
#include "units/unit_types.h"

static __forceinline long unit_inventory_get_weapon(long unit_index, short inventory_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 0x2003);
    return unit->unit.weapon_object_indices[inventory_index];
}

// Checked inventory requests may use NONE for an empty hand.
static __forceinline long unit_inventory_get_weapon_or_none(long unit_index, short inventory_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 8195);
    long weapon_index = -1;
    if (inventory_index != -1)
        weapon_index = unit->unit.weapon_object_indices[inventory_index];
    return weapon_index;
}

// Inventory requests may use NONE for an empty hand.
static __forceinline long unit_inventory_try_get_weapon(long unit_index, short inventory_index)
{
    unit_datum *unit = (unit_datum *)object_header_get(unit_index)->datum;
    long weapon_index = NONE;
    if (inventory_index != NONE)
        weapon_index = unit->unit.weapon_object_indices[inventory_index];
    return weapon_index;
}

#endif

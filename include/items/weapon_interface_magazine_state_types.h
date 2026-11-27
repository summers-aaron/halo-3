#ifndef ITEMS_WEAPON_INTERFACE_MAGAZINE_STATE_TYPES_H
#define ITEMS_WEAPON_INTERFACE_MAGAZINE_STATE_TYPES_H

struct weapon_interface_magazine_state
{
    bool active;
    bool idle;
    short rounds_loaded;
    short rounds_loaded_maximum;
    short rounds_inventory;
    short rounds_inventory_maximum;
};

#endif // ITEMS_WEAPON_INTERFACE_MAGAZINE_STATE_TYPES_H

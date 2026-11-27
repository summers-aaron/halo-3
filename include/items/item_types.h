#ifndef ITEMS_ITEM_TYPES_H
#define ITEMS_ITEM_TYPES_H

#include <stddef.h>

#include "objects/object_datum.h"
#include "items/multiplayer_weapon_tracker.h"

struct item_datum {
    object_datum object;
    unsigned char inventory_flags;
    unsigned char owner_relation;
    short remaining_count;
    long toss_unit_index;
    long relevance_time;
    long unit_object_index;
    long last_unit_object_index;              // (item_in_unit_inventory keeps the last carrier on removal)
};
typedef item_datum s_item_datum;

struct equipment_datum {
    item_datum item;
    long creation_time;
    long activation_time;
    unsigned char charges_used;
    unsigned char proximity_ticks;
    s_damage_owner creator_damage_owner;
    long looping_effect_index;
};
typedef equipment_datum s_equipment_datum;

typedef char equipment_proximity_ticks_offset[(offsetof(equipment_datum, proximity_ticks) == 0x17d) ? 1 : -1];
typedef char equipment_creator_damage_owner_offset[(offsetof(equipment_datum, creator_damage_owner) == 0x180) ? 1 : -1];
typedef char equipment_looping_effect_offset[(offsetof(equipment_datum, looping_effect_index) == 0x18c) ? 1 : -1];

typedef char item_detonation_timer_offset[(offsetof(item_datum, remaining_count) == 0x162) ? 1 : -1];

typedef char item_types_size_item[(sizeof(item_datum) == 0x174) ? 1 : -1];
typedef char item_types_size_equipment[(sizeof(equipment_datum) == 0x190) ? 1 : -1];
typedef char item_types_offset_map_variant_index[(offsetof(item_datum, object.map_variant_index) == 0x18) ? 1 : -1];
typedef char item_types_offset_last_unit[(offsetof(item_datum, last_unit_object_index) == 0x170) ? 1 : -1];

#endif // ITEMS_ITEM_TYPES_H

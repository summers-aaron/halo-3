#ifndef SCENARIO_SCENARIO_LIGHT_TYPES_H
#define SCENARIO_SCENARIO_LIGHT_TYPES_H

#include "scenario/scenario_objects.h"

// Light-specific placement fields extending the common scenario object.
// The common scenario object supplies the palette index used by game_create_objects.
struct s_scenario_light : s_scenario_object
{
    char spare_before_type[8];
    short type;
    unsigned short flags;
    unsigned short lightmap_type;
    char spare_before_endpoint[0x6c - 0x62];
    real_point3d endpoint;
    float near_distance;
    float far_distance;
    float field_of_view;
    float near_width;
    float far_width;
};

// One palette entry describing an object type available for placement.
struct scenario_object_palette_entry
{
    s_tag_reference object;
    // Sandbox menus use the palette entry string identifier at +0x10.
    union { long name; long category; };
    long maximum_allowed;
    float price;
    char spare_after_price[20];
};

typedef char scenario_light_size[(sizeof(s_scenario_light) == 0x8c) ? 1 : -1];
typedef char scenario_light_type_offset[(offsetof(s_scenario_light, type) == 0x5c) ? 1 : -1];
typedef char scenario_object_palette_entry_size[(sizeof(scenario_object_palette_entry) == 0x30) ? 1 : -1];

typedef char scenario_object_palette_entry_index_offset[(offsetof(scenario_object_palette_entry, object) + offsetof(s_tag_reference, index) == 0xc) ? 1 : -1];
typedef char scenario_object_palette_entry_maximum_offset[(offsetof(scenario_object_palette_entry, maximum_allowed) == 0x14) ? 1 : -1];
typedef char scenario_object_palette_entry_price_offset[(offsetof(scenario_object_palette_entry, price) == 0x18) ? 1 : -1];

#endif

#ifndef AI_ZONE_H
#define AI_ZONE_H

#include <stddef.h>

#include "ai/ai_point3d.h"
#include "tag_files/tag_types.h"

// A reference to one area of one zone (ai\Zone.cpp; ZONE_AREA_REF_NONE lives there in the March 2007
// debug build). Copied and compared by value as one word.
#pragma pack(push, 2)
struct zone_area_ref
{
    union
    {
        struct { short zone_index; short area_index; };
        long value;
    };
};
#pragma pack(pop)

typedef char zone_area_ref_size[(sizeof(zone_area_ref) == 4) ? 1 : -1];

extern zone_area_ref ZONE_AREA_REF_NONE;

struct area_definition
{
    char name[32];
    long flags;
    c_ai_point3d point;
    float radius;                              // squads: adoption and dynamic-area falloff
    short first_firing_position;
    short firing_position_count;
    unsigned long cluster_bits[8];             // ai_update_activation intersects these with the BSP PVS.
    short manual_reference_frame;
    short structure_bsp_index;
    s_tag_block flight_hints;
};

struct zone_definition
{
    char name[32];
    unsigned long flags;
    short structure_bsp_index;
    unsigned short bsp_flags;
    s_tag_block firing_positions;
    s_tag_block areas;
};

typedef char area_definition_radius_offset[(offsetof(area_definition, radius) == 0x34) ? 1 : -1];
typedef char area_definition_flags_offset[(offsetof(area_definition, flags) == 0x20) ? 1 : -1];
typedef char area_definition_size[(sizeof(area_definition) == 0x6c) ? 1 : -1];
typedef char area_definition_point_offset[(offsetof(area_definition, point) == 0x24) ? 1 : -1];
typedef char area_definition_firing_position_offset[(offsetof(area_definition, first_firing_position) == 0x38) ? 1 : -1];
typedef char area_definition_flight_hints_offset[(offsetof(area_definition, flight_hints) == 0x60) ? 1 : -1];
typedef char area_definition_cluster_bits_offset[(offsetof(area_definition, cluster_bits) == 0x3c) ? 1 : -1];
typedef char area_definition_name_offset[(offsetof(area_definition, name) == 0) ? 1 : -1];
typedef char area_definition_manual_reference_frame_offset[(offsetof(area_definition, manual_reference_frame) == 0x5c) ? 1 : -1];
typedef char area_definition_structure_bsp_index_offset[(offsetof(area_definition, structure_bsp_index) == 0x5e) ? 1 : -1];
typedef char zone_definition_size[(sizeof(zone_definition) == 0x40) ? 1 : -1];
typedef char zone_definition_name_offset[(offsetof(zone_definition, name) == 0) ? 1 : -1];
typedef char zone_definition_flags_offset[(offsetof(zone_definition, flags) == 0x20) ? 1 : -1];
typedef char zone_definition_structure_bsp_index_offset[(offsetof(zone_definition, structure_bsp_index) == 0x24) ? 1 : -1];
typedef char zone_definition_bsp_flags_offset[(offsetof(zone_definition, bsp_flags) == 0x26) ? 1 : -1];
typedef char zone_definition_areas_offset[(offsetof(zone_definition, areas) == 0x34) ? 1 : -1];
typedef char zone_definition_firing_positions_offset[(offsetof(zone_definition, firing_positions) == 0x28) ? 1 : -1];

// Functions defined in tu/ailibxbox/Zone.cpp that other units call.

area_definition *zone_get_area(short zone_index, short area_index);

#endif // AI_ZONE_H

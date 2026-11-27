#ifndef ITEMS_EQUIPMENT_H
#define ITEMS_EQUIPMENT_H

#include "math/real_math_types.h"

// Functions defined in tu/objectslibxbox/equipment.cpp.

struct object_placement_data;
struct s_scenario_equipment;

bool equipment_new(long equipment_index, object_placement_data *placement_data, bool *out_of_memory);
void equipment_place(long equipment_index, s_scenario_equipment *scenario_equipment);
long equipment_remaining_charges(long equipment_index);
float equipment_active_fraction(long equipment_index);
void equipment_update(long equipment_index, long unit_index);
extern "C" unsigned char equipment_activate(long equipment_index, long unit_index, long flags);
extern "C" void terminal_activate_camera(long object_index);

struct s_equipment_slot
{
    bool in_use;
    bool active;
    char spare_after_active[8 - 2];
    long start_time;
    long identifier;
    real_point3d center;
    float radius;
    float speed;
    union { long function; long point_count; };
};

typedef char equipment_slot_start_time_offset[(offsetof(s_equipment_slot, start_time) == 8) ? 1 : -1];
typedef char equipment_slot_size[(sizeof(s_equipment_slot) == 0x28) ? 1 : -1];

extern "C" int equipment_find_slot(s_equipment_slot *slots, long identifier);

union real_point3d;
extern "C" void equipment_compute_orbit_point(s_equipment_slot *slots, long index, long sub_index, real_point3d *point);

#endif // ITEMS_EQUIPMENT_H

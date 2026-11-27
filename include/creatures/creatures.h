#ifndef CREATURES_CREATURES_H
#define CREATURES_CREATURES_H

// Functions defined in tu/objectslibxbox/creatures.cpp.

union real_vector3d;
struct s_creature_control_data;
struct s_damage_aftermath_result_data;
struct s_damage_data;

struct object_placement_data;
bool creature_new(long object_index, object_placement_data *placement_data, bool *out_of_memory);
void creature_control_clear(s_creature_control_data *data);
void creature_set_actively_controlled(long object_index, bool enable);
void creature_accelerate(long object_index, union real_vector3d const * acceleration);
void creature_damage_aftermath_compute(long creature_index, long damage_flags, const s_damage_data *damage_data, s_damage_aftermath_result_data *result);
bool creature_try_to_impact(long creature_index);

#endif // CREATURES_CREATURES_H

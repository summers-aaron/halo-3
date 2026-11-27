#ifndef SCENARIO_SCENARIO_TRIGGER_VOLUMES_H
#define SCENARIO_SCENARIO_TRIGGER_VOLUMES_H

// Functions defined in tu/blamlibxbox/scenario_trigger_volumes.cpp that other units call.

struct real_matrix4x3;
union real_point3d;
struct scenario_trigger_volume;

bool trigger_volume_get_matrix(const scenario_trigger_volume *trigger_volume, real_matrix4x3 *matrix);
bool scenario_trigger_volume_test_point(long volume_index, const real_point3d *point);
bool scenario_trigger_volume_test_object(long trigger_volume_index, long object_index);
bool scenario_trigger_volume_test_players_all(long trigger_volume_index);
long scenario_trigger_volume_return_objects_by_type(long volume_index, long type_mask);
extern "C" unsigned char scenario_trigger_volumes_test_point(const real_point3d *point);

#endif // SCENARIO_SCENARIO_TRIGGER_VOLUMES_H

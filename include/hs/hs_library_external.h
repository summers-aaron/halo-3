#ifndef HS_HS_LIBRARY_EXTERNAL_H
#define HS_HS_LIBRARY_EXTERNAL_H

// Functions defined in tu/blamlibxbox/hs_library_external.cpp that other units call.

union real_point3d;
union real_vector3d;

void hs_teleport_players_not_in_trigger_volume(short volume, short flag);
float hs_objects_distance_to_object(long object_list_index, long object_index);
extern "C" float hs_objects_distance_to_flag(long object_list, short flag_index);
void hs_object_destroy_by_name(short name_index);
void hs_object_create_anew(short name_index);
void hs_object_create(short name_index);
void hs_object_create_clone(short name_index);
void hs_object_destroy_all_type_mask(long type_mask);
void hs_object_iterate_names_containing(const char *substring,void (*callback)(short));
extern "C" void hs_object_teleport_to_position(long object_index, const real_point3d *position, const real_vector3d *forward, const real_vector3d *up,
	bool update_player, bool);
extern "C" void hs_object_teleport(long object_index, short flag_index);
void hs_objects_predict(long object_list_index,bool low_priority);
extern "C" void hs_effect_new_from_object_marker(long effect_index, long object_index, long marker_name);
void hs_damage_object_effect(long damage_effect, long object_index);
void hs_damage_object(long object_index, long region_name, float damage_amount);
void hs_effect_new_on_ground(long effect_index, long object_index);
void hs_damage_players(long damage_effect);
extern "C" void hs_objects_create_by_name(int id);
bool hs_objects_can_see_object(long object_list_index, long target_object_index, float angle);
bool hs_objects_can_see_flag(long object_list_index, short flag_index, float angle);
extern "C" void hs_objects_delete_by_name(int id);
void hs_damage_new(long p0, short p1);
void hs_effect_new(long effect_index, short flag_index);
void hs_effect_new_at_ai_point(long effect_index, long point_reference);

#include "objects/object_lifecycle.h"

#endif // HS_HS_LIBRARY_EXTERNAL_H

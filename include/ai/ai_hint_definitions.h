#ifndef AI_AI_HINT_DEFINITIONS_H
#define AI_AI_HINT_DEFINITIONS_H

// Functions defined in tu/ailibxbox/ai_hint_definitions.cpp that other units call.

class c_ai_point3d;
class c_sector_ref;
struct object_marker;
struct pathfinding_data;
struct real_matrix4x3;
union real_point3d;
union real_vector3d;
struct special_movement;
enum e_action_hoist;

bool ai_link_hint_traversable(pathfinding_data *scenario, long trigger_index);
short ai_hint_get_vault_markers(long object_index, object_marker *markers, short maximum_marker_count, short flags);
short ai_hint_get_mount_markers(long object_index, object_marker *markers, short maximum_marker_count, short flags);
short ai_hint_get_hoist_markers(long object_index, object_marker *markers, short maximum_marker_count, short flags);
bool ai_hint_vault_range_valid(const real_point3d *start, const real_point3d *end, const special_movement *movement);
bool pathfinding_find_seam_link(short structure_bsp_index, long marker_index, short * out_structure_bsp_index, long * out_element, long * out_value);
bool ai_hint_test_jump(union real_point3d * start, union real_point3d * start2, union real_point3d * end, union real_point3d * end2, short type);
bool ai_hint_find_vault_marker(long actor_index, long object_index, float radius, object_marker *marker_ref, real_point3d *point_ref);
extern "C" unsigned char ai_hint_find_mount_marker(long actor_index, long object_index, float radius, object_marker *marker, real_point3d *point);
bool ai_hint_find_root_entry_matrix(long unit_index, long posture, long hint_type, const real_point3d *point,
    const real_vector3d *vector, real_matrix4x3 *matrix);
bool ai_hint_find_pill_entry_point(long actor_index, long posture, long hint_type, long seat, long state, const real_point3d *point,
    const real_vector3d *vector, real_point3d *point2, c_ai_point3d *ai_point, real_vector3d *vector2, c_sector_ref *sector);
bool ai_hint_find_root_entry_point(long actor_index, long posture, long hint_type, const real_point3d *point,
    const real_vector3d *vector, real_point3d *point2, c_ai_point3d *ai_point, real_vector3d *vector2, c_sector_ref *sector);
bool ai_hint_get_maneuver_point(long actor_index, float radius, long hint_index, const c_ai_point3d *point, c_sector_ref sector,
    c_ai_point3d *entry_point, c_sector_ref *entry_sector, c_ai_point3d *exit_point, c_sector_ref *exit_sector);
enum e_action_hoist ai_hint_get_best_hoist_height(long object_index, float height);
extern "C" bool ai_hint_find_hoist_marker(long actor_index, long object_index, float radius,
    object_marker *marker_ref, real_point3d *point_ref);

struct pathfinding_hint_data;
bool ai_hint_get_jump_maneuver_point(pathfinding_data *data, pathfinding_hint_data *hint, const c_ai_point3d *point, c_sector_ref sector, float radius,
    c_ai_point3d *start_point, c_sector_ref *start_sector, c_ai_point3d *end_point, c_sector_ref *end_sector);

#endif // AI_AI_HINT_DEFINITIONS_H

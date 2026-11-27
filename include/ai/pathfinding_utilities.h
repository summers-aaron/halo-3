#ifndef AI_PATHFINDING_UTILITIES_H
#define AI_PATHFINDING_UTILITIES_H

// Functions defined in tu/ailibxbox/pathfinding_utilities.cpp.

class c_ai_point3d;
class c_sector_ref;
struct prop_state;

bool actor_pathfinding_available(long actor_index);
bool object_pathfinding_immediate(long object_index);
void actor_find_pathfinding_location(long actor_index);
void prop_find_pathfinding_location_internal(long actor_index, long object_index, prop_state *state);
void prop_find_pathfinding_location(long actor_index, long ref_index);
void object_get_pathfinding_location(long object_index, c_sector_ref *sector, c_ai_point3d *point);
bool object_pathfinding_available(long object_index);
void object_get_giant_pathfinding_location(long object_index, c_sector_ref *sector, c_ai_point3d *point);

#endif // AI_PATHFINDING_UTILITIES_H

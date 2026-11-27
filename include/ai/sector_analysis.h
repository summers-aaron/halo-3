#ifndef AI_SECTOR_ANALYSIS_H
#define AI_SECTOR_ANALYSIS_H

#include "ai/firing_position_ref.h"

// Functions defined in tu/ailibxbox/sector_analysis.cpp that other units call.

class c_ai_point3d;
class c_sector_ref;
struct firing_position_definition;
struct pathfinding_data;
union real_point3d;
union real_vector3d;

bool firing_position_get_normal(firing_position_definition *position, real_vector3d *normal);
bool runtime_cornering_get_entry(long actor_index, const c_ai_point3d *search, c_sector_ref search_sector,
    const c_ai_point3d *marker, const real_vector3d *direction, short type, bool alternate, c_ai_point3d *point, c_sector_ref *sector);
bool runtime_cornering_get_info(pathfinding_data *data, long link_index, const c_ai_point3d *ai_point,
    real_vector3d *direction, real_point3d *point, real_vector3d *out_normal, char *out_corner);
bool runtime_manual_cornering_get_info(long actor_index, firing_position_ref reference, c_ai_point3d *point, real_vector3d *direction, char *cornering_type);

#endif // AI_SECTOR_ANALYSIS_H

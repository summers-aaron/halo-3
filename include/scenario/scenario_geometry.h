#ifndef SCENARIO_SCENARIO_GEOMETRY_H
#define SCENARIO_SCENARIO_GEOMETRY_H

// Functions defined in tu/blamlibxbox/scenario_geometry.cpp that other units call.

struct s_render_geometry;

bool scenario_structure_bsp_has_valid_lightmap_geometry(long scenario_index, long structure_bsp_index);
const s_render_geometry *scenario_structure_bsp_get_render_geometry(long scenario_index, long structure_bsp_index);

#endif // SCENARIO_SCENARIO_GEOMETRY_H

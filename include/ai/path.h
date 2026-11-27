#ifndef AI_PATH_H
#define AI_PATH_H

#include "ai/reserved_hints.h"

// Functions defined in tu/ailibxbox/path.cpp.

struct path_result;
struct path_state;
struct path_input;
struct path_error_info;
struct path_debug_storage;
struct special_movement;

void path_state_new(const path_input *input, path_state *state, path_error_info *error,
    path_debug_storage *debug, special_movement *movement, const actor_moving_reserved_hints *reserved_hints);

bool path_state_find(path_state *state);

class c_sector_ref;
class c_ai_point3d;
union real_vector3d;
bool path_state_estimated_distance(path_state *state, const c_ai_point3d *point, c_sector_ref sector_ref,
    float *distance, float *deviation, real_vector3d *direction);
short path_node_from_hash_table(path_state *state, c_sector_ref sector_ref);
bool path_state_build_path(long actor_index, path_state *state, path_result *result);

struct path_link;
struct path_node;
union real_point3d;

bool path_state_traverse(path_state *state);
void path_heap_bubble_up(struct path_state * _sym_p0, short heap_location);
void path_heap_bubble_down(struct path_state * _sym_p0, short heap_location);
void closest_point_to_attractor(const real_point3d *start, const real_point3d *end, const real_point3d *attractor, real_point3d *result);
bool path_state_begin(path_state *state);
short build_path_links_for_sector(const path_node *node, path_link *links, path_state *state);

struct path_step;
struct path_step_source;

short build_raw_path(long unit_index, path_state *state, path_result *result, short node_index,
    short max_steps, path_step *steps, bool *out_finished, short *out_source_count,
    path_step_source *sources, short max_sources);

class c_ai_point3d;
bool path_state_approach_point(path_state *state, const c_ai_point3d *point, c_sector_ref sector, bool *complete, c_ai_point3d *result);

#endif // AI_PATH_H

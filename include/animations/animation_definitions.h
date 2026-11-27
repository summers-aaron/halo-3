#ifndef ANIMATIONS_ANIMATION_DEFINITIONS_H
#define ANIMATIONS_ANIMATION_DEFINITIONS_H

#include "animations/animation_definition_types.h"

// Functions defined in tu/blamlibxbox/animation_definitions.cpp that other units call.

class c_model_animation;
class c_model_animation_graph;
class c_animation_id;
struct render_model_definition;
struct real_matrix4x3;
struct s_tag_block;

extern "C" void animation_definitions_get_animation_root_matrix(long graph_index, c_animation_id reference, float frame,
    const render_model_definition *model, real_matrix4x3 *matrix);

struct animation_aiming_screen_bounds
{
    float bounds[6];
};

typedef char animation_aiming_screen_bounds_size[(sizeof(animation_aiming_screen_bounds) == 24) ? 1 : -1];

float compute_relative_frame_index(const c_model_animation *source, const c_model_animation *destination, float frame);

class c_animation_data;
class c_channel_base;

extern "C" bool animation_definitions_try_and_get_data(const c_channel_base *channel, c_animation_data *out_data);

// Data defined in tu/blamlibxbox/animation_definitions.cpp that other units use.

extern long damage_type_string_id_list[];

union real_vector3d;
extern "C" void animation_definitions_compute_blend_and_playback(const c_model_animation_graph *graph, c_animation_id first_id, c_animation_id second_id,
    const real_vector3d *velocity, bool limited, float *blend, float *playback);
extern "C" bool animation_definitions_animation_has_variation_chain(const c_model_animation_graph *definition, c_animation_id reference);
extern "C" c_animation_id *animation_definitions_find_random_variation(c_animation_id *out, const c_model_animation_graph *definition,
    c_animation_id reference, bool deterministic);
extern "C" void animation_definitions_compute_movement_totals(
    const c_model_animation_graph *graph, c_animation_id reference, short component, float *at_key, float *total);

#endif // ANIMATIONS_ANIMATION_DEFINITIONS_H

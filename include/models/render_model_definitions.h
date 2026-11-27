#ifndef MODELS_RENDER_MODEL_DEFINITIONS_H
#define MODELS_RENDER_MODEL_DEFINITIONS_H

#include <stddef.h>
#include "math/matrix_math_types.h"
#include "geometry/render_geometry_types.h"
#include "models/render_model_marker_types.h"
#include "models/render_model_marker_group_types.h"

struct s_sky_gen_light
{
    real_vector3d direction;
    real_rgb_color color;
    float intensity;
};

typedef char sky_gen_light_size[(sizeof(s_sky_gen_light) == 0x1c) ? 1 : -1];

struct render_model_region
{
    long name;
    s_tag_block permutations;
};

typedef char render_model_region_size[(sizeof(render_model_region) == 0x10) ? 1 : -1];

struct render_model_instance_placement
{
    long name;
    long node_index;
    real_matrix4x3 matrix;
};

typedef char render_model_instance_placement_size[(sizeof(render_model_instance_placement) == 0x3c) ? 1 : -1];

struct render_model_node
{
    long name;
    short parent_node_index;
    short first_child_node_index;
    short next_sibling_node_index;
    unsigned short flags;
    real_point3d default_translation;
    real_quaternion default_rotation;
    real_matrix4x3 default_inverse_matrix;
    float distance_from_parent;
};

// Field order follows the tag layout; sizes are verified below.
struct render_model_definition
{
    long name;
    unsigned short flags;
    short version;
    long checksum;
    s_tag_block regions;
    long : 32;
    long instance_starting_mesh_index;
    s_tag_block instance_placements;
    long node_list_checksum;
    s_tag_block nodes;
    s_tag_block marker_groups;
    s_tag_block materials;
    s_tag_block errors;
    float dont_draw_over_camera_cosine_angle;
    s_render_geometry render_geometry;
    s_tag_block lightgen_lights;
    float ravi_red[16];
    float ravi_green[16];
    float ravi_blue[16];
    s_tag_block volume_samples;
    s_tag_block runtime_node_orientations;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char render_model_node_size[(sizeof(render_model_node) == 0x60) ? 1 : -1];
typedef char render_model_translation_offset[(offsetof(render_model_node, default_translation) == 0x0c) ? 1 : -1];
typedef char render_model_inverse_offset[(offsetof(render_model_node, default_inverse_matrix) == 0x28) ? 1 : -1];
typedef char render_model_definition_size[(sizeof(render_model_definition) == 0x1cc) ? 1 : -1];
typedef char render_model_instance_mesh_offset[(offsetof(render_model_definition, instance_starting_mesh_index) == 0x1c) ? 1 : -1];
typedef char render_model_nodes_offset[(offsetof(render_model_definition, nodes) == 0x30) ? 1 : -1];
typedef char render_model_markers_offset[(offsetof(render_model_definition, marker_groups) == 0x3c) ? 1 : -1];
typedef char render_model_lightgen_lights_offset[(offsetof(render_model_definition, lightgen_lights) == 0xe8) ? 1 : -1];
typedef char render_model_ravi_offset[(offsetof(render_model_definition, ravi_red) == 0xf4) ? 1 : -1];
#endif

#endif // MODELS_RENDER_MODEL_DEFINITIONS_H

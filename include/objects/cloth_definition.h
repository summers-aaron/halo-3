#ifndef OBJECTS_CLOTH_DEFINITION_H
#define OBJECTS_CLOTH_DEFINITION_H

#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

// Serialized clwd layout; ClothProperties ends with 24 padding bytes.
// Evidence: TheGuardians/TagTool, TagTool/Tags/Definitions/Cloth.cs.
struct s_cloth_definition_vertex {
    real_point3d position;
    real_point2d texcoord;
};
struct s_cloth_attachment_edge {
    short vertex_index_a;
    short vertex_index_b;
    float rest_length;
};
struct s_cloth_definition {
    long flags;
    long attachment_marker_name;
    long attachment_end_marker_name;
    s_tag_reference render_method;
    short grid_x_dimension;
    short grid_y_dimension;
    float grid_spacing_x;
    float grid_spacing_y;
    s_tag_block collision_spheres;
    short integration_type;
    short iteration_base;
    float weight;
    float drag;
    float wind_scale;
    float wind_flappiness_scale;
    float longest_rod;
    // TagTool Cloth.cs: 24 Padding bytes after LongestRod.
    char longest_rod_padding[24];
    s_tag_block vertices;
    s_tag_block indices;
    s_tag_block strip_indices;
    s_tag_block attachment_edges;
};

#endif // OBJECTS_CLOTH_DEFINITION_H

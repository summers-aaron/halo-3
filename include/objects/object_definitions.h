#ifndef OBJECTS_OBJECT_DEFINITIONS_H
#define OBJECTS_OBJECT_DEFINITIONS_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "cseries/cseries_wrapped.h"
#include "objects/multiplayer_object_types.h"

struct s_object_attachment_definition { s_tag_reference type; long marker; short change_color; short pad; long primary_scale; long secondary_scale; };

struct s_object_change_color_initial_permutation {
    float weight;                   // +0
    real_rgb_color color_a;         // +4
    real_rgb_color color_b;         // +16
    long variant_name;                 // +28
};

struct s_object_change_color_function {
    // TagTool ChangeColorFunction Unused: 4 pad bytes on Original (MCC drops them, size 0x24).
    char function_padding[4];
    unsigned long flags;     // +4
    real_rgb_color color_a;     // +8
    real_rgb_color color_b;     // +20
    long scale_function;        // +32
    long tint_function;         // +36
};

struct s_object_change_color_definition {
    s_tag_block initial_permutations;
    s_tag_block functions;
};
typedef char object_tag_layout_1[(sizeof(s_object_attachment_definition) == 0x20) ? 1 : -1];
typedef char object_tag_layout_2[(sizeof(s_object_change_color_initial_permutation) == 0x20) ? 1 : -1];
typedef char object_tag_layout_3[(sizeof(s_object_change_color_function) == 0x28) ? 1 : -1];
typedef char object_tag_layout_4[(sizeof(s_object_change_color_definition) == 0x18) ? 1 : -1];
typedef char object_tag_layout_5[(offsetof(s_object_change_color_definition, functions) == 0x0c) ? 1 : -1];

struct s_object_definition
{
    short object_type;
    union
    {
        unsigned short flags;
        c_flags_no_init<long, unsigned short, 16> object_flags;
    };
    float bounding_radius;
    real_point3d bounding_offset;
    float acceleration_scale;
    short lightmap_shadow_mode;
    signed char sweetener_size;
    signed char water_density;
    unsigned long runtime_flags;
    float dynamic_light_sphere_radius;
    real_point3d dynamic_light_sphere_offset;
    long default_model_variant;
    s_tag_reference model;
    s_tag_reference crate_object;
    s_tag_reference collision_damage;
    s_tag_block early_mover_obb;
    s_tag_reference creation_effect;
    s_tag_reference material_effects;
    s_tag_reference melee_sound;
    s_tag_block ai_properties;
    s_tag_block functions;
    short hud_text_message_index;
    unsigned short secondary_flags;
    s_tag_block attachments;
    s_tag_block widgets;
    s_tag_block change_colors;
    s_tag_block predicted_resources;
    c_typed_tag_block<s_multiplayer_object_properties_definition> multiplayer_object;
};

typedef char object_definition_flags_offset[(offsetof(s_object_definition, object_flags) == 2) ? 1 : -1];

typedef char object_definition_model_offset[(offsetof(s_object_definition, model) == 0x34) ? 1 : -1];
typedef char object_definition_multiplayer_offset[(offsetof(s_object_definition, multiplayer_object) == 0xec) ? 1 : -1];
typedef char object_definition_size[(sizeof(s_object_definition) == 0xf8) ? 1 : -1];

#endif // OBJECTS_OBJECT_DEFINITIONS_H

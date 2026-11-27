#ifndef MODELS_MODEL_VARIANT_TYPES_H
#define MODELS_MODEL_VARIANT_TYPES_H

#include "tag_files/tag_types.h"
#include "math/matrix_math_types.h"
#include <stddef.h>

struct s_model_instance_group_member {
    long subgroup;
    long instance_name;
    float weight;
    unsigned long instances[2];
};

struct s_model_instance_group {
    long name;
    long mode;
    s_tag_block members;
    float weight;
};

struct s_model_node {
    long name;
    short parent_node_index;
    short first_child_node_index;
    short next_sibling_node_index;
    short import_node_index;
    real_point3d default_translation;
    real_quaternion default_rotation;
    real_matrix4x3 default_inverse;
};

typedef char model_node_parent_offset[(offsetof(s_model_node, parent_node_index) == 4) ? 1 : -1];
typedef char model_node_size[(sizeof(s_model_node) == 0x5c) ? 1 : -1];
typedef char model_node_rotation_offset[(offsetof(s_model_node, default_rotation) == 0x18) ? 1 : -1];

struct s_model_customization_region {
    long region_name;
    long permutation_name;
};

struct s_model_variant_attachment
{
    long parent_marker;
    long child_marker;
    long child_variant;
    s_tag_reference child_object;
};
typedef char model_variant_attachment_size[(sizeof(s_model_variant_attachment) == 0x1c) ? 1 : -1];
typedef char model_variant_attachment_object_index_offset[
    (offsetof(s_model_variant_attachment, child_object) + offsetof(s_tag_reference, index) == 0x18) ? 1 : -1];

struct s_model_variant {
    long name;
    signed char region_indices[16];
    s_tag_block regions;
    s_tag_block attachments;
    long instance_group_index;
    // The serialized Xbox 360 variant has eight trailing padding bytes.
    unsigned long : 32;
    unsigned long : 32;
};

typedef char model_variant_attachments_offset[(offsetof(s_model_variant, attachments) == 0x20) ? 1 : -1];
typedef char model_variant_size[(sizeof(s_model_variant) == 0x38) ? 1 : -1];

struct s_model_permutation_possibility {
    long permutation_index;
    long variant_index;
    long type;
    long property;
    long effect;
    long flags;
    float weight;
};


struct s_model_permutation_state {
    long name;
    signed char permutation_index;
    unsigned char property;
    unsigned short type;
    s_tag_reference looping_effect;
    long flags;
    float weight;
};

struct s_model_permutation {
    long name;
    signed char index;
    unsigned char flags;
    float weight;
    s_tag_block states;
    signed char runtime_state_permutation_indices[12];
};

struct s_model_variant_region {
    long name;
    signed char render_model_region_index;
    signed char runtime_flags;
    short parent_variant;
    s_tag_block permutations;
    unsigned short zsort;
};
typedef char model_variant_region_zsort_offset[(offsetof(s_model_variant_region, zsort) == 20) ? 1 : -1];
typedef char model_variant_region_size[(sizeof(s_model_variant_region) == 24) ? 1 : -1];


typedef char model_node_translation_offset[(offsetof(s_model_node, default_translation) == 0xc) ? 1 : -1];
typedef char model_node_inverse_offset[(offsetof(s_model_node, default_inverse) == 0x28) ? 1 : -1];
typedef char model_permutation_state_size[(sizeof(s_model_permutation_state) == 0x20) ? 1 : -1];
typedef char model_permutation_state_effect_offset[(offsetof(s_model_permutation_state, looping_effect) == 8) ? 1 : -1];
typedef char model_permutation_size[(sizeof(s_model_permutation) == 0x24) ? 1 : -1];
typedef char model_permutation_runtime_indices_offset[
    (offsetof(s_model_permutation, runtime_state_permutation_indices) == 0x18) ? 1 : -1];

#endif

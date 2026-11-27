#ifndef EFFECTS_PARTICLE_DEFINITIONS_H
#define EFFECTS_PARTICLE_DEFINITIONS_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "effects/editable_property.h"

class c_particle_state_list;
class c_particle;

class c_particle_definition
{
public:
    void initialize_particle(const c_particle_state_list *state_list, c_particle *particle, long lightprobe_index) const;
    __forceinline bool get_particle_is_particle_model() const { return (flags2 & 0x8000) != 0; }
    __forceinline bool get_particle_dies_on_structure_collision() const { return (flags >> 1) & 1; }
    __forceinline bool get_particle_dies_on_media_collision() const { return (flags >> 2) & 1; }
    __forceinline bool get_has_collision_effect() const { return (flags >> 7) & 1; }
private:
    __forceinline bool get_particle_tint_from_lightmap() const { return (flags2 & 8) != 0; }
    __forceinline bool get_particle_tint_from_diffuse_texture() const { return (flags2 & 0x10) != 0; }
    __forceinline void evaluate_animation_rate_if_not_constant_over_time(const c_particle_state_list *state_list, float *value) const;
public:
    unsigned long flags;
    s_tag_block m_attachments;
    unsigned long flags2;
    unsigned short count;
    short m_first_sequence_index;
    short m_sequence_count;
    real_point2d m_center_offset;
    float scalar;
    float angle;
    float near_fade;
    float forward_scale;
    float up_scale;
    float depth_scale;
    s_tag_reference render_method;
    s_tag_block m_render_method_options;
    s_tag_block m_render_method_parameters;
    s_tag_block m_render_method_postprocess;
    unsigned short m_render_method_flags;
    unsigned char m_sort_layer;
    unsigned char m_render_method_version;
    long m_custom_fog_setting_index;
    long m_prediction_atom_index;
    c_editable_property<c_particle_state_list> m_aspect_ratio;
    c_editable_property<c_particle_state_list> m_color;
    c_editable_property<c_particle_state_list> m_intensity;
    c_editable_property<c_particle_state_list> m_alpha;
    unsigned long flags3;
    c_editable_property<c_particle_state_list> m_frame_index;
    c_editable_property<c_particle_state_list> m_animation_rate;
    c_editable_property<c_particle_state_list> m_palette_animation;
    unsigned long m_particle_model_group_tag;
    TAG_POINTER(const char) m_particle_model_name;
    long m_particle_model_name_length;
    long particle_model;
    unsigned long used_states;
    unsigned long m_constant_per_particle_properties;
    unsigned long m_constant_over_time_properties;
    long m_gpu_sprite_count;
    TAG_POINTER(const void) m_sprite_data;
    TAG_POINTER(void) m_gpu_sprite_block_definition;
    long m_sprite_count;
    TAG_POINTER(const void) m_frames_data;
    TAG_POINTER(void) m_gpu_frames_block_definition;
};

typedef char particle_definition_used_states_offset[(offsetof(c_particle_definition, used_states) == 0x170) ? 1 : -1];
typedef char particle_definition_sort_layer_offset[(offsetof(c_particle_definition, m_sort_layer) == 0x72) ? 1 : -1];
typedef char particle_definition_animation_rate_offset[(offsetof(c_particle_definition, m_animation_rate) == 0x120) ? 1 : -1];

typedef char particle_definition_flags_offset[(offsetof(c_particle_definition, flags2) == 0x10) ? 1 : -1];
typedef char particle_definition_render_method_offset[(offsetof(c_particle_definition, render_method) == 0x3c) ? 1 : -1];
typedef char particle_definition_model_offset[(offsetof(c_particle_definition, particle_model) == 0x16c) ? 1 : -1];
typedef char particle_definition_sprite_offset[(sizeof(void *) != 4 || offsetof(c_particle_definition, m_sprite_data) == 0x180) ? 1 : -1];
typedef char particle_definition_frames_offset[(sizeof(void *) != 4 || offsetof(c_particle_definition, m_frames_data) == 0x18c) ? 1 : -1];

typedef char particle_definition_attachments_offset[(offsetof(c_particle_definition, m_attachments) == 4) ? 1 : -1];
typedef char particle_definition_center_offset[(offsetof(c_particle_definition, m_center_offset) == 0x1c) ? 1 : -1];
typedef char particle_definition_options_offset[(offsetof(c_particle_definition, m_render_method_options) == 0x4c) ? 1 : -1];
typedef char particle_definition_aspect_ratio_offset[(offsetof(c_particle_definition, m_aspect_ratio) == 0x7c) ? 1 : -1];
typedef char particle_definition_frame_index_offset[(offsetof(c_particle_definition, m_frame_index) == 0x100) ? 1 : -1];
typedef char particle_definition_palette_animation_offset[(offsetof(c_particle_definition, m_palette_animation) == 0x140) ? 1 : -1];
typedef char particle_definition_constants_offset[(offsetof(c_particle_definition, m_constant_per_particle_properties) == 0x174) ? 1 : -1];
typedef char particle_definition_gpu_sprite_count_offset[(offsetof(c_particle_definition, m_gpu_sprite_count) == 0x17c) ? 1 : -1];
typedef char particle_definition_size[(sizeof(void *) != 4 || sizeof(c_particle_definition) == 0x194) ? 1 : -1];

// Particle collision entry: tag reference to trigger, collision type it handles, and scale selectors.
struct s_particle_collision_entry {
    s_tag_reference reference;
    unsigned char type;
    unsigned char reserved11;
    unsigned char flag_a;
    unsigned char flag_b;
};
typedef char particle_collision_entry_size[(sizeof(s_particle_collision_entry) == 0x14) ? 1 : -1];

struct s_particle_collision_effect {
    unsigned long flags;
    unsigned long count;
    TAG_POINTER(s_particle_collision_entry) entries;
};

#endif // EFFECTS_PARTICLE_DEFINITIONS_H

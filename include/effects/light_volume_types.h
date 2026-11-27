#ifndef EFFECTS_LIGHT_VOLUME_TYPES_H
#define EFFECTS_LIGHT_VOLUME_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include "render_methods/render_method.h"
#include "effects/editable_property.h"
#include "effects/effect_runtime_storage.h"
#include "memory/data.h"

struct real_matrix4x3;

struct s_light_volume_definition
{
    string_id name;
    union
    {
        c_render_method m_shader;
        struct
        {
            s_tag_reference render_method_reference;
            s_tag_block shader_options;
            s_tag_block shader_parameters;
            s_tag_block shader_postprocess;
            unsigned short shader_flags;
            unsigned char shader_sort_layer;
            unsigned char shader_version;
            long shader_atmosphere_index;
        };
    };
    long prediction_atom_index;
    unsigned short appearance_flags;
    unsigned char type;
    unsigned char blend;
    real_vector2d uv_tiling_rate;
    real_vector2d uv_scroll_rate;
    float profile_scale;
    float profile_bias;
    float angle_scale;
    float angle_bias;
    c_editable_property<c_light_volume_states> m_length;
    c_editable_property<c_light_volume_states> m_density;
    c_editable_property<c_light_volume_states> m_profile_length;
    c_editable_property<real_point2d> m_profile_offset;
    c_editable_property<c_light_volume_states> m_profile_rotation;
    c_editable_property<c_light_volume_states> m_profile_thickness;
    c_editable_property<c_light_volume_states> m_profile_color;
    c_editable_property<c_light_volume_states> m_profile_alpha;
    c_editable_property<c_light_volume_states> m_profile_black_point;
    c_editable_property<c_light_volume_states> m_profile_palette;
    c_editable_property<c_light_volume_states> m_profile_intensity;
    unsigned long m_constant_per_profile_properties;
    unsigned long flags;
    unsigned long max_profile_count;
    s_tag_block gpu_properties;
    s_tag_block gpu_functions;
    s_tag_block gpu_colors;

    __forceinline const c_render_method *get_shader() const { return &m_shader; }
    __forceinline void evaluate_length_if_not_constant_over_time(const c_light_volume_states *states, float *value) const
    {
        if (!c_editable_property_base::property_value_is_initialized(value) || !m_length.is_constant_over_time())
            *value = m_length.evaluate(states);
    }
    __forceinline void evaluate_density_if_not_constant_over_time(const c_light_volume_states *states, float *value) const
    {
        if (!c_editable_property_base::property_value_is_initialized(value) || !m_density.is_constant_over_time())
            *value = m_density.evaluate(states);
    }
    __forceinline void evaluate_profile_length_if_not_constant_over_time(const c_light_volume_states *states, float *value) const
    {
        if (!c_editable_property_base::property_value_is_initialized(value) || !m_profile_length.is_constant_over_time())
            *value = m_profile_length.evaluate(states);
    }
    __forceinline bool get_fogged() const { return (appearance_flags & 8) != 0; }
};

struct s_light_volume_system_definition
{
    s_tag_block light_volumes;
    const s_light_volume_definition *get_light_volume_definition(long index) const
    {
        return (const s_light_volume_definition *)tag_block_get_element_with_size(&light_volumes, index, sizeof(s_light_volume_definition));
    }
};

class c_light_volume_location;
class c_light_volume_states;

class c_light_volume_system
{
public:
    static void dispose_from_game();
    void set_should_live(bool value)
    {
        if (value)
            m_flags |= 1;
        else
            m_flags &= ~1;
    }
    void set_should_frame_advance(bool value)
    {
        if (value)
            m_flags |= 2;
        else
            m_flags &= ~2;
    }
    void set_should_render(bool value)
    {
        if (value)
            m_flags |= 4;
        else
            m_flags &= ~4;
    }
    void set_ever_frame_advanced(bool value)
    {
        if (value)
            m_flags |= 8;
        else
            m_flags &= ~8;
    }
    static __forceinline c_light_volume_system *get(long index);
    void frame_advance(float dt);
    static __forceinline void pulse_all(float dt);
    static __forceinline void destroy(long system_index);
    bool get_should_live() const { return ((m_flags >> 0) & 1) != 0; }
    bool get_should_frame_advance() const { return ((m_flags >> 1) & 1) != 0; }


    union { long m_salt; unsigned short m_identifier; };
    union { long m_effect_index; long effect_index; };
    union { unsigned char m_event_block_index; unsigned char event_block_index; };
    unsigned char m_priority;
    unsigned short m_part_block_index;
    long part_index;
    long m_definition_index;
    union { long m_first_location_index; long location_index; };
    unsigned short m_flags;
    unsigned char first_person_type;
    union { unsigned char first_person_flags; signed char m_first_person_flags; };
    float m_age;
    union { float m_effect_scale_a; float scale_a; };
    union { float m_effect_scale_b; float scale_b; };
    long m_lod;

    static void initialize_for_map();
    static void frame_advance_all(float delta_time);
    long add_light_volume_location(const real_matrix4x3 *matrix, unsigned long seed);
    void query_effect(float delta_time);
};

__forceinline c_light_volume_system *c_light_volume_system::get(long index)
{
    s_data_array *systems = (s_data_array *)g_light_volume_system_data;
    return datum_get<c_light_volume_system>(systems, index);
}

class c_light_volume
{
public:
    static void dispose_from_game();
    static long create(unsigned char type, long location, long definition, unsigned long seed);
    void frame_advance(float dt, c_light_volume_states *states);
    __forceinline const s_light_volume_definition *get_definition() const;
    void submit() const;
    void render() const;
    __forceinline float get_length() const { return m_offset; }
    __forceinline float get_profile_density() const { return m_profile_length; }
    __forceinline long get_definition_index() const { return m_definition_index; }
    __forceinline const ::c_light_volume_location *get_parent_location() const;
private:
    static void render_callback(const void *light_volume, long user_data);
public:
    static __forceinline c_light_volume *get(long light_volume_index);

    static void destroy(long light_volume_index);
    long m_identifier;
    long m_next_light_volume_index;
    long m_gpu_light_volume_index;
    long m_definition_index;
    long m_location_index;
    long m_flags;
    float m_length;
    float m_offset;
    float m_density;
    float m_profile_length;
    unsigned long m_random_seed;
    float m_random_values[2];
};

__forceinline c_light_volume *c_light_volume::get(long light_volume_index)
{
    return datum_get<c_light_volume>(g_light_volume_data, light_volume_index);
}

typedef char light_volume_length_property_offset[(offsetof(s_light_volume_definition, m_length) == 0x68) ? 1 : -1];
typedef char light_volume_density_property_offset[(offsetof(s_light_volume_definition, m_density) == 0x88) ? 1 : -1];
typedef char light_volume_profile_length_property_offset[(offsetof(s_light_volume_definition, m_profile_length) == 0xa8) ? 1 : -1];
typedef char light_volume_profile_offset_property_offset[(offsetof(s_light_volume_definition, m_profile_offset) == 0xc8) ? 1 : -1];
typedef char light_volume_profile_rotation_property_offset[(offsetof(s_light_volume_definition, m_profile_rotation) == 0xf8) ? 1 : -1];
typedef char light_volume_definition_size[(sizeof(s_light_volume_definition) == 0x208) ? 1 : -1];
typedef char light_volume_definition_flags_offset[(offsetof(s_light_volume_definition, flags) == 0x1dc) ? 1 : -1];
typedef char light_volume_definition_profiles_offset[(offsetof(s_light_volume_definition, max_profile_count) == 0x1e0) ? 1 : -1];
typedef char light_volume_definition_properties_offset[(offsetof(s_light_volume_definition, gpu_properties) == 0x1e4) ? 1 : -1];
typedef char light_volume_definition_functions_offset[(offsetof(s_light_volume_definition, gpu_functions) == 0x1f0) ? 1 : -1];
typedef char light_volume_definition_colors_offset[(offsetof(s_light_volume_definition, gpu_colors) == 0x1fc) ? 1 : -1];
typedef char light_volume_system_size[(sizeof(c_light_volume_system) == 0x2c) ? 1 : -1];
typedef char light_volume_system_def_offset[(offsetof(c_light_volume_system, m_definition_index) == 0x10) ? 1 : -1];
typedef char light_volume_system_age_offset[(offsetof(c_light_volume_system, m_age) == 0x1c) ? 1 : -1];
typedef char light_volume_system_part_offset[(offsetof(c_light_volume_system, part_index) == 0xc) ? 1 : -1];
typedef char light_volume_system_user_offset[(offsetof(c_light_volume_system, first_person_flags) == 0x1b) ? 1 : -1];
typedef char light_volume_size[(sizeof(::c_light_volume) == 0x34) ? 1 : -1];
typedef char light_volume_offset_offset[(offsetof(::c_light_volume, m_offset) == 0x1c) ? 1 : -1];
typedef char light_volume_random_offset[(offsetof(::c_light_volume, m_random_values) == 0x2c) ? 1 : -1];

#endif // EFFECTS_LIGHT_VOLUME_TYPES_H

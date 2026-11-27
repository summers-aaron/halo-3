#ifndef RASTERIZER_GLOBAL_SHADER_PARAMETER_TYPES_H
#define RASTERIZER_GLOBAL_SHADER_PARAMETER_TYPES_H

// Serialized extern indices, in the order of k_global_shader_parameters.
enum e_render_method_extern
{
    _render_method_extern_none = 0,
    _render_method_extern_texture_global_target_texaccum = 1,
    _render_method_extern_texture_global_target_normal = 2,
    _render_method_extern_texture_global_target_z = 3,
    _render_method_extern_texture_global_target_shadow_buffer1 = 4,
    _render_method_extern_texture_global_target_shadow_buffer2 = 5,
    _render_method_extern_texture_global_target_shadow_buffer3 = 6,
    _render_method_extern_texture_global_target_shadow_buffer4 = 7,
    _render_method_extern_texture_global_target_texture_camera = 8,
    _render_method_extern_texture_global_target_reflection = 9,
    _render_method_extern_texture_global_target_refraction = 10,
    _render_method_extern_texture_lightprobe_texture = 11,
    _render_method_extern_texture_dominant_light_intensity_map = 12,
    _render_method_extern_texture_unused1 = 13,
    _render_method_extern_texture_unused2 = 14,
    _render_method_extern_object_change_color_primary = 15,
    _render_method_extern_object_change_color_secondary = 16,
    _render_method_extern_object_change_color_tertiary = 17,
    _render_method_extern_object_change_color_quaternary = 18,
    _render_method_extern_object_emblem_color_background = 19,
    _render_method_extern_object_emblem_color_primary = 20,
    _render_method_extern_object_emblem_color_secondary = 21,
    _render_method_extern_texture_dynamic_environment_map_0 = 22,
    _render_method_extern_texture_dynamic_environment_map_1 = 23,
    _render_method_extern_texture_cook_torrance_cc0236 = 24,
    _render_method_extern_texture_cook_torrance_dd0236 = 25,
    _render_method_extern_texture_cook_torrance_c78d78 = 26,
    _render_method_extern_light_dir_0 = 27,
    _render_method_extern_light_color_0 = 28,
    _render_method_extern_light_dir_1 = 29,
    _render_method_extern_light_color_1 = 30,
    _render_method_extern_light_dir_2 = 31,
    _render_method_extern_light_color_2 = 32,
    _render_method_extern_light_dir_3 = 33,
    _render_method_extern_light_color_3 = 34,
    _render_method_extern_texture_unused_3 = 35,
    _render_method_extern_texture_unused_4 = 36,
    _render_method_extern_texture_unused_5 = 37,
    _render_method_extern_texture_dynamic_light_gel_0 = 38,
    _render_method_extern_flat_envmap_matrix_x = 39,
    _render_method_extern_flat_envmap_matrix_y = 40,
    _render_method_extern_flat_envmap_matrix_z = 41,
    _render_method_extern_debug_tint = 42,
    _render_method_extern_screen_constants = 43,
    _render_method_extern_active_camo_distortion_texture = 44,
    _render_method_extern_scene_ldr_texture = 45,
    _render_method_extern_scene_hdr_texture = 46,
    _render_method_extern_water_memory_export_address = 47,
    _render_method_extern_tree_animation_timer = 48,
    k_number_of_render_method_externs
};

struct s_global_shader_parameter
{
    long type;
    long register_index;
    unsigned long register_count;
    bool pixel_shader;
    const char *name;
};

typedef char global_shader_parameter_size[(sizeof(void *) != 4 || sizeof(s_global_shader_parameter) == 0x14) ? 1 : -1];

#endif // RASTERIZER_GLOBAL_SHADER_PARAMETER_TYPES_H

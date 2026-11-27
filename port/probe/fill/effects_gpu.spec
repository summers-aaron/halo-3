# GPU effect shader tables (shader index, tag reference, c_slot_set<N> {table, slots_base -> own slots, slot_count,
# any_required, N x {key, value, required}}, vertex type, then the pixel shader), all words; the required bytes are 0.
datum g_contrail_spawn_shaders 0x7be9b8 w p3 w6 w p3 w6
datum g_contrail_update_shaders 0x7bea08 w p3 w18 w p3 w5
datum g_contrail_render_vertex_shader 0x7bea84 p3 w9
datum g_contrail_frame_shader_pair 0x7bf420 w p3 w24 w p3 w5
datum g_effect_shader_cache 0x7bf4b4 p3 w9
datum g_light_volume_gpu_update_shader 0x7bf690 w p3 w24 w p3 w5
datum g_light_volume_gpu_render_shader 0x7bf724 p3 w9
datum g_particle_render_vertex_shader 0x7be8a8 p3 w19
datum g_particle_render_model_vertex_shader 0x7be900 p3 w18
datum k_particle_default_gpu_variant 0x4b884 p w5
# common_gpu_vertex_buffer (0x38 of words: format / element count / element size at +0x2c) and s_gpu_buffer (+ locked).
datum g_contrail_gpu_vertex_buffer 0x7be978 w14
datum g_contrail_frame_vertex_buffer 0x7bf2f4 w14 b4
datum g_light_volume_gpu_buffer 0x7bf508 w14
# Constant-slot keys and required flags.
datum k_particle_block_6slot_required 0x24cd0 b8
# Math / table constants (floats, longs, chars).
datum k_particle_emitter_array_name 0x42918 b472
datum k_contrail_update_constant_required 0x24c70 b8
datum k_contrail_render_constant_required 0x24c78 b8
datum k_particle_block_7slot_required 0x24bd0 b8
datum k_light_volume_render_required 0x24bd8 b4
datum g_motion_blur_enabled 0x72cb02 b1
datum k_particle_block_5slot_required 0x24cd8 b8
# The particle update shader pair at its retail size: the pixel shader carries a c_slot_set<1> the source type lacks
# (s_gpu_pixel_shader holds only a pointer; particle_block.cpp views it as s_gpu_constant_table_base; GAME-CHANGES F26).
datum g_particle_update_shaders 0x7be820 w p3 w21 w p3 w5
# particle_block.cpp g_gpu_particle_default_constants = (const float *)0x820249f4 (GAME-CHANGES F6): the float4s it names.

# GAME-CHANGES F62: word data moved into fill/rdata_pool.spec (retail_real_pool), named here for pointers into it
name 0x24ce4 k_particle_block_6slot_keys
name 0x2588c k_widescreen_aspect_ratio
name 0x3aac8 k_real_math_identity_matrix_storage
name 0x3ac90 global_real_argb_colors
name 0x361a8 k_random_direction_table
name 0x23238 k_posture_variant_indices
name 0x21420 ?g_damage_reporting_type_string_list!!3QBJB
name 0x24c80 k_contrail_update_constant_keys
name 0x24c94 k_contrail_render_constant_keys
name 0x24bdc k_particle_block_7slot_keys
name 0x24bf8 k_light_volume_render_keys
name 0x245fc k_primary_skull_string_ids
name 0x24cfc k_particle_block_5slot_keys
name 0x249f4 retail_249f4

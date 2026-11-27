# GAME-CHANGES F62 (probe stand-in, not the fix): retail .rdata as one datum, every word byte swapped, and each
# 32-bit constant or constant-pool name below placed inside it at its retail offset (retailfill `pool`). Source that
# reads a retail constant pool at a fixed offset from one of these names (`(&k_real_zero_storage)[418]`,
# `k_real_constant_pool_three[-2102]`, `literal_1000_0f[308]`, through a macro or a local pointer) then reads the retail
# word, as the console does. The source fix is the literal or a named `// @rdata` constant per site (F62; census
# tmp/m5g_gameplay/anchor_census.tsv and tmp/m5h/pool/). Only word data: names with halfword, byte or pointer
# layouts keep their own datums in the other specs.
pool retail_real_pool 0x400 0x5d880

# Float constants and pool anchors the probe link had as one-word stubs or as aliases of a PC datum at the same
# retail address (stubgen), whatever follows them on the PC
datum k_real_constant_pool 0x221fc w
datum k_real_constant_pool_one 0x221fc w
datum k_postprocess_real_constants 0x221fc w
datum g_rasterizer_depth_bias_constant_pool 0x221fc w
datum k_real_zero_storage 0x2251c w
datum k_real_constant_pool_zero 0x2251c w
datum g_real_constant_pool_zero 0x2251c w
datum k_ai_dialogue_real_constants 0x2251c w
datum k_weapon_pitch_throttling_constants 0x2251c w
datum k_light_volume_gpu_zero 0x2251c w
datum k_sound_class_minimum_fade_time 0x2251c w
datum k_real_constant_pool_half 0x22dc0 w
datum k_juggernaut_delay_rounding_bias 0x22dc0 w
datum scenery_animation_time_constant_pool 0x2aed4 w
datum k_real_constants_minus_one 0x32938 w
datum k_real_math_origin3d 0x3aa90 w3
datum k_real_math_zero_storage 0x3aa90 w
datum k_real_math_zero_vector3d_storage 0x3aa90 w3
datum k_real_math_zero_vector_storage 0x3aa90 w4
datum k_projectiles_angular_velocity_scale 0xa90 w
datum k_rasterizer_screen_x_offset_scale 0xabc w
datum k_world_time_reciprocal_numerator 0xafc w
datum k_random_word_normalization 0xb34 w
datum k_chud_random_fraction_scale 0xb34 w
datum k_damage_random_fraction_scale 0xb34 w
datum k_particle_random_fraction_scale 0xb34 w

# Word data from fill/effects_gpu.spec
datum k_particle_block_6slot_keys 0x24ce4 w6
datum k_widescreen_aspect_ratio 0x2588c w1
datum k_real_math_identity_matrix_storage 0x3aac8 w13
datum global_real_argb_colors 0x3ac90 w12
datum k_random_direction_table 0x361a8 w3123
datum k_posture_variant_indices 0x23238 w350
datum ?g_damage_reporting_type_string_list!!3QBJB 0x21420 w157
datum k_contrail_update_constant_keys 0x24c80 w5
datum k_contrail_render_constant_keys 0x24c94 w15
datum k_particle_block_7slot_keys 0x24bdc w7
datum k_light_volume_render_keys 0x24bf8 w30
datum k_primary_skull_string_ids 0x245fc w18
datum k_particle_block_5slot_keys 0x24cfc w5
datum retail_249f4 0x249f4 w16

# Word data from fill/jungle_data.spec
datum k_player_fix_position_search_directions 0x3ccc0 w27
datum ?k_standard_interpolation_time!!3MB 0x2aee0 w
datum k_real_half_pool 0x22dc0 w
datum k_real_constant_pool_three 0x242d4 w
datum k_default_reverb_definition 0x35d40 w26
datum k_lightmap_shadows_quality_shader_indices 0x4b7f8 w4
datum k_behavior_surprise_movement_values 0x22518 w
datum k_behavior_posture_type_names 0x231a8 w35

# Word data from fill/m5f_poisoned.spec
datum k_cinematics_perspective_m22 0x5884c w17
datum k_cinematics_perspective_m32 0x58848 w1
datum k_creature_ground_airborne_delay 0x2b10c w62
datum k_equipment_activation_markers 0x22b74 w2
datum k_function_constant_pool_anchor 0x39294 w2
datum k_game_engine_type_string_ids 0x33614 w55
datum k_real_constant_pool_two 0x242d8 w201
datum k_real_math_epsilon_storage 0x2490c w64
datum k_saved_film_sound_gain 0x39540 w100
datum k_scripted_sound_default_decibels 0x35f08 w41
datum k_scripted_sound_default_gain 0x35f08 w41
datum scripted_sound_constant_pool_anchor 0x35f08 w41
datum k_secondary_skull_string_ids 0x24644 w10
datum k_xmscalar_sincos_cos0_coeffs 0xf20 w4
datum k_xmscalar_sincos_cos1_coeffs 0xed0 w4
datum k_xmscalar_sincos_cos2_coeffs 0xe80 w4
datum k_xmscalar_sincos_sin0_coeffs 0xf10 w4
datum k_xmscalar_sincos_sin1_coeffs 0xf30 w4
datum k_xmscalar_sincos_sin2_coeffs 0xe40 w4
datum literal_0_1f 0x22dbc w1
datum literal_0_75f 0x22b14 w24
datum literal_0_95f 0x2215c w40
datum literal_1000_0f 0x336f0 w41
datum literal_10_0f 0x22850 w177
datum literal_neg_0_5f 0x254e4 w5
datum k_xaudio_environment_scalar1 0x58c78 w1
datum k_xaudio_environment_values2_0 0x58c74 w1
datum k_xaudio_environment_values2_3 0x58c6c w1
datum k_xaudio_environment_values2_4 0x58c68 w1

# Word data from fill/tag_resource.spec
datum retail_32958 0x32958 w4
datum retail_32948 0x32948 w4

# GAME-CHANGES F55: retail constants and tables the source references but does not define (milestone 5F audit of the
# probe's poisoned data stubs). Layouts from the source declarations; sizes from the retail survey where the
# declaration is an unsized array or a constant pool anchor (the gap to the next named datum).
# float / long constant pools and tables
datum k_debug_director_camera_modes 0x744618 w2
datum k_terminal_state_string_ids 0x72fe18 w8
datum k_bink_quantization_tables 0x7cea0 w1026
datum literal_3_0 0xc68 q3
# XAUDIOCHANNELMAPENTRY: input channel, output channel (bytes), padding, volume (float)
datum k_xaudio_environment_entries_2 0x744b40 b2 z2 w b2 z2 w
datum k_xaudio_environment_entries_4 0x744b50 b2 z2 w b2 z2 w b2 z2 w b2 z2 w
datum k_xaudio_environment_entries_6 0x744b70 b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w
datum k_xaudio_reverb_entries_1 0x744938 b2 z2 w
datum k_xaudio_reverb_entries_2 0x744940 b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w
datum k_xaudio_master24_reverb4_entries 0x744758 b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w
datum k_xaudio_master36_reverb6_entries 0x744818 b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w
datum k_xaudio_master_entries_12 0x7446f8 b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w
datum k_xaudio_master_entries_6 0x7446c8 b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w
datum k_xaudio_source_entries_16 0x744a00 b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w
datum k_xaudio_source_entries_24 0x744a80 b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w
datum k_xaudio_source_entries_4 0x7449a0 b2 z2 w b2 z2 w b2 z2 w b2 z2 w
datum k_xaudio_source_entries_8 0x7449c0 b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w b2 z2 w
# text
datum k_game_engine_scoring_keys_default 0x41002 b2
datum k_game_engine_scoring_keys_mode0 0x41004 b8
datum k_game_engine_scoring_keys_mode1 0x4100c b20
datum k_main_crash_exception_code_format 0x3b684 b888
datum k_unregistered_thread_name 0x3bcb0 b16
# pointers, vtables, objects (auto: pointers relocated)
datum k_tag_resource_ctor_interface4_vftable 0x3bb80 a4
datum k_tag_resource_runtime_default_vftable 0x3bb80 a4
datum k_waves_library_versions 0x2b040 a48
# Dropped (M5P-C / F131): real C++ object-refresh event definition in simulation_game_objects.cpp replaces this fill.
# datum g_simulation_registered_event_definition_32 0x7bb328 a55
datum g_marketplace_http_downloader 0x7c1c34 a1444
datum g_network_configuration_state 0x79e228 a2404
datum ?g_online_service_record_manager!!3Vc_online_service_record_manager!!A 0x7ac928 a1135
datum ?g_webstats_globals!!3Us_webstats_globals!!A 0x7a07b8 a12336

# GAME-CHANGES F62: word data moved into fill/rdata_pool.spec (retail_real_pool), named here for pointers into it
name 0x5884c k_cinematics_perspective_m22
name 0x58848 k_cinematics_perspective_m32
name 0x2b10c k_creature_ground_airborne_delay
name 0x22b74 k_equipment_activation_markers
name 0x39294 k_function_constant_pool_anchor
name 0x33614 k_game_engine_type_string_ids
name 0x242d8 k_real_constant_pool_two
name 0x2490c k_real_math_epsilon_storage
name 0x39540 k_saved_film_sound_gain
name 0x35f08 k_scripted_sound_default_decibels
name 0x35f08 k_scripted_sound_default_gain
name 0x35f08 scripted_sound_constant_pool_anchor
name 0x24644 k_secondary_skull_string_ids
name 0xf20 k_xmscalar_sincos_cos0_coeffs
name 0xed0 k_xmscalar_sincos_cos1_coeffs
name 0xe80 k_xmscalar_sincos_cos2_coeffs
name 0xf10 k_xmscalar_sincos_sin0_coeffs
name 0xf30 k_xmscalar_sincos_sin1_coeffs
name 0xe40 k_xmscalar_sincos_sin2_coeffs
name 0x22dbc literal_0_1f
name 0x22b14 literal_0_75f
name 0x2215c literal_0_95f
name 0x336f0 literal_1000_0f
name 0x22850 literal_10_0f
name 0x254e4 literal_neg_0_5f
name 0x58c78 k_xaudio_environment_scalar1
name 0x58c74 k_xaudio_environment_values2_0
name 0x58c6c k_xaudio_environment_values2_3
name 0x58c68 k_xaudio_environment_values2_4

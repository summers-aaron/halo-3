# Poisoned data the 010_jungle walk reads (GAME-CHANGES F47 landing list), retail contents in host order.
datum k_hs_value_type_sizes 0x341c8 h83
datum k_render_object_type_flag_16 0x24b5c b16
datum g_squads_area_none 0x72cfbc w
datum g_water_refraction_enabled 0x72cdca b
datum g_user_interface_main_menu_music_active 0x72c4e1 b
# 16 s_priority_definition {speech_type, priority, 20 reserved bytes: four floats and a short}
datum ?global_dialogue_priorities!!3QBUs_priority_definition!!B 0x22f38 h2 w4 h2 h2 w4 h2 h2 w4 h2 h2 w4 h2 h2 w4 h2 h2 w4 h2 h2 w4 h2 h2 w4 h2 h2 w4 h2 h2 w4 h2 h2 w4 h2 h2 w4 h2 h2 w4 h2 h2 w4 h2 h2 w4 h2 h2 w4 h2
# 35 posture type names (string ids), 60 behavior_posture_info {index, name, peek_type, duration, locked, raw[5]}
datum k_behavior_posture_info 0x239e0 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6 w3 h b6
# g_atom_definitions: 29 s_atom_definition pointers, then the 29 definitions {start, update, handle, destroy}
datum ?g_atom_definitions!!3PAPAUs_atom_definition!!A 0x72fc20 p29
datum retail_72d094 0x72d094 p116

# GAME-CHANGES F62: word data moved into fill/rdata_pool.spec (retail_real_pool), named here for pointers into it
name 0x3ccc0 k_player_fix_position_search_directions
name 0x2aee0 ?k_standard_interpolation_time!!3MB
name 0x22dc0 k_real_half_pool
name 0x242d4 k_real_constant_pool_three
name 0x35d40 k_default_reverb_definition
name 0x4b7f8 k_lightmap_shadows_quality_shader_indices
name 0x22518 k_behavior_surprise_movement_values
name 0x231a8 k_behavior_posture_type_names

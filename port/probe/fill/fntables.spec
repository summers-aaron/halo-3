# poisoned function-pointer tables and hand vtables: raw words, pointers relocated (auto layout)
datum g_game_state_lock_procs 0x74b120 a4
datum g_game_state_unlock_procs 0x74b130 a3
datum ?k_boot_betrayer_screen_message_vtable!!3PADA 0x4ecb0 a4
datum k_c_gui_saved_film_selected_item_vtable 0x567f4 p11
# tmp/ppc_abi_audit (F144/F150 class): c_director base vtable, every slot thiscall with its stack-argument bytes; slot 7
# (handle_deleted_object) resolves to the cdecl director_handle_camera_notify(user, object_index), slots 2-5/8 to folded
# bool bodies: retailfill gives those cdecl targets adapters (ecx -> first argument, ret N)
datum k_director_vtable 0x3dca8 a1 t4 t0 t0 t0 t0 t4 t4 t4
# GAME-CHANGES F78-class (M5P-C / F132): slot 2 (terminator_required_bits) of c_replication_event_manager_view is the folded
# body 0x8242d380 (li r3,1), which resolves to the bool tag_d3d_texture_teardown (al only; the scheduler reads eax).
datum k_distributed_view_flag_a_vtable 0x44938 a2 t0=?terminator_required_bits@c_replication_event_manager_view@@EBEJXZ a6
datum k_file_share_screen_vtable 0x514e8 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t4
datum k_gui_screen_107_vftable 0x53598 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8
datum k_gui_screen_109_vftable 0x53660 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8
datum k_gui_screen_1331_vftable 0x508e0 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8
datum k_gui_screen_1390_vftable 0x55df0 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8
datum k_gui_screen_1394_vftable 0x55d28 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8
datum k_gui_screen_139_vftable 0x517b8 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8
datum k_gui_screen_140_vftable 0x51988 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8
datum k_gui_screen_142_vftable 0x51d50 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8
# GAME-CHANGES F78 (M5J): slot 33 (get_gui_location) of the pregame lobby screens is the folded body 0x8242d380 (li r3,1),
# which resolves to the bool tag_d3d_texture_teardown: an i686 bool sets only al, the e_gui_location caller reads eax.
# The slot calls the typed member returning _gui_location_pregame_lobby (1, the same value) instead.
datum k_gui_screen_167_vftable 0x53b68 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0=?get_gui_location@c_gui_screen_pregame_lobby@@UBE?AW4e_gui_location@@XZ t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t0 t0 t0 t0 t0 t4 t0 t0 t0
datum k_gui_screen_168_vftable 0x53d30 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0=?get_gui_location@c_gui_screen_pregame_lobby@@UBE?AW4e_gui_location@@XZ t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t0 t0 t0 t0 t0 t4 t0 t0 t0
datum k_gui_screen_169_vftable 0x53e80 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0=?get_gui_location@c_gui_screen_pregame_lobby@@UBE?AW4e_gui_location@@XZ t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t0 t0 t0 t0 t0 t4 t0 t0 t0
datum k_gui_screen_170_vftable 0x53f98 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0=?get_gui_location@c_gui_screen_pregame_lobby@@UBE?AW4e_gui_location@@XZ t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t0 t0 t0 t0 t0 t4 t0 t0 t0
datum k_gui_screen_171_vftable 0x54138 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0=?get_gui_location@c_gui_screen_pregame_lobby@@UBE?AW4e_gui_location@@XZ t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t0 t0 t0 t0 t0 t4 t0 t0 t0 a1
datum k_gui_screen_174_vftable 0x545e8 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 a152
datum k_gui_screen_179_vftable 0x534d0 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8
datum k_gui_screen_183_vftable 0x550f0 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8
datum k_gui_screen_191_vftable 0x55730 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8
datum k_gui_screen_192_vftable 0x55988 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8
datum k_gui_screen_193_vftable 0x544e0 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8
datum k_gui_screen_200_vftable 0x53130 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t8 t8
datum k_gui_screen_201_vftable 0x53200 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t8 t8
datum k_gui_screen_202_vftable 0x551b8 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 a144
datum k_gui_screen_203_vftable 0x4e800 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8
datum k_gui_screen_263_vftable 0x53728 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8
datum k_gui_screen_820_vftable 0x54b08 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8
datum k_gui_screen_848_object_vftable 0x44130 a9
datum k_gui_screen_848_vftable 0x1ff8 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 a5
datum k_gui_simple_selected_item_vtable 0x56c64 p11
datum k_main_demand_context_vtable 0xd30 a6
datum k_main_game_map_memory_callbacks 0x394f8 a18
datum ?k_object_impulse_sound_trackers!!3QBUs_sound_tracker!!B 0x35fac p16
datum k_online_crash_writer_vtable 0x428d8 a16
datum k_scripted_impulse_sound_tracker 0x35fec p16
datum k_simulation_sandbox_engine_globals_definition_vtable 0x45920 a79
datum k_simulation_sandbox_engine_globals_vtable 0x48680 a30
datum k_start_menu_game_editor_vtable 0x50f70 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t4 a1
datum k_start_menu_pane_133_vftable 0x510c0 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t4 a19
datum k_start_menu_pane_134_vftable 0x511d8 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t4
datum k_start_menu_pane_135_vftable 0x51308 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t4 a1
datum k_start_menu_pane_141_vftable 0x51b98 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t4
datum k_start_menu_pane_146_vftable 0x521d0 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t4 a1
datum k_start_menu_pane_149_vftable 0x52540 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t4
datum k_start_menu_pane_150_vftable 0x52680 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t4 a1
datum k_start_menu_pane_152_vftable 0x529b0 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t4 a1
datum k_start_menu_pane_153_vftable 0x52b40 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t4 a1
datum k_start_menu_pane_154_vftable 0x52cb8 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t4 a1
datum k_start_menu_pane_155_vftable 0x52fc0 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t4 a1
datum k_start_menu_pane_156_vftable 0x52e38 t0 t4 t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t0 t0 t0 t0 t0 t0 t0 t0 t4 t8 t16 t4 t4 t4 t0 t0 t8 t8 t4 a1
datum k_start_menu_settings_datasource_vtable 0x51c78 a28
datum k_tag_resource_cache_handle_callbacks 0x3e4a0 a1
datum k_tag_resource_ctor_interface2_vftable 0x3bb90 a1
datum k_tag_resource_ctor_interface3_vftable 0x3bb94 a7
datum k_tag_resource_ctor_interface5_vftable 0x3bb90 a1
datum k_tag_resource_ctor_interface6_vftable 0x3bbdc a3
datum k_tag_resource_ctor_interface7_vftable 0x3bb90 a1
datum k_tag_resource_ctor_interface8_vftable 0x3bbdc a3
datum k_tag_resource_error_receiver_vtable 0x3bebc a206
datum k_tag_resource_listener_vftable 0x3bbb4 t8 p
datum ?object_deleted_procs!!3PAP6AXJ!ZA 0x73cb08 a12
datum simulation_ctf_definition_vtable_entry 0x475f8 a30
# c_game_state_proc_handler default object (game_state_procs.cpp has raw Xbox words): set_mode(mode, entries), reset(), allocate(index, p1, p2, listener), release(index, arg)
datum retail_vt_40d30 0x40d30 t8 t0 t16 t8
datum g_game_state_proc_default_handler 0x75de54 p p w p w
# game_state.cpp listener objects defined as raw Xbox vtable words (c_game_state_proc_listener: set_mode(mode, entries), update(arg), dispose(arg))
datum retail_vt_3cb88 0x3cb88 t8 t4 t4
datum retail_vt_3cb94 0x3cb94 t8 t4 t4
datum retail_vt_3ae74 0x3ae74 t8 t4 t4
datum ?g_game_state_save_listener!!3KA 0x75dd60 p
datum ?g_game_state_load_listener!!3KA 0x75dd64 p
datum g_game_state_compressor_listener 0x74b800 p
# game_state after-load handler table and animation codec definitions (auto words)
datum g_game_state_after_load_handlers 0x74b150 a28
datum ?g_codec_descriptions!!3PAVc_codec_description!!A 0x79d838 p w2 p10 p w2 p10 p w2 p10 p w2 p10 p w2 p10 p w2 p10 p w2 p10 p w2 p10 p w2 p10 w9
# slot 10, c_gui_widget::get_current_bounds (0x48b870): in source (c_gui_widget.cpp) without its @address tag (GAME-CHANGES F29)
name 0x48b870 ?get_current_bounds@c_gui_widget@@UAEPATreal_rectangle2d@@PAT2@@Z
# c_gui_start_menu_hq_service_record_pane::initialize_datasource (0x4aee50): i686 thiscall mangling (@@UAEXXZ vs PPC @@UAAXXZ)
name 0x4aee50 ?initialize_datasource@c_gui_start_menu_hq_service_record_pane@@UAEXXZ
# Batch 5 virtualized methods: i686 thiscall mangling (@@UAE/@@EAE vs PPC @@UAA/@@EAA)
name 0x21e608 ?add_value@s_tag_resource_cache_external_access_handle@@UAEHJJ@Z
name 0x19c7f0 ?dispose@c_game_state_compressor_listener@@UAEXPAX@Z
name 0x1d6030 ?dispose@c_game_state_save_listener@@UAEXPAX@Z
name 0x4d6b80 ?handle_controller_input_message@c_gui_screen_alpha_motd_editor@@UAE_NPBVc_controller_input_message@@@Z
name 0x41a4b8 ?process_incoming_request@c_replication_event_manager_view@@EAEXPAUs_replication_incoming_request@@@Z
name 0x1ba740 ?report_hash_failure@c_tag_resource_error_receiver@@UAEXXZ
name 0x1ba700 ?report_page_failure@c_tag_resource_error_receiver@@UAEXXZ
name 0x1ba720 ?report_read_failure@c_tag_resource_error_receiver@@UAEXXZ
name 0x1d6028 ?update@c_game_state_save_listener@@UAEXPAX@Z
# c_gui_list_widget hand vtable (c_gui_list_widget.cpp, F29): thiscall slots with their stack-argument bytes (include/interface/gui_widget.h)
datum DAT_82055fc8 0x55fc8 t0 t4=?destroy@probe_list_widget_destroyer@@QAEPAXI@Z t0 t0 t0 t0 t0 t0 t4 t0 t4 t4 t4 t4 t0 t0 t4 t4 t4 t4 t4 t4 t4 t4 t4 t4 t24 t0 t0 t4 t4 t8 t4
# k_unicode_escape_table (unicode.cpp, poisoned): 10 s_escape_table {wchar, unicode, length/parser words}, 0xffff-terminated
datum k_unicode_escape_table 0x330b8 h z2 w p p h z2 w p p h z2 w p p h z2 w p p h z2 w p p h z2 w p p h z2 w p p h z2 w p p h z2 w p p h z2 w p p
# GAME-CHANGES F51: the five folded screen deleting destructors (tmp/probe/extra/m5f_gui_destructors.cpp)
name 0x4b2af8 ?destroy_139@probe_gui_screen_destroyer@@QAEPAXI@Z
name 0x4b42b0 ?destroy_140@probe_gui_screen_destroyer@@QAEPAXI@Z
name 0x4cb920 ?destroy_174@probe_gui_screen_destroyer@@QAEPAXI@Z
name 0x4d9e70 ?destroy_191@probe_gui_screen_destroyer@@QAEPAXI@Z
name 0x4cad10 ?destroy_193@probe_gui_screen_destroyer@@QAEPAXI@Z
name 0x1aefc8 ?allocate@c_cache_file_tag_resource_runtime_manager_storage@@UAEXPAV?$c_basic_buffer@X@@K@Z

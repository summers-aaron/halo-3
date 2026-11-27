// objectslibxbox/objects.obj: from the records 0x99ea0 0x99ee0 0x9a080 0x9c390 0x9cb80 0x9ce98 0x9f748 0x9fcb8 0xa0d80 0xa1b48 0xa94d0 0xac748 0xae108 0xc7f60 0xc9180 0xd6028 0xd6290 0xdc770 0xde710 0xe2b70 0xe2c20 0xe2f78 0xe3070 0xe3120 0xe3258 0xe32f8 0xe3620 0xe3730 0xe3c98 0xe3ec8 0xe4140 0xe4460 0xe66f8 0xe9b88 0xea290 0xebb20 0xef080 0xefd18 0xf1820 0xf53b0 0xf6478 0xfc388 0xfc560 0xfc768 0xfc7c8 0xfc838 0xfc8c8 0xfca20 0xfcaa0 0xfcf30 0xfd350 0x102b88 0x103118 0x107688 0x1077c0 0x107d58 0x10ecf8 0x10ff20 0x1108a8 0x1109a0 0x110a38 0x110a98 0x110b68 0x111810 0x1123e8 0x112918 0x112c68 0x116480 0x1171b0 0x120e88 0x129c18 0x15c040 0x1ba488 0x1ba838 0x1bbe88 0x1bbf08 0x1d74d8 0x20e030 0x20e208 0x23d6f8 0x23d748 0x23f340 0x241778 0x241850 0x28aaf0 0x2f49e0 0x3030c0 0x317498 0x3174b0 0x3174c0 0x3174d0 0x317548 0x317648 0x317660 0x317678 0x317690 0x3176a8 0x3176c0 0x3176d8 0x317720 0x3179f0 0x317b88 0x317db0 0x317f50 0x3180b8 0x318200 0x318270 0x3185e8 0x318760 0x318860 0x318a80 0x318b88 0x318c60 0x318cf0 0x318d78 0x318f30 0x319108 0x3191f0 0x3195b0 0x31a598 0x31a698 0x31a730 0x31a7d0 0x31a950 0x31abc8 0x31ac78 0x31acf8 0x31ad80 0x31ade0 0x31ae90 0x31af60 0x31b138 0x31b248 0x31b370 0x31b438 0x31b7e0 0x31b8b0 0x31b998 0x31bb18 0x31bc10 0x31bda0 0x31be68 0x31bff0 0x31c050 0x31c1b0 0x31c2a0 0x31c388 0x31c6f8 0x31c7e0 0x31c860 0x31c9e8 0x31cac0 0x31cb70 0x31cf08 0x31cf70 0x31d1c8 0x31d4e0 0x31d560 0x31d8d0 0x31e398 0x31e4d0 0x31e6b0 0x31e8c0 0x31eb20 0x31ec30 0x31f088 0x31f2a8 0x31f558 0x31f778 0x31f860 0x31fba0 0x31fda0 0x31ff40 0x31ffb8 0x320050 0x320120 0x3201b0 0x320230 0x3202d0 0x320360 0x320588 0x320618 0x3206f8 0x320760 0x320800 0x3208c8 0x320948 0x320998 0x3209f0 0x320a90 0x320ac8 0x320b68 0x320ba0 0x320c40 0x320c60 0x320d00 0x320d18 0x320d38 0x320dd8 0x320df0 0x320e10 0x320ea8 0x320ee8 0x320fa0 0x320fc8 0x321068 0x321088 0x321130 0x321158 0x321210 0x321238 0x3212d8 0x3212f8 0x3213a0 0x3213c8 0x321420 0x3214a0 0x335660 0x3362c8 0x33d6b8 0x33d7f8 0x33e470 0x33e510 0x33e528 0x3614c8 0x449550 0x4fd5f8 0x5779a8
#include "cache/restricted_memory.h"
#include "objects/object_memory_pool.h"
#include "objects/object_name_list.h"
#include "cutscene/cinematic_new_globals_types.h"
#include "objects/recycling_groups.h"
#include "objects/object_cluster_tls.h"
#include "objects/object_render_state_tls.h"
#include "objects/object_header_data.h"
#include "objects/light_globals.h"
#include "animations/animation_codec_flags.h"
#include "physics/havok.h"
#include "physics/havok_collision_damage.h"
#include "objects/object_cluster_reference.h"
#include "structures/structures.h"
#include "effects/effect_creation.h"
#include "units/unit_dialogue.h"
#include "simulation/game_interface/simulation_game_entities.h"
#include "objects/object_header.h"
#include "animations/animation_definition_types.h"
#include "animations/animation_search_path_iterator.h"
#include "animations/mixing_board/mixing_board_definition.h"
#include "game/game_engine_list.h"
#include "game/game_engine_current_entry.h"
#include "math/real_math_globals.h"
#include "animations/animation_definition_storage.h"
#include "math/matrix_math_functions.h"
#include "math/real_math_functions.h"
#include "objects/object_globals.h"
#include "effects/effects_globals.h"
#include "memory/tls_data_arrays.h"
#include "scenario/scenario_object_name_types.h"
#include "objects/object_bsp_placement_policy_types.h"
#include "units/biped_types.h"
#include "physics/collision_types.h"
#include "physics/character_physics_mode_ground_datum.h"
#include "cseries/cseries_types.h"
#include "objects/object_reference_types.h"
#include "cseries/cseries_wrapped.h"
#include "memory/static_arrays.h"
#include "memory/data_types.h"
#include "memory/data.h"
#include "memory/datum_index_inlines.h"
#include "tag_files/tag_types.h"
#include "math/real_math_types.h"
#include "math/matrix_math_types.h"
#include "game/game_time_types.h"
#include "objects/object_types.h"
#include "objects/damage_reporting_types.h"
#include "objects/object_datum.h"
#include "items/item_types.h"
#include "units/unit_types.h"
#include "objects/object_placement.h"
#include "objects/object_iterator.h"
#include "objects/multiplayer_object_types.h"
#include "units/motor_enums.h"
#include "units/motor_request.h"
#include "animations/animation_id.h"
#include "cache/restricted_memory_regions.h"
#include "game/game_engine.h"
#include "objects/objects.h"
#include "physics/havok_proxies.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "sound/game_sound.h"
#include "sound/sound_scenery.h"
#include "units/units.h"
#include "cache/cache_file_tag_resource_runtime.h"
#include "creatures/creatures.h"
#include "effects/effects.h"
#include "game/game.h"
#include "game/game_engine_candy_monitor.h"
#include "game/game_engine_sandbox.h"
#include "game/game_engine_util.h"
#include "interface/first_person_weapons.h"
#include "items/projectiles.h"
#include "items/weapons.h"
#include "math/color_math.h"
#include "math/periodic_functions.h"
#include "memory/memory_pool_inline.h"
#include "memory/memory_pool_types.h"
#include "models/models.h"
#include "models/render_models.h"
#include "objects/damage.h"
#include "objects/damage_owner.h"
#include "objects/lights.h"
#include "objects/object_broadphase.h"
#include "objects/object_early_movers.h"
#include "objects/object_recycling.h"
#include "objects/object_scripting.h"
#include "objects/widgets/antenna.h"
#include "objects/widgets/cloth.h"
#include "objects/widgets/leaf_system.h"
#include "objects/widgets/muffins.h"
#include "objects/widgets/widgets.h"
#include "objects/widgets/widget_allocator.h"
#include "objects/widgets/widget_type.h"
#include "objects/crates.h"
#include "physics/breakable_surfaces.h"
#include "physics/bsp3d.h"
#include "physics/collisions.h"
#include "physics/collision_sphere_queries.h"
#include "physics/impacts.h"
#include "physics/water_physics.h"
#include "render/render_objects_static_lighting.h"
#include "scenario/scenario.h"
#include "structures/cluster_partitions.h"
#include "units/bipeds.h"
#include "units/vehicle_type_component.h"
#include "game/game_engine_teleporters.h"
#include "game/game_engine_teleporter_types.h"
#include "physics/character_physics_mode_melee.h"
#include "hs/object_lists.h"
#include "render/depth_of_field.h"
#include "game/players.h"
#include "scenario/scenario_map_variant.h"
#include "motor/motor_system.h"
#include "game/player_types.h"
#include "structures/cluster_partition_types.h"
#include "animations/animation_interpolation.h"
#include "physics/collision_bsp_query_types.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "game/map_variant.h"
#include "cache/restricted_memory_layout.h"
#include "ai/scenario_object_reference_types.h"
#include "objects/recycling_methods_types.h"
#include "objects/object_marker_globals_types.h"
#include "cache/render_model_region_types.h"
#include "game/date_and_time_types.h"
#include "objects/objects_tracked_object_types.h"
#include "objects/object_acceleration_info_types.h"
#include "cache/cache_files.h"
#include "effects/effect_vector_types.h"
#include "math/function_definitions.h"
#include "objects/object_type_definition.h"
#include "structures/cluster_bit_vectors.h"
#include "game/game_engine_default.h"
#include "scenario/scenario_objects.h"
#include "scenario/scenario_sky.h"
#include "models/render_model_definitions.h"
#include "multithreading/synchronized_value.h"
#include "objects/widgets/widget_types.h"
#include "scenario/scenario_access.h"
#include "structures/structure_bsp_tag.h"
#include "physics/collision_bsp.h"
#include "game/player_mapping_types.h"
#include "camera/observer_globals_types.h"
#include "cseries/cseries.h"
#include "objects/light_datum.h"
#include "objects/object_definitions.h"
#include "devices/device_definitions.h"
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <ppcintrinsics.h>
#include <math.h>
#include "animations/animation_manager.h"
#include "math/real_constant_pool.h"
#include "cseries/global_real_argb_colors.h"
#include "units/biped_globals.h"
#include "game/game_globals.h"
#include "game/game_globals_storage.h"
#include "items/projectile_definitions.h"
#include "models/model_types.h"
#include "models/model_variant_types.h"
#include "structures/structure_bsp_fake_lightprobe.h"
#include "scenario/cluster_reference_helpers.h"
#include "physics/havok_component_class.h"
#include "physics/havok_component.h"
#include "physics/havok_component_inline.h"
#include "objects/object_function_definitions.h"
#include "objects/emblem_info.h"
#include "objects/emblems.h"
#include "game/game_engine_scoring_globals.h"
#include "objects/object_broadphase_types.h"
#include "objects/objects_hierarchy_list.h"
#include "effects/effect_node_attachment_types.h"
#include "multithreading/message_queue.h"
#include "objects/object_render_thread_message.h"
#include "game/game_engine_candy_monitor_types.h"
#include "physics/havok_object_bounds.h"
#include "objects/object_cluster_reference_types.h"
#include "render/render_objects.h"
#include "cache/restricted_memory_region_members.h"
#include "multithreading/thread_tls.h"
#include "cutscene/recorded_animation_globals.h"
#include "objects/object_runtime_globals_types.h"
#include "objects/object_allocation.h"
#include "objects/widgets/widget_debug.h"
#include "objects/scenery.h"
#include "objects/object_lifecycle.h"
#include "structures/cluster_partition_new.h"
#include "sound/game_looping_sound_data.h"
#include "physics/havok_monitor_storage.h"
#include "cseries/read_write_barrier.h"
#include "physics/character_physics_ground_helpers.h"
#include "structures/structure_marker_types.h"
#include "rasterizer/rasterizer_depth_bias.h"
#include "objects/object_animation.h"
#include "math/integer_math_bit_vectors.h"
#include "physics/havok_object_acceleration.h"
#include "objects/object_early_movers_tls.h"
#include "animations/animation_event_data.h"
#include <new>
#include "tag_files/tag_index.h"
#include "render/objects_texture_camera.h"
#include "effects/effect_creation_flags.h"
#include "effects/effect_creation_data.h"
#include "units/vehicle_definitions.h"
#include "objects/object_soft_ceiling_queries.h"
#include "models/model_damage_info.h"
#include "scenario/scenario_lightmap_types.h"
#include "scenario/scenario_lightmap_lighting_types.h"
#include "math/random_math_range.h"
#include "tag_files/tag_groups.h"
#include "tag_files/string_ids.h"
#include "game/game_time_access.h"
#include "math/real_math_trigonometry.h"
#include "game/game_simulation_predicates.h"
#include "memory/datum_index.h"
#include "physics/collision_bsp_cache_types.h"
#include "objects/object_recycling_storage.h"
#include "objects/object_node_matrices_inline.h"
#include "objects/object_region_information_inline.h"
#include "objects/object_ai_properties.h"
// @address 0x11df8c0 g_cinematic_light_globals
// @address 0x11df870 g_havok_memory_task_data
// @address 0x7f4310 g_object_marker_globals
// @address 0x11df6b0 object_globals
// @address 0x11df60c collideable_object_cluster_partition
// @address 0x11df618 noncollideable_object_cluster_partition
// @address 0x73e318 g_widget_types
// @address 0x11df608 g_object_memory_pool
// @address 0x11df830 g_object_name_list
// @flags /TP /O2 /Oc
// @address 0x11df878 g_object_message_queue
// @address 0x11df758 g_tracked_object_globals
extern "C" {
__declspec(thread) s_objects_tracked_object_globals *g_tracked_object_globals;
__declspec(thread) s_memory_pool *g_object_memory_pool;
__declspec(thread) void *g_recycling_volumes;
__declspec(thread) t_message_queue<s_object_render_thread_message, 2048> *g_object_message_queue;
__declspec(thread) long *g_object_name_list;
__declspec(thread) long *g_object_render_data;
}

// @address 0x11df6a8 object_header_data
__declspec(thread) s_data_array *object_header_data;
__declspec(thread) s_object_globals *object_globals;
__declspec(thread) cluster_partition collideable_object_cluster_partition;
__declspec(thread) cluster_partition noncollideable_object_cluster_partition;

// @address 0x11df8c8 g_object_render_data
// @address 0x11df720 g_recycling_volumes
// @address 0x9b308 ?normalize3d@@YAMPATreal_vector3d@@@Z
// @address 0xa0d80 ?objects_in_sphere@@YAFKKPBUs_location@@PBTreal_point3d@@MPAJF@Z
// @address 0xa1150 ?structure_clusters_in_sphere@@YAJUs_cluster_reference@@PBTreal_point3d@@MJPAU1@PAJ@Z
// @address 0x31c388 ?object_function_get_function_value@@YA_NJPAUs_object_function_definition@@JPAMPA_N@Z
// @address 0xe9b88 ?unit_compute_function_value@@YA_NJJJPAMPA_N1@Z
// @address 0x241850 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_cinematic_light_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x320ba0 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_object_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x320c40 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_object_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x110a98 ?object_detach@@YAXJ@Z
// @address 0x318860 ?object_set_position_internal@@YA_NJPBTreal_point3d@@PBTreal_vector3d@@1PBUs_location@@_N333@Z
// @address 0x31a950 ?object_reconnect_to_map@@YAXJ_NPBUs_location@@@Z
// @address 0x31b248 ?object_align_marker_to_matrix@@YAXJPBUobject_marker@@PBUreal_matrix4x3@@@Z
// @address 0x31b998 ?object_detach_immediate@@YAXJ@Z
// @address 0x31fba0 ?object_connect_lights_recursive@@YAXJ_N000@Z
// @address 0x99ea0 ?object_get_ultimate_parent@@YAJJ@Z
// @address 0x99ee0 ?object_accepted_local_physics_object_get@@YAJJ@Z
// @address 0x9f748 ?object_get_region_information@@YAXJPAJPAPAD1PAPAUobject_region_information@@@Z
// @address 0xa1b48 ?object_get_orientation@@YAXJPATreal_vector3d@@0@Z
// @address 0xa94d0 ?object_get_location@@YAPAUs_location@@JPAU1@@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0xc7f60 ?object_is_hidden@@YA_NJ@Z
// @address 0xc9180 ?object_get_world_matrix@@YAPAUreal_matrix4x3@@JPAU1@@Z
// @address 0xe4140 ?object_get_function_value@@YA_NJJJPAMPA_N@Z
// @address 0xe4460 ?object_compute_function_value@@YA_NJJJPAMPA_N1@Z
// @address 0xfc560 ?object_delete@@YAXJ@Z
// @address 0xfc768 ?object_is_connected_to_map@@YA_NJ@Z
// @address 0xfc7c8 ?object_set_invalid_for_recycling@@YAXJ_N@Z
// @address 0xfd350 ?object_owns_object@@YA_NJJ@Z
// @address 0x103118 ?object_iterator_next_internal@c_object_iterator_base@@IAA_NXZ
// @address 0x111810 ?object_iterator_next_with_match_flags_internal@c_object_iterator_base@@IAA_NXZ
// @address 0x112c68 ?objects_get_contiguous_free_memory_size@@YAJXZ
// @address 0x1171b0 ?object_get_change_color@@YA_NJJPATreal_rgb_color@@@Z
// @address 0x120e88 ?object_start_interpolation@@YA_NJM@Z
// @address 0x1ba838 ?get_object_postion_controlling_rigid_body_index@c_havok_component@@QBA?BFXZ
// @address 0x1123e8 ?objects_untrack_object@@YAXJ@Z
// @address 0x23d6f8 ?effects_node_attachment_try_and_get_object@@YAJPAUeffects_node_attachment@@@Z
// @address 0x23d748 ?effects_node_attachment_new@@YAXJJFPBTreal_point3d@@@Z
// @address 0xe3120 ?object_header_block_allocate@@YA_NJFFF@Z
// @address 0x320800 objects_compact_memory_pool
// @address 0x1d74d8 ?get_maximum_shield_vitality@c_player_trait_shield_vitality@@QBAMXZ
// @address 0x318b88 ?object_set_scale@@YAXJMM_N@Z
// @address 0x321130 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_collideable_cluster_reference_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x3213a0 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_noncollideable_cluster_reference_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x3212f8 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_noncollideable_cluster_reference_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0xe3730 ?cluster_build_object_payload@@YAXJPIAUs_object_cluster_payload@@@Z
// @address 0x116480 objects_cluster_reference_get_first
// @address 0x1bbf08 ?object_reset@@YAXJ@Z
// @address 0x9c390 ?objects_in_clusters_by_indices@@YAJKKKJPBUs_cluster_reference@@JPAJ@Z
// @address 0xae108 ?object_get_root_object@@YAJJ@Z
// @address 0xe32f8 object_reset_after_teleport
// @address 0x31ae90 object_attach_to_marker_with_type
// @address 0x31a698 object_find_predicted_object
// @address 0x31ad80 objects_cluster_reference_get_next
// @address 0x33e470 ?reserve_memory@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_tracked_object_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
// @address 0x3362c8 ?__tls_set_g_tracked_object_globals_allocator@@YAXPAX@Z
// @address 0x31ff40 object_update_ai_flags
// @address 0x31ffb8 object_try_and_get_location
// @address 0x3208c8 objects_unregister_scenario_object
// @address 0x320998 object_placement_reset_scale_for_physics
// @address 0x3213c8 ?clear@?$t_message_queue@Us_object_render_thread_message@@$0IAA@@@QAAXXZ
// @address 0x321420 ?add_message@?$t_message_queue@Us_object_render_thread_message@@$0IAA@@@QAAXPBUs_object_render_thread_message@@@Z
// @address 0x3209f0 ?reserve_memory@?$t_restricted_allocation_manager@$02$1?adopt_old_object_render_message_queue@@YAXPAX@Z$0A@$1?__tls_set_g_first_person_weapons_allocator@@YAX0@Z@@QAAPAXPBD0IJ@Z
// @address 0x320ac8 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_object_broadphase_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
// @address 0x320c60 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_object_name_list_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
// @address 0x320d38 ?reserve_memory@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_object_render_data_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
// @address 0x3174b0 ?__tls_set_g_object_name_list_allocator@@YAXPAX@Z
// @address 0x3174c0 ?__tls_set_g_object_render_data_allocator@@YAXPAX@Z
// @address 0x321238 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_noncollideable_data_reference_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x321158 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_noncollideable_first_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x321088 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_collideable_cluster_reference_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x320fc8 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_collideable_data_reference_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x320ee8 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_collideable_first_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0xea290 object_get_player_index
// @address 0x107d58 object_initialize_node_orientations
// @address 0x110b68 ?objects_handle_deleted_object@@YAXJ@Z
// @address 0x20e030 objects_hierarchy_list_add_cluster
// @address 0x31c6f8 ?object_set_base_change_color_by_index@@YA_NJJPBTreal_rgb_color@@@Z
// @address 0x3614c8 ?object_compute_activation@@YA_NJPBUs_game_cluster_bit_vectors@@PA_N@Z
// @address 0x5779a8 ?object_compute_activation_unconditional@@YA_NJPBUs_game_cluster_bit_vectors@@PA_N@Z
// @address 0x20e208 objects_hierarchy_list_add_object
// @address 0xe2b70 ?object_set_velocities@@YAXJPBTreal_vector3d@@0@Z
// @address 0xe3070 ?object_set_requires_motion@@YAXJ@Z
// @address 0xfc838 ?object_update_collision_culling@@YAXJ@Z
// @address 0xfcf30 ?internal_object_get_markers_by_string_id@@YAFJJPAUobject_marker@@F_N@Z
// @address 0x1077c0 ?object_force_inside_bsp@@YA_NJPBTreal_point3d@@J@Z
// @address 0x1ba488 object_get_water_properties_index
// @address 0x3206f8 ?adopt_old_object_render_message_queue@@YAXPAX@Z
// @address 0x33e510 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_tracked_object_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x320360 ?object_reverse_compute_node_orientations_from_matrices@@YAXJPAKPBUrender_model_definition@@JPAUreal_orientation@@@Z
// @address 0x320230 ?object_freeze_node_orientations@@YAXJ_N@Z

// @address 0xef080 object_physics_update
union real_point4d { struct { float x, y, z, w; }; float n[4]; real_point3d point; };


/* the flags word of an object header (0x9f748): bit 11 is the size bias */


/* c_object_identifier (8 bytes): unique id, origin bsp index, type and source (the record named it s_key) */
// object_get_damage_reporting_type reads the common derived-tag reporting byte.

extern "C" {
       // object header array



// @address 0x73d028 g_object_types_deleted_player_mask
// @address 0x330b4 g_object_world_coordinate_bounds
}





 







struct objects_sound_scenery_definition : s_object_definition { float minimum_distance; float maximum_distance; };
struct s_placement_owner_constants { s_damage_owner unknown; float neighbor; };



struct s_objects_attached_light {
    unsigned char type;
    long light_index;
};

template<typename t_datum> class c_data_iterator_match {
public:
    void begin(s_data_array *data, long flags_offset, unsigned char mask, unsigned char match)
    {
        m_iterator.data = data;
        m_flags_offset = flags_offset;
        m_mask = mask;
        m_match = match;
        m_iterator.absolute_index = -1;
        m_iterator.index = -1;
    }
    long get_index() const { return m_iterator.index; }
    t_datum *m_datum;
    long m_flags_offset;
    unsigned char m_mask;
    unsigned char m_match;
    s_data_iterator m_iterator;
};
typedef struct __regfix_ret_objects_lw_dot_product3d { float v; } __regfix_ret_objects_lw_dot_product3d;
struct s_attachment_effect_parameters {
    c_tag_index definition_index;
    short change_color;
    long secondary_scale;
    long primary_scale;
    long marker_name;

    __forceinline s_attachment_effect_parameters(const s_object_attachment_definition *definition)
        : definition_index(definition->type.index), change_color(definition->change_color),
          secondary_scale(definition->secondary_scale), primary_scale(definition->primary_scale),
          marker_name(definition->marker) {}
    __forceinline s_attachment_effect_parameters(const s_attachment_effect_parameters &other)
        : definition_index(other.definition_index), change_color(other.change_color),
          secondary_scale(other.secondary_scale), primary_scale(other.primary_scale),
          marker_name(other.marker_name) {}
};



static __forceinline bool object_test(s_object_header *header, object_datum *object)
{
    return (header->flags & 0x10) != 0 || (object->flags & 1) != 0;
}

static __forceinline real_vector3d *matrix4x3_rotate_vector(const real_matrix4x3 *matrix, const real_vector3d *vector, real_vector3d *result)
{
    float i = vector->i;
    float j = vector->j;
    float k = vector->k;
    result->i = matrix->forward.i * i + matrix->left.i * j + matrix->up.i * k;
    result->j = matrix->forward.j * i + matrix->left.j * j + matrix->up.j * k;
    result->k = matrix->forward.k * i + matrix->left.k * j + matrix->up.k * k;
    return result;
}

static __forceinline char *heap_pointer(s_memory_pool *heap, int offset)
{
    return offset ? (char *)heap + offset : 0;
}

static __forceinline int heap_used(s_memory_pool *heap)
{
    int *top = (int *)heap_pointer(heap, heap->last_block_handle);
    if (!heap->last_block_handle)
        return 0;
    char *bottom = heap_pointer(heap, heap->data_offset);
    return (int)((char *)top + *top - bottom);
}

// object_get_ultimate_parent: the outermost parent of the object (itself when it has none), -1 for none
long object_get_ultimate_parent(long object_index)
{
  long root_index = -1;
  while (object_index != -1) {
    root_index = object_index;
    object_index = object_get(object_index)->parent_object_index;
  }
  return root_index;
}

// object_accepted_local_physics_object_get: -1 unless the object's flags (+0xac) have bit 3 and bit 1 or 2 set;
// then its +0xa4
long object_accepted_local_physics_object_get(long object_index)
{
  long result = -1;
  object_datum *object = object_get(object_index);
  unsigned long flags = object->physics_flags;
  if ((bool)(flags >> 3 & 1) && ((bool)(flags >> 1 & 1) || (bool)(flags >> 2 & 1)))
    result = object->local_physics_object_index;
  return result;
}

/* object_get_origin: the object's position in the world; when the object is attached to a parent node, its position
   transformed by that node's matrix */
static __forceinline real_matrix4x3 *objects_get_node_matrix(long object_index, short node_index)
{
    object_datum *object = object_get(object_index);
    return (real_matrix4x3 *)((char *)object + object->node_matrices.offset + node_index * 52);
}

real_point3d *object_get_origin(long object_index, real_point3d *origin)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    if (object->parent_object_index == -1)
        *origin = object->position;
    else {
        real_matrix4x3 *node_matrix = objects_get_node_matrix(object->parent_object_index, object->parent_node_index);
        matrix4x3_transform_point(node_matrix, &object->position, origin);
    }
    return origin;
}

// 0x9cb80
// the havok component of an object as object_get_velocities and object_get_center_of_mass read it
// match: 0x9cb80: the controlling-body range check is inlined at velocity and transform callers.
static __forceinline short objects_havok_controlling_body(const c_havok_component *component)
{
    long index = component->m_object_postion_controlling_rigid_body_index >= 0 &&
        component->m_object_postion_controlling_rigid_body_index < component->get_rigid_body_count() ?
        component->m_object_postion_controlling_rigid_body_index : -1;
    return (short)index;
}
// match: 0x31d1c8: retain the Boolean return boundary of the inlined sentinel query.
static __forceinline bool objects_havok_is_sentinel(const c_havok_component *component)
{
    return (component->m_flags & (1 << 5)) != 0;
}
// match: 0x31d560: retain the Boolean return boundary of the inlined dynamic-body query.
static __forceinline bool objects_havok_has_dynamic_bodies(const c_havok_component *component)
{
    return (component->m_flags & (1 << 10)) != 0;
}
// match: 0x31d1c8: the component's dynamic controlling-body test is inlined here.
static __forceinline bool objects_havok_dynamic_controlling_body(const c_havok_component *component)
{
    short rigid_body_index = objects_havok_controlling_body(component);
    bool result = false;
    if (rigid_body_index != -1) {
        unsigned char motion_type = component->m_rigid_bodies[rigid_body_index].m_rigid_body->m_motion.m_type;
        result = motion_type != 6 && motion_type != 7;
    }
    return result;
}
// match: 0x31d1c8: the infinite-inertia body scan is inlined here.
static __forceinline bool objects_havok_infinite_inertia(const c_havok_component *component)
{
    bool result = false;
    for (long rigid_body_index = 0; rigid_body_index < component->m_rigid_bodies.getSize(); rigid_body_index++)
        result |= component->get_rigid_body_has_infinite_inertia_tensor(rigid_body_index);
    return result;
}


// @address 0x9cb80 ?object_get_velocities@@YAXJPATreal_vector3d@@0@Z
void object_get_velocities(long object_index, real_vector3d *linear_velocity, real_vector3d *angular_velocity)
{
    long root_index = inl_object_get_ultimate_parent(object_index);
    object_datum *object = ((object_datum *)object_get_and_verify_type(root_index, -1));
    bool found = false;
    bool at_rest = (bool)(_rotl(object->physics_flags, 23) & 1);
    if (at_rest) {
        if (linear_velocity)
            *linear_velocity = *global_zero_vector3d;
        if (angular_velocity)
            *angular_velocity = *global_zero_vector3d;
        found = true;
    }
    if (!found && objects_is_main_thread_with_lock() && object->havok_component_index != -1) {
        c_havok_component *component = objects_havok_component_get(object->havok_component_index);
        long rigid_body_index = objects_havok_controlling_body(component);
        if (rigid_body_index != -1) {
            if (linear_velocity)
                component->get_rigid_body_linear_velocity( rigid_body_index, linear_velocity);
            if (angular_velocity)
                component->get_rigid_body_angular_velocity(rigid_body_index, angular_velocity);
            found = true;
        }
    }
    if (!found) {
        if (linear_velocity)
            *linear_velocity = object->linear_velocity;
        if (angular_velocity)
            *angular_velocity = object->angular_velocity;
    }
}

void object_get_region_information(long object_index, long *region_count, char **region_permutations,
    char **visible_permutations, object_region_information **region_information)
{
    object_get_region_information_inline(object_index, region_count, region_permutations, visible_permutations, region_information);
}

// 0x9fcb8

static __forceinline long objects_center_get_ultimate_parent(long object_index)
{
    long result = -1;
    while (object_index != -1) {
        result = object_index;
        object_index = ((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum->parent_object_index;
    }
    return result;
}
static __forceinline c_havok_component *objects_read_component_reference(const volatile long *component_index)
{
    long absolute_index = *component_index & 0xffff;
    s_data_array *data = g_havok_component_data;
    // match: 0x9fcb8: component-index range assumption preserves the lookup addressing
    __assume(absolute_index < data->maximum_count);
    char *base = data->data;
    return (c_havok_component *)(base + absolute_index * sizeof(c_havok_component));
}
// @address 0x9fcb8 ?object_get_center_of_mass@@YAPATreal_point3d@@JPAT1@@Z
real_point3d *object_get_center_of_mass(long object_index, real_point3d *center)
{
    long ultimate_parent_index = objects_center_get_ultimate_parent(object_index);
    object_datum *object = ((object_datum *)object_get_and_verify_type(ultimate_parent_index, -1));
    bool found = false;
    if (object->havok_component_index != -1 && objects_is_main_thread_with_lock()) {
        c_havok_component *component = objects_read_component_reference(&object->havok_component_index);
        short rigid_body_index = objects_havok_controlling_body(component);
        if (rigid_body_index != -1) {
            (component)->get_rigid_body_center_of_mass(rigid_body_index, center);
            found = true;
        }
    }
    if (!found)
        {
        float bounding_radius;
        object_get_bounding_sphere(ultimate_parent_index, center, &bounding_radius);
    }
    return center;
}

// object_get_orientation
void object_get_orientation(long object_index, real_vector3d *forward, real_vector3d *up)
{
    object_datum *object = object_get(object_index);
    if (object->parent_object_index == -1) {
        if (forward)
            *forward = object->forward;
        if (up)
            *up = object->up;
    } else {
        object_datum *parent = object_get(object->parent_object_index);
        const real_matrix4x3 *node = (const real_matrix4x3 *)((char *)parent + parent->node_matrices.offset) + object->parent_node_index;
        if (forward)
            matrix4x3_rotate_vector(node, &object->forward, forward);
        if (up)
            matrix4x3_rotate_vector(node, &object->up, up);
    }
}

// object_get_location: the location of the object's outermost parent
s_location *object_get_location(long object_index, s_location *location)
{
    long root_index = -1;
    while (object_index != -1) {
        root_index = object_index;
        object_index = object_get(object_index)->parent_object_index;
    }
    *location = object_get(root_index)->location;
    return location;
}

// object_try_and_get_and_verify_type: the object a handle names when its
// header is live and its type is in the mask, else 0
void *object_try_and_get_and_verify_type(long object_index, unsigned long type_mask)
{
    s_data_array *data = object_header_data;
    void *result = 0;
    if (object_index != -1) {
        int absolute_index = object_index & 0xffff;
        if (absolute_index < data->first_unallocated) {
            s_object_header *header = (s_object_header *)((char *)data->data + data->size * absolute_index);
            int identifier = header->identifier;
            if (identifier != 0 && identifier == (object_index >> 16)
                && ((1 << header->object_type) & type_mask) != 0)
                result = header->datum;
        }
    }
    return result;
}

// 0xae108
// (object_get_root_object: named by address, simulation_game_generics.cpp defines an external inline of that name)
long object_get_root_object(long object_index)
{
    while (object_index != -1) {
        object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
        if (!(bool)((object->flags >> 26) & 1))
            break;
        object_index = object->parent_object_index;
    }
    return object_index;
}

// object_is_hidden: whether the object or any object it is attached to has header flag 0x10 or object flag bit 0
bool object_is_hidden(long object_index)
{
    bool result = false;
    do {
        s_object_header *header = object_header_get(object_index);
        object_datum *object = header->datum;
        if (object_test(header, object)) {
            result = true;
            break;
        }
        object_index = object->parent_object_index;
    } while (object_index != -1);
    return result;
}

// object_get_world_matrix: the object's forward, up and position as a matrix, composed with its parent's node matrix
real_matrix4x3 *object_get_world_matrix(long object_index, real_matrix4x3 *matrix)
{
    object_datum *object = object_get(object_index);
    matrix4x3_rotation_from_vectors(matrix, &object->forward, &object->up);
    matrix->position = object->position;
    if (object->parent_object_index != -1) {
        object_datum *parent = object_get(object->parent_object_index);
        matrix4x3_compute_product((const real_matrix4x3 *)((char *)parent + parent->node_matrices.offset) + object->parent_node_index, matrix, matrix);
    }
    return matrix;
}

// @address 0xdc770 ?valid@c_animation_manager@@QBA_NXZ
#if defined(_XBOX) && !defined(__clang__)
inline
#endif
bool c_animation_manager::valid() const
{
    return valid_graph() && valid_goals() && m_state_channel.valid();
}

// flag test and tag lookup helpers
// The pointer form below lowers bit tests to the extract form the image needs; TEST_BIT's mask form differs here.
static __forceinline bool objects_flags_test_ptr(const unsigned long *flags, long bit)
{
    return (*flags & (1 << bit)) != 0;
}
static __forceinline void objects_flags_set(unsigned long *flags, long bit, bool enable)
{
    if (enable)
        *flags |= 1 << bit;
    else
        *flags &= ~(1 << bit);
}
static __forceinline bool object_get_function_value(long object_index, long function_name, long function_owner_definition_index, float
    *out_function_magnitude)
{
    bool deterministic;
    return object_get_function_value(object_index, function_name, function_owner_definition_index, out_function_magnitude, &deterministic);
}
static __forceinline float objects_get_function_value_simple(long object_index, long function_name, long function_owner_definition_index)
{
    float function_magnitude;
    bool success = object_get_function_value(object_index, function_name, function_owner_definition_index, &function_magnitude);
    if (!success)
        function_magnitude = 0.0f;
    return function_magnitude;
}
// match: 0x31f2a8: change-color callers require this inline body; the placed entry stays callable.
float object_get_function_value_simple(long object_index, long function_name, long function_owner_definition_index)
{
    return objects_get_function_value_simple(object_index, function_name, function_owner_definition_index);
}
// 0xe4140
bool object_get_function_value(long object_index, long function_name, long function_owner_definition_index, float *out_function_magnitude, bool
    *deterministic)
{
    bool result = false;
    *deterministic = true;
    *out_function_magnitude = 0.0f;
    if (object_scripting_get_function_variable(object_index, function_name, out_function_magnitude)) {
        result = true;
    } else if (function_name == 0 || function_name == _string_id_one) {
        result = true;
        *out_function_magnitude = 1.0f;
    } else if (function_name == _string_id_zero) {
        result = false;
        *out_function_magnitude = 0.0f;
    }
    else if (object_index != -1 &&
        !::object_type_compute_function_value(object_index, function_name, function_owner_definition_index,
            (float *)(out_function_magnitude), (bool *)(&result), (bool *)(deterministic)))
    {
        bool found = false;
        object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
        s_object_definition *definition = (s_object_definition *)tag_get('obje', object->definition_index);
        for (long function_index = 0; function_index < definition->functions.count; function_index++) {
            s_object_function_definition *function = &((s_object_function_definition *)definition->functions.address)[function_index];
            if (function->import_name == function_name) {
                result = object_function_get_function_value(object_index, function, function_owner_definition_index, out_function_magnitude,
                    deterministic);
                found = true;
                break;
            }
        }
        if (!found && objects_flags_test_ptr(&object->flags, 26) && object->parent_object_index != -1) {
            result = object_get_function_value(
                object->parent_object_index, function_name, function_owner_definition_index, out_function_magnitude, deterministic);
            found = true;
        }
        if (!found) {
            result = false;
            *out_function_magnitude = 0.0f;
        }
    }
    return result;
}

// 0xe66f8
static __forceinline void *objects_object_header_block_get_with_count(
    object_datum *object, const s_object_block_reference *reference, unsigned int element_size, long *count)
{
    *count = reference->size / element_size;
    return (char *)object + reference->offset;
}


// 0xe9b88

// @address 0x8c4434 g_object_render_data_count
long g_object_render_data_count;
// 0xea290
static __forceinline long objects_player_index_from_unit_index(long unit_index)
{
    unit_datum *unit = (unit_datum *)object_try_and_get_and_verify_type(unit_index, 0x1003);
    long player_index = -1;
    if (unit)
        player_index = unit->unit.player_index;
    return player_index;
}


long object_get_player_index(long object_index)
{
    long player_index = objects_player_index_from_unit_index(object_index);
    if (player_index == -1) {
        s_object_header *header = object_header_get(object_index);
        long controller_index = -1;
        if ((1 << header->object_type) & 1)
            controller_index = ((unit_datum *)header->datum)->unit.controller_player_index;
        if (controller_index != -1 && datum_try_and_get(player_data, controller_index))
            player_index = controller_index;
        else if (game_engine_globals->game_engine_index >= 1 && game_engine_globals->game_engine_index <= 10 &&
            game_engines[game_engine_globals->game_engine_index])
            player_index = current_game_engine()->object_get_emblem_player(object_index);
    }
    return player_index;
}

// 0xf1820
bool object_is_or_contains_player(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    bool result = objects_player_index_from_unit_index(object_index) + 1 != 0;
    if (!result) {
        long child_index = object->first_child_object_index;
        while (child_index != -1) {
            object_datum *child_object = ((object_datum *)object_get_and_verify_type(child_index, -1));
            if (object_is_or_contains_player(child_index)) {
                result = true;
                break;
            }
            child_index = child_object->next_object_index;
        }
    }
    if (!result) {
        long parent_index = object->parent_object_index;
        while (parent_index != -1) {
            object_datum *parent_object = ((object_datum *)object_get_and_verify_type(parent_index, -1));
            if (objects_player_index_from_unit_index(parent_index) != -1) {
                result = true;
                break;
            }
            parent_index = parent_object->parent_object_index;
        }
    }
    if (!result && ((1 << object->identifier_type) & 12)) {
        item_datum *item = (item_datum *)object;
        if ((item->inventory_flags & (1 << 1)) != 0)
            result = true;
    }
    return result;
}

// 0xf53b0
static __forceinline unsigned char lift_sel_bit_u16_2(unsigned short value)
{
    return (value >> 2) & 1;
}

// @address 0xf53b0 object_set_damage_owner
void object_set_damage_owner(long object_index, const s_damage_owner *damage_owner, bool skip_validation)
{
    unsigned char skip = (unsigned char)skip_validation;
    int valid = 1;
    object_datum *object = object_get(object_index);
    if (skip == 0) {
        if (((valid << object->identifier_type) & 32) != 0)
            valid = 0;
        else {
            const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
            if ((unsigned int)lift_sel_bit_u16_2(definition->flags) != 0)
                valid = 0;
        }
    }
    if ((unsigned char)valid == 0)
        return;

    object->damage_owner = *damage_owner;
    if (skip != 0)
        return;

    ::simulation_action_object_update(object_index, 0x400);
}

// 0xf6478
// match: 0xf6478: rotl intrinsic avoids a CRT call for the reporting-type rotation
#pragma intrinsic(_rotl)
// @address 0xf6478 object_get_damage_reporting_type
unsigned char object_get_damage_reporting_type(long object_index)
{
    unsigned char result = _damage_reporting_type_unknown;
    if (object_index != -1) {
        const projectile_definition *definition = (const projectile_definition *)tag_get('proj', object_get(object_index)->definition_index);
        result = (unsigned char)_rotl(definition->reporting_type, 2);
    }
    return result;
}

// object_is_connected_to_map: bit 6 of the header flags of the object's outermost parent
bool object_is_connected_to_map(long object_index)
{
  long root_index = inl_object_get_ultimate_parent(object_index);
  s_object_header *header = object_header_get(root_index);
  return ((header->flags >> 6) & 1) != 0;
}

// object_set_invalid_for_recycling: sets or clears bit 0 of the byte at 0x11a of an object, if it exists
void object_set_invalid_for_recycling(long object_index, bool enable)
{
  if (object_index != -1) {
    unsigned char *object = (unsigned char *)object_try_and_get_and_verify_type(object_index, 0xffffffff);
    if (object) {
      if (enable) {
        object[0x11a] |= 1;
      } else {
        object[0x11a] &= ~1;
      }
    }
  }
}

// 0xfca20
void object_name_list_delete(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    if (object->object_name_index != -1) {
        scenario *scenario = global_scenario;
        object->object_name_index = -1;
        for (short name_index = 0; name_index < scenario->object_names.count; name_index++) {
            if (((long *)g_object_name_list)[name_index] == object_index)
                ((long *)g_object_name_list)[name_index] = -1;
        }
    }
}

// 0xfcf30

static __forceinline long render_model_get_markers_by_name(long render_model_definition_index, long marker_name, const char
    *region_permutation_indices,
    long node_remap_count, const long *node_remapping_table, long node_count, const real_matrix4x3 *node_matrices, bool mirror, object_marker
        *markers, long maximum_marker_count)
{
    long marker_group_index = render_model_find_marker_group_by_name(render_model_definition_index, marker_name);
    long marker_count = render_model_get_markers_by_group_index(render_model_definition_index, marker_group_index,
        region_permutation_indices, node_remap_count, node_remapping_table, node_count, node_matrices, mirror, markers,
        maximum_marker_count);
    return marker_count;
}
short internal_object_get_markers_by_string_id(long object_index, long marker_name, object_marker *markers, short maximum_marker_count, bool
    use_object_index)
{
    long marker_object_index = object_index;
    short marker_count = 0;
    if (!use_object_index)
        marker_object_index = object_get_marker_object(object_index);
    if (marker_object_index != -1) {
        object_datum *object = ((object_datum *)object_get_and_verify_type(marker_object_index, -1));
        const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
        if (definition->model.index != -1) {
            const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', definition->model.index);
            long region_count;
            char *region_permutation_indices;
            object_get_region_information_inline(marker_object_index, &region_count, &region_permutation_indices, NULL, NULL);
            long node_count;
            real_matrix4x3 *node_matrices = object_get_node_matrices(marker_object_index, &node_count);
            marker_count = (short)render_model_get_markers_by_name(model->render_model_index, marker_name, region_permutation_indices, -1, NULL,
                node_count, node_matrices, (bool)((object->flags >> 10) & 1), markers, maximum_marker_count);
        }
    }
    if (marker_count == 0) {
        object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
        markers->node_index = 0;
        matrix4x3_identity(&markers->node_to_marker);
        memcpy(&markers->matrix, objects_get_node_matrix(object_index, 0), sizeof(real_matrix4x3));
        markers->scale = 0.0f;
        if ((bool)((object->flags >> 10) & 1))
            negate_vector3d(&markers->matrix.left, &markers->matrix.left);
        if (marker_name == 0)
            marker_count = 1;
    }
    return marker_count;
}

// object_owns_object: whether ancestor_index is object_index or one of its parents
bool object_owns_object(long ancestor_index, long object_index)
{
  long index = -1;
  while (object_index != -1 && index != ancestor_index) {
    index = object_index;
    object_index = object_get(object_index)->parent_object_index;
  }
  return ancestor_index != -1 && index == ancestor_index;
}

// 0x1077c0
static __forceinline bool objects_location_valid(const s_location *location)
{
    return ((const s_cluster_reference *)location)->bsp_index != -1;
}
bool object_force_inside_bsp(long object_index, const real_point3d *point, long ignore_object_index)
{
    s_collision_test_flags flags;
    flags.collision_flags = k_collision_test_structure.collision_flags | (1 << 17);
    flags.object_flags = k_collision_test_structure.object_flags;
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    collision_result collision;
    bool success = false;
    if (!::collision_test_line(flags, point, &object->position, object_index,
            ignore_object_index, &collision) &&
        objects_location_valid(&object->location))
    {
        success = true;
    } else if (objects_location_valid(&collision.location)) {
        object_set_position_direct(object_index, &collision.point, NULL, NULL, &collision.location, false);
        success = true;
    }
    return success;
}

// 0x107d58

void object_initialize_node_orientations(s_objects_element_array *array, long count, char (*destination)[32])
{
    render_model_definition *render_model = (render_model_definition *)array;
    long copy_count = render_model->nodes.count;
    if (count <= copy_count)
        copy_count = count;
    XMemCpy(destination, (char (*)[32])render_model->runtime_node_orientations.address, copy_count * 32);
    for (long index = render_model->nodes.count; index < count; index++)
        XMemCpy(destination + index, &k_real_math_identity_orientation_storage, sizeof(real_orientation));
}

// @address 0x804480 g_object_node_orientation_scratch
#if !defined(_XBOX) || defined(__clang__)
extern "C" {
real_orientation g_object_node_orientation_scratch[255];
}
#endif

// @address 0x10ecf8 object_get_node_orientations
real_orientation *object_get_node_orientations(s_objects_element_array *array, unsigned int count)
{
    object_initialize_node_orientations(array, count, (char (*)[32])g_object_node_orientation_scratch);
    unsigned int remaining = 255 - count;
    if (remaining != 0) {
        XMemSet(&g_object_node_orientation_scratch[count], 0, remaining * 32);
    }
    return g_object_node_orientation_scratch;
}

// 0x10ff20
long object_get_attachment_marker_name(long object_index, short attachment_index)
{
    long marker_name = -1;
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    if (attachment_index >= 0 && attachment_index < definition->attachments.count) {
        const s_object_attachment_definition *attachment = &((s_object_attachment_definition *)definition->attachments.address)[attachment_index];
        marker_name = attachment->marker;
    }
    return marker_name;
}

// 0x110a98
static __forceinline void objects_object_type_detach_from_parent(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    if (object->parent_object_index != -1) {
        object_type_definition *definition = object_type_definitions[object->identifier_type];
        for (short part_index = 0; definition->parts[part_index]; part_index++) {
            object_type_definition *part = definition->parts[part_index];
            if (part->detach_from_parent) {
                part->detach_from_parent(object_index);
                break;
            }
        }
    }
}
void object_detach(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    objects_object_type_detach_from_parent(object_index);
    if (object->parent_object_index != -1)
        object_detach_immediate(object_index);
}

// 0x110b68
// @address 0x73cc34 g_object_types_deleted_object_mask
static __forceinline void objects_object_type_handle_deleted_object(long object_index, long deleted_object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    object_type_definition *definition = object_type_definitions[object->identifier_type];
    for (short part_index = 0; definition->parts[part_index]; part_index++) {
        object_type_definition *part = definition->parts[part_index];
        if (part->handle_deleted_object)
            part->handle_deleted_object(object_index, deleted_object_index);
    }
}

// 0x111810
inline bool objects_multiplayer_object_type_equals(const c_enum<e_multiplayer_object_type, char, 0, 24> &type, long value)
{
    return value == type.get();
}

static __forceinline s_multiplayer_object_properties *try_get_multiplayer_properties(long object_index)
{
    object_datum *object = (object_datum *)object_try_and_get_and_verify_type(object_index, -1);
    s_multiplayer_object_properties *properties = 0;
    if (object && object->multiplayer_properties.offset != -1)
        properties = (s_multiplayer_object_properties *)((char *)object + object->multiplayer_properties.offset);
    return properties;
}
// @address 0xd6290 objects_handle_deleted_player
void objects_handle_deleted_player(long player_index)
{
    c_object_iterator<object_datum> iterator;
    iterator.begin(g_object_types_deleted_player_mask, 0);
    while (iterator.next()) {
        object_type_definition *type_definition = object_type_definitions[object_get(iterator.get_index())->identifier_type];
        for (short part_index = 0; type_definition->parts[part_index]; ++part_index) {
            object_type_definition *part = type_definition->parts[part_index];
            if (part->handle_deleted_player) {
                part->handle_deleted_player(iterator.get_index(), player_index);
            }
        }
    }
}
bool c_object_iterator_base::object_iterator_next_internal(void)
{
    s_data_array *array = object_header_data;
    int index = m_absolute_index;
    object_datum *value = 0;
    s_object_header *datum = &((s_object_header *)array->data)[index];
    while (index < array->first_unallocated) {
        int datum_index = (datum->identifier << 16) | index;
        index++;
        if (datum->identifier != 0 &&
            (datum->flags & m_header_mask) == m_header_mask &&
            ((1 << datum->object_type) & m_type_mask) != 0) {
            value = datum->datum;
            m_index = datum_index;
            break;
        }
        datum++;
    }
    m_absolute_index = index;
    m_datum = value;
    return value != 0;
}
void objects_handle_deleted_object(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    unsigned long type_mask = object_type_definitions[object->identifier_type]->mask & g_object_types_deleted_object_mask;
    if (type_mask) {
        c_object_iterator<object_datum> iterator;
        iterator.begin(type_mask, 0);
        while (iterator.next())
            objects_object_type_handle_deleted_object(iterator.get_index(), object_index);
    }
}
bool c_object_iterator_base::object_iterator_next_with_match_flags_internal()
{
    bool result;
    for (;;) {
        result = object_iterator_next_internal();
        if (!result)
            break;
        if (m_match_flags != 0) {
            s_multiplayer_object_properties *properties = try_get_multiplayer_properties(get_index());
            if (properties) {
                unsigned long match_flags = m_match_flags;
                if (result && (match_flags & (1 << 0)))
                    result &= objects_multiplayer_object_type_equals(properties->object_type, _multiplayer_object_type_none);
                if (result && (match_flags & (1 << 2)))
                    result &= objects_multiplayer_object_type_equals(properties->object_type, _multiplayer_object_type_spawn_point) &&
                        properties->flags.test(_multiplayer_object_valid_initial_player_spawn_bit);
                if (result && (match_flags & (1 << 1)))
                    result &= objects_multiplayer_object_type_equals(properties->object_type, _multiplayer_object_type_spawn_point) &&
                        !properties->flags.test(_multiplayer_object_valid_initial_player_spawn_bit);
                if (result && (match_flags & (1 << 3)))
                    result &= objects_multiplayer_object_type_equals(properties->object_type, _multiplayer_object_type_respawn_zone);
                if (result && (match_flags & ((1 << 4) | (1 << 5) | (1 << 6)))) {
                    switch (properties->object_type.get()) {
                    case _multiplayer_object_type_teleporter_2way: result = true; break;
                    case _multiplayer_object_type_teleporter_sender: result = (match_flags >> 5) & 1; break;
                    case _multiplayer_object_type_teleporter_receiver: result = (match_flags >> 4) & 1; break;
                    default: result = false; break;
                    }
                }
                if (result && (match_flags & (1 << 7)))
                    result &= objects_multiplayer_object_type_equals(properties->object_type, _multiplayer_object_type_oddball_ball_spawn_point);
                if (result && (match_flags & (1 << 8)))
                    result &= objects_multiplayer_object_type_equals(properties->object_type, _multiplayer_object_type_flag_or_bomb_spawn_point);
                if (result && (match_flags & (1 << 9)))
                    result &= objects_multiplayer_object_type_equals(properties->object_type, 16);
                if (result && (match_flags & (1 << 10)))
                    result &= objects_multiplayer_object_type_equals(properties->object_type, _multiplayer_object_type_flag_return_or_bomb_goal_area);
                if (result && (match_flags & (1 << 11)))
                    result &= objects_multiplayer_object_type_equals(properties->object_type, _multiplayer_object_type_koth_hill);
                if (result && (match_flags & (1 << 12)))
                    result &= objects_multiplayer_object_type_equals(properties->object_type, _multiplayer_object_type_infection_haven);
                if (result && (match_flags & (1 << 13)))
                    result &= objects_multiplayer_object_type_equals(properties->object_type, _multiplayer_object_type_territory);
                if (result && (match_flags & (1 << 14)))
                    result &= objects_multiplayer_object_type_equals(properties->object_type, _multiplayer_object_type_vip_boundary);
                if (result && (match_flags & (1 << 15)))
                    result &= objects_multiplayer_object_type_equals(properties->object_type, _multiplayer_object_type_vip_destination);
                if (result && (match_flags & (1 << 16)))
                    result &= objects_multiplayer_object_type_equals(properties->object_type, _multiplayer_object_type_juggernaut_destination);
                if (result && (match_flags & (1 << 17)))
                    result &= objects_multiplayer_object_type_equals(properties->object_type, _multiplayer_object_type_grenade) ||
                        objects_multiplayer_object_type_equals(properties->object_type, 3);
                if (result && (match_flags & (1 << 18)))
                    result &= properties->flags.test(_multiplayer_object_data_flag_selected);
            } else {
                result = false;
            }
        }
        if (result)
            break;
    }
    return result;
}

// 0x1123e8
// objects that a deleted object leaves: every slot naming it is cleared and marked changed
void objects_untrack_object(long object_index)
{
    s_objects_tracked_object_globals *globals = (s_objects_tracked_object_globals *)g_tracked_object_globals;
    if (globals->active) {
        for (long index = 0; index < globals->count; index++) {
            if (globals->objects[index].object_index == object_index) {
                globals->objects[index].object_index = -1;
                globals->changed_flags.set(index, true);
            }
        }
    }
}

// objects_get_contiguous_free_memory_size: free bytes of the thread's self-relative heap
long objects_get_contiguous_free_memory_size(void)
{
    s_memory_pool *heap = g_object_memory_pool;
    return heap->size - heap_used(heap);
}

// object_get_change_color
// match: 0x1171b0: restrict on the color out-pointer prevents alias reloads of the object
bool object_get_change_color(long object_index, long change_color_index, real_rgb_color *__restrict color)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    bool result = false;
    if (change_color_index == 0)
    {
        *color = global_real_argb_colors[_real_argb_white].rgb;
    }
    else
    {
        long change_color_count = object->change_colors.size / 12u;
        const real_rgb_color *change_colors = (const real_rgb_color *)((const char *)object + object->change_colors.offset);
        long index = change_color_index - 1;
        long half_count = change_color_count / 2;
        if (index >= 0 && index < half_count)
        {
            *color = change_colors[half_count + index];
            result = true;
        }
    }
    return result;
}

// 0x120e88
static __forceinline bool object_can_interpolate(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    return object->node_orientations.offset != -1;
}

bool object_start_interpolation(long object_index, float interpolation_time)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    bool result = false;
    if (object_can_interpolate(object_index) && definition->model.index != -1)
    {
        c_animation_manager *animation = (c_animation_manager *)((char *)object + object->animation.offset);
        long node_orientation_count = object->node_orientations.size / 32u;
        real_orientation *node_orientations = (real_orientation *)((char *)object + object->node_orientations.offset);
        real_orientation *original_node_orientations = (real_orientation *)((char *)object + object->original_node_orientations.offset);
        if (animation->start_interpolation(interpolation_time, node_orientation_count, original_node_orientations, node_orientations))
        {
            object_wake(object_index);
            result = true;
        }
    }
    return result;
}

// 0x129c18

bool object_get_localized_velocities(long object_index, real_vector3d *linear_velocity, real_vector3d *angular_velocity,
    real_vector3d *early_mover_linear_velocity, real_vector3d *early_mover_angular_velocity)
{
    real_vector3d local_linear_velocity = *global_zero_vector3d;
    real_vector3d local_angular_velocity = *global_zero_vector3d;
    real_point3d center_of_mass;
    object_get_velocities(object_index, linear_velocity, angular_velocity);
    object_get_center_of_mass(object_index, &center_of_mass);
    bool result = ::object_get_early_mover_local_space_velocity(object_index, &center_of_mass,
        &local_linear_velocity, &local_angular_velocity, true, false);
    if (result) {
        if (linear_velocity)
            subtract_vectors3d(linear_velocity, &local_angular_velocity, linear_velocity);
        if (angular_velocity)
            subtract_vectors3d(angular_velocity, &local_angular_velocity, angular_velocity);
    }
    if (early_mover_linear_velocity)
        *early_mover_linear_velocity = local_linear_velocity;
    if (early_mover_angular_velocity)
        *early_mover_angular_velocity = local_angular_velocity;
    return result;
}

// 0x15c040
// @address 0x15c040 cluster_partition_get_next_datum_and_payload
long cluster_partition_get_next_datum_and_payload(long *iterator, const void **payload)
{
    long reference_index = *iterator;
    s_data_array *array = (s_data_array *)tls_collideable_object_cluster_partition().data;
    if (reference_index != -1) {
        int datum_size = array->size;
        char *data = (char *)array->data;
        s_cluster_partition_element *element = (s_cluster_partition_element *)(data + (unsigned short)reference_index * datum_size);
        long next = element->next;
        *payload = (char *)element + 12;
        if (next != -1) {
            __dcbt(0, (void *)((unsigned short)next * datum_size + (char *)data));
        }
        long datum_index = element->datum_index;
        *iterator = next;
        return datum_index;
    }
    return -1;
}

// 0x1ba488


extern "C" bool object_get_water_properties_index(long object_index)
{
    bool result = false;
    s_object_header *header = object_header_get(object_index);
    long type_mask = 1 << header->object_type;
    if (type_mask & 0x1003) {
        if (!((bool)((header->datum->damage_flags >> 2) & 1) && (type_mask & 1) && motor_system_is_relaxed(object_index) != 0))
            result = true;
    }
    return result;
}

/* Validates a signed-byte index against a count: returns the index, or -1 when out of range. */
short const c_havok_component::get_object_postion_controlling_rigid_body_index(void) const
{
  int index = m_object_postion_controlling_rigid_body_index;
  if (index < 0 || !(index < m_rigid_bodies.m_size)) {
    index = -1;
  }
  return index;
}

// the shield vitality trait: the shield multiplier setting (byte 4 of the trait) sets the maximum shield vitality

// 0x1bbe88: rebuild the havok world of a parentless object with no component yet

// @address 0x1bbe88 object_rebuild_havok
extern "C" void object_rebuild_havok(long object_index)
{
    object_datum *object = object_get(object_index);
    if (object->parent_object_index != -1)
        return;
    long havok_component_index = object->havok_component_index;
    if (havok_component_index == -1) {
        ::havok_component_aquire(object_index, true, false);
        return;
    }
    s_data_array *components = g_havok_component_data;
    unsigned long flags = datum_get<c_havok_component>(components, havok_component_index)->m_flags;
    if ((bool)(_rotl(flags, 20) & 1))
        return;
    ::havok_object_rebuild(object_index, 0);
}

// 0x1bbf08


void object_reset(long object_index)
{
    s_object_header *header = object_header_get(object_index);
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    if (object->parent_object_index == -1) {
        if (((1 << header->object_type) & 1) && motor_system_is_relaxed(object_index) == 1
            && ((biped_datum *)object)->biped.physics.m_mode == 3) {
            c_motor_request request;
            request.setup_force_relaxation_state(_action_none, 1, false);
            motor_system_submit(object_index, &request);
        }
        if (object->havok_component_index != -1)
            havok_component_release(object_index, true);
    }
}

#if defined(_XBOX) && !defined(__clang__)
inline float c_player_trait_shield_vitality::get_maximum_shield_vitality() const
#else
float c_player_trait_shield_vitality::get_maximum_shield_vitality() const
#endif
{
    int multiplier;
    switch (m_shield_multiplier - 1) {
    case 0: multiplier = 0; break;
    case 1: multiplier = 1; break;
    case 2: multiplier = 2; break;
    case 3: multiplier = 3; break;
    case 4: multiplier = 4; break;
    default: multiplier = 1; break;
    }
    return (float)(double)multiplier;
}

static __forceinline long cluster_partition_get_next_datum_and_payload(
    const cluster_partition *partition, long *iterator, long payload_size, const void **payload)
{
    s_data_array *array = partition->data;
    long datum_index;
    if (*iterator != -1) {
        int datum_size = array->size;
        char *data = (char *)array->data;
        s_cluster_partition_element *element = (s_cluster_partition_element *)(data + (*iterator & 0xffff) * datum_size);
        long next = element->next;
        *payload = (char *)element + 12;
        if (next != -1)
            __dcbt(0, data + (next & 0xffff) * datum_size);
        datum_index = element->datum_index;
        *iterator = next;
    } else {
        datum_index = -1;
    }
    return datum_index;
}

// 0x20e030
/* the object list of a hierarchy: a bit per object index, the object indices and their definition indices */
/* the objects of the two cluster partitions in one cluster that flag themselves for hierarchy lists (payload flag 3) */
void objects_hierarchy_list_add_cluster(s_objects_hierarchy_list *list, const s_cluster_reference *cluster)
{
    const s_object_cluster_payload *payload;
    {
        long iterator;
        for (long object_index = cluster_partition_get_first_datum_and_payload(&tls_noncollideable_object_cluster_partition(), &iterator, *cluster,
            sizeof(s_object_cluster_payload), (const void **)&payload);
            object_index != -1;
            object_index = cluster_partition_get_next_datum_and_payload(&tls_noncollideable_object_cluster_partition(), &iterator,
                sizeof(s_object_cluster_payload), (const void **)&payload))
            {
            if (payload->flags & 8) {
                if (!list->object_set.test(object_index & 0xffff))
                    objects_hierarchy_list_add_object(list, object_index);
                list->cluster_object_count++;
            }
        }
    }
    {
        long iterator;
        for (long object_index = cluster_partition_get_first_datum_and_payload(&tls_collideable_object_cluster_partition(), &iterator, *cluster,
            sizeof(s_object_cluster_payload), (const void **)&payload);
            object_index != -1;
            object_index = cluster_partition_get_next_datum_and_payload(&tls_collideable_object_cluster_partition(), &iterator,
                sizeof(s_object_cluster_payload), (const void **)&payload))
            {
            if (payload->flags & 8) {
                if (!list->object_set.test(object_index & 0xffff))
                    objects_hierarchy_list_add_object(list, object_index);
                list->cluster_object_count++;
            }
        }
    }
}

// 0x20e208
void objects_hierarchy_list_add_object(s_objects_hierarchy_list *list, long object_index)
{
    do {
        object_datum *object = object_get(object_index);
        list->object_indices.add(object_index);
        list->definition_indices.add(object);
        list->object_set.set(object_index & 0xffff);
        if (object->next_object_index != -1)
            objects_hierarchy_list_add_object(list, object->next_object_index);
        object_index = object->first_child_object_index;
    } while (object_index != -1);
}

// 0x23d6f8, 0x23d748: the effect node attachments (44-byte datums)
static __forceinline s_data_array *objects_effect_node_attachments_get()
{
    return g_static_lighting_effect_attachments;
}

long effects_node_attachment_try_and_get_object(effects_node_attachment *attachment)
{
    long result = -1;
    if (attachment->type == 1) {
        long object_index = attachment->object_index;
        if (object_try_and_get_and_verify_type(object_index, 0xffffffff))
            result = object_index;
    }
    return result;
}

void effects_node_attachment_new(long owner, long object_index, short node_index, const real_point3d *point)
{
    long attachment_index = datum_new(objects_effect_node_attachments_get());
    if (attachment_index != -1) {
        effects_node_attachment *attachment = datum_get<effects_node_attachment>(objects_effect_node_attachments_get(), attachment_index);
        attachment->definition_index = owner;
        if (object_index != -1) {
            attachment->object_index = object_index;
            attachment->node_index = node_index;
            attachment->type = 1;
            attachment->local_point = *point;
            effect_attachment_update_position(attachment);
        } else {
            attachment->type = 0;
            attachment->world_point = *point;
        }
        attachment->age = 0.0f;
    }
}

// __tls_set_g_object_globals_allocator: stores the allocator's value in the thread's slot 0x1c
void __tls_set_g_object_globals_allocator(void *value)
{
    cinematic_new_globals = (s_cinematic_new_globals_definition *)value;
}


// 0x3030c0

// @address 0x3030c0 object_clear_excess_flag_bits
void object_clear_excess_flag_bits(unsigned long *data, long bit_count)
{
    if (bit_count == 0) {
        memset(data, 0, sizeof(unsigned long) * 8);
        return;
    }
    long last_word = ((bit_count + 31) >> 5) - 1;
    long remaining_words = 7 - last_word;
    if (remaining_words != 0) {
        XMemSet(&data[last_word + 1], 0, remaining_words * 4);
    }
    data[last_word] &= (unsigned long)((1i64 << (bit_count - (last_word << 5))) - 1);
}


// 0x317498
// @address 0x317498 __tls_set_g_object_data_allocator
void __tls_set_g_object_data_allocator(void *object)
{
    data_set_new_base_address(&object_header_data, (s_data_array *)object);
}

// 0x3174b0
void __tls_set_g_object_name_list_allocator(void *allocator)
{
    g_object_name_list = (long *)allocator;
}

// 0x3174c0
void __tls_set_g_object_render_data_allocator(void *allocator)
{
    g_object_render_data = (long *)allocator;
}




// 0x3174d0
static __forceinline void objects_object_queue_render_thread_message(long object_index, short type, short unknown)
{
    s_object_render_thread_message message;
    message.object_index = object_index;
    message.type = type;
    message.unknown = unknown;
    g_object_message_queue->add_message(&message);
}
void reset_object_cached_render_state(long object_index)
{
    if (!restricted_region_locked_for_current_thread(2))
        objects_object_queue_render_thread_message(object_index, 0, 0);
    else
        g_object_render_data[datum_index_to_absolute_index(object_index)] = -1;
}

// 0xfc560
extern "C" {

}
void object_delete(long object_index)
{
    s_object_header *header = object_header_get(object_index);
    object_datum *object = header->datum;
    if (!object_is_hidden(object_index) && object_is_connected_to_map(object_index))
        object_connect_lights_recursive(object_index, true, false, false, false);
    if ((header->flags & 16) == 0) {
        objects_unregister_scenario_object(object_index);
        if (game_engine_running()) {
            if (game_engine_globals->game_engine_index >= 1 && game_engine_globals->game_engine_index <= 10
                && game_engines[game_engine_globals->game_engine_index]
                && game_globals->options.game_mode == 2) {
                game_engine_globals->get_candy_monitor_manager()->detach_object(object_index);
                current_game_engine()->handle_deleted_object(object_index);
            }
            map_variant_detach_object(&game_engine_globals->map_info, object_index);
        }
        header->flags |= 16;
        if ((bool)(_rotl(object->physics_flags, 15) & 1) && object->havok_component_index != -1) {
            real_rectangle3d bounds;
            unsigned long filter_info = (unsigned long)havok_get_object_bounds_and_group(object_index, &bounds);
            havok_update_bodies_in_bounds(filter_info, &bounds);
        }
    }
    object_update_collision_culling(object_index);
    object_name_list_delete(object_index);
    object->identifier.m_source = -1;
    object->identifier.m_origin_bsp_index = -1;
    object->identifier.m_unique_id = -1;
    object_set_garbage(object_index, false, -1);
    simulation_action_object_delete(object_index);
    object_scripting_clear_all_function_variables(object_index);
    g_object_message_queue->remove_messages_for_object(object_index);
}

/* set_memory_pool_address: stores the memory pool's address; every live object header's datum is rebased into the pool
   (its handle is the offset into the pool), or cleared when there is no pool */
void set_memory_pool_address(void *address)
{
    g_object_memory_pool = (s_memory_pool *)address;
    if (object_header_data != 0 && object_header_data->valid) {
        c_data_iterator<s_object_header> iterator;
        iterator.begin(object_header_data);
        if (address != 0) {
            while (iterator.next()) {
                s_object_header *header = (s_object_header *)iterator.get_datum();
                char *datum = header->handle != 0 ? (char *)address + header->handle : 0;
                if ((char *)header->datum != datum)
                    header->datum = (object_datum *)datum;
            }
        } else {
            while (iterator.next()) {
                ((s_object_header *)iterator.get_datum())->datum = 0;
            }
        }
    }
}

void __tls_set_g_collideable_first_data_allocator(void *value)
{
    tls_collideable_object_cluster_partition().cluster_heads = (long *)value;
}

void __tls_set_g_collideable_data_reference_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&tls_collideable_object_cluster_partition().data, (s_data_array *)object);
}

void __tls_set_g_collideable_cluster_reference_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&tls_collideable_object_cluster_partition().references, (s_data_array *)object);
}

void __tls_set_g_noncollideable_first_data_allocator(void *value)
{
    tls_noncollideable_object_cluster_partition().cluster_heads = (long *)value;
}

void __tls_set_g_noncollideable_data_reference_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&tls_noncollideable_object_cluster_partition().data, (s_data_array *)object);
}

void __tls_set_g_noncollideable_cluster_reference_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&tls_noncollideable_object_cluster_partition().references, (s_data_array *)object);
}

// the header's handle is the offset of the datum in the memory pool (0 when the datum is not in the pool)
void objects_update_header_callback(long object_index, long handle)
{
    s_object_header *header = object_header_get(object_index);
    header->handle = handle;
    if (handle != 0)
        header->datum = (object_datum *)((char *)g_object_memory_pool + handle);
    else
        header->datum = 0;
}

// 0x3180b8: type callbacks may supply the activation state.
static __forceinline bool object_type_compute_activation(long object_index, const s_game_cluster_bit_vectors *cluster_bit_vectors, bool *active)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    object_type_definition *definition = object_type_definitions[object->identifier_type];
    bool result = false;
    for (long type_index = 0; !result && definition->parts[type_index]; type_index++) {
        object_type_definition *type = definition->parts[type_index];
        if (type->compute_activation)
            result = type->compute_activation(object_index, cluster_bit_vectors, active);
    }
    return result;
}

bool object_should_be_active(long object_index, const s_game_cluster_bit_vectors *cluster_bit_vectors)
{
    s_object_header *header = object_header_get(object_index);
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    bool result = false;
    if ((bool)((object->flags >> 1) & 1) || game_globals->options.game_mode == 2) {
        result = true;
    } else if (!(((1 << header->object_type) & 704) && object_type_compute_activation(object_index, cluster_bit_vectors, &result))
        && header->cluster_reference.bsp_index != -1) {
        long cluster_index = header->cluster_reference.cluster_index;
        result = (cluster_bit_vectors->bit_vectors[header->cluster_reference.bsp_index].m_storage[cluster_index >> 5] & (1 << (cluster_index & 31))) != 0;
    }
    return result;
}

// 0x318200
bool object_should_be_deleted_when_deactivated(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    bool result = false;
    if (!simulation_query_object_is_predicted(object_index))
        result = (bool)((object->flags >> 17) & 1);
    return result;
}

// 0x3185e8
bool objects_activation_find_scenario_reference_in_currently_active_clusters(
    const s_game_cluster_bit_vectors *active_clusters,
    const s_game_cluster_bit_vectors *current_clusters,
    const s_scenario_object_reference *object_reference)
{
    bool found = false;
    for (long bsp_index = global_structure_bsp_first_active_index_get();
         !found && bsp_index != -1;
         bsp_index = global_structure_bsp_next_active_index_get(bsp_index)) {
        structure_bsp *bsp = global_structure_bsp_get(bsp_index);
        for (long cluster_index = 0; !found && cluster_index < bsp->clusters.count; cluster_index++) {
            s_cluster_reference cluster_reference;
            cluster_reference_set(&cluster_reference, bsp_index, cluster_index);
            if (game_clusters_test(current_clusters, cluster_reference)
                && game_clusters_test(active_clusters, cluster_reference)) {
                structure_cluster *cluster = &((structure_cluster *)bsp->clusters.address)[cluster_index];
                for (long reference_index = 0; reference_index < cluster->pvs_bound_object_references.count; reference_index++) {
                    const s_scenario_object_reference *reference = &((s_scenario_object_reference
                        *)cluster->pvs_bound_object_references.address)[reference_index];
                    if (reference->type == object_reference->type && reference->index == object_reference->index) {
                        found = true;
                        break;
                    }
                }
            }
        }
    }
    return found;
}

// 0x318a80
void object_set_position_direct(long object_index, const real_point3d *position, const real_vector3d *forward, const real_vector3d *up,
        // match: 0x318a80: restrict on the location pointer prevents alias reloads of the object
    const s_location *__restrict location, bool unknown)
{
    real_point3d *position_pointer = 0;
    real_vector3d *forward_pointer = 0;
    real_vector3d *up_pointer = 0;
    real_point3d position_copy;
    real_vector3d forward_copy;
    real_vector3d up_copy;
    if (position) {
        position_copy = *position;
        position_pointer = &position_copy;
    }
    if (forward) {
        forward_copy = *forward;
        forward_pointer = &forward_copy;
    }
    if (up) {
        up_copy = *up;
        up_pointer = &up_copy;
    }
    ::object_type_fix_transform(object_index, position_pointer, forward_pointer, up_pointer);
    if (!object_set_position_internal(object_index, position_pointer, forward_pointer, up_pointer, location, true, true, unknown, false)
        && !simulation_query_object_is_predicted(object_index))
        object_delete(object_index);
    object_wake(object_index);
}


// 0x318b88
void object_set_scale(long object_index, float scale, float interpolation_time, bool compute_node_matrices)
{
    if (object_index != -1 && scale > 0.0f) {
        object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
        float old_scale = object->scale;
        object->scale = scale;
        if (object_start_interpolation(object_index, interpolation_time)) {
            float scale_ratio = old_scale / scale;
            real_orientation *orientation = (real_orientation *)((char *)object + object->original_node_orientations.offset);
            orientation->scale *= scale_ratio;
            orientation->translation.x *= scale_ratio;
            orientation->translation.y *= scale_ratio;
            orientation->translation.z *= scale_ratio;
        }
        if (compute_node_matrices)
            object_compute_node_matrices(object_index);
    }
}

void object_set_velocities_internal(long object_index, const real_vector3d *linear_velocity, const real_vector3d *angular_velocity, bool suppress_notify)
{
    object_datum *object = object_get(object_index);
    unsigned long flags = 0;
    if (linear_velocity != 0) {
        object->linear_velocity = *linear_velocity;
        flags |= 16;
    }
    if (angular_velocity != 0) {
        object->angular_velocity = *angular_velocity;
        flags |= 32;
    }
    if (!suppress_notify && flags != 0)
        ::simulation_action_object_update(object_index, flags);
}

static __forceinline bool objects_object_is_multiplayer_cinematic_object(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    return (bool)((object->flags_d6 >> 1) & 1);
}


static __forceinline s_tag_block *objects_identifier_scenario_datums(scenario *scenario, long object_type, long *datum_size)
{
    // match: 0x319108: object-type range assumption guides the definition-slot load
    __assume(object_type >= 0 && object_type < 14);
    object_type_definition *const *slot = &object_type_definitions[object_type];
    object_type_definition *definition = *slot;
    long offset = (short)definition->scenario_datums_offset;
    long size = definition->scenario_datum_size;
    if (datum_size) *datum_size = size;
    return (s_tag_block *)((char *)scenario + offset);
}
// 0x319108
s_scenario_object *c_object_identifier::find_scenario_object_from_scenario(scenario *scenario, long *scenario_datum_index) const throw()
{
    struct scenario *current_scenario = global_scenario;
    memcpy(&scenario, &current_scenario, sizeof(scenario));
    s_scenario_object *result = 0;
    long source = m_source;
    if (source == 1 || source == 0) {
        if (object_type_definitions[m_type]->scenario_datums_offset != -1) {
            long datum_size = 0;
            s_tag_block *datums = objects_identifier_scenario_datums(scenario, m_type, &datum_size);
            for (long index = 0; index < datums->count; index++) {
                s_scenario_object *scenario_object = (s_scenario_object *)((char *)datums->address + index * datum_size);
                if (scenario_object->object_identifier.is_equal(this)) {
                    result = scenario_object;
                    if (scenario_datum_index)
                        *scenario_datum_index = index;
                    break;
                }
            }
        }
    }
    return result;
}

// 0x31a698
long object_find_predicted_object(long object_index)
{
    long result = -1;
    if (simulation_query_object_is_predicted(object_index)) {
        result = object_index;
    } else {
        for (long child_index = object_get(object_index)->first_child_object_index; child_index != -1;
            child_index = object_get(child_index)->next_object_index)
        {
            long child_result = object_find_predicted_object(child_index);
            if (child_result != -1) {
                result = child_result;
                break;
            }
        }
    }
    return result;
}

// 0x31ade0
static __forceinline bool object_definition_uses_parent_markers(const object_datum *object)
{
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    return (definition->flags & (1 << 4)) != 0;
}


static __forceinline void objects_physics_flags_set(unsigned long *flags, long bit, bool enable)
{
    if (enable)
        *flags |= 1 << bit;
    else
        *flags &= ~(1 << bit);
}

// 0x318cf0
static __forceinline unsigned char objects318cf0_sel_bit_u16_11(unsigned short value)
{
    return (value >> 11) & 1;
}

// @address 0x318cf0 object_set_movable

extern "C" void object_set_movable(long object_index, unsigned char enable)
{
    object_datum *object = object_get(object_index);
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    if ((unsigned int)objects318cf0_sel_bit_u16_11(definition->flags) != 0)
        return;
    if (((object->physics_flags >> 20) & 1) == enable)
        return;
    objects_physics_flags_set(&object->physics_flags, 20, enable != 0);
    ::havok_object_rebuild(object_index, 0);
}

// 0x31abc8

// 0x31ac78
void object_disconnect_from_physics(long object_index)
{
    object_datum *object = object_get(object_index);
    if (object->havok_component_index != -1) {
        c_havok_component *component = objects_havok_component_get(object->havok_component_index);
        component->disconnect_from_havok_world();
    }
    objects_physics_flags_set(&object->physics_flags, 7, false);
}

// 0x31acf8
void object_reconnect_to_physics(long object_index)
{
    object_datum *object = object_get(object_index);
    if (object->havok_component_index != -1) {
        c_havok_component *component = objects_havok_component_get(object->havok_component_index);
        component->connect_to_havok_world();
    }
    objects_physics_flags_set(&object->physics_flags, 7, true);
    object->physics_reconnect_time = -32768;
}

// 0x31ad80
static __forceinline s_cluster_reference objects_cluster_reference_from_datum_index(long datum_index)
{
    union {
        short index;
        s_cluster_reference cluster;
    } reference;
    reference.index = (short)datum_index;
    return reference.cluster;
}
static __forceinline s_cluster_reference objects_cluster_reference_iterator_next(s_cluster_reference_iterator *iterator)
{
    s_data_array *array = iterator->partition->references;
    long reference_index = iterator->reference_index;
    long cluster_datum;
    if (reference_index != -1) {
        int datum_size = array->size;
        char *data = (char *)array->data;
        s_cluster_partition_reference *reference = (s_cluster_partition_reference *)(data + (reference_index & 0xffff) * datum_size);
        long next = reference->next;
        if (next != -1)
            __dcbt(0, data + (next & 0xffff) * datum_size);
        iterator->reference_index = next;
        cluster_datum = reference->cluster_datum;
    } else {
        cluster_datum = -1;
    }
    return objects_cluster_reference_from_datum_index(cluster_datum);
}

s_cluster_reference objects_cluster_reference_get_next(s_cluster_reference_iterator *iterator, long object_index)
{
    return objects_cluster_reference_iterator_next(iterator);
}

// 0x116480

s_cluster_reference objects_cluster_reference_get_first(s_cluster_reference_iterator *iterator, long object_index)
{
    long root_index = inl_object_get_ultimate_parent(object_index);
    object_datum *root = ((object_datum *)object_get_and_verify_type(root_index, -1));
    iterator->partition = (bool)((root->flags >> 9) & 1) ? &tls_collideable_object_cluster_partition() : &tls_noncollideable_object_cluster_partition();
    iterator->reference_index = root->first_cluster_reference;
    return objects_cluster_reference_iterator_next(iterator);
}

static __forceinline bool object_has_marker_object(const object_datum *object)
{
    bool result = false;
    if (((const item_datum *)object)->owner_relation == 1)
        result = true;
    return result;
}
long object_get_marker_object(long object_index)
{
    long result = object_index;
    object_datum *object = object_get(object_index);
    if (object->parent_object_index != -1 && object_definition_uses_parent_markers(object))
        result = object_get_marker_object(object->parent_object_index);
    else if ((object->flags & 1) != 0 && ((1 << object->identifier_type) & 0xc) != 0 && object_has_marker_object(object))
        result = ((item_datum *)object)->unit_object_index;
    return result;
}

static __forceinline long model_find_variant_by_name(long model_index, long variant_name)
{
    long result = -1;
    if (model_index != -1) {
        const s_model_definition *definition = (const s_model_definition *)tag_get('hlmt', model_index);
        if (variant_name == 0) {
            if (definition->variants.count > 0)
                result = 0;
        } else {
            for (long variant_index = 0; variant_index < definition->variants.count; variant_index++) {
                const s_model_variant *variant = &((s_model_variant *)definition->variants.address)[variant_index];
                if (variant->name == variant_name) {
                    result = variant_index;
                    break;
                }
            }
        }
    }
    return result;
}

static __forceinline long object_lookup_variant_index_from_name(long object_index, long variant_name)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    if (variant_name == 0)
        variant_name = definition->default_model_variant;
    return model_find_variant_by_name(definition->model.index, variant_name);
}

// 0x31ae90
static __forceinline void object_type_attach_to_marker(long parent_object_index, long parent_marker_name, long child_object_index, long
    child_marker_name)
{
    object_datum *child_object = ((object_datum *)object_get_and_verify_type(child_object_index, -1));
    object_type_definition *definition = object_type_definitions[child_object->identifier_type];
    for (long type_index = 0; definition->parts[type_index]; type_index++) {
        object_type_definition *type = definition->parts[type_index];
        if (type->attach_to_marker) {
            type->attach_to_marker(parent_object_index, parent_marker_name, child_object_index, child_marker_name);
            break;
        }
    }
}
extern "C" void object_attach_to_marker_with_type(long parent_object_index, long parent_marker_name, long child_object_index, long child_marker_name)
{
    object_datum *child_object = ((object_datum *)object_get_and_verify_type(child_object_index, -1));
    if (child_object->parent_object_index == -1)
        object_type_attach_to_marker(parent_object_index, parent_marker_name, child_object_index, child_marker_name);
    if (child_object->parent_object_index == -1)
        object_attach_to_marker(parent_object_index, parent_marker_name, child_object_index, child_marker_name);
}

// 0x31b138
// match: noalias declaration preserves caller live registers

void object_prepare_axis_vectors(long object_index, real_vector3d *forward, real_vector3d *up)
{
    real_vector3d left;
    cross_product3d(forward, up, &left);
    cross_product3d(&left, forward, up);
    if (normalize3d(forward) > 0.0f) {
        if (normalize3d(up) <= 0.0f)
            generate_up_vector3d(forward, up);
    } else {
        object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
        *forward = object->forward;
        *up = object->up;
    }
}

// 0x31b7e0
static __forceinline void objects_remove_from_list(long *list, long object_index)
{
    while (*list != -1) {
        object_datum *object = ((object_datum *)object_get_and_verify_type(*list, -1));
        if (*list == object_index) {
            *list = object->next_object_index;
            object->next_object_index = -1;
            break;
        }
        list = &object->next_object_index;
    }
}
void object_detach_internal(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    s_object_header *header = object_header_get(object_index);
    object_datum *parent_object = ((object_datum *)object_get_and_verify_type(object->parent_object_index, -1));
    long ultimate_parent_index = inl_object_get_ultimate_parent(object_index);
    objects_remove_from_list(&parent_object->first_child_object_index, object_index);
    header->flags &= ~0x80;
    object->parent_object_index = -1;
    object->parent_node_index = -1;
    objects_physics_flags_set(&object->physics_flags, 4, false);
    object_compute_bounding_sphere(ultimate_parent_index);
}

// 0x31b8b0

// 0x31bb18

// 0x31bda0
void object_choose_variant(long object_index, long variant_name)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    long variant_index = object_lookup_variant_index_from_name(object_index, variant_name);
    object->variant_index = (signed char)variant_index;
}

static __forceinline void *objects_header_block_get(long object_index, short reference_offset)
{
    object_datum *object = object_get(object_index);
    const s_object_block_reference *reference = (const s_object_block_reference *)((const char *)object + reference_offset);
    return (char *)object + reference->offset;
}
static __forceinline void *objects_header_block_get_with_count(long object_index, short reference_offset, unsigned int element_size, long *count)
{
    void *block = objects_header_block_get(object_index, reference_offset);
    const s_object_block_reference *reference = (const s_object_block_reference *)((const char *)object_get(object_index) + reference_offset);
    *count = reference->size / (long)element_size;
    return block;
}
// 0x31bff0
bool object_get_render_model_instance_bitfield_active(long object_index, unsigned long **bitfield)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    if ((bool)((object->flags >> 11) & 1)) {
        long count;
        char *block = (char *)objects_header_block_get_with_count(object_index, offsetof(object_datum, region_information), 1, &count);
        *bitfield = (unsigned long *)(block + (count - 16));
        return true;
    }
    return false;
}


// 0x31c050


void object_compute_instance_bitfield_visible(long object_index)
{
    long region_count;
    char *region_permutations;
    char *unused_a;
    object_region_information *unused_b;
    object_get_region_information(object_index, &region_count, &region_permutations, &unused_a, &unused_b);
    unsigned long *active_bitfield;
    unsigned long *visible_bitfield;
    if (object_get_render_model_instance_bitfield_active(object_index, &active_bitfield) &&
        object_get_render_model_instance_bitfield_visible(object_index, (void **)&visible_bitfield))
    {
        object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
        const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
        const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', definition->model.index);
        const render_model_definition *render_model = (const render_model_definition *)tag_get('mode', model->render_model_index);
        memset(visible_bitfield, 0, sizeof(unsigned long) * 2);
        const signed char *permutation_indices = (const signed char *)region_permutations;
        for (long region_index = 0; region_index < render_model->regions.count; region_index++) {
            if (permutation_indices[region_index] != -1) {
                const s_render_model_permutation *permutation = &((const s_render_model_region
                    *)render_model->regions.address)[region_index].permutations[permutation_indices[region_index]];
                for (long word_index = 0; word_index < 2; word_index++)
                    visible_bitfield[word_index] |= permutation->instance_mask[word_index];
            }
        }
        for (long word_index = 0; word_index < 2; word_index++)
            visible_bitfield[word_index] &= active_bitfield[word_index];
    }
}
// 0x31c388


bool object_set_base_change_color_by_index(long object_index, long change_color_index, const real_rgb_color *color)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    bool result = false;
    real_rgb_color *change_colors = (real_rgb_color *)((char *)object + object->change_colors.offset);
    long change_color_count = object->change_colors.size / 12u;
    long half_count = change_color_count / 2;
    if (change_color_index >= 0 && change_color_index < half_count)
    {
        change_colors[change_color_index] = *color;
        change_colors[half_count + change_color_index] = *color;
        render_object_update_change_colors(object_index, true);
        result = true;
    }
    return result;
}

// object_try_and_get_unsafe_and_verify_type: a copy of object_try_and_get_and_verify_type that also rejects a
// negative absolute index: the object a handle names when its header is live and its type is in the mask, else 0
void *object_try_and_get_unsafe_and_verify_type(long object_index, unsigned long type_mask)
{
    s_data_array *data = object_header_data;
    void *result = 0;
    if (object_index != -1) {
        int absolute_index = object_index & 0xffff;
        if (absolute_index >= 0 && absolute_index < data->first_unallocated) {
            s_object_header *header = (s_object_header *)((char *)data->data + data->size * absolute_index);
            int identifier = header->identifier;
            if (identifier != 0 && identifier == (object_index >> 16)
                && ((1 << header->object_type) & type_mask) != 0)
                result = header->datum;
        }
    }
    return result;
}

// 0x31c9e8
static __forceinline void object_type_attach_gamestate_entity(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    object_type_definition *definition = object_type_definitions[object->identifier_type];
    for (short type_index = 0; definition->parts[type_index]; type_index++) {
        object_type_definition *type = definition->parts[type_index];
        if (type->attach_gamestate_entity)
            type->attach_gamestate_entity(object_index);
    }
}
void object_attach_gamestate_entity(long object_index, long gamestate_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    inl_object_get_ultimate_parent(object_index);
    object->gamestate_index = gamestate_index;
    object_type_attach_gamestate_entity(object_index);
}

// 0x31cac0
static __forceinline void object_type_detach_gamestate_entity(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    object_type_definition *definition = object_type_definitions[object->identifier_type];
    for (short type_index = 0; definition->parts[type_index]; type_index++) {
        object_type_definition *type = definition->parts[type_index];
        if (type->detach_gamestate_entity)
            type->detach_gamestate_entity(object_index);
    }
}
void object_detach_gamestate_entity(long object_index, long gamestate_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    object_type_detach_gamestate_entity(object_index);
    object->gamestate_index = -1;
    object->flags_d6 = 0;
}

void object_header_delete(long object_index)
{
    s_object_header *header = object_header_get(object_index);
    if (header->datum != 0) {
        memory_pool_block_free(g_object_memory_pool, (long)((char *)header->datum - (char *)g_object_memory_pool));
        header->datum = 0;
    }
    ::datum_delete(object_header_data, object_index);
}


static __forceinline void object_type_postprocess_node_matrices(long object_index, long node_count, real_matrix4x3 *matrices)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    object_type_definition *definition = object_type_definitions[object->identifier_type];
    for (short type_index = 0; definition->parts[type_index]; type_index++) {
        object_type_definition *type = definition->parts[type_index];
        if (type->postprocess_node_matrices)
            type->postprocess_node_matrices(object_index, node_count, matrices);
    }
}


// 0x31d4e0
void recursive_wake_children_awoken_by_movement(long object_index)
{
    s_object_header *header = object_header_get(object_index);
    object_datum *object = header->datum;
    if ((1 << header->object_type) & 0x1803)
        object_wake(object_index);
    for (long child_index = object->first_child_object_index; child_index != -1; ) {
        s_object_header *child_header = object_header_get(child_index);
        recursive_wake_children_awoken_by_movement(child_index);
        child_index = child_header->datum->next_object_index;
    }
}

// 0x31e4d0
static __forceinline void objects_matrix4x3_translation(real_matrix4x3 *matrix, const real_point3d *translation)
{
    matrix->scale = 1.0f;
    matrix->forward.i = 1.0f;
    matrix->forward.j = 0.0f;
    matrix->forward.k = 0.0f;
    matrix->left.i = 0.0f;
    matrix->left.j = 1.0f;
    matrix->left.k = 0.0f;
    matrix->up.i = 0.0f;
    matrix->up.j = 0.0f;
    matrix->up.k = 1.0f;
    matrix->position = *translation;
}
static __forceinline void objects_matrix4x3_scale(real_matrix4x3 *matrix, float scale)
{
    matrix->scale = scale;
    matrix->forward.i = 1.0f;
    matrix->forward.j = 0.0f;
    matrix->forward.k = 0.0f;
    matrix->left.i = 0.0f;
    matrix->left.j = 1.0f;
    matrix->left.k = 0.0f;
    matrix->up.i = 0.0f;
    matrix->up.j = 0.0f;
    matrix->up.k = 1.0f;
    matrix->position.x = 0.0f;
    matrix->position.y = 0.0f;
    matrix->position.z = 0.0f;
}

void object_compute_origin_matrix(long object_index, const real_point3d *position, const real_vector3d *forward, const real_vector3d *up,
    float scale, bool negate_left, const real_matrix4x3 *parent_matrix, bool parent_negate_left, real_matrix4x3 *matrix)
{
    real_matrix4x3 translation_matrix;
    real_matrix4x3 rotation_matrix;
    real_matrix4x3 scale_matrix;
    real_matrix4x3 parent_copy;
    real_matrix4x3 combined_matrix;
    const real_matrix4x3 *left_matrix;
    objects_matrix4x3_translation(&translation_matrix, position);
    matrix4x3_rotation_from_vectors(&rotation_matrix, forward, up);
    if (negate_left)
        negate_vector3d(&rotation_matrix.left, &rotation_matrix.left);
    if (scale != 1.0f) {
        objects_matrix4x3_scale(&scale_matrix, scale);
        matrix4x3_compute_product(&rotation_matrix, &scale_matrix, &rotation_matrix);
    }
    if (parent_matrix) {
        if (parent_matrix->scale != 1.0f || parent_negate_left) {
            parent_copy = *parent_matrix;
            if (parent_copy.scale != 1.0f) {
                translation_matrix.position.x *= parent_copy.scale;
                translation_matrix.position.y *= parent_copy.scale;
                translation_matrix.position.z *= parent_copy.scale;
                parent_copy.scale = 1.0f;
            }
            if (parent_negate_left)
                negate_vector3d(&parent_copy.left, &parent_copy.left);
            parent_matrix = &parent_copy;
        }
        matrix4x3_compute_product(parent_matrix, &translation_matrix, &combined_matrix);
        left_matrix = &combined_matrix;
    } else {
        left_matrix = &translation_matrix;
    }
    matrix4x3_compute_product(left_matrix, &rotation_matrix, matrix);
}
// 0x31eb20
void object_postprocess_node_matrices(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    s_object_definition *definition = (s_object_definition *)tag_get('obje', object->definition_index);
    if (definition->model.index != -1) {
        s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
        if (model->render_model_index != -1 && model->animation_graph.index != -1) {
            long node_count;
            real_matrix4x3 *matrices = object_get_node_matrices(object_index, &node_count);
            object_type_postprocess_node_matrices(object_index, node_count, matrices);
        }
    }
}

// 0x31fba0

typedef char objects_attached_light_size[(sizeof(s_objects_attached_light) == 8) ? 1 : -1];
typedef char objects_attached_light_index_offset[(offsetof(s_objects_attached_light, light_index) == 4) ? 1 : -1];
void object_connect_lights_recursive(long object_index, bool disconnect, bool reconnect, bool recursive, bool parent_visible)
{
    s_object_header *header = object_header_get(object_index);
    object_datum *object = header->datum;
    bool hidden;
    bool done = false;
    if (parent_visible)
        hidden = object_test(header, object);
    else
        hidden = object_is_hidden(object_index);
    if (hidden)
        done = true;
    if (!done) {
        if ((bool)((object->flags >> 4) & 1)) {
            long count = object->attached_lights.size / sizeof(s_objects_attached_light);
            s_objects_attached_light *lights = (s_objects_attached_light *)((char *)object + object->attached_lights.offset);
            for (long i = 0; i < count; i++) {
                if (lights[i].type == 0 && lights[i].light_index != -1 && !hidden) {
                    if (disconnect)
                        light_disconnect_from_map(lights[i].light_index);
                    if (reconnect)
                        light_reconnect_to_map(lights[i].light_index);
                }
            }
        }
        if ((bool)((object->flags >> 6) & 1)) {
            c_data_iterator<light_datum> iterator;
            iterator.begin((s_data_array *)light_data);
            while (iterator.next()) {
                light_datum *light = (light_datum *)iterator.get_datum();
                if (light->owner_object_index == object_index) {
                    if (disconnect)
                        light_disconnect_from_map(iterator.get_index());
                    if (reconnect)
                        light_reconnect_to_map(iterator.get_index());
                }
            }
        }
        if (object->first_child_object_index != -1)
            object_connect_lights_recursive(object->first_child_object_index, disconnect, reconnect, true, !hidden);
        if (recursive && object->next_object_index != -1)
            object_connect_lights_recursive(object->next_object_index, disconnect, reconnect, recursive, parent_visible);
    }
}

// 0x31ff40
void object_update_ai_flags(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    if (definition->ai_properties.count > 0) {
        if ((bool)(((unsigned long)((object_ai_properties *)definition->ai_properties.address)->flags >> 3) & 1))
            object->flags_bb |= 4;
        else
            object->flags_bb &= ~4;
    }
}

// 0x31ffb8
bool object_try_and_get_location(long object_index, s_location *location)
{
    object_datum *object = object_get(object_index);
    scenario_location_from_point(location, &object->bounding_sphere_center);
    if (*(unsigned char *)location == 255 &&
        object_placement_find_surface_from_sphere(
            g_active_structure_bsp_mask, &object->bounding_sphere_center, object->bounding_sphere_radius, (surface_identifier *)location) == 0)
        scenario_location_from_point(location, &object->position);
    return (signed char)*(unsigned char *)location + 1 != 0;
}

long c_object_identifier::find_object_index() const
{
    long result = -1;
    if ((unsigned char)m_source != 0xff) {
        c_object_iterator<object_datum> iterator;
        iterator.begin(1 << m_type, 0);
        bool found = iterator.next();
        while (found) {
            if (is_equal(&iterator.get_datum()->identifier))
                result = iterator.get_index();
            found = iterator.next();
        }
    }
    return result;
}

// Identifiers compare id, type and source; scenario identifiers also compare their origin BSP.
bool c_object_identifier::is_equal(const c_object_identifier *other) const throw()
{
  bool result = true;
  result &= m_type == other->m_type;
  result &= (char)m_source == (char)other->m_source;
  result &= m_unique_id == other->m_unique_id;
  if (result && (char)m_source == 0)
    result &= m_origin_bsp_index == other->m_origin_bsp_index;
  return result;
}

// 0x3201b0
void resursive_add_child_objects_to_list(long object_index, long definition_index, long object_list_index)
{
    object_datum *object = object_get(object_index);
    if (object->definition_index == definition_index || definition_index == -1)
        object_list_add(object_list_index, object_index);
    if (object->next_object_index != -1)
        resursive_add_child_objects_to_list(object->next_object_index, definition_index, object_list_index);
    if (object->first_child_object_index != -1)
        resursive_add_child_objects_to_list(object->first_child_object_index, definition_index, object_list_index);
}

// 0x320618
void object_find_structure_bsp_fake_lightprobe_index(long structure_bsp_index, long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    structure_bsp *bsp = global_structure_bsp_get(structure_bsp_index);
    bool found = false;
    if (definition->object_flags.test(8)) {
        for (long index = 0; index < bsp->object_fake_lightprobes.count; index++) {
            s_structure_bsp_fake_lightprobe *lightprobe = &((s_structure_bsp_fake_lightprobe *)bsp->object_fake_lightprobes.address)[index];
            if (lightprobe->object_identifier.is_equal(&object->identifier)) {
                object->structure_bsp_fake_lightprobe_index = (short)index;
                found = true;
                break;
            }
        }
        if (!found)
            object->structure_bsp_fake_lightprobe_index = -1;
    } else {
        object->structure_bsp_fake_lightprobe_index = -1;
    }
}

// 0x320360


static __forceinline void objects_reverse_matrix4x3_to_orientation(const real_matrix4x3 *matrix, real_orientation *orientation)
{
    matrix3x3_rotation_to_quaternion((const real_matrix3x3 *)&matrix->forward, &orientation->rotation);
    orientation->translation = matrix->position;
    orientation->scale = matrix->scale;
}
void object_reverse_compute_node_orientations_from_matrices(long object_index, unsigned long *node_mask,
    const render_model_definition *render_model, long node_count, real_orientation *orientations)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    const real_matrix4x3 *parent_matrix = NULL;
    bool parent_mirrored = false;
    real_matrix4x3 *node_matrices = (real_matrix4x3 *)((char *)object + object->node_matrices.offset);
    if (object->parent_object_index != -1) {
        parent_matrix = objects_get_node_matrix(object->parent_object_index, object->parent_node_index);
        parent_mirrored = (bool)((((object_datum *)object_get_and_verify_type(object->parent_object_index, -1))->flags >> 10) & 1);
    }
    real_matrix4x3 origin_matrix;
    object_compute_origin_matrix(object_index, &object->position, &object->forward, &object->up, object->scale,
        (bool)((object->flags >> 10) & 1), parent_matrix, parent_mirrored, &origin_matrix);
    real_matrix4x3 inverse_origin_matrix;
    matrix4x3_inverse(&origin_matrix, &inverse_origin_matrix);
    if (!node_mask || (node_mask[0] & 1)) {
        real_matrix4x3 matrix;
        matrix4x3_compute_product(&inverse_origin_matrix, &node_matrices[0], &matrix);
        matrix4x3_to_orientation(&matrix, &orientations[0]);
    }
    if (!node_mask) {
        for (long node_index = 1; node_index < node_count; node_index++) {
            const render_model_node *node = &((const render_model_node *)render_model->nodes.address)[node_index];
            real_orientation *orientation = &orientations[node_index];
            real_matrix4x3 parent_inverse;
            real_matrix4x3 matrix;
            matrix4x3_inverse(&node_matrices[node->parent_node_index], &parent_inverse);
            matrix4x3_compute_product(&parent_inverse, &node_matrices[node_index], &matrix);
            objects_reverse_matrix4x3_to_orientation(&matrix, orientation);
        }
    } else {
        for (long node_index = 1; node_index < node_count; node_index++) {
            if (node_mask[node_index >> 5] & (1 << (node_index & 31))) {
                const render_model_node *node = &((const render_model_node *)render_model->nodes.address)[node_index];
                real_matrix4x3 parent_inverse;
                real_matrix4x3 matrix;
                matrix4x3_inverse(&node_matrices[node->parent_node_index], &parent_inverse);
                matrix4x3_compute_product(&parent_inverse, &node_matrices[node_index], &matrix);
                matrix4x3_to_orientation(&matrix, &orientations[node_index]);
            }
        }
    }
}

// 0x320230
void object_freeze_node_orientations(long object_index, bool freeze)
{
    if (object_index != -1 && object_can_interpolate(object_index)) {
        object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
        if (!freeze && (bool)((object->flags >> 29) & 1))
            object_reverse_compute_node_orientations_from_matrices(object_index, NULL);
        if (freeze)
            object->flags |= 1u << 29;
        else
            object->flags &= ~(1u << 29);
    }
}

// 0x3202d0
// the node orientations of the object's physics render model, computed backwards from its node matrices
void object_reverse_compute_node_orientations_from_matrices(long object_index, unsigned long *node_mask)
{
    if (!object_can_interpolate(object_index))
        return;
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    const s_object_definition *object_definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    if (object_definition->model.index == -1)
        return;
    const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', object_definition->model.index);
    if (model->render_model_index == -1)
        return;
    real_orientation *node_orientations = (real_orientation *)((char *)object + object->node_orientations.offset);
    long node_count = object->node_orientations.size / sizeof(real_orientation);
    object_reverse_compute_node_orientations_from_matrices(object_index, node_mask,
        (const render_model_definition *)tag_get('mode', model->render_model_index), node_count, node_orientations);
}

// 0x3191f0
// @address 0x3191f0 ?object_load_scenario_placement_matrices@@YA_NJ@Z
bool object_load_scenario_placement_matrices(long object_index)
{
    bool result = false;
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    if (object->parent_object_index == -1 && ((1 << object->identifier.get_source()) & 3) != 0) {
        long scenario_datum_index;
        const s_scenario_object *scenario_object = object->identifier.find_scenario_object_from_scenario(global_scenario, &scenario_datum_index);
        if (scenario_object && scenario_object->node_orientations.count() != 0) {
            const s_object_definition *object_definition = (const s_object_definition *)tag_get('obje', object->definition_index);
            if (object_definition->model.index != -1) {
                const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', object_definition->model.index);
                if (model->render_model_index != -1) {
                    const render_model_definition *render_model = (const render_model_definition *)tag_get('mode', model->render_model_index);
                    const s_scenario_object_node_orientations *scenario_node_orientations = scenario_object->node_orientations.begin();
                    if (scenario_node_orientations->node_count != 0 && scenario_node_orientations->node_count == render_model->nodes.count) {
                        long node_matrices_count = object->node_matrices.size / sizeof(real_matrix4x3);
                        real_matrix4x3 *node_matrices = (real_matrix4x3 *)((char *)object + object->node_matrices.offset);
                        const unsigned char *bit_vector = scenario_node_orientations->bit_vector.begin();
                        long node_orientation_count;
                        real_orientation *node_orientations;
                        if (object_can_interpolate(object_index)) {
                            node_orientations = (real_orientation *)objects_object_header_block_get_with_count(
                                object, &object->node_orientations, sizeof(real_orientation), &node_orientation_count);
                        } else {
                            node_orientations = object_get_node_orientations((s_objects_element_array *)render_model,
                                node_matrices_count);
                            node_orientation_count = node_matrices_count;
                        }
                        long element_index = 0;
                        for (long node_index = 0; node_index < node_orientation_count; node_index++) {
                            const render_model_node *node = &((const render_model_node *)render_model->nodes.address)[node_index];
                            node_orientations[node_index].scale = 1.0f;
                            node_orientations[node_index].rotation = node->default_rotation;
                            node_orientations[node_index].translation = node->default_translation;
                            if (bit_vector[node_index >> 3] & (1 << (node_index & 7))) {
                                const short *rotation = &scenario_node_orientations->orientations[element_index];
                                float *quaternion = (float *)&node_orientations[node_index].rotation;
                                for (long component = 0; component < 4; component++)
                                    quaternion[component] = (float)rotation[component] / 32767.0f;
                                element_index += 4;
                            }
                            if (bit_vector[(scenario_node_orientations->node_count + node_index) >> 3] &
                                (1 << ((scenario_node_orientations->node_count + node_index) & 7)))
                            {
                                const short *translation = &scenario_node_orientations->orientations[element_index];
                                real_point3d *point = &node_orientations[node_index].translation;
                                for (long component = 0; component < 3; component++)
                                    point->n[component] = 15.0f * (float)translation[component] / 32767.0f;
                                element_index += 3;
                            }
                        }
                        if (render_model->nodes.count > 0) {
                            real_matrix4x3 world_matrix;
                            object_get_world_matrix(object_index, &world_matrix);
                            matrix4x3_multiply_by_orientation(&world_matrix, &node_orientations[0], &node_matrices[0]);
                            for (long node_index = 1; node_index < render_model->nodes.count; node_index++) {
                                const render_model_node *node = &((const render_model_node *)render_model->nodes.address)[node_index];
                                matrix4x3_multiply_by_orientation(
                                    &node_matrices[node->parent_node_index], &node_orientations[node_index], &node_matrices[node_index]);
                            }
                        }
                        object_compute_bounding_sphere(object_index);
                        result = true;
                    }
                }
            }
        }
    }
    return result;
}

// 0x320588
void objects_setup_structure_bsp_fake_lightprobes(long structure_bsp_index)
{
    c_object_iterator<object_datum> iterator;
    iterator.begin(-1, 0);
    while (iterator.next())
        object_find_structure_bsp_fake_lightprobe_index(structure_bsp_index, iterator.get_index());
}


// @address 0x241778 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_object_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_globals_allocator>::deallocate(void *address)
{
    cinematic_new_globals = 0;
    restricted_region_free_member(3, m_member_index);
}

// @address 0x320d18 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_object_name_list_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_name_list_allocator>::deallocate(void *address)
{
    g_object_name_list = 0;
    restricted_region_free_member(3, m_member_index);
}

// @address 0x320df0 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_object_render_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_render_data_allocator>::deallocate(void *address)
{
    g_object_render_data = 0;
    restricted_region_free_member(2, m_member_index);
}
template<long k_region_index, void (*k_adopt_function)(void *), void (*k_post_copy_function)(void *), void (*k_set_allocator_function)(void *)>
void *t_restricted_allocation_manager<k_region_index, k_adopt_function, k_post_copy_function, k_set_allocator_function>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    m_member_index = restricted_region_add_member<t_restricted_allocator_publisher<k_set_allocator_function> >(
        k_region_index, name, type, size, alignment_bits, k_set_allocator_function, k_adopt_function, k_post_copy_function);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(k_region_index, m_member_index);
}


// 0xfcaa0
static __forceinline bool objects_recycling_flags_test(const unsigned char *flags, long bit)
{
    return (*flags & (1 << bit)) != 0;
}
static __forceinline void objects_recycling_flags_set(unsigned char *flags, long bit, bool enable)
{
    if (enable)
        *flags |= 1 << bit;
    else
        *flags &= ~(1 << bit);
}

// 0x3208c8
void objects_unregister_scenario_object(long object_index)
{
    if (game_globals->options.game_mode != 2) {
        object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
        short name_index = object->name_index;
        if (name_index != -1) {
            long first_name_index = object_globals->first_name_index[object->identifier_type];
            if (first_name_index != -1)
                object_globals->object_indices[first_name_index + name_index] = -1;
        }
    }
}

// 0x320948
// @address 0x320948 objects_is_main_thread_with_lock
bool objects_is_main_thread_with_lock(void)
{
    return get_current_thread_index() == 1 && restricted_region_is_available(3);
}

// 0x320998
void object_placement_reset_scale_for_physics(object_placement_data *data)
{
    if (data->definition_index != -1) {
        const s_object_definition *definition = (const s_object_definition *)tag_get('obje', data->definition_index);
        if (definition->model.index != -1) {
            const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', definition->model.index);
            if (model->physics_model_index != -1)
                data->scale = 1.0f;
        }
    }
}

// 0x3206f8
void adopt_old_object_render_message_queue(void *address)
{
    g_object_message_queue->try_to_prepend_message_queue((t_message_queue<s_object_render_thread_message, 2048> *)address);
}


// 0x320760

// 0x3209f0
template<> void *t_restricted_allocation_manager<3, &adopt_old_object_render_message_queue, static_cast<void (*)(void *)>(0),
    &__tls_set_g_first_person_weapons_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    m_member_index = restricted_region_add_member<t_restricted_allocator_publisher<__tls_set_g_first_person_weapons_allocator> >(
        3, name, type, size, alignment_bits, __tls_set_g_first_person_weapons_allocator, adopt_old_object_render_message_queue, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// 0x320a90
// @address 0x320a90 ?allocate@?$t_restricted_allocation_manager@$02$1?adopt_old_object_render_message_queue@@YAXPAX@Z$0A@$1?__tls_set_g_first_person_weapons_allocator@@YAX0@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3, &adopt_old_object_render_message_queue, static_cast<void (*)(void *)>(0),
    &__tls_set_g_first_person_weapons_allocator>::allocate(
    unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x320ac8
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_broadphase_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x320ac8: zero-alignment assumption folds the member-registration path
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_add_member<t_restricted_allocator_publisher<__tls_set_g_object_broadphase_allocator> >(
        3, name, type, size, alignment_bits, __tls_set_g_object_broadphase_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// 0x320b68
// @address 0x320b68 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_object_broadphase_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_broadphase_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_broadphase_allocator>::deallocate(void *address)
{
    free_memory();
}

// 0x320ba0

// 0x320c40

// 0x320c60
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_name_list_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x320c60: zero-alignment assumption folds the member-registration path
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_add_member<t_restricted_allocator_publisher<__tls_set_g_object_name_list_allocator> >(
        3, name, type, size, alignment_bits, __tls_set_g_object_name_list_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// 0x320d00
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_name_list_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x320d38
template<> void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_render_data_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x320d38: zero-alignment assumption folds the member-registration path
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_add_member<t_restricted_allocator_publisher<__tls_set_g_object_render_data_allocator> >(
        2, name, type, size, alignment_bits, __tls_set_g_object_render_data_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(2, m_member_index);
}

// 0x320dd8
template<> void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_render_data_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x320e10
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &set_memory_pool_address>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}
// 0x320ea8
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &set_memory_pool_address>::deallocate(void *address)
{
    set_memory_pool_address(0);
    restricted_region_free_member(3, m_member_index);
}

// 0x320ee8
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_collideable_first_data_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x320fa0 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_collideable_first_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_collideable_first_data_allocator>::deallocate(void *address)
{
    tls_collideable_object_cluster_partition().cluster_heads = 0;
    restricted_region_free_member(3, m_member_index);
}

// 0x320fc8
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_collideable_data_reference_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x321068 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_collideable_data_reference_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_collideable_data_reference_allocator>::deallocate(void *address)
{
    tls_collideable_object_cluster_partition().data = 0;
    restricted_region_free_member(3, m_member_index);
}

// 0x321088
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_collideable_cluster_reference_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x321130
// @address 0x321130 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_collideable_cluster_reference_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_collideable_cluster_reference_allocator>::deallocate(void *address)
{
    tls_collideable_object_cluster_partition().references = 0;
    restricted_region_free_member(3, m_member_index);
}

// 0x321158
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_noncollideable_first_data_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x321210 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_noncollideable_first_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_noncollideable_first_data_allocator>::deallocate(void *address)
{
    tls_noncollideable_object_cluster_partition().cluster_heads = 0;
    restricted_region_free_member(3, m_member_index);
}

// 0x321238
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_noncollideable_data_reference_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x3212d8 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_noncollideable_data_reference_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_noncollideable_data_reference_allocator>::deallocate(void *address)
{
    tls_noncollideable_object_cluster_partition().data = 0;
    restricted_region_free_member(3, m_member_index);
}

// 0x3212f8
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_noncollideable_cluster_reference_allocator>::allocate(
    unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x3213a0
// @address 0x3213a0 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_noncollideable_cluster_reference_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_noncollideable_cluster_reference_allocator>::deallocate(void *address)
{
    tls_noncollideable_object_cluster_partition().references = 0;
    restricted_region_free_member(3, m_member_index);
}

// 0x3213c8
template<class t_message, long k_count> void t_message_queue<t_message, k_count>::clear()
{
    this->message_count.set(0);
    in_use.set_array(0);
}

// 0x321420
template<class t_message, long k_count> void t_message_queue<t_message, k_count>::add_message(const t_message *message)
{
    long index = this->message_count.increment() - 1;
    ((c_synchronized_bitvector<512> *)&in_use)->set_bit(index, false);
    messages[index] = *message;
    ((c_synchronized_bitvector<512> *)&in_use)->set_bit(index, true);
}
#if !defined(_XBOX) || defined(__clang__)
template void t_message_queue<s_object_render_thread_message, 2048>::clear();
template void t_message_queue<s_object_render_thread_message, 2048>::add_message(const s_object_render_thread_message *message);
#endif

// 0x3214a0
template<class t_message, long k_count> long t_message_queue<t_message, k_count>::try_to_append_message_queue(t_message_queue *source)
{
    long message_count = source->message_count.peek();
    long failed_count = 0;
    for (long index = 0; index < message_count; index++) {
        if (source->in_use.test_bit(index) && !try_add_message(&source->messages[index]))
            failed_count++;
    }
    return failed_count;
}


// 0x3362c8
void __tls_set_g_tracked_object_globals_allocator(void *allocator)
{
    g_tracked_object_globals = (s_objects_tracked_object_globals *)allocator;
}

// 0x33e470
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_tracked_object_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x33e470: zero-alignment assumption folds the member-registration path
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_add_member<t_restricted_allocator_publisher<__tls_set_g_tracked_object_globals_allocator> >(
        1, name, type, size, alignment_bits, __tls_set_g_tracked_object_globals_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}

// 0x33e510
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_tracked_object_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x33e528 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_tracked_object_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_tracked_object_globals_allocator>::deallocate(void *address)
{
    g_tracked_object_globals = 0;
    restricted_region_free_member(1, m_member_index);
}

static __forceinline s_object_marker_globals *objects_get_object_marker_globals()
{
    return &g_object_marker_globals[get_current_thread_index() - 1];
}
static __forceinline void objects_object_marker_begin()
{
    s_object_marker_globals *globals = objects_get_object_marker_globals();
    globals->mark++;
    globals->active = true;
}
static __forceinline void objects_object_marker_end()
{
    s_object_marker_globals *globals = objects_get_object_marker_globals();
    globals->active = false;
}
static __forceinline bool objects_object_mark(long object_index)
{
    s_object_marker_globals *globals = objects_get_object_marker_globals();
    long absolute_index = (unsigned short)object_index;
    long *slot = &globals->marks[absolute_index];
    long previous_mark = *slot;
    long mark = globals->mark;
    *slot = mark;
    return previous_mark != mark;
}
// 0x9c390
long objects_in_clusters_by_indices(unsigned long class_flags, unsigned long object_flags, unsigned long type_flags, long cluster_count,
    const s_cluster_reference *clusters, long maximum_count, long *object_indices)
{
    // match: 0x9c390: flag and count assumptions fold the cluster-scan setup branches
    __assume(class_flags == 1);
    // match: 0x9c390: flag and count assumptions fold the cluster-scan setup branches
    __assume(type_flags == 0x200);
    // match: 0x9c390: flag and count assumptions fold the cluster-scan setup branches
    __assume(maximum_count == 2048);
    long object_count = 0;
    objects_object_marker_begin();
    for (long cluster_index = 0; cluster_index < cluster_count; cluster_index++) {
        s_cluster_reference cluster = clusters[cluster_index];
        if (class_flags & 1) {
            long iterator;
            const s_object_cluster_payload *payload;
            for (long object_index = cluster_partition_get_first_datum_and_payload(&tls_collideable_object_cluster_partition(), &iterator, cluster,
                sizeof(s_object_cluster_payload), (const void **)&payload);
                object_index != -1;
                object_index = cluster_partition_get_next_datum_and_payload(&tls_collideable_object_cluster_partition(), &iterator,
                    sizeof(s_object_cluster_payload), (const void **)&payload))
                {
                if ((payload->cull_flags & type_flags) && objects_object_mark(object_index)) {
                    if (object_count >= maximum_count) {
                        objects_object_marker_end();
                        return object_count;
                    }
                    object_indices[object_count++] = object_index;
                }
            }
        }
        if (class_flags & 2) {
            long iterator;
            const s_object_cluster_payload *payload;
            for (long object_index = cluster_partition_get_first_datum_and_payload(&tls_noncollideable_object_cluster_partition(), &iterator,
                cluster, sizeof(s_object_cluster_payload), (const void **)&payload);
                object_index != -1;
                object_index = cluster_partition_get_next_datum_and_payload(&tls_noncollideable_object_cluster_partition(), &iterator,
                    sizeof(s_object_cluster_payload), (const void **)&payload))
                {
                if ((payload->cull_flags & type_flags) && objects_object_mark(object_index)) {
                    if (object_count >= maximum_count) {
                        objects_object_marker_end();
                        return object_count;
                    }
                    object_indices[object_count++] = object_index;
                }
            }
        }
    }
    objects_object_marker_end();
    return object_count;
}

// 0xa0d80

static __forceinline long cluster_partition_begin_payload_iteration(
    const cluster_partition *partition, long *iterator, s_cluster_reference cluster, long payload_size, const void **payload)
{
    s_data_array *array = partition->data;
    long handle = partition->cluster_heads[cluster.bsp_index * 256 + cluster.cluster_index];
    *iterator = handle;
    long datum_index;
    if (handle != -1) {
        int datum_size = array->size;
        char *data = (char *)array->data;
        s_cluster_partition_element *element = (s_cluster_partition_element *)(data + (handle & 0xffff) * datum_size);
        long next = element->next;
        *payload = (char *)element + 12;
        if (next != -1)
            __dcbt(0, data + (next & 0xffff) * datum_size);
        datum_index = element->datum_index;
        *iterator = next;
    } else {
        datum_index = -1;
    }
    return datum_index;
}
short objects_in_sphere(unsigned long class_flags, unsigned long type_flags, const s_location *location, const real_point3d *center,
    float radius, long *object_indices, short maximum_count)
{
    long object_count = 0;
    if (type_flags == 0)
        type_flags = -1;
    if (class_flags == 0)
        class_flags = -1;
    s_cluster_reference clusters[256];
    long cluster_count = structure_clusters_in_sphere(location->cluster_reference, center, radius, 256, clusters, 0);
    objects_object_marker_begin();
    for (long cluster_index = 0; cluster_index < cluster_count; cluster_index++) {
        s_cluster_reference cluster = clusters[cluster_index];
        if (class_flags & 1) {
            long iterator;
            const s_object_cluster_payload *payload = 0;
            for (long object_index = cluster_partition_get_first_datum_and_payload(&tls_collideable_object_cluster_partition(), &iterator, cluster,
                sizeof(s_object_cluster_payload), (const void **)&payload);
                object_index != -1;
                object_index = cluster_partition_get_next_datum_and_payload(&tls_collideable_object_cluster_partition(), &iterator,
                    sizeof(s_object_cluster_payload), (const void **)&payload))
                {
                if (((1 << payload->object_type) & type_flags) && objects_object_mark(object_index)
                    && point_in_sphere(center, &payload->bounding_sphere_center, radius + payload->bounding_sphere_radius)
                    && !::game_engine_is_object_being_edited(object_index, (long *)(0))) {
                    if (object_count >= maximum_count) {
                        objects_object_marker_end();
                        return object_count;
                    }
                    object_indices[object_count++] = object_index;
                }
            }
        }
        if (class_flags & 2) {
            long iterator;
            const s_object_cluster_payload *payload = 0;
            for (long object_index = cluster_partition_begin_payload_iteration(&tls_noncollideable_object_cluster_partition(), &iterator,
                cluster, sizeof(s_object_cluster_payload), (const void **)&payload);
                object_index != -1;
                object_index = cluster_partition_get_next_datum_and_payload(&tls_noncollideable_object_cluster_partition(), &iterator,
                    sizeof(s_object_cluster_payload), (const void **)&payload))
                {
                if (((1 << payload->object_type) & type_flags) && objects_object_mark(object_index)
                    && point_in_sphere(center, &payload->bounding_sphere_center, radius + payload->bounding_sphere_radius)) {
                    if (object_count >= maximum_count) {
                        objects_object_marker_end();
                        return object_count;
                    }
                    object_indices[object_count++] = object_index;
                }
            }
        }
    }
    objects_object_marker_end();
    return object_count;
}

// 0x9ce98


static __forceinline long objects_find_marker_group_with_marker(const render_model_definition *render_model, long marker_name, long marker_index)
{
    for (long group_index = 0; group_index < render_model->marker_groups.count; group_index++) {
        const render_model_marker_group *group = &((const render_model_marker_group *)render_model->marker_groups.address)[group_index];
        if (marker_name == group->name && marker_index >= 0 && marker_index < group->markers.count)
            return group_index;
    }
    return -1;
}
// @address 0x9ce98 object_try_and_get_marker_point
bool object_try_and_get_marker_point(long object_index, long marker_name, long marker_index, real_point3d *point)
{
    bool result = false;
    if (object_index != -1 && marker_index != -1 && point) {
        object_datum *object = object_get(object_index);
        if (object->definition_index != -1) {
            const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
            long model_index = definition->model.index;
            if (model_index != -1) {
                const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', model_index);
                if (model->render_model_index != -1) {
                    const render_model_definition *render_model = (const render_model_definition *)tag_get('mode', model->render_model_index);
                    long marker_group_index = objects_find_marker_group_with_marker(render_model, marker_name, marker_index);
                    if (marker_group_index != -1) {
                        const render_model_marker *marker = &((const render_model_marker *)((const render_model_marker_group
                            *)render_model->marker_groups.address)[marker_group_index].markers.address)[marker_index];
                        matrix4x3_transform_point(objects_get_node_matrix(object_index, marker->node_index), &marker->translation, point);
                        result = true;
                    }
                }
            }
        }
    }
    return result;
}







static s_widget_type g_widget_types[] = {
    { 'ant!', 2, antennas_initialize, antennas_initialize_for_new_map,
      antennas_dispose_from_old_map, antennas_dispose, antenna_new, antenna_delete,
      0, (void (*)(float))antennas_update, antenna_render_debug, 0, 0,
      (void (*)(const void *, long))antenna_render_transparent, antenna_get_sorting_position, 0 },
    { 'clwd', 1, cloth_initialize, cloth_initialize_for_new_map,
      cloth_invalidate_all, cloth_dispose, cloth_new, cloth_delete,
      0, cloth_update, cloth_render_debug, cloth_render_opaque,
      cloth_get_is_transparent, cloth_render_transparent, cloth_get_sorting_position, 0 },
    { 'lswd', 4, leaf_system_initialize, leaf_system_initialize_for_new_map,
      leaf_system_dispose_from_old_map, leaf_system_dispose, leaf_system_new, leaf_system_delete,
      0, leaf_system_update, leaf_system_render_debug, leaf_system_render_opaque,
      leaf_system_get_is_transparent, 0, leaf_system_get_location, leaf_system_unattached_visible_render },
    { 'mffn', 0, muffins_initialize, muffins_initialize_data_for_new_map,
      muffins_dispose_data_from_old_map, muffins_dispose, muffins_new, muffins_delete,
      muffins_transfer, muffins_update, muffins_render_debug, muffins_render_opaque,
      muffins_get_is_transparent, 0, 0, 0 }
};
   // the light cluster partition
static __forceinline void objects_data_make_invalid(s_data_array *data)
{
    data->valid = false;
}
static __forceinline void objects_cluster_partition_make_invalid(cluster_partition *partition)
{
    if (partition->references->valid)
        objects_data_make_invalid(partition->references);
    if (partition->data->valid)
        objects_data_make_invalid(partition->data);
}
static __forceinline void release_object_header(long object_index)
{
    s_object_header *header = object_header_get(object_index);
    void *payload = header->datum;
    if (payload) {
        memory_pool_block_free(g_object_memory_pool, &payload);
        header->datum = 0;
    }
    ::datum_delete(object_header_data, object_index);
}
static __forceinline void objects_widgets_dispose_from_old_map()
{
    for (short type_index = 0; type_index < 4; type_index++) {
        s_widget_type *type = &g_widget_types[type_index];
        if (type->dispose_from_old_map)
            type->dispose_from_old_map();
    }
    objects_data_make_invalid(g_widget_data);
}
static __forceinline void objects_object_types_dispose_from_old_map()
{
    for (object_type_definition *definition = first_object_type_definition; definition; definition = definition->next) {
        if (definition->dispose_from_old_map)
            definition->dispose_from_old_map();
    }
}
static __forceinline void objects_lights_dispose_from_old_map()
{
    objects_data_make_invalid((s_data_array *)light_data);
    objects_cluster_partition_make_invalid((cluster_partition *)&light_cluster_partition);
}
static __forceinline void objects_clear_all_object_render_data()
{
    for (long index = 0; index < g_object_render_data_count; index++)
        ((long *)g_object_render_data)[index] = -1;
}
static __forceinline void objects_recycling_dispose_from_old_map()
{
    memset(g_recycling_volumes, 0, sizeof(s_recycling_volumes));
    objects_data_make_invalid((s_data_array *)tls_recycling_groups());
    object_recycling_reset_volume_list();
}


// 0xe3c98
void objects_dispose_from_old_map()
{
    objects_widgets_dispose_from_old_map();
    objects_object_types_dispose_from_old_map();
    objects_lights_dispose_from_old_map();
    objects_clear_all_object_render_data();
    objects_recycling_dispose_from_old_map();
    if (object_header_data->valid) {
        for (long object_index = ::data_next_index(object_header_data, -1); object_index != -1;
            object_index = inl_data_next_index(object_header_data, object_index))
            release_object_header(object_index);
        objects_data_make_invalid(object_header_data);
    }
    objects_cluster_partition_make_invalid(&tls_collideable_object_cluster_partition());
    objects_cluster_partition_make_invalid(&tls_noncollideable_object_cluster_partition());
}

// 0x3179f0
static __forceinline void objects_widgets_dispose()
{
    for (short type_index = 0; type_index < 4; type_index++) {
        s_widget_type *type = &g_widget_types[type_index];
        if (type->dispose)
            type->dispose();
    }
}
static __forceinline void objects_object_types_dispose()
{
    for (object_type_definition *definition = first_object_type_definition; definition; definition = definition->next) {
        if (definition->dispose)
            definition->dispose();
    }
}
static __forceinline void objects_cluster_partition_delete(cluster_partition *partition)
{
    if (partition->cluster_heads)
        partition->cluster_heads = NULL;
    if (partition->references)
        partition->references = NULL;
    if (partition->data)
        partition->data = NULL;
}

// 0x317b88
static __forceinline void objects_widgets_initialize_for_new_map()
{
    data_make_valid(g_widget_data);
    for (short type_index = 0; type_index < 4; type_index++) {
        s_widget_type *type = &g_widget_types[type_index];
        if (type->initialize_for_new_map)
            type->initialize_for_new_map();
    }
}
static __forceinline void objects_object_types_initialize_for_new_map()
{
    for (object_type_definition *definition = first_object_type_definition; definition; definition = definition->next) {
        if (definition->initialize_for_new_map)
            definition->initialize_for_new_map();
    }
}
static __forceinline void objects_lights_initialize_for_new_map()
{
    lights_game_globals_definition *light_globals;
    data_make_valid((s_data_array *)light_data);
    ((lights_game_globals_definition *)lights_game_globals)->enabled = true;
    cluster_partition_make_valid((cluster_partition *)&light_cluster_partition);
    light_globals = (lights_game_globals_definition *)lights_game_globals;
    light_globals->can_use_flashlights = true;
    light_globals->can_use_flashlights_time = 0;
    memset(light_globals->enabled_lights, 0, sizeof(light_globals->enabled_lights));
}
static __forceinline void objects_recycling_initialize_for_new_map()
{
    memset(g_recycling_volumes, 0, sizeof(s_recycling_volumes));
    data_make_valid((s_data_array *)tls_recycling_groups());
    object_recycling_reset_volume_list();
}
void objects_initialize_for_new_map()
{
    memset(object_globals, 0, sizeof(s_object_globals));
    memset(g_tracked_object_globals, 0, sizeof(*g_tracked_object_globals));
    objects_widgets_initialize_for_new_map();
    objects_object_types_initialize_for_new_map();
    objects_lights_initialize_for_new_map();
    objects_recycling_initialize_for_new_map();
    data_make_valid(object_header_data);
    memset(g_object_name_list, -1, sizeof(long) * 2048);
    cluster_partition_make_valid(&tls_collideable_object_cluster_partition());
    cluster_partition_make_valid(&tls_noncollideable_object_cluster_partition());
    object_globals->objects_updating = false;
    object_globals->first_garbage_object_index = -1;
    object_globals->recyclable_garbage_object_count = 0;
    object_globals->active_garbage_object_count = 0;
    object_globals->last_garbage_fail_time = 0;
    object_globals->last_loud_weapon_fire_time = 0;
    object_globals->custom_animations_prevent_lipsync_head_movement = false;
    object_globals->custom_animations_hold_on_last_frame = false;
    object_globals->object_identifier_counter = 0;
    object_globals->current_object_update_absolute_index = -1;
    long none = -1;
    long *object_indices = object_globals->object_indices;
    for (long index = 0; index < NUMBEROF(object_globals->object_indices); index++)
        object_indices[index] = none;
    long none_by_type = -1;
    long *first_name_index = object_globals->first_name_index;
    for (long index = 0; index < NUMBEROF(object_globals->first_name_index); index++)
        first_name_index[index] = none_by_type;
    scenario *scenario = global_scenario;
    long first_index = 0;
    for (long object_type = 0; object_type < 14; object_type++) {
        object_type_definition *definition = object_type_definitions[object_type];
        if (definition->scenario_datums_offset != -1 && definition->scenario_palette_offset != -1) {
            long datum_size;
            s_tag_block *datums = scenario_get_object_type_scenario_datums(scenario, (e_object_type)object_type, &datum_size);
            object_globals->first_name_index[object_type] = first_index;
            first_index += datums->count;
        }
    }
}

void __tls_set_g_object_header_data_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&actor_data, (s_data_array *)object);
}

// 0x31cb70 object_visible_to_any_player: an object in a potentially visible cluster that is near (zoom scaled) a
// player's unit head or inside its view cone



static __forceinline float point_distance_squared3d(const real_point3d *a, const real_point3d *b)
{
    float dx = b->x - a->x;
    float dy = b->y - a->y;
    float dz = b->z - a->z;
    return dx * dx + dy * dy + dz * dz;
}
static __forceinline void get_unit_head_marker_position(long unit_index, real_point3d *position)
{
    object_marker marker;
    internal_object_get_markers_by_string_id(unit_index, _string_id_head, &marker, 1, false);
    *position = marker.matrix.position;
}


static const float objects_visibility_distances[2] = { 40.0f, 1.2f };
bool object_visible_to_any_player(long object_index)
{
    bool in_visible_cluster = false;
    bool result = false;
    long root_index = inl_object_get_ultimate_parent(object_index);
    s_object_header *root_header = object_header_get(root_index);
    object_datum *root = ((object_datum *)object_get_and_verify_type(root_index, -1));
    if ((root_header->flags & 1) && (bool)((root->flags >> 8) & 1) && !(bool)((root->flags >> 18) & 1)) {
        const s_game_cluster_bit_vectors *cluster_pvs = (const s_game_cluster_bit_vectors *)&game_globals->cluster_pvs;
        s_cluster_reference_iterator iterator;
        for (s_cluster_reference cluster = objects_cluster_reference_get_first(&iterator, root_index);
            cluster.bsp_index != -1;
            cluster = objects_cluster_reference_get_next(&iterator, root_index)) {
            if (game_clusters_test(cluster_pvs, cluster)) {
                in_visible_cluster = true;
                break;
            }
        }
    }
    if (in_visible_cluster) {
        object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
        float bounding_radius = object->bounding_radius;
        float radius_squared = bounding_radius * bounding_radius;
        const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
        float view_distance_squared;
        if (definition->model.index != -1) {
            const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', definition->model.index);
            if (model->fade.disappear_distance > 0.0f)
            {
                float view_distance = objects_visibility_distances[1] * model->fade.disappear_distance;
                view_distance_squared = view_distance * (objects_visibility_distances[1] * model->fade.disappear_distance);
            }
            else
                view_distance_squared = objects_visibility_distances[0] * objects_visibility_distances[0];
        } else {
            view_distance_squared = 0.0f;
        }
        for (long player_index = ::data_next_index(player_data, -1); player_index != -1;
            player_index = inl_data_next_index(player_data, player_index))
        {
            player_datum *player = datum_get<player_datum>(player_data, player_index);
            if (player->unit_index != -1) {
                real_point3d head_position;
                get_unit_head_marker_position(player->unit_index, &head_position);
                float distance_squared = point_distance_squared3d(&head_position, &object->bounding_center);
                float zoom = ::unit_get_zoom_magnification(player->unit_index, ((unit_datum *)object_get(player->unit_index))->unit.zoom_level);
                if (zoom > 0.0001f)
                    distance_squared = distance_squared / (zoom * zoom);
                if (distance_squared < radius_squared) {
                    result = true;
                    break;
                } else if (distance_squared < view_distance_squared) {
                    float angle = 0.7853982f;
                    real_vector3d direction;
                    vector_from_points3d(&head_position, &object->bounding_center, &direction);
                    float distance = normalize3d(&direction);
                    float radius = object->bounding_radius;
                    float dot = dot_product3d(&((unit_datum *)object_get(player->unit_index))->unit.desired_aiming_vector, &direction);
                    if (dot > (float)cos((double)(angle + arctangent(radius, distance)))) {
                        result = true;
                        break;
                    }
                }
            }
        }
    }
    return result;
}

// 0x3614c8: an object type's compute_activation callback: active when a cluster of the object is in the given set
bool object_compute_activation(long object_index, const s_game_cluster_bit_vectors *cluster_bit_vectors, bool *active)
{
    s_scenery_object *object = (s_scenery_object *)object_get(object_index);
    bool result = false;
    if (((long)object->flags) & 4) {
        s_cluster_reference_iterator iterator;
        for (s_cluster_reference cluster = objects_cluster_reference_get_first(&iterator, object_index);
            cluster.bsp_index != -1;
            cluster = objects_cluster_reference_get_next(&iterator, object_index)) {
            if (game_clusters_test(cluster_bit_vectors, cluster)) {
                result = true;
                *active = true;
                break;
            }
        }
    }
    return result;
}

// 0x5779a8: the same callback of a type without flags
bool object_compute_activation_unconditional(long object_index, const s_game_cluster_bit_vectors *cluster_bit_vectors, bool *active)
{
    bool result = false;
    s_cluster_reference_iterator iterator;
    for (s_cluster_reference cluster = objects_cluster_reference_get_first(&iterator, object_index);
        cluster.bsp_index != -1;
        cluster = objects_cluster_reference_iterator_next(&iterator)) {
        if (game_clusters_test(cluster_bit_vectors, cluster)) {
            result = true;
            *active = true;
            break;
        }
    }
    return result;
}

// 0xe2b70
#define objects_real_epsilon 0.0001f



void object_set_velocities(long object_index, const real_vector3d *linear_velocity, const real_vector3d *angular_velocity)
{
    object_set_velocities_internal(object_index, linear_velocity, angular_velocity, false);
    havok_object_set_velocities(object_index, linear_velocity, angular_velocity, true);
    if ((linear_velocity && magnitude_squared3d(linear_velocity) > objects_real_epsilon * objects_real_epsilon) ||
        (angular_velocity && magnitude_squared3d(angular_velocity) > objects_real_epsilon * objects_real_epsilon)) {
        object_set_at_rest(object_index, false);
        object_wake(object_index);
        object_set_requires_motion(object_index);
    }
}

// 0xe2c20
void object_wake(long object_index)
{
    s_object_header *header = object_header_get(object_index);
    if (!(header->flags & 2)) {
        while (object_index != -1) {
            header = object_header_get(object_index);
            header->flags |= 2;
            object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
            object_index = object->parent_object_index;
        }
    }
}

static __forceinline bool objects_render_object_has_lightmap_shadow(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    bool result = false;
    if (!(bool)((object->flags >> 16) & 1)) {
        const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
        if (!(definition->flags & 1)) {
            long object_type = definition->object_type;
            unsigned long lightmap_shadow_mode = definition->lightmap_shadow_mode;
            if (lightmap_shadow_mode >= 1) {
                if (lightmap_shadow_mode != 1)
                    result = object_type != 5 && object_type != 3 && object_type != 9;
            } else {
                result = object_type == 0 || object_type == 1 || object_type == 2;
            }
        }
    }
    return result;
}
// 0xe2f78

void object_set_at_rest(long object_index, bool at_rest)
{
    long object_type = object_header_get(object_index)->object_type;
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    if (((1 << object_type) & 0x1cdf) && (bool)((object->physics_flags >> 7) & 1) && object->havok_component_index != -1) {
        if (!at_rest) {
            havok_component_activate_rigid_bodies(objects_havok_component_get(object->havok_component_index), 0);
            objects_physics_flags_set(&object->physics_flags, 9, false);
        }
    } else {
        objects_physics_flags_set(&object->physics_flags, 9, at_rest);
    }
    if (!at_rest)
        object_wake(object_index);
    if ((1 << object_type) & 12)
        ::simulation_action_object_update(object_index, 0x4000);
    else if ((1 << object_type) & 32)
        ::simulation_action_object_update(object_index, 0x4000);
}

// 0xe3730
// match: 0xe3730: restrict on the payload out-pointer prevents alias reloads across tag lookups
void cluster_build_object_payload(long object_index, s_object_cluster_payload *__restrict payload)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    unsigned short cull_flags = 0;
    if ((bool)((object->flags >> 9) & 1))
        cull_flags = collision_compute_object_cull_flags(object_index);
    bool has_render_model = false;
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    if (definition->model.index != -1) {
        const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', definition->model.index);
        has_render_model = model->render_model_index + 1 != 0;
    }
    unsigned char flags = objects_render_object_has_lightmap_shadow(object_index) ? 0 : 2;
    if (has_render_model && !object_is_hidden(object_index) && object->identifier_type != 9)
        flags &= ~1;
    else
        flags |= 1;
    if (object->identifier_type == 0 && (bool)((object->damage_flags >> 2) & 1))
        flags |= 4;
    else
        flags &= ~4;
    if (!object_is_hidden(object_index) && object->identifier_type != 9) {
        flags |= 8;
        payload->bounding_sphere_radius = object->bounding_sphere_radius;
    } else {
        flags &= ~8;
        payload->bounding_sphere_radius = object->bounding_sphere_radius;
    }
    payload->object_type = object->identifier_type;
    payload->cull_flags = cull_flags;
    payload->flags = flags;
    payload->bounding_sphere_center = object->bounding_sphere_center;
}

// 0xe3258
void object_compute_node_matrices(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    object_compute_node_matrices_non_recursive(object_index);
    object_compute_bounding_sphere(object_index);
    for (long child_index = object->first_child_object_index; child_index != -1; ) {
        object_datum *child_object = ((object_datum *)object_get_and_verify_type(child_index, -1));
        if (!(bool)((child_object->physics_flags >> 4) & 1) && !((1 << child_object->identifier_type) & 0x1080))
            object_compute_node_matrices(child_index);
        child_index = child_object->next_object_index;
    }
}

// 0xe32f8
extern "C" void object_reset_after_teleport(long object_index)
{
    object_set_velocities(object_index, global_zero_vector3d, global_zero_vector3d);
    object_set_at_rest(object_index, false);
    havok_component_release(object_index, false);
    object_type_definition *definition = object_type_definitions[object_get(object_index)->identifier_type];
    for (short part_index = 0; definition->parts[part_index] != 0; part_index++) {
        void (*update_part)(long) = definition->parts[part_index]->update_part;
        if (update_part != 0)
            update_part(object_index);
    }
    ::havok_component_aquire(object_index, false, false);
}

// 0xfc838

void object_update_collision_culling(long object_index)
{
    s_object_header *header = object_header_get(object_index);
    object_datum *object = header->datum;
    if ((bool)((header->flags >> 6) & 1)) {
        if ((bool)((object->flags >> 9) & 1)) {
            s_object_cluster_payload payload;
            cluster_build_object_payload(object_index, &payload);
            object_broadphase_set_handle_data(object_index, (const s_object_broadphase_handle_data *)&payload);
            cluster_partition_update_payload(&tls_collideable_object_cluster_partition(), object_index, object->first_cluster_reference,
                sizeof(payload), &payload);
        }
    }
    object_update_visibility_culling(object_index);
}

// 0xfc8c8


void object_update_visibility_culling(long object_index)
{
    s_object_header *header = object_header_get(object_index);
    object_datum *object = header->datum;
    if ((bool)((header->flags >> 6) & 1)) {
        s_object_cluster_payload payload;
        cluster_build_object_payload(object_index, &payload);
        if ((bool)((object->flags >> 9) & 1)) {
            object_broadphase_set_handle_data(object_index, (const s_object_broadphase_handle_data *)&payload);
            cluster_partition_update_payload(&tls_collideable_object_cluster_partition(), object_index, object->first_cluster_reference,
                sizeof(payload), &payload);
        } else {
            cluster_partition_update_payload(&tls_noncollideable_object_cluster_partition(), object_index, object->first_cluster_reference,
                sizeof(payload), &payload);
        }
    }
}

// 0xe3070
void object_set_requires_motion(long object_index)
{
    s_object_header *header = object_header_get(object_index);
    if (header->flags & 0x80) {
        object_index = inl_object_get_ultimate_parent(object_index);
        header = object_header_get(object_index);
    }
    if (header->flags & 1) {
        object_wake(object_index);
        header->flags |= 4;
    }
    recursive_wake_children_awoken_by_movement(object_index);
}

bool object_header_block_allocate(long object_index, short block_reference_offset, short size, short alignment_bits)
{
  bool result = false;
  s_object_header *header = object_header_get(object_index);
  if (size == 0) {
    s_object_block_reference *block = (s_object_block_reference *)((char *)object_get(object_index) + block_reference_offset);
    block->offset = -1;
    block->size = 0;
    result = true;
  } else {
    int alignment_mask = (1 << alignment_bits) - 1;
    int padded_size = size + alignment_mask;
    { int lift_a0 = header->handle; int lift_a1 = header->data_size + padded_size;
      header->handle = memory_pool_block_reallocate_handle((char *)g_object_memory_pool, lift_a0, lift_a1, 0, 0); }
    if (header->handle) {
      int offset = header->data_size;
      header->datum = (object_datum *)heap_pointer(g_object_memory_pool, header->handle);
      header->data_size += padded_size;
      s_object_block_reference *block = (s_object_block_reference *)((char *)object_get(object_index) + block_reference_offset);
      block->offset = (offset + alignment_mask) & ~alignment_mask;
      block->size = size;
      memset((char *)header->datum + offset, 0, padded_size);
      result = true;
    } else {
      s_object_block_reference *block = (s_object_block_reference *)((char *)object_get(object_index) + block_reference_offset);
      block->offset = -1;
      block->size = 0;
    }
  }
  return result;
}

long objects_compact_memory_pool(void)
{
  long free_size_before = g_object_memory_pool->size - heap_used(g_object_memory_pool);
  ::memory_pool_compact(g_object_memory_pool);
  return g_object_memory_pool->size - heap_used(g_object_memory_pool) - free_size_before;
}


// 0xe66f8
void object_adjust_node_orientations_for_facing_change(long object_index, const real_vector3d *old_facing_vector, const real_vector3d *new_facing_vector)
{
    if (fabsf(old_facing_vector->i - new_facing_vector->i) >= 0.0001f ||
        fabsf(old_facing_vector->j - new_facing_vector->j) >= 0.0001f ||
        fabsf(old_facing_vector->k - new_facing_vector->k) >= 0.0001f) {
        object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
        real_quaternion rotation;
        quaternion_from_unit_vectors(&rotation, new_facing_vector, old_facing_vector);
        long node_count;
        real_orientation *node_orientations = (real_orientation *)objects_object_header_block_get_with_count(
            object, &object->node_orientations, sizeof(real_orientation), &node_count);
        if (node_count > 0)
            quaternions_multiply(&rotation, &node_orientations->rotation, &node_orientations->rotation);
        if (object_can_interpolate(object_index)) {
            long original_node_count;
            real_orientation *original_node_orientations = (real_orientation *)objects_object_header_block_get_with_count(
                object, &object->original_node_orientations, sizeof(real_orientation), &original_node_count);
            if (original_node_count > 0)
                quaternions_multiply(&rotation, &original_node_orientations->rotation, &original_node_orientations->rotation);
        }
    }
}

// 0xe4460


static __forceinline float objects_signed_angular_difference(float a, float b)
{
    float difference = b - a;
    if (difference >= biped_half_turn_angle())
        difference -= biped_full_turn_angle();
    if (difference <= -biped_half_turn_angle())
        difference += biped_full_turn_angle();
    return difference;
}

const real k_objects_function_value_maximum = 1.0f;
const real k_objects_function_value_two_pi = 6.2831855f;
#define k_objects_function_value_elapsed_delay 15.0f

static __forceinline float objects_function_value_elapsed()
{
    float result = k_objects_function_value_maximum;
    if (object_globals->last_loud_weapon_fire_time != 0 && game_time_get() > object_globals->last_loud_weapon_fire_time) {
        float delay = k_objects_function_value_elapsed_delay;
        float maximum = k_objects_function_value_maximum;
        float elapsed = game_ticks_to_seconds((float)(game_time_get() - object_globals->last_loud_weapon_fire_time));
        if (elapsed > maximum) {
            result = 0.0f;
        } else {

            if (elapsed > 20.0f) {
                result = k_objects_function_value_maximum;
            } else {
                float difference = elapsed - delay;
                result = difference / 5.0f;
            }
        }
    }
    return result;
}
static __forceinline float objects_function_value_hour()
{
    s_date_and_time date_and_time;
    game_time_get_date_and_time(&date_and_time);
    long hour = (date_and_time.hour > 1 ? date_and_time.hour : 1) < 24 ? (date_and_time.hour > 1 ? date_and_time.hour : 1) : 24;
    return (float)(hour - hour / 12 * 12) / 12.0f;
}
static __forceinline float objects_function_value_minute()
{
    s_date_and_time date_and_time;
    game_time_get_date_and_time(&date_and_time);
    long minute = (date_and_time.minute > 0 ? date_and_time.minute : 0) < 59 ? (date_and_time.minute > 0 ? date_and_time.minute : 0) : 59;
    return (float)minute / 60.0f;
}
static __forceinline float objects_function_value_second()
{
    s_date_and_time date_and_time;
    game_time_get_date_and_time(&date_and_time);
    long second = (date_and_time.second > 0 ? date_and_time.second : 0) < 59 ? (date_and_time.second > 0 ? date_and_time.second : 0) : 59;
    return (float)second / 60.0f;
}
static __forceinline float objects_game_flashlight_magnitude()
{
    float time = game_ticks_to_seconds((float)(game_time_get() - ((lights_game_globals_definition
        *)lights_game_globals)->can_use_flashlights_time));
    float magnitude = transition_function_evaluate(5, time < k_objects_function_value_maximum ? time : k_objects_function_value_maximum);
    if (!((lights_game_globals_definition *)lights_game_globals)->can_use_flashlights)
        magnitude = k_objects_function_value_maximum - magnitude;
    return magnitude;
}
static __forceinline float game_engine_compute_object_function(long object_index, long function_name)
{
    float result = 0.0f;
    const c_game_engine *engine = current_game_engine_entry();
    if (engine != 0)
        result = engine->compute_object_function(object_index, function_name);
    return result;
}
static __forceinline float game_engine_teleporter_cannot_send(long object_index)
{
    c_teleporter_area *area = teleporter_try_and_get_from_object_index(object_index);
    float result = 0.0f;
    if (area != 0 && teleporter_is_sender(area))
        result = k_objects_function_value_maximum;
    return result;
}
static __forceinline float game_engine_teleporter_recently_teleported(long object_index)
{
    c_teleporter_area *area = teleporter_try_and_get_from_object_index(object_index);
    float result = 0.0f;
    if (area != 0) {
        long recharge_timer = area->update_countdown;
        if (recharge_timer > 0) {
            long recharge_time = ::get_teleporter_recharge_time_ticks();
            if (recharge_time > 0)
                result = (float)recharge_timer / (float)recharge_time;
        }
    }
    return result;
}
bool object_compute_function_value(long object_index, long function_name, long argument, float *value, bool *active, bool *deterministic)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    bool result = true;
    float function_value = 0.0f;
    switch (function_name) {
    case _string_id_current_body_damage:
        function_value = object->current_body_damage;
        break;
    case _string_id_current_shield_damage:
        function_value = object->current_shield_damage;
        break;
    case _string_id_body_vitality:
        function_value = object->body_vitality;
        break;
    case _string_id_shield_vitality:
        function_value = object->shield_vitality;
        break;
    case _string_id_alive:
        function_value = objects_flags_test_ptr(&object->damage_flags, 2) ? 0.0f : k_objects_function_value_maximum;
        break;
    case _string_id_compass: {
        real_matrix4x3 *matrix = object_get_node_matrix(object_index, 0);
        if (fabsf(matrix->forward.k) < 0.995f) {
            float angle = arctangent(matrix->forward.i, matrix->forward.j);
            float difference = objects_signed_angular_difference(global_scenario->local_north, angle);
            if (difference < 0.0f)
                difference += k_objects_function_value_two_pi;
            function_value = difference / 6.2831855f;
            if (function_value <= 0.0f)
                function_value = k_objects_function_value_maximum;
        } else {
            function_value = 0.0f;
        }
        break;
    }
    case _string_id_variant: {
        const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
        if (definition->model.index != -1) {
            const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', definition->model.index);
            if (model->variants.count > 0)
                function_value = (float)(((object->variant_index + 1 > 0 ? object->variant_index + 1 : 0) < model->variants.count
                                         ? (object->variant_index + 1 > 0 ? object->variant_index + 1 : 0)
                                         : model->variants.count)) /
                    (float)model->variants.count;
        }
        break;
    }
    case _string_id_object_overshield_amount:
        function_value = object->shield_vitality - k_objects_function_value_maximum;
        break;
    case _string_id_one:
        function_value = k_objects_function_value_maximum;
        break;
    case _string_id_zero:
        function_value = 0.0f;
        break;
    case _string_id_shield_depleted: {
        float maximum_shield_vitality = object->maximum_shield_vitality;
        float recharge_rate = 1.0f;
        if (game_globals->options.game_mode == 2 && ((1 << object->identifier_type) & 4099) != 0 && ((unit_datum *)object)->unit.player_index != -1) {
            const player_datum *player =
                datum_get<player_datum>(player_data, ((unit_datum *)object)->unit.player_index);
            const c_player_trait_shield_vitality *traits = &player->traits.m_shield_vitality_traits;
            maximum_shield_vitality = traits->get_maximum_shield_vitality();
            recharge_rate = traits->get_shield_recharge_rate_percentage();
        }
        bool shield_recharging;
        if (recharge_rate > 0.0f) shield_recharging = true;
        else shield_recharging = false;
        if (maximum_shield_vitality > 0.0f && shield_recharging) {
            function_value = object->shield_vitality == 0.0f ? k_objects_function_value_maximum : 0.0f;
        } else {
            function_value = 0.0f;
        }
        break;
    }
    case _string_id_random_constant: {
        unsigned long random_seed = (unsigned long)object_index;
        function_value = _real_random_range(&random_seed, 0.0f, 1.0f);
        break;
    }
    case _string_id_electrical_power:
        function_value = k_objects_function_value_maximum;
        break;
    case _string_id_all_quiet:
        function_value = objects_function_value_elapsed();
        break;
    case _string_id_time_hours:
        function_value = objects_function_value_hour();
        *deterministic = false;
        break;
    case _string_id_time_minutes:
        function_value = objects_function_value_minute();
        *deterministic = false;
        break;
    case _string_id_time_seconds:
        function_value = objects_function_value_second();
        *deterministic = false;
        break;
    case _string_id_phantom_power:
        function_value = (float)object->phantom_power / 255.0f;
        break;
    case _string_id_phantom_speed:
        function_value = (float)object->phantom_speed / 255.0f;
        break;
    case _string_id_phantom_on:
        function_value = objects_flags_test_ptr(&object->physics_flags, 17) ? k_objects_function_value_maximum : 0.0f;
        break;
    case _string_id_jetpack_exhaust:
        function_value = ((1 << object->identifier_type) & 1) != 0
            ? (biped_get_scale(object_index) < k_objects_function_value_maximum ? k_objects_function_value_maximum : 0.0f)
            : 0.0f;
        break;
    case _string_id_morse_code:
        function_value = k_objects_function_value_maximum;
        break;
    case _string_id_flashlights_allowed:
        function_value = objects_game_flashlight_magnitude();
        break;
    case _string_id_holiday:
        function_value = game_time_get_holiday();
        *deterministic = false;
        break;
    case _string_id_scripted_object_function_a:
        function_value = object_globals->shadowless_values[0];
        break;
    case _string_id_scripted_object_function_b:
        function_value = object_globals->shadowless_values[1];
        break;
    case _string_id_scripted_object_function_c:
        function_value = object_globals->shadowless_values[2];
        break;
    case _string_id_scripted_object_function_d:
        function_value = object_globals->shadowless_values[3];
        break;
    case _string_id_oddball_effect:
    case _string_id_mp_goal_active:
        function_value = game_engine_compute_object_function(object_index, function_name);
        break;
    case _string_id_teleporter_active:
        function_value = ((1 << object->identifier_type) & 1088) != 0 ? teleporter_is_channel_linked(object_index) : 0.0f;
        break;
    case _string_id_teleporter_blocked:
        function_value = 0.0f;
        break;
    case _string_id_teleporter_cannot_send:
        function_value = ((1 << object->identifier_type) & 1088) != 0 ? game_engine_teleporter_cannot_send(object_index) : 0.0f;
        break;
    case _string_id_teleporter_sent:
        function_value = ((1 << object->identifier_type) & 1088) != 0 ? game_engine_teleporter_recently_teleported(object_index) : 0.0f;
        break;
    default:
        result = false;
        break;
    }
    function_value = (function_value > 0.0f ? function_value : 0.0f) < k_objects_function_value_maximum
        ? (function_value > 0.0f ? function_value : 0.0f)
        : k_objects_function_value_maximum;
    *value = function_value;
    *active = function_value > 0.0f;
    return result;
}

// 0xe9b88
bool unit_compute_function_value(long unit_index, long function_name, long argument, float *value, bool *active, bool *deterministic)
{
    unit_datum *unit = (unit_datum *)((object_datum *)object_get_and_verify_type(unit_index, 8195));
    float function_value = 0.0f;
    bool result = true;
    switch (function_name) {
    case _string_id_crouch:
        function_value = unit->unit.crouch_amount;
        break;
    case _string_id_active_camouflage:
        function_value = unit->unit.active_camouflage;
        break;
    case _string_id_emp_disabled:
        function_value = unit->unit.stun_ticks ? 1.0f : 0.0f;
        break;
    case _string_id_boost:
        function_value = ::unit_compute_boost_fraction(unit_index);
        break;
    case _string_id_driver_seat_power: {
        const signed char acceleration_flags = unit->unit.seat_acceleration_mask;
        if (acceleration_flags & 1)
            function_value = unit->unit.seat_acceleration_fractions[0];
        else
            result = false;
        break;
    }
    case _string_id_gunner_seat_power: {
        const signed char acceleration_flags = unit->unit.seat_acceleration_mask;
        if (acceleration_flags & 2)
            function_value = unit->unit.seat_acceleration_fractions[1];
        else
            result = false;
        break;
    }
    case _string_id_aiming_change:
        function_value = (float)unit->unit.aiming_change / 255.0f;
        break;
    case _string_id_horizontal_aiming_change:
        function_value = (float)unit->unit.looking_change / 255.0f;
        break;
    case _string_id_mouth_aperture:
        function_value = unit->unit.mouth_aperture;
        break;
    case _string_id_integrated_light_power:
        function_value = unit->unit.vision_mode_fraction;
        break;
    case _string_id_can_blink:
        function_value = !(bool)((unit->object.damage_flags >> 2) & 1) && !(bool)((unit->unit.unit_flags >> 17) & 1) ? 1.0f : 0.0f;
        break;
    case _string_id_driver_seat_occupied:
        function_value = unit->unit.driver_object_index == -1 && !(bool)((unit->unit.unit_flags >> 1) & 1) ? 0.0f : 1.0f;
        break;
    case _string_id_gunner_seat_occupied:
        function_value = unit->unit.gunner_object_index != -1 ? 1.0f : 0.0f;
        break;
    case _string_id_unit_open:
        function_value = unit_get_boarding_progress(unit_index);
        break;
    case _string_id_unit_closed:
        function_value = 1.0f - unit_get_boarding_progress(unit_index);
        break;
    case _string_id_vertical_thrust:
        function_value = unit->unit.throttle.k;
        break;
    default:
        result = false;
        break;
    }
    if (result) {
        function_value = (function_value > 0.0f ? function_value : 0.0f) < 1.0f ? (function_value > 0.0f ? function_value : 0.0f) : 1.0f;
        *value = function_value;
        *active = function_value > 0.0f;
    } else {
        long player_index = object_get_player_index(unit_index);
        if (player_index != -1)
            result = player_function_get_function_value(player_index, function_name, value, active);
        else if (game_globals->options.game_mode == 3) {
            switch (function_name) {
            case _string_id_emblem_active: function_value = 1.0f; result = true; break;
            case _string_id_emblem_foreground_index: function_value = (float)g_emblem_render_info.foreground_emblem; result = true; break;
            case _string_id_emblem_background_index: function_value = (float)g_emblem_render_info.background_emblem; result = true; break;
            case _string_id_emblem_alternate_foreground_enabled: function_value = (g_emblem_render_info.flags & 1) ? 0.0f : 1.0f; result = true; break;
            case _string_id_emblem_flip_foreground_image: function_value = (g_emblem_render_info.flags & 2) ? 1.0f : 0.0f; result = true; break;
            case _string_id_emblem_flip_background_image: function_value = (g_emblem_render_info.flags & 4) ? 1.0f : 0.0f; result = true; break;
            case _string_id_emblem_primary_color_index: function_value = (float)g_emblem_render_info.primary_color; result = true; break;
            case _string_id_emblem_secondary_color_index: function_value = (float)g_emblem_render_info.secondary_color; result = true; break;
            case _string_id_emblem_background_color_index: function_value = (float)g_emblem_render_info.background_color; result = true; break;
            }
            if (result) {
                *value = function_value;
                *active = true;
            }
        }
        if (!result) {
            long weapon_function = 0;
            switch (function_name) {
            case _string_id_weapon_illumination: weapon_function = _string_id_illumination; break;
            case _string_id_barrel_spin: weapon_function = _string_id_barrel_spin; break;
            case _string_id_barrel_spin_rate: weapon_function = _string_id_barrel_spin_rate; break;
            }
            if (weapon_function) {
                long weapon_index = -1;
                if (unit->unit.current_weapon_index != -1)
                    weapon_index = unit->unit.weapon_object_indices[unit->unit.current_weapon_index];
                if (weapon_index != -1)
                    result = ::weapon_compute_function_value(
                        weapon_index, weapon_function, argument, (float *)(value), (bool *)(active), (bool *)(deterministic));
            }
        }
    }
    return result;
}

// 0xfcaa0
void object_set_garbage(long object_index, bool garbage, long garbage_time)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    if (garbage) {
        if (!objects_recycling_flags_test(&object->recycling_flags, 1) && !objects_recycling_flags_test(&object->recycling_flags, 0)) {
            s_object_header *header = object_header_get(object_index);
            object->next_garbage_object_index = object_globals->first_garbage_object_index;
            object_globals->first_garbage_object_index = object_index;
            objects_recycling_flags_set(&object->recycling_flags, 1, true);
            if ((header->flags & 1) && !(header->flags & 0x80) && !objects_recycling_flags_test(&object->recycling_flags, 2)) {
                object_globals->recyclable_garbage_object_count++;
                objects_recycling_flags_set(&object->recycling_flags, 2, true);
            }
            object->parent_recycling_group = -1;
            object->next_recycling_group_member = -1;
        }
        object->garbage_time = object->garbage_time > game_time_get() + garbage_time ? object->garbage_time : game_time_get() +
            garbage_time;
    } else if (objects_recycling_flags_test(&object->recycling_flags, 1)) {
        long *object_index_reference = &object_globals->first_garbage_object_index;
        while (*object_index_reference != object_index)
            object_index_reference = &((object_datum *)object_get_and_verify_type(*object_index_reference, -1))->next_garbage_object_index;
        *object_index_reference = object->next_garbage_object_index;
        object->next_garbage_object_index = -1;
        if (objects_recycling_flags_test(&object->recycling_flags, 2)) {
            object_globals->recyclable_garbage_object_count--;
            objects_recycling_flags_set(&object->recycling_flags, 2, false);
        }
        objects_recycling_flags_set(&object->recycling_flags, 1, false);
        object_recycling_group_remove_object(object_index);
    }
}

// 0x241850
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_light_globals_allocator>::deallocate(void *address)
{
    g_cinematic_light_globals = 0;
    restricted_region_free_member(3, m_member_index);
}

// @address 0x7f1954 object_memory_non_gamestate
bool object_memory_non_gamestate;

// 0x3179f0
void objects_dispose()
{
    objects_widgets_dispose();
    objects_object_types_dispose();
    objects_cluster_partition_delete((cluster_partition *)&light_cluster_partition);
    if (object_memory_non_gamestate) {
        ::memory_pool_dispose(g_object_memory_pool);
        data_dispose(object_header_data);
    }
    object_header_data = NULL;
    g_object_memory_pool = NULL;
    objects_clear_all_object_render_data();
    objects_cluster_partition_delete(&tls_collideable_object_cluster_partition());
    objects_cluster_partition_delete(&tls_noncollideable_object_cluster_partition());
}

// 0x31abc8
void object_disconnect_from_map(long object_index, bool recursive)
{
    s_object_header *header = object_header_get(object_index);
    object_datum *object = header->datum;
    object_connect_lights_recursive(object_index, true, false, false, false);
    cluster_partition_disconnect((bool)((object->flags >> 9) & 1) ? &tls_collideable_object_cluster_partition() :
        &tls_noncollideable_object_cluster_partition(),
        object_index, &object->first_cluster_reference);
    object->flags &= ~(1 << 8);
    header->flags &= ~(1 << 6);
    if ((bool)((object->flags >> 9) & 1) && recursive)
        object_broadphase_remove_object(object_index);
}

// 0x31b8b0
void object_detach_from_node(long object_index, const real_matrix4x3 *node_matrix)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    real_matrix4x3 matrix;
    objects_matrix4x3_translation(&matrix, &object->position);
    real_matrix4x3 rotation;
    matrix4x3_rotation_from_vectors(&rotation, &object->forward, &object->up);
    real_matrix4x3 temp;
    matrix4x3_compute_product(node_matrix, &matrix, &temp);
    matrix4x3_compute_product(&temp, &rotation, &matrix);
    object_set_position_direct(object_index, &matrix.position, &matrix.forward, &matrix.up, NULL, false);
}

// 0x31bb18
void object_set_hidden(long object_index, bool hidden)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    if (hidden && !TEST_BIT(object->flags, 0)) {
        if (object_is_connected_to_map(object_index))
            object_connect_lights_recursive(object_index, true, false, false, false);
        objects_flags_set(&object->flags, 0, true);
        object_update_collision_culling(object_index);
    } else if (!hidden && TEST_BIT(object->flags, 0)) {
        objects_flags_set(&object->flags, 0, false);
        if (object_is_connected_to_map(object_index))
            object_connect_lights_recursive(object_index, false, true, false, false);
        object_update_collision_culling(object_index);
    }
}

// 0x31c388
bool object_function_get_function_value(long object_index, s_object_function_definition *function, long function_owner_definition_index,
    float *out_function_magnitude, bool *deterministic)
{
    if (object_scripting_get_function_variable(object_index, function->scripting_name, out_function_magnitude))
        return true;
    bool always_active = !(function->flags & 2);
    float value = 0.0f;
    float scale;
    bool scale_deterministic;
    bool periodic = ((const s_function_definition_internal *)(const void *)function->function.m_data.address)->type == 3;
    bool active = object_get_function_value(object_index, function->scripting_name, function_owner_definition_index, &value, deterministic);
    if (periodic) {
        float seconds = game_ticks_to_seconds((float)game_time_get());
        *deterministic = false;
        if (function->flags & 8) {
            unsigned long random_seed = (unsigned long)object_index;
            seconds += _real_random_range(&random_seed, 0.0f, 1.0f);
        }
        value = function->function.evaluate_scalar(seconds, 1.0f) * value;
    } else if (active || always_active) {
        if (function->flags & 1)
            value = 1.0f - value;
        value = function->function.evaluate_scalar(value, 1.0f);
        if (always_active)
            active = value > 0.0f;
    }
    if (function->scale_by != 0) {
        scale = 0.0f;
        scale_deterministic = false;
        bool scale_active = object_get_function_value(object_index, function->scale_by, function_owner_definition_index, &scale, &scale_deterministic);
        value = scale * value;
        *deterministic = *deterministic && scale_deterministic;
        active = active && scale_active;
    }
    if (active && function->min_value > 0.0f)
        active = value > function->min_value;
    if (function->flags & 4)
        active = true;
    if (function->turn_off_with != 0 && active) {
        float turn_off_value = 0.0f;
        bool turn_off_deterministic = false;
        bool turn_off_active = object_get_function_value(
            object_index, function->turn_off_with, function_owner_definition_index, &turn_off_value, &turn_off_deterministic);
        if (turn_off_active && (function->flags & 0x20))
            turn_off_active = turn_off_value != 0.0f;
        active = active && turn_off_active;
        *deterministic = *deterministic && turn_off_deterministic;
    }
    if (!active && !(function->flags & 0x10))
        value = 0.0f;
    else
        value = (value > 0.0f ? value : 0.0f) < 1.0f ? (value > 0.0f ? value : 0.0f) : 1.0f;
    *out_function_magnitude = value;
    return active;
}

// 0x320760
void handle_object_render_message(s_object_render_thread_message *object_render_thread_message, long parameter)
{
    long object_index = object_render_thread_message->object_index;
    if (object_try_and_get_and_verify_type(object_index, -1)) {
        switch (object_render_thread_message->type) {
        case 0:
            reset_object_cached_render_state(object_index);
            break;
        case 1:
            render_object_invalidate_render_state(object_index);
            break;
        case 2:
            render_object_update_change_colors(object_index, object_render_thread_message->unknown != 0);
            break;
        }
    }
}

// 0x320ba0
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_data_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x320c40
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_data_allocator>::deallocate(void *address)
{
    object_header_data = 0;
    restricted_region_free_member(3, m_member_index);
}

// ==== claim 019 (u-objectslibxbox-objects-26): object delete-path functions ====
// Callee declarations: accepted units define these under these names (KIT.md).

#define DATA_ITERATOR_MATCH_NEXT(it, t_datum) \
    (((it).m_datum = (t_datum *)data_iterator_next_with_flags(&(it).m_iterator, (it).m_flags_offset, (it).m_mask, (it).m_match)) != NULL)

// @address 0x1109a0 ?objects_purge_deleted_objects@@YAXXZ
void objects_purge_deleted_objects(void)
{
    c_data_iterator_match<s_object_header> iterator;
    iterator.begin(object_header_data, 2, 16, 16);
    while (DATA_ITERATOR_MATCH_NEXT(iterator, s_object_header))
        objects_purge_deleted_object(iterator.get_index());
}

// @address 0x110a38 objects_purge_deleted_object
extern "C" void objects_purge_deleted_object(long object_index)
{
    long child_index = object_find_predicted_object(object_index);
    if (child_index != -1) {
        do {
            object_detach(child_index);
            child_index = object_find_predicted_object(object_index);
        } while (child_index != -1);
    }
    object_pre_delete_recursive(object_index, true);
    object_delete_recursive(object_index, true);
}




static __forceinline bool object_has_predicted_simulation_entity(long object_index)
{
    bool result = false;
    if (game_simulation::game_is_predicted())
    {
        object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, ~0ul));
        result = object->gamestate_index != -1;
    }
    return result;
}

static __forceinline bool should_delete_deactivated_object(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, ~0ul));
    bool result = false;
    if (!object_has_predicted_simulation_entity(object_index))
        result = (bool)((object->flags >> 17) & 1);
    return result;
}

// @address 0x31fda0 object_new_from_scenario_internal
static __forceinline long objects_object_name_get(short name)
{
    long result = -1;
    if (name >= 0 && name < 0x800)
        result = ((long *)g_object_name_list)[name];
    return result;
}

static __forceinline void objects_object_name_set(long object_index, short name)
{
    object_datum *object = object_get(object_index);
    if (((long *)g_object_name_list)[name] == -1) {
        ((long *)g_object_name_list)[name] = object_index;
        object->object_name_index = name;
    }
}

long object_new_from_scenario_internal(long object_type, short index, s_scenario_object *scenario_object, s_tag_block *palette,
    long definition_override, bool query_simulation, bool a, bool b)
{
    long object_index = -1;
    if (scenario_object->palette_index != -1 &&
        (scenario_object->name_index == -1 ||
         objects_object_name_get((short)scenario_object->name_index) == -1)) {
        object_placement_data placement;
        if (object_placement_data_new_from_scenario_object((e_object_type)object_type, index, scenario_object, palette, a, b, &placement)) {
            if (!query_simulation || simulation_query_object_placement(&placement)) {
                if (definition_override != -1)
                    placement.definition_index = definition_override;
                object_index = object_new(&placement);
                if (object_index != -1) {
                    object_type_place(object_index, scenario_object);
                    if (scenario_object->name_index != -1)
                        objects_object_name_set(object_index, (short)scenario_object->name_index);
                    if (!game_simulation::game_is_predicted() && game_globals->options.game_mode == 2 && game_engine_running())
                        c_map_variant::find_definition_index(object_index, -1, 1);
                    game_engine_register_object(object_index);
                    simulation_action_object_create(object_index);
                }
            }
        }
    }
    return object_index;
}

// @address 0x318270 ?objects_activation@@YAXPBUs_game_cluster_bit_vectors@@0@Z
void objects_activation(const s_game_cluster_bit_vectors *active_clusters, const s_game_cluster_bit_vectors *current_clusters)
{
    if (game_globals->options.game_mode != 2) {
        scenario *current_scenario = global_scenario;
        long bsp_index = global_structure_bsp_first_active_index_get();
        if (bsp_index != -1) {
            do {
                structure_bsp *bsp = global_structure_bsp_get(bsp_index);
                s_cluster_reference cluster_reference;
                for (long cluster_index = 0; cluster_index < bsp->clusters.count; cluster_index++) {
                    structure_cluster *cluster = &((structure_cluster *)bsp->clusters.address)[cluster_index];
                    cluster_reference_set(&cluster_reference, bsp_index, cluster_index);
                    if (!game_clusters_test(current_clusters, cluster_reference))
                        continue;
                    if (game_clusters_test(active_clusters, cluster_reference))
                        continue;
                    for (long reference_index = 0; reference_index < cluster->pvs_bound_object_references.count; reference_index++) {
                        const s_scenario_object_reference *reference = &((s_scenario_object_reference
                            *)cluster->pvs_bound_object_references.address)[reference_index];
                        long object_type = reference->type;
                        long first_name_index = object_globals->first_name_index[object_type];
                        if (first_name_index == -1 || object_globals->object_indices[(long)(short)reference->index + first_name_index] == -1) {
                            if (!objects_activation_find_scenario_reference_in_currently_active_clusters(active_clusters, current_clusters, reference)) {
                                object_type_definition *definition = object_type_definitions[object_type];
                                long datums_offset = (short)definition->scenario_datums_offset;
                                s_tag_block *palette = (s_tag_block *)((char *)current_scenario + (short)definition->scenario_palette_offset);
                                s_scenario_object *scenario_object =
                                    (s_scenario_object *)((char *)((s_tag_block *)((char *)current_scenario + datums_offset))->address +
                                        (short)definition->scenario_datum_size * (short)reference->index);
                                object_new_from_scenario_internal(object_type, reference->index, scenario_object, palette, -1, true, true, false);
                            }
                        }
                    }
                }
                bsp_index = global_structure_bsp_next_active_index_get(bsp_index);
            } while (bsp_index != -1);
        }
    }
    c_data_iterator_match<s_object_header> iterator;
    iterator.begin(object_header_data, 2, 64, 64);
    while (DATA_ITERATOR_MATCH_NEXT(iterator, s_object_header)) {
        long object_index = iterator.get_index();
        bool active = object_should_be_active(object_index, current_clusters);
        if ((iterator.m_datum->flags & 1) != 0) {
            if (!active) {
                if (should_delete_deactivated_object(object_index))
                    object_delete(object_index);
                else
                    ::object_deactivate(object_index, false);
            }
        }
        else if (active) {
            object_activate(object_index);
        }
    }
}

// @address 0x318760 ?object_deactivate@@YAXJ_N@Z
void object_deactivate(long object_index, bool deactivate_children)
{
    s_object_header *header = object_header_get(object_index);
    unsigned char flags = header->flags;
    if ((flags & 1) != 0) {
        object_datum *object = header->datum;
        const s_scenario_object *placement;
        if (object->name_index != -1)
            placement = scenario_object_try_and_get(global_scenario, (e_object_type)header->object_type, object->name_index);
        else
            placement = 0;
        header->flags &= ~1;
        header->flags &= ~4;
        if (!deactivate_children && havok_object_needs_physics(object_index, object->definition_index))
            object_reset(object_index);
        if ((object->recycling_flags & 4) != 0) {
            object_globals->recyclable_garbage_object_count--;
            object->recycling_flags &= ~4;
        }
        if (placement != 0 && !deactivate_children && (_rotl(placement->placement_flags, 22) & 1) != 0)
            object_delete(object_index);
    }
}

// @address 0x73cb08 ?object_deleted_procs@@3PAP6AXJ@ZA
// @address 0x31a730 ?object_pre_delete_recursive@@YAXJ_N@Z
void object_pre_delete_recursive(long object_index, bool recursive)
{
    object_datum *object = object_get(object_index);
    object_delete(object_index);
    long child_index = object->first_child_object_index;
    if (child_index != -1) {
        do {
            long next_index = object_get(child_index)->next_object_index;
            object_pre_delete_recursive(child_index, false);
            child_index = next_index;
        } while (child_index != -1);
    }
    unsigned long remaining = 11;
    void (**callback)(long) = object_deleted_procs;
    do {
        (*callback)(object_index);
        remaining--;
        callback++;
    } while (remaining != 0);
}

// widget views for 0x31a7d0 (layouts from tu/objectslibxbox/widgets.cpp)


// @address 0x31a7d0 ?object_delete_recursive@@YAXJ_N@Z
void object_delete_recursive(long object_index, bool recursive)
{
    object_datum *object = object_get(object_index);
    if (recursive) {
        if (object->parent_object_index != -1)
            object_detach_internal(object_index);
        ::object_deactivate(object_index, false);
        if ((_rotl(object->flags, 24) & 1) != 0)
            object_disconnect_from_map(object_index, true);
    }
    object_datum *parent = object_get(object_index);
    long widget_index = parent->first_widget_index;
    if (widget_index != -1) {
        do {
            s_widget_datum *widget = &((s_widget_datum *)g_widget_data->data)[(unsigned short)widget_index];
            long datum_index = widget->type_data;
            long next_index = widget->next_widget;
            if (datum_index != -1) {
                s_widget_type *type = (s_widget_type *)&g_widget_types[widget->type];
                type->delete_widget(datum_index);
            }
            ::datum_delete(g_widget_data, widget_index);
            widget_index = next_index;
        } while (widget_index != -1);
    }
    parent->first_widget_index = -1;
    attachments_delete(object_index);
    long child_index = object->first_child_object_index;
    if (child_index != -1) {
        do {
            long next_index = object_get(child_index)->next_object_index;
            object_delete_recursive(child_index, false);
            child_index = next_index;
        } while (child_index != -1);
    }
    impacts_notify_object_deleted(object_index);
    havok_proxy_release_internal(object_index, true);
    havok_component_release(object_index, false);
    object_early_mover_delete(object_index);
    object_type_delete(object_index);
    object_header_delete(object_index);
}

// 0x317720 objects_initialize: pasted from the byte-exact anchored record
// src/31/sub_317720.cpp (anchored_relocation), callees renamed to the accepted
// real names, lifter locals renamed in declaration order.















// @address 0x317720 ?objects_initialize@@YAXXZ
void objects_initialize(void)
{
    int unused_init;
    void *depth_globals;
    long render_data_count;
    void *object_globals_new;
    void *message_queue_new;
    s_data_array *widget_data;
    short widget_index;
    void *render_data;
    long render_index;
    g_tracked_object_globals = (s_objects_tracked_object_globals *)g_tracked_object_globals_allocator.reserve_memory("damage globals",
        0, 0x810, 0);
    widget_data = data_new("widget", 64, 12, 0, (c_allocation_base *)&g_widget_data_allocator);
    widget_index = 0;
    g_widget_data = widget_data;
    do {
        s_widget_type *e = &g_widget_types[widget_index];
        if (e->initialize)
            e->initialize();
        widget_index++;
    } while (widget_index < 4);
    object_types_initialize();
    lights_initialize();
    tls_recycling_groups() = (s_data_array *)data_new("recycling_group", 128, 20, 0, (c_allocation_base *)&g_recycling_groups_allocator);
    depth_globals = depth_of_field_new_game_globals(
        &g_recycling_volumes_allocator, "recycling_volumes", 0, 0x148, 0);
    g_recycling_volumes = (s_depth_of_field *)depth_globals;
    memset(depth_globals, 0, sizeof(s_recycling_volumes));
    object_recycling_reset_volume_list();
    g_object_render_data_count = 0x800;
    object_header_data = data_new("object", 0x800, 16, 0, (c_allocation_base *)&g_object_data_allocator);
    g_object_memory_pool = memory_pool_new("objects", 0x180000, 1, (unsigned int
        *)&g_object_memory_pool_allocator);
    object_memory_non_gamestate = false;
    render_data = g_object_render_data_allocator.reserve_memory("OBJ: Render Data", "Render", 0x2000, 0);
    render_data_count = g_object_render_data_count;
    g_object_render_data = (long *)render_data;
    for (render_index = 0; render_index < render_data_count; render_index++)
        ((int *)render_data)[render_index] = -1;
    object_globals_new = g_object_broadphase_data_allocator.reserve_memory("object globals", 0, 0x5e64, 0);
    object_globals = (s_object_globals *)object_globals_new;
    memset(object_globals_new, 0, sizeof(s_object_globals));
    memset(g_object_marker_globals, 0, sizeof(s_object_marker_globals) * 2);
    message_queue_new = g_object_render_message_queue_allocator.reserve_memory("object messaging queue", 0, 0x4104, 4);
    g_object_message_queue = (t_message_queue<s_object_render_thread_message, 2048> *)message_queue_new;
    ((t_message_queue<s_object_render_thread_message, 2048> *)message_queue_new)->clear();
    g_object_name_list = (long *)g_object_name_list_allocator.reserve_memory("object name list", 0, 0x2000, 0);
    cluster_partition_new((cluster_partition *)&tls_collideable_object_cluster_partition(), "collideable object", 20,
        (c_allocation_base *)&g_collideable_first_data_allocator, (c_allocation_base *)&g_collideable_data_reference_allocator,
        (c_allocation_base *)&g_collideable_cluster_reference_allocator);
    cluster_partition_new((cluster_partition *)&tls_noncollideable_object_cluster_partition(), "noncollideable object", 20,
        (c_allocation_base *)&g_noncollideable_first_data_allocator, (c_allocation_base *)&g_noncollideable_data_reference_allocator,
        (c_allocation_base *)&g_noncollideable_cluster_reference_allocator);
}

// Delete every attachment and release its widget or effect.

extern "C" {
}
static __forceinline void objects23_flags_set(unsigned long *flags, long bit, bool enable)
{
    if (enable)
        *flags |= 1UL << bit;
    else
        *flags &= ~(1UL << bit);
}

// @address 0x31f778 ?attachments_delete@@YAXJ@Z
void attachments_delete(long object_index)
{
    object_datum *object = object_get(object_index);
    long size = object->attached_lights.size;
    long offset = object->attached_lights.offset;
    long count = (unsigned long)size >> 3;
    char *base = (char *)object + offset;
    // match: 0x31f778: value range assumption guides branch lowering
    __assume(base != 0);
    if (count > 0) {
        char *cursor = base + 4;
        unsigned long remaining = (unsigned long)count;
        do {
            char *e = cursor - 4;
            signed char type = *e;
            if (type != -1) {
                long datum = *(long *)cursor;
                if (datum != -1) {
                    switch (type) {
                    case 0:
                        light_delete_internal(datum, true);
                        break;
                    case 1:
                        ::datum_delete(game_looping_sound_data, datum);
                        break;
                    case 2:
                        effect_handle_deleted_object(datum);
                        break;
                    }
                }
            }
            remaining--;
            *(long *)cursor = -1;
            cursor += 8;
            *e = -1;
        } while (remaining != 0);
    }
    objects23_flags_set(&object->flags, 4, false);
    objects23_flags_set(&object->flags, 5, false);
}

// Disconnect objects and release their type-specific BSP state.


// @address 0x317f50 ?objects_dispose_from_old_structure_bsp@@YAXK@Z
void objects_dispose_from_old_structure_bsp(unsigned long old_structure_bsp_mask)
{
    g_havok_monitor_globals.lock_count_4c++;
    lights_dispose_from_old_structure_bsp(old_structure_bsp_mask);
    c_object_iterator<object_datum> iterator;
    iterator.begin(0, 0);
    while (iterator.next())
    {
        object_datum *object = iterator.get_datum();
        long object_index = iterator.get_index();
        s_object_header *object_header = object_header_get(object_index);
        if ((_rotl(object->flags, 24) & 1) != 0)
            object_disconnect_from_map(object_index, true);
        object_type_definition *type_definition = (object_type_definition *)object_type_definitions[object_get(object_index)->identifier_type];
        for (short i = 0; type_definition->parts[i]; i++)
        {
            if (type_definition->parts[i]->dispose_from_old_structure_bsp)
                type_definition->parts[i]->dispose_from_old_structure_bsp(object_index);
        }
        object_header->cluster_index = -1;
        location_invalidate(&object->location);
    }
    object_globals->can_connect_to_map = 0;
    widgets_dispose_from_old_structure_bsp(old_structure_bsp_mask);
    g_havok_monitor_globals.lock_count_4c--;
}

// match: intrinsic pragma selects hardware instruction form
// match: 0xe3620: preserve the flags reload across the activity test.
#pragma intrinsic(_ReadWriteBarrier)
// @address 0xe3620 ?object_set_always_active@@YAXJ_N@Z
void object_set_always_active(long object_index, bool always_active)
{
    if (object_index != -1) {
        object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
        unsigned long outer_flags = object->flags;
        _ReadWriteBarrier();
        if ((bool)((outer_flags >> 1) & 1) != always_active) {
            if (always_active)
                object->flags |= 2;
            else
                object->flags &= ~2u;
            if (always_active) {
                object_activate(object_index);
                object_wake(object_index);
                return;
            }
            s_object_header *header = object_header_get(object_index);
            if ((header->flags & 1) != 0 && !game_test_cluster_activation(&header->cluster_reference)) {
                if (object_should_be_deleted_when_deactivated(object_index)) {
                    object_delete(object_index);
                    return;
                }
                ::object_deactivate(object_index, false);
            }
        }
    }
}
// @address 0xe3ec8 ?object_activate@@YAXJ@Z
void object_activate(long object_index)
{
    s_object_header *header = object_header_get(object_index);
    unsigned char flags = header->flags;
    if ((flags & 1) == 0 && (flags & 0x80) == 0) {
        object_datum *object = header->datum;
        header->flags = (unsigned char)((flags & 0xFE) | 1);
        if (havok_object_needs_physics(object_index, object->definition_index))
            object_rebuild_havok(object_index);
        if ((object->recycling_flags & 2) != 0 && (object->recycling_flags & 4) == 0) {
            object_globals->recyclable_garbage_object_count++;
            object->recycling_flags |= 4;
        }
    }
}

static __forceinline unsigned char objects39_can_connect_to_map()
{
    unsigned char result = object_globals && object_globals->can_connect_to_map;
    return result;
}
// @address 0xfc388 ?object_set_in_limbo@@YAXJ_N@Z
void object_set_in_limbo(long object_index, bool in_limbo)
{
    object_datum *object = object_get(object_index);
    if (in_limbo) {
        ::object_deactivate(object_index, false);
        if ((bool)((object->flags >> 8) & 1))
            object_disconnect_from_map(object_index, true);
        if ((bool)((object->physics_flags >> 7) & 1))
            object_disconnect_from_physics(object_index);
    }
    if (in_limbo) object->flags |= 1 << 7;
    else object->flags &= ~(1 << 7);
    if (!in_limbo && !(bool)((object->flags >> 8) & 1) && objects39_can_connect_to_map()) {
        object_reconnect_to_map(object_index, true, NULL);
        object_reconnect_to_physics(object_index);
    }
}


// @address 0x317db0 ?objects_initialize_for_new_structure_bsp@@YAXK@Z
void objects_initialize_for_new_structure_bsp(unsigned long mask)
{
    s_location location;
    c_object_iterator<object_datum> iterator;
    g_havok_monitor_globals.lock_count_4c++;
    iterator.begin(-1, 0);
    object_globals->can_connect_to_map = true;
    while (iterator.next()) {
        object_datum *datum = iterator.get_datum();
        long object_index = iterator.get_index();
        unsigned char hidden = (datum->flags >> 7) & 1;
        if (datum->parent_object_index == -1 && !hidden) {
            object_try_and_get_location(object_index, &location);
            object_reconnect_to_map(object_index, true, &location);
        }
        object_type_definition *type = (object_type_definition *)object_type_definitions[object_get(object_index)->identifier_type];
        for (short i = 0; type->parts[i]; i++) {
            void (*callback)(unsigned long) = type->parts[i]->initialize_for_new_structure_bsp;
            if (callback) callback(object_index);
        }
    }
    for (long slot = 0; slot < 16; slot++)
        if (mask & (1 << slot)) objects_setup_structure_bsp_fake_lightprobes(slot);
    lights_initialize_for_new_structure_bsp(mask);
    objects_initialize_structure_widgets(mask);
    g_havok_monitor_globals.lock_count_4c--;
}

/* ---- the object position/connectivity entry points ---- */




#if defined(_XBOX) && !defined(__clang__)
// match: 0x318860: the 32768.0 upper bound (0x8203364c) loads at +0x598 from the g_object_world_coordinate_bounds
// base register held for the lower bound; a 32768.0f literal loads from its own address and 21 words change
#define OBJECT_WORLD_COORDINATE_UPPER_BOUND g_object_world_coordinate_bounds[358]
#else
#define OBJECT_WORLD_COORDINATE_UPPER_BOUND 32768.0f
#endif
// @address 0x318860 ?object_set_position_internal@@YA_NJPBTreal_point3d@@PBTreal_vector3d@@1PBUs_location@@_N333@Z
/* Places an object: bounds-checks the new position, copies the axes, tells havok, and reconnects the
   object to the cluster partition afterwards. Returns false when the position was rejected. */
bool object_set_position_internal(long object_index, const real_point3d *position, const real_vector3d *forward, const real_vector3d *up,
                                  const s_location *location, bool compute_node_matrices, bool set_havok_position, bool force, bool keep_connected)
{
    object_datum *object = object_get(object_index);
    s_location location_buffer;
    bool result = true;
    bool connected_to_map = (bool)((object->flags >> 8) & 1);
    if (connected_to_map && !keep_connected)
        object_disconnect_from_map(object_index, false);
    if (position != NULL) {
        if (object->parent_object_index == -1) {
            bool in_bounds = position->x >= g_object_world_coordinate_bounds[0] && position->x <= OBJECT_WORLD_COORDINATE_UPPER_BOUND
                          && position->y >= g_object_world_coordinate_bounds[0] && position->y <= OBJECT_WORLD_COORDINATE_UPPER_BOUND
                          && position->z >= g_object_world_coordinate_bounds[0] && position->z <= OBJECT_WORLD_COORDINATE_UPPER_BOUND;
            if (!in_bounds)
                result = false;
        }
        if (result) {
            object->position = *position;
            ::simulation_action_object_update(object_index, 2);
        }
    }
    if (forward != NULL) {
        object->forward = *forward;
        object->up = *up;
        ::simulation_action_object_update(object_index, 4);
    }
    if (set_havok_position)
        havok_object_set_position(object_index, position == NULL, force, false);
    if (compute_node_matrices)
        object_compute_node_matrices(object_index);
    if (connected_to_map && !keep_connected) {
        if (force && location == NULL && object_try_and_get_location(object_index, &location_buffer))
            location = &location_buffer;
        object_reconnect_to_map(object_index, false, location);
        if ((bool)((object->flags >> 9) & 1))
            object_broadphase_update_object(object_index);
    }
    return result;
}
#undef OBJECT_WORLD_COORDINATE_UPPER_BOUND

// @address 0x31a950 ?object_reconnect_to_map@@YAXJ_NPBUs_location@@@Z
/* Puts an object into a cluster partition: picks a location when none is given, rebuilds the payload and
   updates the partition, the broadphase handle and the activation state of the object. */
void object_reconnect_to_map(long object_index, bool broadphase, const s_location *location)
{
    s_object_header *header = object_header_get(object_index);
    object_datum *object = header->datum;
    s_location location_buffer;
    if (location == NULL) {
        // c7 symbol-id padding: these dead locals shift center_of_mass's id above the
        // shared constants so the center-of-mass height adjustment below evaluates last.
        float pad01 = 0.0f, pad02 = 0.0f, pad03 = 0.0f, pad04 = 0.0f, pad05 = 0.0f;
        float pad06 = 0.0f, pad07 = 0.0f, pad08 = 0.0f, pad09 = 0.0f, pad10 = 0.0f;
        float pad11 = 0.0f, pad12 = 0.0f, pad13 = 0.0f, pad14 = 0.0f, pad15 = 0.0f;
        float pad16 = 0.0f, pad17 = 0.0f, pad18 = 0.0f, pad19 = 0.0f, pad20 = 0.0f;
        float pad21 = 0.0f, pad22 = 0.0f, pad23 = 0.0f, pad24 = 0.0f, pad25 = 0.0f;
        float pad26 = 0.0f, pad27 = 0.0f, pad28 = 0.0f, pad29 = 0.0f, pad30 = 0.0f;
        real_point4d center_of_mass;
        object_get_center_of_mass(object_index, &center_of_mass.point);
        float bounding_radius = object->bounding_sphere_radius - 0.001f;
        if (point_distance_squared3d(&center_of_mass.point, &object->bounding_sphere_center) > bounding_radius * bounding_radius) {
            scenario_location_from_point(&location_buffer, &object->bounding_sphere_center);
        } else {
            scenario_location_from_point(&location_buffer, &center_of_mass.point);
            if (!objects_location_valid(&location_buffer)) {
                center_of_mass.point.z += 0.2f;
                scenario_location_from_point(&location_buffer, &center_of_mass.point);
            }
        }
        location = &location_buffer;
    }
    if (objects_location_valid(location)) {
        object->location = *location;
        header->cluster_reference = object->location.cluster_reference;
        object->flags &= ~(1u << 18);
    } else {
        object->flags |= 1u << 18;
    }
    s_game_cluster_bit_vectors visibility_bits;
    const s_game_cluster_bit_vectors *visibility = NULL;
    bool overflow = false;
    if ((bool)((object->flags >> 21) & 1)) {
        memset(&visibility_bits, -1, sizeof(visibility_bits));
        visibility = &visibility_bits;
    }
    if (object->identifier.m_source == 4)
        visibility = c_scenerio_sky_objects_helper::calculate_sky_object_cluster_visibility(object_index, &visibility_bits);
    {
        s_object_cluster_payload payload;
        cluster_build_object_payload(object_index, &payload);
        cluster_partition *partition = (bool)((object->flags >> 9) & 1) ? &tls_collideable_object_cluster_partition() :
            &tls_noncollideable_object_cluster_partition();
        cluster_partition_reconnect(partition, object_index, &object->first_cluster_reference, &object->bounding_sphere_center,
                                    object->bounding_sphere_radius, &object->location, visibility, sizeof(payload),
                                    &payload, &overflow);
        object->flags |= 1u << 8;
        header->flags |= 0x40;
        if ((bool)((object->flags >> 9) & 1)) {
            if (broadphase)
                object_broadphase_add_object(object_index, (const s_object_broadphase_handle_data *)&payload);
            else
                object_broadphase_set_handle_data(object_index, (const s_object_broadphase_handle_data *)&payload);
        }
    }
    object_connect_lights_recursive(object_index, false, true, false, false);
    if (object_should_be_active(object_index, (const s_game_cluster_bit_vectors *)&game_globals->cluster_activation)) {
        object_activate(object_index);
        return;
    }
    if (object_should_be_deleted_when_deactivated(object_index)) {
        object_delete(object_index);
        return;
    }
    ::object_deactivate(object_index, false);
}

// @address 0x31b248 ?object_align_marker_to_matrix@@YAXJPBUobject_marker@@PBUreal_matrix4x3@@@Z
/* Moves an object so that one of its markers lands on a destination matrix. */
void object_align_marker_to_matrix(long object_index, const object_marker *marker, const real_matrix4x3 *matrix)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    real_matrix4x3 world;
    real_matrix4x3 inverse;
    real_matrix4x3 relative;
    matrix4x3_rotation_from_vectors(&world, &object->forward, &object->up);
    world.position = object->position;
    matrix4x3_inverse(&world, &inverse);
    matrix4x3_compute_product(&inverse, &marker->matrix, &relative);
    matrix4x3_inverse(&relative, &inverse);
    matrix4x3_compute_product(matrix, &inverse, &world);
    real_vector3d forward = world.forward;
    real_vector3d up = world.up;
    object_prepare_axis_vectors(object_index, &forward, &up);
    object_set_position_internal(object_index, &world.position, &forward, &up, NULL, true, true, false, false);
}

// @address 0x31b998 ?object_detach_immediate@@YAXJ@Z
/* Detaches an object from its parent node right away: keeps the parent's velocities, reactivates the object
   and re-roots it in the early mover list. */
void object_detach_immediate(long object_index)
{
    object_datum *object = object_get(object_index);
    long parent_index = object->parent_object_index;
    bool connected_to_map = object_is_connected_to_map(parent_index);
    object_datum *parent_object = object_get(parent_index);
    const real_matrix4x3 *node_matrix = (const real_matrix4x3 *)((char *)parent_object + parent_object->node_matrices.offset
                                                                + object->parent_node_index * sizeof(real_matrix4x3));
    if (connected_to_map)
        object_connect_lights_recursive(object_index, true, false, false, false);
    object_detach_internal(object_index);
    object_detach_from_node(object_index, node_matrix);
    if (connected_to_map)
        object_reconnect_to_map(object_index, true, NULL);
    if (!(bool)((object->physics_flags >> 8) & 1)) {
        if (((1 << object_header_get(object_index)->object_type) & 0x80) == 0)
            object_reconnect_to_physics(object_index);
    }
    real_vector3d linear_velocity;
    real_vector3d angular_velocity;
    object_get_velocities(parent_index, &linear_velocity, &angular_velocity);
    object_set_velocities(object_index, &linear_velocity, &angular_velocity);
    object_activate(object_index);
    object_wake(object_index);
    object_set_at_rest(object_index, false);
    long root_index = -1;
    long cursor = parent_index;
    unsigned long *flags = &object->flags;
    *flags &= ~0x4000000u;
    object->marker = -1;
    if (parent_index != -1) {
        do {
            root_index = cursor;
            cursor = object_get(cursor)->parent_object_index;
        } while (cursor != -1);
    }
    ::object_in_early_mover_join(root_index, object_index);
}



static __forceinline bool objects23_structure_bsp_is_active(long index)
{
    return index >= 0 && index < 16 && (g_active_structure_bsp_mask & (1 << index)) != 0;
}
static __forceinline collision_bsp *objects23_collision_global_bsp_get(long index)
{
    return const_cast<collision_bsp *>(static_cast<const collision_bsp *>(g_collision_bsp_cache.thread_users[g_registered_thread_index].collision_bsps[index]));
}
static __forceinline structure_bsp *objects23_collision_global_structure_bsp_get(long index)
{
    long tag_index = ((scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[index].structure_bsp.index;
    return (structure_bsp *)tag_get('sbsp', tag_index);
}

// @address 0x28aaf0 collision_test_sphere
extern "C" bool collision_test_sphere(
    s_collision_test_flags flags, const real_point3d *point, float radius, long first_ignore, long second_ignore)
{
    bool result = false;
    for (long index = 0; index < 16; ++index) {
        if (objects23_structure_bsp_is_active(index)) {
            bool valid;
            do {
                if (bsp3d_test_point((const bsp3d *)::global_bsp3d_get(index), 0, point) != -1) {
                    collision_bsp_test_sphere_result sphere;
                    const collision_bsp *bsp = objects23_collision_global_bsp_get(index);
                    long material = objects23_collision_global_structure_bsp_get(index)->breakable_surfaces.count;
                    s_breakable_surface_set *breakable = ::breakable_surface_get_first_set(index, -1);
                    c_collision_bsp_test_sphere_flags flags;
                    flags.set(_collision_bsp_test_sphere_leaves_bit, true);
                    flags.set(_collision_bsp_test_sphere_surfaces_bit, true);
                    bool tested = collision_bsp_test_sphere(bsp, flags, material, breakable, point, radius, &sphere);
                    if (!tested) {
                        valid = false;
                        break;
                    }
                }
                valid = true;
            } while (false);
            if (valid) {
                result = true;
                break;
            }
        }
    }
    return result;
}


// @address 0x2f49e0 object_clamp_melee_velocity
extern "C" void object_clamp_melee_velocity(const s_melee_clamp_data *clamp_data, long object_index)
{
    real_vector3d linear_velocity;
    real_vector3d early_mover_velocity;
    real_vector3d combined_velocity[2];
    object_get_localized_velocities(object_index, &linear_velocity, 0, &early_mover_velocity, 0);
    if (character_physics_mode_melee_clamp_velocity(clamp_data, &linear_velocity)) {
        combined_velocity[0].i = linear_velocity.i + early_mover_velocity.i;
        combined_velocity[0].j = linear_velocity.j + early_mover_velocity.j;
        combined_velocity[0].k = linear_velocity.k + early_mover_velocity.k;
        object_set_velocities(object_index, &combined_velocity[0], 0);
    }
    s_object_header *header = object_header_get(object_index);
    if ((1 << header->object_type) & 1) {
        biped_datum *object = (biped_datum *)header->datum;
        object->biped.flags |= 0x4000;
    }
}


static __forceinline long objects_lw_model_find_permutation(long model_index, long region_index, long permutation_name)
{
    long result = -1;
    if (model_index != -1 && region_index != -1 && permutation_name != 0) {
        const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', model_index);
        const s_model_region *region = &((const s_model_region *)model->regions.address)[region_index];
        for (long permutation_index = 0; permutation_index < region->permutations.count; ++permutation_index) {
            if (((const s_model_region_permutation *)region->permutations.address)[permutation_index].name == permutation_name) {
                result = permutation_index;
                break;
            }
        }
    }
    return result;
}
static __forceinline void objects_lw_set_region_permutation(
    long region_index, long permutation_index, char *permutations, object_region_information *information)
{
    permutations[region_index] = (char)permutation_index;
    information[region_index].variant_index = -1;
    information[region_index].state = 0;
    information[region_index].properties = 0;
    information[region_index].effect = -1;
}
// @address 0x31bc10 ?object_set_region_permutation_direct@@YAXJJJ@Z
void object_set_region_permutation_direct(long object_index, long region_name, long permutation_name)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    if (definition->model.index != -1) {
        long requested_region = model_find_region_by_name(definition->model.index, region_name);
        long region_count;
        char *permutation_indices;
        object_region_information *region_information;
        object_get_region_information(object_index, &region_count, &permutation_indices, 0, &region_information);
        long count = region_count;
        char old_permutations[16];
        memcpy(old_permutations, (const void *)permutation_indices, count);
        for (long region_index = 0; region_index < count; ++region_index) {
            if (region_index == requested_region || region_name == 0) {
                long permutation = objects_lw_model_find_permutation(definition->model.index, region_index, permutation_name);
                if (permutation != -1 || permutation_name == 0)
                    objects_lw_set_region_permutation(region_index, permutation, permutation_indices, region_information);
            }
        }
        object_compute_instance_bitfield_visible(object_index);
        effects_object_model_state_changed(object_index);
        havok_notify_object_changed_region_permutation(object_index, count, old_permutations);
    }
}
static __forceinline void objects_model_select_initial_instances(long model_index, long variant_index, unsigned long *instances)
{
    const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', model_index);
    memset(instances, 0, sizeof(unsigned long) * 2);
    if (variant_index != -1) {
        const s_model_variant *variant = &((s_model_variant *)model->variants.address)[variant_index];
        if (variant->instance_group_index != -1)
            model_instance_group_choose_instances((const s_model_definition *)model, variant->instance_group_index, instances);
    }
}


// @address 0x31be68 ?object_choose_initial_permutation@@YAXJJPAUs_model_customization_region@@J@Z
void object_choose_initial_permutation(
    long object_index, long region_mask, s_model_customization_region *customization_regions, long customization_region_count)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    char *visible_permutations;
    long region_count;
    char *region_permutations;
    object_region_information *region_information;
    object_get_region_information(object_index, &region_count, &region_permutations, &visible_permutations, &region_information);
    long count = region_count;
    char old_permutations[16];
    memcpy(old_permutations, (const void *)region_permutations, count);
    if (definition->model.index != -1) {
        model_select_initial_permutation(definition->model.index, object->variant_index, region_permutations, region_information,
            region_mask, customization_regions, customization_region_count);
        unsigned long *instances;
        if (object_get_render_model_instance_bitfield_active(object_index, &instances))
            objects_model_select_initial_instances(definition->model.index, object->variant_index, instances);
    } else {
        for (long region_index = 0; region_index < region_count; region_index++) {
            region_permutations[region_index] = 0;
            object_region_information *region = &region_information[region_index];
            region->variant_index = -1;
            region->state = 0;
            region->properties = 0;
        }
    }
    memcpy((void *)visible_permutations, (const void *)region_permutations, region_count);
    object_compute_instance_bitfield_visible(object_index);
    effects_object_model_state_changed(object_index);
    havok_notify_object_changed_region_permutation(object_index, count, old_permutations);
}
// @address 0x31c1b0 ?object_set_model_state@@YAXJJJ_N@Z
void object_set_model_state(long object_index, long region_name, long state, bool force)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    if (definition->model.index != -1) {
        long region_count;
        char *permutation_indices;
        object_region_information *region_information;
        object_get_region_information(object_index, &region_count, &permutation_indices, 0, &region_information);
        long count = region_count;
        char old_permutations[16];
        memcpy(old_permutations, (const void *)permutation_indices, count);
        model_set_state_or_property_per_region(object_index, definition->model.index, object->variant_index, region_name, state, force, -1,
            0, permutation_indices, region_information);
        object_compute_instance_bitfield_visible(object_index);
        effects_object_model_state_changed(object_index);
        havok_notify_object_changed_region_permutation(object_index, count, old_permutations);
        object_wake(object_index);
    }
}

// @address 0x31c2a0 ?object_set_model_state_property_per_region@@YAXJJJF_N@Z
void object_set_model_state_property_per_region(long object_index, long region_name, long property, short value, bool force)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    if (definition->model.index != -1) {
        long region_count;
        char *permutation_indices;
        object_region_information *region_information;
        object_get_region_information(object_index, &region_count, &permutation_indices, 0, &region_information);
        long count = region_count;
        char old_permutations[16];
        memcpy(old_permutations, (const void *)permutation_indices, count);
        model_set_state_or_property_per_region(object_index, definition->model.index, object->variant_index, -1, -1, force, property, value,
            permutation_indices, region_information);
        object_compute_instance_bitfield_visible(object_index);
        effects_object_model_state_changed(object_index);
        havok_notify_object_changed_region_permutation(object_index, count, old_permutations);
    }
}
extern "C" {
}
// c_flags_no_init<e_object_physics_flags, unsigned long, 19>::test on object->physics_flags
static __forceinline bool objects_physics_flags_test(const object_datum *object, long bit)
{
    return (object->physics_flags & (1 << bit)) != 0;
}

// @address 0x31d1c8 ?object_respond_to_physics@@YAXJ@Z
void object_respond_to_physics(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    bool at_rest;
    if (objects_physics_flags_test(object, 7)) {
        c_havok_component *component = objects_havok_component_get(object->havok_component_index);
        unsigned char is_active = havok_component_has_active_entity(component);
        if (is_active || objects_physics_flags_test(object, 9)) {
            if (is_active)
                component->m_flags &= ~0x800;
            if (component->m_rigid_bodies.getSize() > 0) {
                if (!objects_havok_is_sentinel(component) && objects_havok_dynamic_controlling_body(component)) {
                    real_matrix4x3 transform;
                    long object_type = object_header_get(object_index)->object_type;
                    component->calculate_object_transform(&transform);
                    real_vector3d *forward = &transform.forward;
                    real_vector3d *up = &transform.up;
                    if ((((1 << object_type) & 0x800) != 0 || ((1 << object_type) & 1) != 0) &&
                        objects_havok_infinite_inertia(component)) {
                        forward = NULL;
                        up = NULL;
                    } else {
                        object_prepare_axis_vectors(object_index, forward, up);
                    }
                    object_set_position_internal(object_index, &transform.position, forward, up, NULL, false, false, false, true);
                    if (is_active) {
                        real_vector3d linear_velocity;
                        real_vector3d angular_velocity;
                        long rigid_body_index = objects_havok_controlling_body(component);
                        const real_vector3d *angular_velocity_pointer = NULL;
                        component->get_rigid_body_linear_velocity( rigid_body_index, &linear_velocity);
                        if (!component->get_rigid_body_has_infinite_inertia_tensor(rigid_body_index)) {
                            component->get_rigid_body_angular_velocity(rigid_body_index, &angular_velocity);
                            angular_velocity_pointer = &angular_velocity;
                        }
                        object_set_velocities_internal(object_index, &linear_velocity, angular_velocity_pointer, false);
                    } else {
                        object_set_velocities_internal(object_index, global_zero_vector3d, global_zero_vector3d, false);
                    }
                } else {
                    object_set_velocities_internal(object_index, global_zero_vector3d, global_zero_vector3d, false);
                }
            }
        }
        at_rest = !is_active;
        if (is_active)
            havok_collision_damage_update_object(object_index, object->havok_component_index);
    } else {
        long parent_index = object->parent_object_index;
        if (parent_index != -1)
            at_rest = objects_physics_flags_test(object_get(parent_index), 9);
        else
            at_rest = true;
    }
    if (((1 << object_header_get(object_index)->object_type) & 0x1cdf) != 0) {
        if (at_rest)
            object->physics_flags |= 0x200;
        else
            object->physics_flags &= ~0x200;
    }
}

typedef char objects_sound_scenery_size[(sizeof(s_sound_scenery_datum) == 0x17c) ? 1 : -1];
typedef char objects_sound_scenery_minimum_distance_offset[(offsetof(s_sound_scenery_datum, minimum_distance) == 0x168) ? 1 : -1];
typedef char objects_sound_scenery_maximum_distance_offset[(offsetof(s_sound_scenery_datum, maximum_distance) == 0x16c) ? 1 : -1];
// @address 0x31e8c0 ?object_compute_bounding_sphere_recursive@@YAXJPBTreal_point3d@@PAM@Z
void object_compute_bounding_sphere_recursive(long object_index, const real_point3d *point, float *radius)
{
    object_datum *object = object_get(object_index);
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    real_point3d recursive_center;
    float bound_radius;
    bool check_havok_thread = get_current_thread_index() == 1 && restricted_region_is_available(3);
    if (!(check_havok_thread
        && object->havok_component_index != -1
        && ((1 << object->identifier_type) & (1 << 10)) != 0
        && objects_physics_flags_test(object, 7)
        && objects_havok_component_get(object->havok_component_index)->calculate_physics_bounding_sphere(&recursive_center, &bound_radius)))
    {
        object_datum *datum = object_get(object_index);
        matrix4x3_transform_point((real_matrix4x3 *)((char *)datum + datum->node_matrices.offset), &definition->bounding_offset, &recursive_center);
        bound_radius = object->scale * definition->bounding_radius;
        if ((unsigned char)object->identifier_type == 9)
        {
            bound_radius = 0.0f;
            if (object_index != -1 && datum->definition_index != -1)
            {
                const objects_sound_scenery_definition *motor_definition = (const objects_sound_scenery_definition
                    *)tag_get('ssce', datum->definition_index);
                const s_sound_scenery_datum *dims = (const s_sound_scenery_datum *)datum;
                float size = dims->minimum_distance;
                float bound = size > 0.0f ? size : motor_definition->minimum_distance;
                float weight = dims->maximum_distance;
                if (weight <= 0.0f)
                    weight = motor_definition->maximum_distance;
                if (bound > weight)
                    weight = bound;
                bound_radius = datum->scale * weight;
            }
        }
    }
    if (point == NULL)
    {
        point = &object->bounding_center;
        radius = &object->bounding_radius;
        object->bounding_center = recursive_center;
        *radius = bound_radius;
    }
    else
    {
        float dz = 0.0f;
        float dx = recursive_center.x - point->x;
        dz = recursive_center.z - point->z;
        float dy = recursive_center.y - point->y;
        float grown = sqrtf(dy * dy + (dx * dx + dz * dz)) + bound_radius;
        if (*radius > grown)
            grown = *radius;
        *radius = grown;
    }
    long child = object->first_child_object_index;
    if (child != -1)
    {
        object_datum *child_object;
        do {
            child_object = object_get(child);
            object_compute_bounding_sphere_recursive(child, point, radius);
            child = child_object->next_object_index;
        } while (child != -1);
    }
}

static __forceinline float objects_lw_pin(float value, float lower, float upper)
{
    if (!(value <= lower) && value >= upper)
        return upper;
    else if (value > lower)
        return value;
    else
        return lower;
}
// @address 0x31ec30 ?object_set_initial_change_colors@@YAXJKPBTreal_rgb_color@@@Z




static __forceinline real_rgb_color *objects_lw_change_colors_get(object_datum *object, long *count)
{
    const s_object_block_reference *reference = &object->change_colors;
    *count = reference->size / 12u;
    return (real_rgb_color *)((char *)object + reference->offset);
}
// @address 0x31f2a8 ?object_compute_change_colors@@YA_NJ@Z
bool object_compute_change_colors(long object_index)
{
    bool result = false;
    object_datum *object = object_get(object_index);
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    if (definition->runtime_flags & 1)
    {
        real_rgb_color *change_colors = (real_rgb_color *)((char *)object + object->change_colors.offset);
        long change_color_count = (unsigned long)object->change_colors.size / 12;
        long count = change_color_count / 2;
        for (long i = 0; i < count; ++i)
        {
            const s_object_change_color_definition *entry = (const s_object_change_color_definition *)tag_block_get_element_with_size(
                &definition->change_colors, i, sizeof(s_object_change_color_definition));
            real_rgb_color *color = &change_colors[count + i];
            *color = change_colors[i];
            for (long j = 0; j < entry->functions.count; ++j)
            {
                const s_object_change_color_function *function =
                    (const s_object_change_color_function *)tag_block_get_element_with_size(
                        &entry->functions, j, sizeof(s_object_change_color_function));
                if (function->tint_function)
                {
                    float value = objects_get_function_value_simple(object_index, function->tint_function, object->definition_index);
                    rgb_colors_interpolate(color, function->flags, &function->color_a, &function->color_b, value);
                }
            }
            for (long j = 0; j < entry->functions.count; ++j)
            {
                const s_object_change_color_function *function =
                    (const s_object_change_color_function *)tag_block_get_element_with_size(
                        &entry->functions, j, sizeof(s_object_change_color_function));
                if (function->scale_function)
                {
                    float scale = objects_get_function_value_simple(object_index, function->scale_function, object->definition_index);
                    color->red *= scale;
                    color->green *= scale;
                    color->blue *= scale;
                }
            }
            color->red = ((color->red > 0.0f ? color->red : 0.0f) < 1.0f ? (color->red > 0.0f ? color->red : 0.0f) : 1.0f);
            color->green = ((color->green > 0.0f ? color->green : 0.0f) < 1.0f ? (color->green > 0.0f ? color->green : 0.0f) : 1.0f);
            color->blue = ((color->blue > 0.0f ? color->blue : 0.0f) < 1.0f ? (color->blue > 0.0f ? color->blue : 0.0f) : 1.0f);
            result = true;
        }
    }
    return result;
}


typedef char objects_widget_reference_size[(sizeof(s_structure_bsp_widget_reference) == 0x14) ? 1 : -1];
typedef char objects_widget_reference_offset[(offsetof(s_structure_bsp_widget_reference, widget) == 4) ? 1 : -1];

static __forceinline s_widget_type *objects_widget_type_get(short type_index)
{
    return (s_widget_type *)&g_widget_types[type_index];
}
static __forceinline real_point3d *objects39_set_real_point3d(real_point3d *point, float x, float y, float z)
{
    point->x = x;
    point->y = y;
    point->z = z;
    return point;
}
static __forceinline void objects39_matrix4x3_rotation_from_quaternion(real_matrix4x3 *matrix, const real_quaternion *quaternion)
{
    matrix3x3_rotation_from_quaternion((real_matrix3x3 *)&matrix->forward, quaternion);
    objects39_set_real_point3d(&matrix->position, 0.0f, 0.0f, 0.0f);
    matrix->scale = 1.0f;
}
// @address 0x335660 objects_initialize_structure_widgets
extern "C" void objects_initialize_structure_widgets(unsigned long mask)
{
    for (long structure_bsp_index = 0; structure_bsp_index < 16; structure_bsp_index++) {
        if (mask & (1 << structure_bsp_index)) {
            struct structure_bsp *structure_bsp = global_structure_bsp_get(structure_bsp_index);
            for (long widget_index = 0; widget_index < structure_bsp->widget_references.count; widget_index++) {
                s_structure_bsp_widget_reference *reference = &((s_structure_bsp_widget_reference *)structure_bsp->widget_references.address)[widget_index];
                if (reference->marker != -1) {
                    for (long type_index = 0; type_index < 4; type_index++) {
                        short type = (short)type_index;
                        s_widget_type *widget_type = objects_widget_type_get(type);
                        if (widget_type->group_tag == reference->widget.group_tag) {
                            structure_marker *marker = &((structure_marker *)structure_bsp->markers.address)[reference->marker];
                            real_matrix4x3 matrix;
                            objects39_matrix4x3_rotation_from_quaternion(&matrix, &marker->rotation);
                            matrix.position = marker->position;
                            long datum_index = ::datum_new(g_widget_data);
                            if (datum_index != -1) {
                                s_widget_datum *widget = &((s_widget_datum *)g_widget_data->data)[(unsigned short)datum_index];
                                widget->type = type;
                                if (widget_type->new_on_map) {
                                    widget->type_data = widget_type->new_on_map(reference->widget.index, -1, &matrix);
                                    if (widget->type_data != -1)
                                        widget->next_widget = -1;
                                    else
                                        ::datum_delete(g_widget_data, datum_index);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


static __forceinline float objects_lw_dot_product3d(const real_vector3d *a, const real_vector3d *b)
{
    return a->k * b->k + (a->i * b->i + a->j * b->j);
}
static __forceinline __regfix_ret_objects_lw_dot_product3d objects_lw_dot_product3d_rfret(const real_vector3d *a, const real_vector3d *b)
{
    { __regfix_ret_objects_lw_dot_product3d rf_ret; rf_ret.v = (a->k * b->k + (a->i * b->i + a->j * b->j)); return rf_ret; }
}
static __forceinline real_vector3d *objects_lw_scale_vector(const real_vector3d *v, float scale, real_vector3d *out)
{
    out->i = v->i * scale;
    out->j = v->j * scale;
    out->k = v->k * scale;
    return out;
}
static __forceinline float objects_lw_dot_components(
    const float ai, const float aj, const float ak, const float bi, const float &bj, const float &bk)
{
    return ak * bk + (ai * bi + aj * bj);
}
// @address 0x33d6b8 object_clamp_acceleration
extern "C" void object_clamp_acceleration(long object_index, real_vector3d *vector)
{
    real_vector3d direction = *vector;
    float length = normalize3d(&direction);
    if (length != 0.0f) {
        bool is_biped = (bool)((1 << object_header_get(object_index)->object_type) & 1);
        float limit = is_biped ? 8.0f : 20.0f;
        if (!(limit > length))
            limit = length;
        real_vector3d velocity;
        object_get_localized_velocities(object_index, &velocity, 0, 0, 0);
        float excess = limit - (velocity.k * direction.k + (direction.i * velocity.i + velocity.j * direction.j));
        if (length > excess)
            length = excess;
        if (!(length > 0.0f))
            length = 0.0f;
        vector->i = direction.i * length;
        vector->j = direction.j * length;
        vector->k = direction.k * length;
    }
}


// @address 0x33d7f8 object_apply_acceleration_info
extern "C" void object_apply_acceleration_info(const s_object_acceleration_info *info)
{
    real_vector3d acceleration = info->acceleration;
    long object_index = info->object_index;
    long node_index = info->node_index;
    unsigned short flags = info->flags;
    bool ignore_node_index = (bool)(flags & 1);
    bool set_biped_flag = (bool)((flags >> 2) & 1);
    s_object_header *header = object_header_get(object_index);
    unsigned long object_type = header->object_type;
    object_clamp_acceleration(object_index, &acceleration);
    switch (object_type) {
    case _object_type_vehicle:
    case _object_type_machine:
    case _object_type_crate:
        if (ignore_node_index) {
            object_apply_acceleration(object_index, -1, &info->point, &acceleration, 0);
            return;
        }
        object_apply_acceleration(object_index, node_index, &info->point, &acceleration, 0);
        return;
    case _object_type_projectile:
        projectile_accelerate(object_index, &acceleration, 0);
        return;
    case _object_type_weapon:
    case _object_type_equipment: {
        item_datum *datum = (item_datum *)object_get(object_index);
        if ((datum->inventory_flags & 4) == 0) {
            object_apply_acceleration(object_index, -1, 0, &acceleration, 0);
            return;
        }
        return;
    }
    case _object_type_biped: {
        c_motor_request request;
        request.setup_accelerate(_action_none, &acceleration);
        motor_system_submit(object_index, &request);
        if (set_biped_flag) {
            biped_datum *datum = (biped_datum *)object_get(object_index);
            datum->biped.flags |= 0x4000;
            return;
        }
        return;
    }
    case _object_type_creature:
        ::creature_accelerate(object_index, &acceleration);
    }
}

// 0x102b88
// flushes every queued tracked-object acceleration, then empties the queue
// @address 0x102b88 objects_flush_tracked_accelerations
extern "C" void objects_flush_tracked_accelerations(void)
{
    for (long index = 0; index < ((s_objects_tracked_object_globals *)g_tracked_object_globals)->count; index++) {
        if (((s_objects_tracked_object_globals *)g_tracked_object_globals)->objects[index].object_index != -1)
            object_apply_acceleration_info((const s_object_acceleration_info *)&((s_objects_tracked_object_globals
                *)g_tracked_object_globals)->objects[index]);
    }
    s_objects_tracked_object_globals *globals = (s_objects_tracked_object_globals *)g_tracked_object_globals;
    globals->count = 0;
    globals->changed_flags.m_storage[0] = 0;
    globals->changed_flags.m_storage[1] = 0;
    globals->active = false;
}

// @address 0x31a598 ?object_reinitialize_from_placement@@YAXJPAUobject_placement_data@@_N@Z
void object_reinitialize_from_placement(long object_index, object_placement_data *placement, bool force)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    object->bsp_placement_policy = placement->bsp_placement_policy;
    object_choose_variant(object_index, placement->model_variant_index);
    object_choose_initial_permutation(object_index, placement->region_permutation_flags,
        (s_model_customization_region *)placement->region_permutations, placement->region_permutation_count);
    object_set_initial_change_colors(object_index, placement->active_change_colors, placement->change_colors);
    if (object->parent_object_index != -1)
        object_detach(object_index);
    long parent_index = placement->location_bsp_index;
    if (parent_index != -1) {
        object_datum *parent = (object_datum *)object_try_and_get_and_verify_type(parent_index, -1);
        if (parent) {
            if (((1 << object->identifier_type) & 0x1003) && ((1 << parent->identifier_type) & 0x1003)
                && unit_placement_try_to_put_unit_in_seat_using_marker_name(object_index, parent_index, placement->location_cluster_index))
                return;
            object_attach_to_marker_with_type(placement->location_bsp_index, placement->location_cluster_index, object_index,
                placement->location_leaf_index);
        }
    }
}


static __forceinline bool objects_lw_spheres_equal(
    const real_point3d *old_center, float old_radius, const real_point3d *new_center, float new_radius, float tolerance)
{
    float dx = new_center->x - old_center->x;
    float dz = new_center->z - old_center->z;
    float dy = new_center->y - old_center->y;
    float dx_squared = dx * dx;
    float distance = __fsqrts(dy * dy + (dx_squared + dz * dz));
    float delta = new_radius - old_radius;
    return !(distance + delta < rasterizer_states_mode_5_slope_scale_bias) && distance + delta <= tolerance;
}
static __forceinline real_point3d *objects_lw_bounds_point_from_line(
    const real_point3d *point, const real_point3d *vector, float t, real_point3d *out)
{
    out->x = point->x + vector->x * t;
    out->y = point->y + vector->y * t;
    out->z = point->z + vector->z * t;
    return out;
}
#if defined(_XBOX) && !defined(__clang__)
// match: 0x31e6b0: the 0.1 pad loads at +0x8a0 from the k_real_constant_pool_zero base register held for the 0.0
// radius test; a 0.1f literal loads from its own address and the frame grows by one saved register
#define OBJECT_BOUNDING_SPHERE_PAD k_real_constant_pool_zero[552]
#else
#define OBJECT_BOUNDING_SPHERE_PAD 0.1f
#endif
// @address 0x31e6b0 ?object_compute_bounding_sphere@@YA_NJ@Z
bool object_compute_bounding_sphere(long object_index)
{
    bool result = false;
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    bool connected_to_map = (bool)((object->flags >> 8) & 1);
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    object_compute_bounding_sphere_recursive(object_index, 0, 0);

    if (definition->dynamic_light_sphere_radius > k_real_constant_pool_zero[0]) {
        objects_lw_bounds_point_from_line(&object->bounding_center, &definition->dynamic_light_sphere_offset, 1.0f, &object->dynamic_light_sphere_center);
        object->dynamic_light_sphere_radius = object->scale * definition->dynamic_light_sphere_radius;
    } else {
        object->dynamic_light_sphere_center = object->bounding_center;
        object->dynamic_light_sphere_radius = object->bounding_radius;
    }
    real_point3d center;
    float radius;
    if ((bool)((definition->flags >> 12) & 1)) {
        center = object->dynamic_light_sphere_center;
        radius = object->dynamic_light_sphere_radius + OBJECT_BOUNDING_SPHERE_PAD;
    } else {
        radius = object->bounding_radius + OBJECT_BOUNDING_SPHERE_PAD;
        center = object->bounding_center;
    }
    if (!connected_to_map || !objects_lw_spheres_equal(&object->bounding_sphere_center, object->bounding_sphere_radius, &center, radius,
        OBJECT_BOUNDING_SPHERE_PAD)) {
    if (connected_to_map)
        object_disconnect_from_map(object_index, false);
    object->bounding_sphere_radius = radius;
    object->bounding_sphere_center = center;
    if (connected_to_map) {
        object_reconnect_to_map(object_index, false, 0);
        if ((bool)(_rotl(object->flags, 23) & 1))
            object_broadphase_update_object(object_index);
    }
    result = true;
    }
    return result;
}

// 0x31d560
static __forceinline long objects_havok_last_state_reset_time_get()
{
    return *g_havok_memory_task_data;
}
static __forceinline void objects_object_type_move(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    object_type_definition *definition = object_type_definitions[object->identifier_type];
    for (long part_index = 0; definition->parts[part_index]; part_index++) {
        object_type_definition *part = definition->parts[part_index];
        if (part->move)
            part->move(object_index);
    }
}
// @address 0x31d560 ?object_move@@YAXJ@Z
void object_move(long object_index)
{
    s_object_header *header = object_header_get(object_index);
    object_datum *object = header->datum;
    if (object->havok_component_index != -1)
        object_respond_to_physics(object_index);
    if (!((1 << header->object_type) & 0x1080)) {
        object_compute_node_matrices_non_recursive(object_index);
    } else if (object->havok_component_index != -1 && objects_physics_flags_test(object, 7)) {
        c_havok_component *component = objects_havok_component_get(object->havok_component_index);
        if (objects_havok_has_dynamic_bodies(component)) {
            c_static_flags<255> simulation_controlled_nodes;
            simulation_controlled_nodes.clear_range(object->node_matrices.size / sizeof(real_matrix4x3));
            component->body_slam_object_node_matrices(&simulation_controlled_nodes, 0.0f);
        }
    }
    objects_object_type_move(object_index);
    objects_physics_flags_set(&object->physics_flags, 1, objects_physics_flags_test(object, 2));
    objects_physics_flags_set(&object->physics_flags, 2, false);
    if (objects_havok_last_state_reset_time_get() != game_time_get()) {
        objects_physics_flags_set(&object->physics_flags, 15, objects_physics_flags_test(object, 16));
        objects_physics_flags_set(&object->physics_flags, 16, false);
    }
    bool bounding_sphere_valid = object_compute_bounding_sphere(object_index);
    if ((header->flags & 0x40) && !bounding_sphere_valid)
        object_connect_lights_recursive(object_index, true, true, false, false);
    for (long child_index = object->first_child_object_index; child_index != -1; child_index = object_get(child_index)->next_object_index)
        object_move(child_index);
    object_postprocess_node_matrices(object_index);
}

// 0x31d8d0


// @address 0x119a59c g_object_render_only_node_flags
// @address 0x11db0e4 g_object_render_only_node_flags_initialization
static __forceinline bool objects_biped_flags_test(long biped_index, long bit)
{
    return (bool)((((biped_datum *)((object_datum *)object_get_and_verify_type(biped_index, 1)))->biped.flags >> bit) & 1);
}
static __forceinline bool objects_node_orientations_frozen(long object_index)
{
    return (bool)((((object_datum *)object_get_and_verify_type(object_index, -1))->flags >> 29) & 1);
}
static __forceinline bool objects_has_animation_manager(long object_index)
{
    return ((object_datum *)object_get_and_verify_type(object_index, -1))->animation.offset != -1;
}
static __forceinline const c_model_animation *objects_animation_manager_get_state_animation(const c_animation_manager *animation_manager)
{
    const c_model_animation *animation = NULL;
    if (animation_manager->m_state_channel.m_animation_id.valid())
        animation = ((const c_model_animation_graph *)tag_get('jmad', animation_manager->m_state_channel.m_graph_index))->get_animation(
            animation_manager->m_state_channel.m_animation_id);
    return animation;
}
static __forceinline void objects_build_render_only_node_flags(c_static_flags<255> *flags, const flags256 *model_flags, long node_count)
{
    long word_count = NUMBEROF(flags->m_storage);
    memcpy(flags->m_storage, model_flags->m_storage, word_count * sizeof(flags->m_storage[0]));
    unsigned long *end = flags->m_storage + word_count;
    // match: 0x31d8d0: end-relative indexing keeps the cursor/end cmpw at 0x31dc38. A zero-based index counts down.
    for (long offset = -word_count; offset < 0; ++offset)
        end[offset] = ~end[offset];
    long extra_bits = (255 + 1) & 31;           // bits used past the last full word: none
    unsigned long mask = 0xffffffff;
    if (extra_bits > 0)
        mask >>= 32 - extra_bits;
    flags->m_storage[word_count - 1] &= mask;
    object_clear_excess_flag_bits(flags->m_storage, node_count);
}
static __forceinline void object_type_preprocess_root_node_matrix(long object_index, real_matrix4x3 *node_matrix)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    object_type_definition *definition = object_type_definitions[object->identifier_type];
    for (short type_index = 0; definition->parts[type_index]; type_index++) {
        object_type_definition *type = definition->parts[type_index];
        if (type->preprocess_root_node_matrix)
            type->preprocess_root_node_matrix(object_index, node_matrix);
    }
}
static __forceinline void objects_postprocess_biped_node_matrices(long object_index, long node_matrices_count, real_matrix4x3 *node_matrices)
{
    if (node_matrices && node_matrices_count > 0) {
        const s_object_definition *definition = (const s_object_definition *)tag_get('obje', ((object_datum
            *)object_get_and_verify_type(object_index, -1))->definition_index);
        if ((1 << definition->object_type) & 1)
            biped_adjust_node_matrices(object_index, node_matrices_count, node_matrices);
    }
}
// @address 0x31d8d0 ?object_compute_node_matrices_non_recursive@@YAXJ@Z
void object_compute_node_matrices_non_recursive(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    long node_matrices_count = object->node_matrices.size / sizeof(real_matrix4x3);
    real_matrix4x3 *node_matrices = (real_matrix4x3 *)((char *)object + object->node_matrices.offset);
    c_static_flags<255> simulation_controlled_nodes;
    bool success = false;
    objects_flags_set(&object->flags, 30, false);
    objects_flags_set(&object->flags, 14, false);
    simulation_controlled_nodes.clear();
    if (object->havok_component_index != -1 && objects_physics_flags_test(object, 7)) {
        if (!((1 << definition->object_type) & 1) || !objects_biped_flags_test(object_index, 9)) {
            c_havok_component *component = objects_havok_component_get(object->havok_component_index);
            float z_offset;
            if ((1 << definition->object_type) & 1)
                z_offset = biped_get_physics_model_node_matrix_z_offset(object_index);
            else
                z_offset = 0.0f;
            component->body_slam_object_node_matrices(&simulation_controlled_nodes, z_offset);
        }
    }
    if (definition->model.index != -1) {
        const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', definition->model.index);
        if (model->render_model_index != -1) {
            const render_model_definition *render_model = (const render_model_definition *)tag_get('mode', model->render_model_index);
            bool use_render_only_node_flags = true;
            const real_matrix4x3 *parent_matrix = NULL;
            bool parent_mirrored = false;
            bool world_relative = false;
            bool node_orientations_initialized = false;
            real_orientation *node_orientations;
            if (object_can_interpolate(object_index)) {
                node_orientations = (real_orientation *)((char *)object + object->node_orientations.offset);
            } else {
                node_orientations = object_get_node_orientations((s_objects_element_array *)render_model, node_matrices_count);
                node_orientations_initialized = true;
                use_render_only_node_flags = false;
            }
            if (object->parent_object_index != -1) {
                parent_matrix = objects_get_node_matrix(object->parent_object_index, object->parent_node_index);
                parent_mirrored = (bool)((((object_datum *)object_get_and_verify_type(object->parent_object_index, -1))->flags >> 10) & 1);
            }
            if (!objects_node_orientations_frozen(object_index)
                && !((1 << definition->object_type) & 1 && motor_system_is_relaxed(object_index) != 0 && !objects_biped_flags_test(object_index, 9))) {
                if (!node_orientations_initialized)
                    object_initialize_node_orientations((s_objects_element_array *)render_model, node_matrices_count, (char (*)[32])node_orientations);
                if (objects_has_animation_manager(object_index)) {
                    c_animation_manager *animation_manager = (c_animation_manager *)((char *)object + object->animation.offset);
                    if (animation_manager->m_graph_tag_index != -1) {
                        const c_static_flags<255> *node_mask = NULL;
#if defined(_XBOX) && !defined(__clang__)
                        c_static_flags<255> &render_node_flags = g_object_render_only_node_flags;
                        if (!(g_object_render_only_node_flags_initialization & 1))
                            g_object_render_only_node_flags_initialization |= 1;
#else
                        static c_static_flags<255> render_node_flags;
#endif
                        if (use_render_only_node_flags && !animation_node_flags_are_empty(&model->node_mask)) {
                            objects_build_render_only_node_flags(&render_node_flags, &model->node_mask, node_matrices_count);
                            node_mask = &render_node_flags;
                        }
                        internal_object_compute_animated_node_orientations(
                            object_index, 1.0f, render_model, animation_manager, node_mask, node_matrices_count, node_orientations, true);
                        objects_flags_set(&object->flags, 30, node_mask != NULL);
                        if (animation_manager->valid())
                            world_relative = objects_animation_manager_get_state_animation(animation_manager)->animation_is_world_relative();
                    }
                }
            }
            if (render_model->nodes.count == 0)
                node_orientations[0] = *global_identity_orientation;
            if (!simulation_controlled_nodes.test(0)) {
                if (world_relative) {
                    matrix4x3_from_orientation(&node_matrices[0], &node_orientations[0]);
                } else {
                    real_point3d position = object->position;
                    real_vector3d forward = object->forward;
                    real_vector3d up = object->up;
                    if (object->gamestate_index != -1 && (object->flags_d6 & 1)) {
                        real_point3d interpolated_position;
                        if (simulation_get_object_interpolated_position(object_index, &interpolated_position))
                            position = interpolated_position;
                        real_vector3d interpolated_forward;
                        real_vector3d interpolated_up;
                        if (simulation_get_object_interpolated_orientation(object_index, &interpolated_forward, &interpolated_up)) {
                            forward = interpolated_forward;
                            up = interpolated_up;
                        }
                    }
                    real_matrix4x3 origin_matrix;
                    object_compute_origin_matrix(object_index, &position, &forward, &up, object->scale, (bool)((object->flags >> 10) & 1),
                        parent_matrix, parent_mirrored, &origin_matrix);
                    matrix4x3_multiply_by_orientation(&origin_matrix, &node_orientations[0], &node_matrices[0]);
                }
                object_type_preprocess_root_node_matrix(object_index, &node_matrices[0]);
            }
            if (render_model->nodes.count > 1) {
                long prefetch_size = render_model->nodes.count * sizeof(real_matrix4x3);
                if ((unsigned long)prefetch_size > 1024)
                    prefetch_size = 1024;
                for (long offset = 0; offset < prefetch_size; offset += 128)
                    __dcbt(offset, node_matrices);
                if (simulation_controlled_nodes.is_clear()) {
                    const render_model_node *node = TAG_BLOCK_GET_ELEMENT(&render_model->nodes, 1, render_model_node);
                    for (long node_index = 1; node_index < render_model->nodes.count; node++, node_index++)
                        matrix4x3_multiply_by_orientation(&node_matrices[node->parent_node_index], &node_orientations[node_index],
                            &node_matrices[node_index]);
                } else {
                    const render_model_node *node = TAG_BLOCK_GET_ELEMENT(&render_model->nodes, 1, render_model_node);
                    for (long node_index = 1; node_index < render_model->nodes.count; node++, node_index++) {
                        if (!simulation_controlled_nodes.test(node_index))
                            matrix4x3_multiply_by_orientation(
                                &node_matrices[node->parent_node_index], &node_orientations[node_index], &node_matrices[node_index]);
                    }
                }
            }
            objects_postprocess_biped_node_matrices(object_index, node_matrices_count, node_matrices);
            success = true;
        }
    }
    if (!success) {
        const real_matrix4x3 *parent_matrix = NULL;
        bool parent_mirrored = false;
        if (object->parent_object_index != -1) {
            parent_matrix = objects_get_node_matrix(object->parent_object_index, object->parent_node_index);
            parent_mirrored = (bool)((((object_datum *)object_get_and_verify_type(object->parent_object_index, -1))->flags >> 10) & 1);
        }
        object_compute_origin_matrix(object_index, &object->position, &object->forward, &object->up, object->scale,
            (bool)((object->flags >> 10) & 1), parent_matrix, parent_mirrored, node_matrices);
    }
}
static __forceinline real_vector3d *objects_lw_transform_vector(const real_matrix4x3 *matrix, const real_vector3d *vector, real_vector3d *result)
{
    float x = vector->i;
    float y = vector->j;
    float z = vector->k;
    result->i = x * matrix->forward.i + y * matrix->left.i + z * matrix->up.i;
    result->j = x * matrix->forward.j + y * matrix->left.j + z * matrix->up.j;
    result->k = x * matrix->forward.k + y * matrix->left.k + z * matrix->up.k;
    return result;
}
static __forceinline void objects_lw_add_to_list(long *head, long object_index)
{
    ((s_object_header *)object_header_data->data)[(unsigned short)object_index].datum->next_object_index = *head;
    *head = object_index;
}
// @address 0x31b438 ?object_attach_to_node@@YAXJJF@Z
void object_attach_to_node(long parent_index, long child_index, short node_index)
{
    unsigned short absolute_child_index = (unsigned short)child_index;
    s_object_header *const &headers = (s_object_header *const &)object_header_data->data;
    object_datum *child = headers[absolute_child_index].datum;
    bool can_attach = true;
    if (child->parent_object_index != -1 || child->next_object_index != -1)
        can_attach = false;
    if (((1 << child->identifier_type) & 12) != 0 && ((item_datum *)child)->owner_relation != 0 && ((item_datum *)child)->unit_object_index !=
        parent_index)
        can_attach = false;
    for (long ancestor = parent_index; ancestor != -1; ancestor = headers[datum_index_to_absolute_index(ancestor)].datum->parent_object_index) {
        if (ancestor == child_index) {
            can_attach = false;
            break;
        }
    }
    if (can_attach) {
        s_object_header *child_header = &headers[absolute_child_index];
        object_datum *parent = headers[datum_index_to_absolute_index(parent_index)].datum;
        bool requires_motion = (bool)((child_header->flags >> 2) & 1);
        if ((bool)(_rotl(child->flags, 25) & 1))
            object_set_in_limbo(child_index, false);
        ::object_deactivate(child_index, true);
        if ((bool)(_rotl(child->flags, 24) & 1))
            object_disconnect_from_map(child_index, true);
        if ((bool)(_rotl(child->physics_flags, 25) & 1) && ((1 << child_header->object_type) & 128) == 0)
            object_disconnect_from_physics(child_index);
        const real_matrix4x3 *matrices = objects_get_node_matrix(parent_index, 0);
        child->parent_object_index = parent_index;
        child->parent_node_index = (char)node_index;
        long &first_child = parent->first_child_object_index;
        ((s_object_header *)object_header_data->data)[absolute_child_index].datum->next_object_index = first_child;
        first_child = child_index;
        child_header->flags |= 128;
        location_invalidate(&child->location);
        real_matrix4x3 inverse;
        matrix4x3_inverse(matrices + node_index, &inverse);
        real_point3d position;
        real_vector3d forward;
        real_vector3d up;
        matrix4x3_transform_point(&inverse, &child->position, &position);
        objects_lw_transform_vector(&inverse, &child->forward, &forward);
        objects_lw_transform_vector(&inverse, &child->up, &up);
        object_prepare_axis_vectors(child_index, &forward, &up);
        object_set_position_internal(child_index, &position, &forward, &up, 0, true, true, false, false);
        object_set_at_rest(child_index, true);
        if (object_is_connected_to_map(parent_index))
            object_connect_lights_recursive(child_index, false, true, false, false);
        object_compute_node_matrices(child_index);
        if (object_globals->current_object_update_absolute_index == absolute_child_index)
            child_header->flags |= 32;
        long root = inl_object_get_ultimate_parent(child_index);
        if (requires_motion && (object_header_get(root)->flags & 4) == 0)
            object_set_requires_motion(root);
        object_wake(parent_index);
        object_wake(child_index);
        object_compute_bounding_sphere(root);
    }
}
static __forceinline void objects_lw_marker_rotate(const real_matrix4x3 *matrix, const real_vector3d *vector, real_vector3d *result)
{
    result->i = matrix->forward.i * vector->i + matrix->left.i * vector->j + matrix->up.i * vector->k;
    result->j = matrix->forward.j * vector->i + matrix->left.j * vector->j + matrix->up.j * vector->k;
    result->k = matrix->forward.k * vector->i + matrix->left.k * vector->j + matrix->up.k * vector->k;
}
static __forceinline real_vector3d *transform_attachment_normal(const real_matrix4x3 *matrix, const real_vector3d *normal, real_vector3d *result)
{
    float k = normal->k;
    result->i = normal->i * matrix->forward.i + normal->j * matrix->left.i + k * matrix->up.i;
    result->j = normal->i * matrix->forward.j + normal->j * matrix->left.j + k * matrix->up.j;
    result->k = normal->i * matrix->forward.k + normal->j * matrix->left.k + k * matrix->up.k;
    return result;
}
// @address 0x31af60 ?object_attach_to_marker@@YAXJJJJ@Z
void object_attach_to_marker(long parent_index, long parent_marker_name, long child_index, long child_marker_name)
{
    object_datum *child = object_get(child_index);
    object_marker parent_marker;
    object_marker child_marker;
    ::object_deactivate(child_index, true);
    if ((bool)((child->flags >> 8) & 1))
        object_disconnect_from_map(child_index, true);
    internal_object_get_markers_by_string_id(parent_index, parent_marker_name, &parent_marker, 1, false);
    internal_object_get_markers_by_string_id(child_index, child_marker_name, &child_marker, 1, false);
    if (child_marker_name != -1 && child_marker_name != 0)
    {
        object_align_marker_to_matrix(child_index, &child_marker, &parent_marker.matrix);
    }
    else
    {
        real_matrix4x3 inverse;
        real_point3d position;
        real_vector3d forward;
        real_vector3d up;
        matrix4x3_inverse(&child_marker.node_to_marker, &inverse);
        matrix4x3_transform_point(&inverse, &parent_marker.matrix.position, &position);
        transform_attachment_normal(&inverse, &parent_marker.matrix.forward, &forward);
        transform_attachment_normal(&inverse, &parent_marker.matrix.up, &up);
        object_prepare_axis_vectors(child_index, &forward, &up);
        object_set_position_internal(child_index, &position, &forward, &up, NULL, true, true, false, false);
    }
    object_attach_to_node(parent_index, child_index, parent_marker.node_index);
}

// @address 0x31b370 object_attach_to_node_with_type
extern "C" void object_attach_to_node_with_type(long parent_index, long child_index, short node_index)
{
    object_datum *child = ((object_datum *)object_get_and_verify_type(child_index, -1));
    if (child->parent_object_index == -1) {
        object_type_definition *definition = object_type_definitions[child->identifier_type];
        object_type_definition *part_definition = definition->parts[0];
        object_type_definition **part = definition->parts;
        while (part_definition) {
            if (part_definition->attach_to_node) {
                part_definition->attach_to_node(parent_index, child_index, node_index);
                break;
            }
            part_definition = *++part;
        }
    }
    if (child->parent_object_index == -1)
        object_attach_to_node(parent_index, child_index, (short)node_index);
}


static __forceinline bool objects_lw_placement_is_multiplayer_cinematic_object(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    return (bool)((object->flags_d6 >> 1) & 1);
}

// @rdata 0x820221fc
static const float objects_placement_default_scale = 1.0f;
// @rdata 0x82029720..0x82029730
static const s_placement_owner_constants objects_placement_owner = { { -1, -1, -1, 0 }, 0.033333335f };
// @rdata-end
const s_damage_owner *damage_owner_unknown() { return &objects_placement_owner.unknown; }
static __forceinline const float &objects_placement_scale() { return objects_placement_default_scale; }
// match: 0x318f30: out-of-line memset calls for placement data and flag clears
#pragma function(memset)
// @address 0x318f30 ?object_placement_data_new@@YAXPAUobject_placement_data@@JJPBUs_damage_owner@@@Z
void object_placement_data_new(object_placement_data *data, long definition_index, long owner_object_index, const s_damage_owner *damage_owner)
{
    memset(data, 0, sizeof(*data));
    data->definition_index = definition_index;
    data->model_variant_index = 0;
    data->flags = 0;
    data->forward = *global_forward3d;
    data->up = *global_up3d;
    data->scale = 1.0f;
    data->active_change_colors = 0;
    data->destroyed_constraints = -1;
    char *policy = &data->bsp_placement_policy;
    *policy = _object_bsp_placement_policy_default;
    object_datum *owner = (object_datum *)object_try_and_get_and_verify_type(owner_object_index, ~0UL);
    if (owner)
    {
        data->owner_object_index = owner_object_index;
        data->player_index = -1;
        data->team_index = -1;
        if (objects_physics_flags_test(owner, 2))
            data->flags |= 32;
        else
            data->flags &= ~32;
        if (((1 << owner->identifier_type) & 0x1003) != 0)
        {
            data->player_index = ((unit_datum *)owner)->unit.player_index;
            data->team_index = ((unit_datum *)owner)->unit.team;
        }
    }
    else
    {
        data->owner_object_index = -1;
        data->player_index = -1;
        data->team_index = -1;
    }
    if (damage_owner)
        data->damage_owner = *damage_owner;
    else
        data->damage_owner = objects_placement_owner.unknown;
    data->object_identifier.clear();
    data->scenario_datum_index = -1;
    data->placed_by_script = false;
    if (owner_object_index != -1)
        data->multiplayer_cinematic_object = objects_lw_placement_is_multiplayer_cinematic_object(owner_object_index);
    data->location_bsp_index = -1;
    data->location_cluster_index = -1;
    data->location_leaf_index = -1;
    for (unsigned long i = 0; i < sizeof(data->region_permutations) / sizeof(data->region_permutations[0]); ++i)
    {
        data->region_permutations[i].region_name = -1;
        data->region_permutations[i].permutation_name = -1;
    }
    data->region_permutation_count = 0;
}


// @address 0x112918 ?objects_garbage_collection@@YAXXZ
void objects_garbage_collection(void)
{
    if (object_globals->garbage_collect_now)
        objects_recycling_update((e_recycling_methods)((object_globals->garbage_collect_aggressive != 0) + 1));
    else
        objects_recycling_update(_recycling_normal);
    object_globals->garbage_collect_now = false;
    object_globals->garbage_collect_aggressive = 0;
}


// @address 0x31e398 ?internal_object_compute_animated_node_orientations@@YAXJMPBUrender_model_definition@@PAVc_animation_manager@@PBV?$c_static_flags@$0PP@@@JPAUreal_orientation@@_N@Z
void internal_object_compute_animated_node_orientations(long object_index, float blend_ratio, const render_model_definition *render_model,
    c_animation_manager *animation_manager, const c_static_flags<255> *node_mask, long node_orientations_count,
    real_orientation *node_orientations, bool interpolate)
{
    if (node_mask != NULL) {
        long highest_bit = bit_vector_find_highest_bit((const unsigned long *)node_mask, node_orientations_count);
        if (highest_bit >= 0 && highest_bit + 1 < node_orientations_count)
            node_orientations_count = highest_bit + 1;
    }
    if (node_orientations_count != 0) {
        animation_manager->get_node_orientations(render_model, blend_ratio, node_mask, node_orientations_count, node_orientations, 0, object_index);
        object_apply_function_overlay_node_orientations(object_index, render_model, animation_manager, node_mask, node_orientations_count,
            node_orientations);
        object_type_preprocess_node_orientations(object_index, node_mask, node_orientations_count, node_orientations);
        if (interpolate && animation_manager->m_interpolator.enabled() && !animation_manager->m_interpolator.finished()) {
            object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
            object_type_preprocess_node_orientations(&animation_manager->m_interpolator, (short)node_orientations_count, node_mask,
                (const real_orientation *)((char *)object + object->original_node_orientations.offset), node_orientations);
        }
    }
}

// match: noalias declaration preserves caller live registers

// @address 0x318d78 ?object_apply_acceleration@@YAXJJPBTreal_point3d@@PBTreal_vector3d@@1@Z
void object_apply_acceleration(long object_index, long unknown, const real_point3d *position, const real_vector3d *linear_acceleration,
    const real_vector3d *angular_acceleration)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    if (objects_physics_flags_test(object, 4))
        object_in_early_mover_detach(object_index);
    long applied_object_index;
    real_vector3d linear_velocity;
    real_vector3d angular_velocity;
    if (havok_object_apply_acceleration(object_index, unknown, position, linear_acceleration, angular_acceleration, &applied_object_index,
            &linear_velocity, &angular_velocity))
    {

        object_datum *applied_object = ((object_datum *)object_get_and_verify_type(applied_object_index, -1));
        if (objects_physics_flags_test(applied_object, 4))
            object_in_early_mover_detach(applied_object_index);
        object_set_velocities(applied_object_index, &linear_velocity, &angular_velocity);
        if (linear_acceleration != NULL)
        {
            short t = applied_object->physics_reconnect_time;
            applied_object->physics_reconnect_time = t < 0 ? t : 0;
        }
        return;
    }
    object_get_velocities(object_index, &linear_velocity, &angular_velocity);
    if (linear_acceleration != NULL) {
        linear_velocity.i += linear_acceleration->i;
        linear_velocity.j += linear_acceleration->j;
        linear_velocity.k += linear_acceleration->k;
    }
    real_vector3d *angular_velocity_pointer;
    if (angular_acceleration != NULL) {
        angular_velocity.i += angular_acceleration->i;
        angular_velocity.j += angular_acceleration->j;
        angular_velocity.k += angular_acceleration->k;
        angular_velocity_pointer = &angular_velocity;
    } else {
        angular_velocity_pointer = NULL;
    }
    real_vector3d *linear_velocity_pointer = linear_acceleration != NULL ? &linear_velocity : NULL;
    object_set_velocities_internal(object_index, linear_velocity_pointer, angular_velocity_pointer, true);
    havok_object_set_velocities(object_index, linear_velocity_pointer, angular_velocity_pointer, true);
}


// @address 0x107688 ?objects_move@@YAXXZ
void objects_move()
{
    s_owner_table *early_movers = g_object_early_movers_globals;
    const long *owners = early_movers->owners;
    long owner_count = early_movers->owner_count;
    object_globals->objects_updating = true;
    for (long owner_index = 0; owner_index < owner_count; owner_index++)
    {
        long object_index = owners[owner_index];
        s_object_header *header = object_header_get(object_index);
        if ((header->flags & 4) != 0 && (header->flags & 8) == 0)
        {
            object_move(object_index);
            object_early_mover_move(object_index, owner_index);
        }
    }
    c_data_iterator_match<s_object_header> iterator;
    iterator.begin(object_header_data, 2, 12, 4);
    while (DATA_ITERATOR_MATCH_NEXT(iterator, s_object_header))
    {
        if ((iterator.m_datum->datum->physics_flags & 1) == 0)
            object_move(iterator.get_index());
    }
    weapons_fire_barrels();
    object_globals->objects_updating = false;
}


// @address 0xd6028 ?objects_update@@YAXXZ
void objects_update()
{
    s_owner_table *early_movers = g_object_early_movers_globals;
    const long *owners = early_movers->owners;
    long owner_count = early_movers->owner_count;
    object_globals->objects_updating = true;
    object_globals->current_object_update_absolute_index = 0;
    for (long owner_index = 0; owner_index < owner_count; owner_index++)
    {
        long object_index = owners[owner_index];
        s_object_header *header = object_header_get(object_index);
        if ((header->flags & 1) != 0 && (header->flags & 2) != 0 && (header->flags & 8) == 0)
        {
            object_update(object_index);
            object_early_mover_update_state(object_index, owner_index);
            if ((header->flags & 4) != 0 && (header->flags & 8) == 0)
                object_early_mover_notify_local_objects(object_index);
        }
    }
    c_data_iterator_match<s_object_header> iterator;
    iterator.begin(object_header_data, 2, 11, 3);
    while (DATA_ITERATOR_MATCH_NEXT(iterator, s_object_header))
    {
        long object_index = iterator.get_index();
        if ((object_get(object_index)->physics_flags & 1) == 0)
        {
            object_globals->current_object_update_absolute_index = (short)object_index;
            object_update(object_index);
        }
    }
    object_globals->current_object_update_absolute_index = -1;
    object_globals->objects_updating = false;
}

// @address 0x1108a8 ?objects_post_update@@YAXXZ
void objects_post_update()
{
    object_globals->objects_updating = true;
    c_data_iterator<s_object_header> iterator;
    iterator.begin(object_header_data);
    while (iterator.next())
    {
        long object_index = iterator.get_index();
        s_object_header *header = (s_object_header *)iterator.get_datum();
        bool was_ = (header->flags & 8) != 0;
        header->flags &= ~0x20;
        header->flags &= ~0x08;
        if (was_ && (header->flags & 1) != 0 && (header->flags & 2) != 0 && (header->flags & 0x10) == 0)
        {
            object_update(object_index);
            if ((header->flags & 4) != 0)
                object_move(object_index);
        }
    }
    weapons_fire_barrels();
    ::objects_purge_deleted_objects();
    object_globals->objects_updating = false;
    objects_garbage_collection();
}



extern "C" bool object_physics_update(long object_index)
{
    bool result = false;
    object_datum *object = object_get(object_index);
    if ((bool)(_rotl(object->physics_flags, 19) & 1) && object->havok_component_index != -1) {
        havok_component_update_phantom_state(objects_havok_component_get(object->havok_component_index), true);
        result = true;
    }
    if (!(bool)(_rotl(object->physics_flags, 23) & 1)) {
        result = true;
        object->last_motion_time = game_time_get();
    }
    if ((bool)(_rotl(object->flags, 24) & 1))
        object_in_early_mover_physics_update(object_index);
    if (!game_simulation::game_is_predicted() && ((crate_datum *)object)->lifetime_ticks != -1) {
        if (--((crate_datum *)object)->lifetime_ticks <= 0) {
            object_delete(object_index);
            return false;
        }
    }
    return result;
}


void object_set_initial_change_colors(long object_index, unsigned long flags, const real_rgb_color *colors)
{
    object_datum *object = object_get(object_index);
    s_object_definition *definition = (s_object_definition *)tag_get('obje', object->definition_index);
    real_rgb_color *base = (real_rgb_color *)((char *)object + object->change_colors.offset);
    long count = (unsigned long)object->change_colors.size / 12;
    count /= 2;
    for (long i = 0; i < count; ++i)
    {
        s_object_change_color_definition *entry = &((s_object_change_color_definition *)definition->change_colors.address)[i];
        if (flags & (1UL << i))
            base[i] = colors[i];
        else
        {
            base[i] = objects_get_default_argb_white()->rgb;
            if (entry->initial_permutations.count > 0)
            {
                long last = -1;
                unsigned long allowed = 0;
                long variant_name = 0;
                float total = 0.0f;
                float random_choice = (float)i * 431.12894f +
                    (object->position.y * 587.12946f + (object->position.z * 744.12415f + object->position.x * 315.89313f));
                float random_color = (float)i * 0.71211f + object->position.y;
                random_choice = (float)fmod((double)fabsf(random_choice), 1.0);
                random_color = (float)fmod((double)fabsf(random_color), 1.0);
                if (object->variant_index != -1 && definition->model.index != -1)
                {
                    s_model_definition *model = (s_model_definition *)tag_get('hlmt', definition->model.index);
                    variant_name = ((s_model_variant *)model->variants.address)[object->variant_index].name;
                }
                for (long j = 0; j < entry->initial_permutations.count; ++j)
                {
                    s_object_change_color_initial_permutation *variant = &((s_object_change_color_initial_permutation
                        *)entry->initial_permutations.address)[j];
                    if (variant->variant_name == 0 || variant->variant_name == variant_name)
                    {
                        allowed |= 1 << j;
                        last = j;
                        total += variant->weight;
                    }
                }
                if (allowed && total > 0.0f)
                {
                    float cumulative = 0.0f;
                    random_choice *= total;
                    for (long j = 0; j < entry->initial_permutations.count; ++j)
                    {
                        s_object_change_color_initial_permutation *variant = &((s_object_change_color_initial_permutation
                            *)entry->initial_permutations.address)[j];
                        if (allowed & (1 << j))
                        {
                            float weight = variant->weight;
                            float previous_weight = cumulative;
                            cumulative = previous_weight + weight;
                            if (j == last || cumulative >= random_choice)
                            {
                                rgb_colors_interpolate(&base[i], 1, &variant->color_a, &variant->color_b, random_color);
                                break;
                            }
                        }
                    }
                }
            }
        }
        base[i].red = ((base[i].red > 0.0f ? base[i].red : 0.0f) < 1.0f ? (base[i].red > 0.0f ? base[i].red : 0.0f) : 1.0f);
        base[i].green = ((base[i].green > 0.0f ? base[i].green : 0.0f) < 1.0f ? (base[i].green > 0.0f ? base[i].green : 0.0f) : 1.0f);
        base[i].blue = ((base[i].blue > 0.0f ? base[i].blue : 0.0f) < 1.0f ? (base[i].blue > 0.0f ? base[i].blue : 0.0f) : 1.0f);
        base[count + i] = base[i];
    }
}

// @address 0x31cf70 ?object_update@@YA_NJ@Z
bool object_update(long object_index)
{
    s_object_header *header = object_header_get(object_index);
    bool result = false;
    if (header->flags & 2)
    {
        if (header->flags & 0x20)
        {
            result = true;
        }
        else
        {
            object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
            header->flags &= ~4;
            if (objects_physics_flags_test(object, 2))
                object_set_requires_motion(object_index);
            bool simulate = false;
            if ((unsigned long)(game_globals->options.game_simulation - 4) <= 1)
                simulate = true;
            if (simulate && (object->flags_d6 & 1))
                simulation_update_object_interpolation(object_index);
            if (objects_has_animation_manager(object_index))
            {
                c_animation_manager *manager = (c_animation_manager *)((char *)object + object->animation.offset);
                if (manager->m_graph_tag_index != -1)
                    result = animation_manager_update_interpolator(manager);
            }
            result |= object_type_update(object_index);
            result |= object_damage_update(object_index);
            result |= object_compute_change_colors(object_index);
            result |= water_physics_update(object_index);
            for (long child_index = object->first_child_object_index; child_index != -1;
                child_index = ((object_datum *)object_get_and_verify_type(child_index, -1))->next_object_index)
                result |= object_update(child_index);
            if (object->havok_component_index != -1 && objects_physics_flags_test(object, 7))
            {
                c_havok_component *component = objects_havok_component_get(object->havok_component_index);
                component->pre_simulation_update();
                if (!objects_physics_flags_test(object, 9) && !(header->flags & 4))
                    object_set_requires_motion(object_index);
            }
            if (havok_crate_needs_physics_update(object_index))
                simulation_action_object_update(object_index, 0x4000);
        }
        attachments_update(object_index);
        if (!result && !(header->flags & 4))
            header->flags &= ~2;
    }
    return result;
}






// @address 0xebb20 ?object_animation_callback@@YAXPBUs_animation_event_data@@J@Z
void object_animation_callback(const s_animation_event_data *event_data, long user_param)
{
    if (event_data->event_type == 1 && event_data->definition_index != -1)
    {
        long player_index = -1;
        if (((1 << object_get(user_param)->identifier_type) & 0x1003) != 0)
            player_index = objects_player_index_from_unit_index(user_param);
        if (((event_data->flags & 1) != 0) || player_index == -1)
        {
            object_marker marker;
            long marker_name = event_data->marker_name;
            if (marker_name == -1 || marker_name == 146
                || internal_object_get_markers_by_string_id(user_param, marker_name, &marker, 1, false) == 0)
            {
                marker.node_index = 0;
                marker.node_to_marker.position = *global_origin3d;
                marker.node_to_marker.forward = *global_forward3d;
            }
            object_impulse_sound_new(user_param, event_data->definition_index, marker.node_index,
                &marker.node_to_marker.position, &marker.node_to_marker.forward, 1.0f);
        }
    }
    else if (event_data->event_type == 2 && event_data->definition_index != -1)
    {
        s_damage_owner owner;
        damage_compute_damage_owner_from_object_index(user_param, &owner);
        if (event_data->marker_name == -1 || event_data->marker_name == 0)
        {
            long tag_storage;
            c_tag_index *tag = new (&tag_storage) c_tag_index(event_data->definition_index);
            effect_new_from_object(*tag, &owner, 104, user_param, 0.0f, 0.0f, 0, 0, 1);
        }
        else
        {
            long tag_storage;
            c_tag_index *tag = new (&tag_storage) c_tag_index(event_data->definition_index);
            ::effect_new_attached_from_marker_name(*tag, &owner, 104, user_param, -1, event_data->marker_name,
                0.0f, 0.0f, 0, 0, 0, 1);
        }
    }
    else if (event_data->event_type == 3
        && (short)event_data->dialogue_index >= 0
        && (short)event_data->dialogue_index < 9
        && ((1 << object_header_get(user_param)->object_type) & 0x1003) != 0)
    {
        short scream_type = -1;
        switch ((short)event_data->dialogue_index)
        {
        case 0:
            scream_type = 20;
            break;
        case 1:
            scream_type = 18;
            break;
        case 2:
            scream_type = 19;
            break;
        case 3:
            scream_type = 17;
            break;
        case 4:
            scream_type = 22;
            break;
        case 5:
            scream_type = 23;
            break;
        case 6:
            scream_type = 24;
            break;
        case 7:
            scream_type = 25;
            break;
        case 8:
            scream_type = 26;
            break;
        }
        if (scream_type == -1)
            return;
        unit_dialogue_test_scream(user_param, scream_type, -1, 11, 0);
    }
    else
    {
        if (object_header_get(user_param)->object_type == 0)
            biped_handle_footfall_events(user_param, event_data);
    }
}

#ifndef PIN
#endif



static __forceinline bool objects_user_has_player(long user_index)
{
    bool result = false;
    if (user_index != -1)
        result = player_mapping_globals->player_indices[user_index] != -1;
    return result;
}
static __forceinline float objects_observer_distance_squared(const object_datum *object, const s_player_block *observer)
{
    real_vector3d delta;
    delta.j = 0.0f;
    delta.k = 0.0f;
    delta.i = object->bounding_center.x - observer->camera_result.position.x;
    delta.j = object->bounding_center.y - observer->camera_result.position.y;
    delta.k = object->bounding_center.z - observer->camera_result.position.z;
    return magnitude_squared3d(&delta) * observer->camera_result.field_of_view_scale;
}

// @address 0x449550 object_compute_animation_blend
extern "C" bool object_compute_animation_blend(long object_index, float inner_radius, float outer_radius, float *fade)
{
    object_datum *object = object_get(object_index);
    float inner_distance = object->bounding_radius + inner_radius;
    float outer_distance = object->bounding_radius + outer_radius;
    float inner_distance_squared = inner_distance * inner_distance;
    float outer_distance_squared = outer_distance * outer_distance;
    float closest_distance_squared = 3.402823466e+38f;
    for (long user_index = 0; user_index < 4; user_index++)
    {
        if (objects_user_has_player(user_index))
        {
            const s_player_block *observer = &observer_get_globals()->blocks[user_index];
            float distance_squared = objects_observer_distance_squared(object, observer);
            if (distance_squared < closest_distance_squared)
                closest_distance_squared = distance_squared;
        }
    }
    objects_render_texture_camera camera;
    if (render_texture_camera_try_and_get(1, &camera, 0))
    {
        real_vector3d delta;
        vector_from_points3d(&camera.position, &object->bounding_center, &delta);
        float distance_squared = magnitude_squared3d(&delta);
        if (closest_distance_squared > distance_squared)
            closest_distance_squared = distance_squared;
    }
    bool visible = closest_distance_squared <= outer_distance_squared;
    if (fade)
    {
        *fade = visible ? 1.0f : 0.0f;
        if (closest_distance_squared >= inner_distance_squared)
        {
            float range = outer_distance_squared - inner_distance_squared;
            if (range > 0.0001f)
            {
                *fade = (closest_distance_squared - inner_distance_squared) / range;
                *fade = PIN(1.0f - *fade, 0.0f, 1.0f);
            }
        }
    }
    return visible;
}

static __forceinline void objects_identifier_create_dynamic(c_object_identifier *identifier, e_object_type type)
{
    ++object_globals->object_identifier_counter;
    identifier->m_type = (char)type;
    identifier->m_source = 2;
    identifier->m_origin_bsp_index = -1;
    identifier->m_unique_id = object_globals->object_identifier_counter;
}
static __forceinline void objects_name_placement_info(long table_offset, long *offset, short *size)
{
    // match: 0x449550: table-offset range assumption guides the definition load
    __assume(table_offset >= 0 && table_offset < 14 * sizeof(object_type_definition *));
    object_type_definition *const *slot = (object_type_definition *const *)(
        (const char *)object_type_definitions + table_offset);
    object_type_definition *definition = *slot;
    *offset = (short)definition->scenario_datums_offset;
    *size = definition->scenario_datum_size;
}
static __forceinline s_tag_block *objects_name_scenario_datums(scenario *current_scenario, long table_offset, short *datum_size)
{
    long offset;
    short size;
    objects_name_placement_info(table_offset, &offset, &size);
    if (datum_size) *datum_size = size;
    return (s_tag_block *)((char *)current_scenario + offset);
}
static __forceinline s_tag_block *objects_scenario_get_type_palette(scenario *current_scenario, long table_offset)
{
    object_type_definition *definition = *(object_type_definition *const *)(
        (const char *)object_type_definitions + table_offset);
    long offset = (short)definition->scenario_palette_offset;
    return (s_tag_block *)((char *)current_scenario + offset);
}
static __forceinline long objects_name_lookup_for_create(short name)
{
    if (name >= 0 && name < 2048)
        return ((long *)g_object_name_list)[name];
    return -1;
}
static __forceinline long objects_name_element_offset(const short &element_size, const long &index)
{
    return element_size * index;
}
// @address 0x31c860 ?object_new_by_name@@YAJF_N0@Z
long object_new_by_name(short name_index, bool clone, bool cinematic)
{
    long placement_index;
    long object_index = objects_name_lookup_for_create(name_index);
    long result = -1;
    if (clone)
    {
        if (object_index != -1)
        {
            object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, ~0ul));
            e_object_type type = (e_object_type)object_header_get(object_index)->object_type;
            object_name_list_delete(object_index);
            object_index = -1;
            objects_unregister_scenario_object(object_index);
            object->name_index = -1;
            objects_identifier_create_dynamic(&object->identifier, type);
            object->bsp_placement_policy = 0;
        }
    }
    if (object_index == -1)
    {
        const s_scenario_object_name *name = &((const s_scenario_object_name *)global_scenario->object_names.address)[name_index];
        placement_index = name->placement_index;
        if (placement_index != -1)
        {
            scenario *current_scenario = global_scenario;
            e_object_type type = (e_object_type)name->object_type;
            long table_offset = type * sizeof(object_type_definition *);
            short datum_size;
            s_tag_block *datums = objects_name_scenario_datums(current_scenario, table_offset, &datum_size);
            s_tag_block *palette = objects_scenario_get_type_palette(current_scenario, table_offset);
            long placement_offset = objects_name_element_offset(datum_size, placement_index);
            char *scenario_address = (char *)datums->address;
            s_scenario_object *scenario_object = (s_scenario_object *)(scenario_address + placement_offset);
            if (cinematic)
                result = object_new_from_scenario_internal(type, (short)placement_index, scenario_object, palette, -1, false, false, cinematic);
            else
                result = object_new_from_scenario_internal(type, (short)placement_index, scenario_object, palette, -1, true, false, false);
        }
    }
    return result;
}


// Initialize the attached light datum.


static __forceinline long light_new_attached(long definition_index, long object_index, short attachment_index, long attachment_flags, short
    attachment_color)
{
    long light_index = datum_new(*(s_data_array **)&light_data);
    if (light_index != -1) {
        light_datum *light = datum_get<light_datum>(light_data, light_index);
        light->initial_scale = 1.0f;
        light->definition_index = definition_index;
        light->scale = 1.0f;
        light->creation_time = -1;
        light->scenario_index = -1;
        light->attachment_object_index = object_index;
        light->attachment_index = attachment_index;
        light->attachment_flags = attachment_flags;
        light->change_color = attachment_color;
        light->owner_object_index = -1;
        light->owner_node_index = -1;
        light->flags = 1;
        light->first_cluster_reference_index = -1;
        light_update_state(light_index);
        light->reservede0 = -1;
    }
    return light_index;
}

// @address 0x31f558 ?attachments_new@@YAXJ@Z
void attachments_new(long object_index)
{
    object_datum *object = object_get(object_index);
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    if (definition->attachments.count > 0) {
        s_objects_attached_light *slots = (s_objects_attached_light *)((char *)object + object->attached_lights.offset);
        for (long i = 0; i < definition->attachments.count; i++) {
            const s_object_attachment_definition *attachment = &((s_object_attachment_definition *)definition->attachments.address)[i];
            short kind = -1;
            long index = -1;
            long definition_index = attachment->type.index;
            if (definition_index != -1) {
                unsigned long group_tag = (unsigned long)attachment->type.group_tag;
                switch (group_tag) {
                case 'ligh':
                    kind = 0;
                    break;
                case 'lsnd':
                    kind = 1;
                    break;
                case 'effe':
                    kind = 2;
                    break;
                case 'lens':
                    kind = 3;
                    break;
                }
            }
            switch (kind) {
            case 0:
                index = light_new_attached(definition_index, object_index, (short)i, attachment->primary_scale, attachment->change_color);
                if (index != -1)
                    object->flags |= 16;
                break;
            case 1:
                index = game_looping_sound_attachment_new(object_index, definition_index, i, attachment->primary_scale);
                if (index != -1)
                    object->flags |= 32;
                break;
            }
            slots[i].type = (unsigned char)kind;
            slots[i].light_index = index;
        }
    }
    if (((1 << object_header_get(object_index)->object_type) & 0x2000) == 0)
        attachments_update(object_index);
}









static __forceinline long objects_attachments_effect_new_looping(s_attachment_effect_parameters parameters,
    const s_damage_owner *damage_owner, long damage_reporting_type, long object_index)
{
    s_effect_creation_data creation_data;
    creation_data.m_flags.clear();
    void (&initialize_creation_data)(s_effect_creation_data *) = effect_creation_data_initialize;
    initialize_creation_data(&creation_data);
    creation_data.definition_index = parameters.definition_index;
    creation_data.structure_bsp_index = object_index;
    creation_data.m_flags.set(_effect_creation_flag1, true);
    creation_data.m_flags.set(_effect_creation_deterministic_bit, true);
    creation_data.m_flags.set(_effect_creation_node_bit, true);
    creation_data.parent_effect_index = parameters.marker_name;
    creation_data.lightprobe_slot_index = parameters.primary_scale;
    creation_data.user_mask = parameters.secondary_scale;
    creation_data.first_person_user_index = parameters.change_color;
    creation_data.damage_reporting_type = damage_reporting_type;
    real_point3d origin;
    s_effect_vector vectors[2];
    object_get_origin(object_index, &origin);
    build_gravity_effect_vectors(&origin, vectors);
    creation_data.effect_vectors = vectors;
    creation_data.effect_vector_count = 2;
    if (damage_owner)
        creation_data.damage_owner = *damage_owner;
    return effect_new_from_creation_data(&creation_data);
}

// @address 0x31f860 ?attachments_update@@YAXJ@Z
void attachments_update(long object_index)
{
    object_datum *object = object_get(object_index);
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    if (definition->attachments.count > 0) {
        long attachment_count;
        s_objects_attached_light *attachments = (s_objects_attached_light *)objects_object_header_block_get_with_count(
            object, &object->attached_lights, sizeof(s_objects_attached_light), &attachment_count);
        for (long attachment_index = 0; attachment_index < definition->attachments.count; ++attachment_index) {
            s_objects_attached_light *attachment = &attachments[attachment_index];
            if (attachment->type == 2) {
                const s_object_attachment_definition *attachment_definition = &((s_object_attachment_definition
                    *)definition->attachments.address)[attachment_index];
                float magnitude;
                bool deterministic;
                bool active = attachment_definition->primary_scale == 0 || object_get_function_value(
                    object_index, attachment_definition->primary_scale, object->definition_index, &magnitude, &deterministic);
                if (attachment->light_index == -1) {
                    if (active) {
                        s_damage_owner damage_owner = object_get(object_index)->damage_owner;
                        // match: 0x31f860: byte local moves the reporting byte with mr; a long local inserts clrlwi zero-extend
                        unsigned char damage_reporting_type = _damage_reporting_type_unknown;
                        if ((1 << object->identifier_type) & 0x20)
                            damage_reporting_type = object_get_damage_reporting_type(object_index);
                        attachment->light_index = objects_attachments_effect_new_looping(
                            s_attachment_effect_parameters(attachment_definition), &damage_owner,
                            damage_reporting_type, object_index);
                    }
                } else if (!active) {
                    effect_delete_internal(attachments[attachment_index].light_index, false);
                    attachments[attachment_index].light_index = -1;
                }
            }
        }
    }
}

// ---- 0x31f088 object_create_children dependencies (hlmt variant children) ----
                              // 28

                                                           // 56



static __forceinline void objects_identifier_create_from_parent(c_object_identifier *identifier, long object_type)
{
    object_globals->object_identifier_counter++;
    identifier->m_type = (char)object_type;
    identifier->m_source = 5;
    identifier->m_origin_bsp_index = -1;
    identifier->m_unique_id = object_globals->object_identifier_counter;
}

enum e_simulation_entity_type;

static __forceinline void objects_get_damage_owner(long object_index, s_damage_owner *out)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    *out = object->damage_owner;
}

// @address 0x31f088 ?object_create_children@@YAXJ@Z
void object_create_children(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    if (object->variant_index != -1) {
        const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
        long child_index = 0;
        const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', definition->model.index);
        const s_model_variant *variant = &((s_model_variant *)model->variants.address)[object->variant_index];
        if (variant->attachments.count > 0) {
            for (; child_index < variant->attachments.count; child_index++) {
                const s_model_variant_attachment *child = &((s_model_variant_attachment *)variant->attachments.address)[child_index];
                long child_definition_index = child->child_object.index;
                if (child_definition_index != -1) {
                    bool create = true;
                    if (game_globals->options.game_mode == 2
                        && game_is_predicted()
                        && (long)simulation_entity_type_from_object_creation(child_definition_index, object_index, false) != -1)
                        create = false;
                    if (create) {
                        object_datum *ultimate_object = ((object_datum *)object_get_and_verify_type(object_get_ultimate_parent(object_index), -1));
                        const s_object_definition *child_definition = (const s_object_definition *)tag_get('obje', child_definition_index);
                        s_damage_owner damage_owner;
                        object_placement_data placement;
                        objects_get_damage_owner(object_index, &damage_owner);
                        object_placement_data_new(&placement, child->child_object.index, object_index, &damage_owner);
                        placement.model_variant_index = child->child_variant;
                        objects_identifier_create_from_parent(&placement.object_identifier, child_definition->object_type);
                        object_get_origin(object_index, &placement.position);
                        if ((bool)((ultimate_object->physics_flags >> 9) & 1))
                            placement.flags |= 8;
                        else
                            placement.flags &= ~8;
                        long child_object_index = object_new(&placement);
                        if (child_object_index != -1) {
                            object_datum *child_object = ((object_datum *)object_get_and_verify_type(child_object_index, -1));
                            object_attach_to_marker_with_type(object_index, child->parent_marker, child_object_index, child->child_marker);
                            child_object->marker = (signed char)child_index;
                        }
                    }
                }
            }
        }
    }
}




static __forceinline void objects_soft_ceiling_add_velocity(
    const real_vector3d *velocity, const real_vector3d *normal, const float &scale, real_vector3d *result)
{
    real_vector3d impulse;
    impulse.i = normal->i * scale;
    impulse.j = normal->j * scale;
    impulse.k = normal->k * scale;
    result->i = velocity->i + impulse.i;
    result->j = velocity->j + impulse.j;
    result->k = velocity->k + impulse.k;
}

static __forceinline void objects_soft_ceiling_blend(const real_vector3d *a, float a_scale, const real_vector3d *b, float b_scale, real_vector3d *result)
{
    real_vector3d scaled_a, scaled_b;
    scaled_b.i = b->i * b_scale;
    scaled_b.j = b->j * b_scale;
    scaled_b.k = b->k * b_scale;
    scaled_a.i = a->i * a_scale;
    scaled_a.j = a->j * a_scale;
    scaled_a.k = a->k * a_scale;
    result->i = scaled_a.i + scaled_b.i;
    result->j = scaled_a.j + scaled_b.j;
    result->k = scaled_a.k + scaled_b.k;
}

static __forceinline float objects_soft_ceiling_speed_squared(const real_vector3d *v)
{
    return v->k * v->k + v->j * v->j + v->i * v->i;
}

inline float objects_soft_ceiling_dot(const real_vector3d *a, const real_vector3d *b)
{
    return a->i * b->i + a->j * b->j + a->k * b->k;
}

// @rdata 0x820221fc
static const float objects_soft_ceiling_one = 1.0f;
// @rdata 0x820242d8
static const float objects_soft_ceiling_two = 2.0f;
// @rdata 0x820242d4
static const float objects_soft_ceiling_three = 3.0f;
// @rdata 0x82032900
static const float objects_soft_ceiling_twenty = 20.0f;
// @rdata 0x820397e4
static const float objects_soft_ceiling_six = 6.0f;
// @rdata 0x8202af7c
static const float objects_soft_ceiling_thirty = 30.0f;

// @address 0xefd18 object_update_soft_ceiling
extern "C" bool object_update_soft_ceiling(long object_index, real_point3d *position)
{
    bool result = false;
    object_datum *object = object_get(object_index);
    if (!(bool)((object->damage_flags >> 2) & 1)) {
        long parent_index = objects_center_get_ultimate_parent(object_index);
        object_datum *parent = object_get(parent_index);
        long type = object_header_get(parent_index)->object_type;
        const vehicle_definition *definition = (const vehicle_definition *)tag_get('vehi', parent->definition_index);
        long type_mask = 1 << type;
        float acceleration = (unsigned long)(type_mask & 1) ? objects_soft_ceiling_thirty : objects_soft_ceiling_twenty;
        float perpendicular_damping = (unsigned long)(type_mask & 1) ? objects_soft_ceiling_two : objects_soft_ceiling_one;
        float parallel_damping = (unsigned long)(type_mask & 1) ? 8.0f : 4.0f;
        bool was_touching = (bool)((parent->physics_flags >> 19) & 1);
        bool grounded = false;
        bool vehicle_flag = false;
        if (type_mask & 2) {
            // match: 0xefd18: const reference binding preserves temporary lifetime
            const unsigned long &vehicle_flag_value = (unsigned char)((definition->flags >> 11) & 1);
            vehicle_flag = (vehicle_flag_value & 1) != 0;
        }
        real_vector3d velocity;
        object_get_velocities(parent_index, &velocity, 0);
        if (object->parent_object_index == -1 && ((1 << object_header_get(object_index)->object_type) & 1))
            grounded = ((biped_datum *)object_get(object_index))->biped.physics.m_mode == 1;
        real_point3d center;
        real_point3d origin;
        object_get_center_of_mass(parent_index, &center);
        object_get_origin(parent_index, &origin);
        real_vector3d displacement;
        vector_from_points3d(&center, position, &displacement);
        real_vector3d original_velocity;
        original_velocity.i = velocity.i;
        original_velocity.k = velocity.k;
        original_velocity.j = velocity.j;
        float distance = sqrtf(magnitude_squared3d(&displacement)) * objects_soft_ceiling_two;
        distance = MAX(distance, sqrtf(objects_soft_ceiling_speed_squared(&original_velocity)) *
                                 game_time_globals->tick_length * objects_soft_ceiling_two);
        *position = center;
        float maximum_distance = (unsigned long)(type_mask & 1) ? objects_soft_ceiling_six : 10.0f;
        float minimum_distance = (unsigned long)(type_mask & 1) ? objects_soft_ceiling_one : objects_soft_ceiling_three;
        // match: 0xefd18: const reference binding preserves temporary lifetime
        const unsigned long &was_touching_value = (unsigned char)was_touching;
        if (was_touching_value)
            minimum_distance = maximum_distance;
        minimum_distance = MIN(MAX(minimum_distance, distance), maximum_distance);
        objects_soft_ceiling_results contacts;
        unsigned long hit_mask;
        scenario_soft_ceilings_query_triangles_in_sphere(
            &contacts, &hit_mask, type, vehicle_flag, definition->bounding_radius, &origin, &center, grounded, minimum_distance);
        if (contacts.count > 0) {
            objects_soft_ceiling_indices push_indices;
            objects_soft_ceiling_indices pull_indices;
            real_vector3d push_normal;
            real_vector3d pull_normal;
            float push_height, pull_height;
            float push_magnitude, pull_magnitude;
            if (scenario_soft_ceilings_compute_collision_normal(&contacts, &center, &push_indices, &push_normal, &push_height, &push_magnitude, false) &&
                (!scenario_soft_ceilings_compute_collision_normal(&contacts, &center, &pull_indices, &pull_normal, &pull_height, &pull_magnitude, true) ||
                 push_indices.count > pull_indices.count || push_magnitude < pull_magnitude)) {
                float timestep = game_time_globals->tick_length;
                real_vector3d new_velocity;
                float impulse_scale = (timestep * acceleration) * push_height;
                objects_soft_ceiling_add_velocity(&original_velocity, &push_normal, impulse_scale, &new_velocity);
                float perpendicular_scale = objects_soft_ceiling_one - timestep * perpendicular_damping;
                real_vector3d parallel;
                float projection = objects_soft_ceiling_dot(&new_velocity, &push_normal);
                parallel.i = push_normal.i * projection;
                float parallel_scale = objects_soft_ceiling_one - timestep * parallel_damping;
                parallel.j = push_normal.j * projection;
                parallel.k = push_normal.k * projection;
                real_vector3d perpendicular;
                perpendicular.i = new_velocity.i - parallel.i;
                perpendicular.j = new_velocity.j - parallel.j;
                perpendicular.k = new_velocity.k - parallel.k;
                objects_soft_ceiling_blend(&perpendicular, perpendicular_scale, &parallel, parallel_scale, &new_velocity);
                object_set_velocities(parent_index, &new_velocity, 0);
                result = true;
            }
        }
        objects_flags_set(&parent->physics_flags, 19, result);
        if (hit_mask & 2) {
            unit_datum *unit = (unit_datum *)object_get(object_index);
            long minimum_ticks = (long)((float)game_time_globals->ticks_per_second * objects_soft_ceiling_three + 0.5f);
            long maximum_ticks = (long)((float)game_time_globals->ticks_per_second * objects_soft_ceiling_six + 0.5f);
            long elapsed = unit->unit.soft_ceiling_contact_time == -1 ? -1 : game_time_get() - unit->unit.soft_ceiling_contact_time;
            if (elapsed == -1 || elapsed > maximum_ticks)
                unit->unit.soft_ceiling_contact_time = game_time_get();
            else if (elapsed >= minimum_ticks && object_index != -1) {
                object_get(object_index)->damage_flags |= 64;
                object_wake(object_index);
            }
        }
        if (hit_mask & 4) {
            biped_datum *biped = (biped_datum *)object_get(object_index);
            if (biped->biped.physics.m_mode == 1)
                biped->biped.physics.get_mode_ground()->notify_in_slip_surface_volume();
        }
    }
    return result;
}







static __forceinline void *objects_new_pool_block_pointer(s_memory_pool *pool, long handle)
{
    return handle ? (char *)pool + handle : 0;
}

static __forceinline long objects_new_header(short size)
{
    long index = datum_new(object_header_data);
    if (index != -1) {
        s_object_header *header = object_header_get(index);
        header->handle = memory_pool_block_allocate_handle(g_object_memory_pool, index, size, 0, 0);
        if (header->handle) {
            header->datum = (object_datum *)objects_new_pool_block_pointer(g_object_memory_pool, header->handle);
            header->data_size = size;
            memset(header->datum, 0, size);
        } else {
            datum_delete(object_header_data, index);
            index = -1;
        }
    }
    return index;
}

static __forceinline bool objects_new_definition_can_be_placed(long definition_index)
{
    bool available = true;
    available = available && scenario_active_zone_is_tag_available(definition_index);
    return available;
}

static __forceinline bool objects_new_validate_graph(long graph_index, long model_index)
{
    bool valid = false;
    if (model_index != -1) {
        valid = ((const c_model_animation_graph *)
            global_tag_instances[datum_index_to_absolute_index(graph_index)].definition)->get_node_count() ==
            ((const s_model_definition *)
            global_tag_instances[datum_index_to_absolute_index(model_index)].definition)->nodes.count;
    }
    return valid;
}

static __forceinline void objects_new_register_scenario_object(long object_index)
{
    if (game_globals->options.game_mode != 2) {
        object_datum *object = object_get(object_index);
        long name_index = object->name_index;
        if (name_index != -1) {
            long first_index = object_globals->first_name_index[object->identifier_type];
            if (first_index != -1)
                object_globals->object_indices[first_index + name_index] = object_index;
        }
    }
}

static __forceinline void objects_new_early_mover(long object_index)
{
    object_datum *object = object_get(object_index);
    const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
    if ((bool)((definition->flags >> 6) & 1)) {
        s_owner_table *globals = g_object_early_movers_globals;
        long owner_count = globals->owner_count;
        if (owner_count < 32) {
            object->physics_flags |= 1;
            globals->owners[globals->owner_count++] = object_index;
        }
    }
}

static __forceinline bool objects_new_can_connect_to_map()
{
    return object_globals && object_globals->can_connect_to_map;
}

static __forceinline void *objects_new_header_block_get(long object_index, const s_object_block_reference *reference)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    // match: 0x3195b0: positive-offset assumption folds the block null check
    __assume(reference->offset > 0);
    return (char *)object + reference->offset;
}

static __forceinline void objects_new_reset_interpolation(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    c_animation_manager *manager = (c_animation_manager *)objects_new_header_block_get(object_index, &object->animation);
    manager->m_interpolator.m_time = 0.0f;
    manager->m_interpolator.m_duration = 0.0f;
    manager->m_interpolator.m_flags = 0;
    object_wake(object_index);
}

static __forceinline bool objects_new_has_animation_manager(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    // match: 0x3195b0: const reference binding preserves the narrowed offset temporary
    const unsigned long &offset = (unsigned short)object->animation.offset;
    return offset != 0xffff;
}

static __forceinline void objects_new_simulation_flags_set(unsigned char *flags, long bit, unsigned long enable)
{
    if ((bool)enable)
        *flags |= 1 << bit;
    else
        *flags &= ~(1 << bit);
}

static __forceinline void objects_new_identifier_create_dynamic(c_object_identifier *identifier, e_object_type type)
{
    object_globals->object_identifier_counter++;
    identifier->m_type = (char)type;
    identifier->m_source = 2;
    identifier->m_origin_bsp_index = -1;
    identifier->m_unique_id = object_globals->object_identifier_counter;
}

// @address 0x3195b0 ?object_new@@YAJPAUobject_placement_data@@@Z
long object_new(object_placement_data *data)
{
    long object_index = -1;
    bool out_of_memory = false;
    if (!(data->flags & (1 << 4)) && data->definition_index != -1) {
        object_type_adjust_placement(data);
    }
    if (data->definition_index != -1 && objects_new_definition_can_be_placed(data->definition_index)) {
        const s_object_definition *definition = (const s_object_definition *)tag_get('obje', data->definition_index);
        const s_model_definition *model = 0;
        object_type_definition *type_definition = object_type_definitions[definition->object_type];
        if (definition->model.index != -1)
            model = (const s_model_definition *)tag_get('hlmt', definition->model.index);
        if (havok_object_needs_physics(-1, data->definition_index))
            havok_memory_garbage_collect_locked();
        object_index = objects_new_header(type_definition->datum_size);
        if (object_index != -1) {
            s_object_header *header = object_header_get(object_index);
            object_datum *object = header->datum;
            bool initialized = false;
            bool created = false;
            bool success = true;
            bool has_animation = false;
            bool has_multiplayer_properties = definition->multiplayer_object.s_tag_block::count > 0;
            bool hide = false;
            header->flags |= 8;
            header->object_type = definition->object_type;
            objects_flags_set(&object->flags, 2, true);
            object->definition_index = data->definition_index;
            object->map_variant_index = -1;
            object->next_garbage_object_index = -1;
            object->garbage_time = -1;
            object->parent_recycling_group = -1;
            object->next_recycling_group_member = -1;
            object->cinematic_light_index = -1;
            object->cinematic_object_index = -1;
            if (data->object_identifier.m_source == -1) {
                objects_new_identifier_create_dynamic(&object->identifier, (e_object_type)definition->object_type);
                object->name_index = -1;
            } else {
                object->identifier = data->object_identifier;
                object->name_index = (short)data->scenario_datum_index;
            }
            object->position = data->position;
            object->forward = data->forward;
            object->up = data->up;
            object->linear_velocity = data->linear_velocity;
            object->angular_velocity = data->angular_velocity;
            object->scale = data->scale;
            if ((bool)(data->flags & 1))
                objects_flags_set(&object->flags, 10, true);
            else
                objects_flags_set(&object->flags, 10, false);
            objects_flags_set(&object->flags, 9, model && model->collision_model_index != -1);
            objects_flags_set(&object->flags, 31, false);
            if (object->identifier.get_source() == 5)
                objects_flags_set(&object->flags, 26, true);
            else
                objects_flags_set(&object->flags, 26, false);
            if (definition->model.index != -1) {
                bool has_instances = false;
                const s_model_definition *instance_model = (const s_model_definition *)tag_get('hlmt', definition->model.index);
                if (instance_model->render_model_index != -1) {
                    const render_model_definition *render_model = (const render_model_definition *)tag_get('mode',
                        instance_model->render_model_index);
                    has_instances = render_model->instance_placements.count > 0 && render_model->instance_starting_mesh_index != -1;
                }
                objects_flags_set(&object->flags, 11, has_instances);
            }
            header->cluster_reference.value() = -1;
            location_invalidate(&object->location);
            object->first_cluster_reference = -1;
            object->bsp_placement_policy = data->bsp_placement_policy;
            object->parent_object_index = -1;
            object->next_object_index = -1;
            object->first_child_object_index = -1;
            object->first_widget_index = -1;
            object->object_name_index = -1;
            object->recycling_variant = -1;
            object->recycling_permutation = -1;
            object_set_hidden(object_index, false);
            object_set_damage_owner(object_index, &data->damage_owner, true);
            object->variant_index = -1;
            object->structure_bsp_fake_lightprobe_index = -1;
            object->havok_component_index = -1;
            object->physics_flags = 0;
            if ((bool)((definition->flags >> 14) & 1))
                objects_flags_set(&object->physics_flags, 21, true);
            else
                objects_flags_set(&object->physics_flags, 21, false);
            if ((bool)(definition->secondary_flags & 1))
                objects_flags_set(&object->physics_flags, 22, true);
            else
                objects_flags_set(&object->physics_flags, 22, false);
            if ((bool)((data->flags >> 3) & 1))
                objects_flags_set(&object->physics_flags, 9, true);
            else
                objects_flags_set(&object->physics_flags, 9, false);
            object->gamestate_index = -1;
            object->destroyed_regions = -1;
            object->flags_d6 = 0;
            object->marker = -1;
            object->physics_reconnect_time = -32768;
            object->broken_constraints = data->broken_constraints;
            object->loosened_constraints = data->loosened_constraints;
            if (data->multiplayer_cinematic_object)
                objects_new_simulation_flags_set(&object->flags_d6, 1, true);
            else
                objects_new_simulation_flags_set(&object->flags_d6, 1, false);
            object->simulation_object_index = -1;
            long node_count = 1;
            long region_count = 1;
            long instance_size = objects_flags_test_ptr(&object->flags, 11) ? 16 : 0;
            long model_variant_region_count = 0;
            bool needs_orientations = false;
            if (model) {
                if (model->nodes.count >= 1)
                    node_count = model->nodes.count;
                if (model->regions.count >= 1)
                    region_count = model->regions.count;
                if (model->damage_info.count > 0)
                    model_variant_region_count = ((s_model_damage_info *)model->damage_info.address)->sections.count;
                if (model->animation_graph.index != -1 &&
                    (objects_new_validate_graph(model->animation_graph.index, definition->model.index) || definition->model.index == -1)) {
                    has_animation = true;
                    needs_orientations = ((1 << definition->object_type) & 0x23f0) == 0;
                    if (((1 << definition->object_type) & 0x190) &&
                        ((const s_device_definition *)tag_get(0, data->definition_index))->device_flags & 4)
                        needs_orientations = true;
                }
            }
            success = success && object_header_block_allocate(object_index, 0x142, (short)(definition->attachments.count * 8), 2);
            success = success && object_header_block_allocate(object_index, 0x146, (short)(model_variant_region_count * 8), 1);
            success = success && object_header_block_allocate(object_index, 0x14a, (short)(definition->change_colors.count * 24), 2);
            success = success && object_header_block_allocate(object_index, 0x13a, (short)(node_count * 52), 2);
            success = success && object_header_block_allocate(object_index, 0x13e, (short)(region_count * 10 + instance_size), 2);
            success = success &&
                object_header_block_allocate(
                    object_index, 0x136, (unsigned short)(needs_orientations ? node_count * sizeof(real_orientation) : 0), 4);
            success = success &&
                object_header_block_allocate(
                    object_index, 0x132, (unsigned short)(needs_orientations ? node_count * sizeof(real_orientation) : 0), 4);
            long animation_size = 0;
            if (has_animation) {
                animation_size = 176;
                long mixing_board_index = type_definition->mixing_board_definition_index;
                const c_mixing_board_definition *mixing = 0;
                if (mixing_board_index != -1 && (unsigned long)mixing_board_index <= 2)
                    mixing = &((c_mixing_board_definition *)g_mixing_board_definition_list)[mixing_board_index];
                if (mixing)
                    animation_size += mixing->total_size;
            }
            success = success && object_header_block_allocate(object_index, 0x14e, (short)animation_size, 3);
            long multiplayer_size = 0;
            if (has_multiplayer_properties)
                multiplayer_size = 76;
            success = success && object_header_block_allocate(object_index, 0x152, (short)multiplayer_size, 2);
            if (success)
                success = havok_can_allocate_space_for_instance_of_object_definition(data->definition_index);
            if (!success)
                out_of_memory = true;
            object = object_get(object_index);
            if (success) {
                if (has_animation) {
                    c_animation_manager *manager = (c_animation_manager *)((char *)object + object->animation.offset);
                    manager->initialize(object_index, type_definition->mixing_board_definition_index);
                    manager->reset_graph(model->animation_graph.index, definition->model.index, true);
                }
                if (has_multiplayer_properties) {
                    s_multiplayer_object_properties *properties = (s_multiplayer_object_properties *)((char *)object +
                        object->multiplayer_properties.offset);
                    properties->initialize_from_object_definition_and_scenario_placement(data->definition_index,
                        (const s_multiplayer_object_properties_definition *)definition->multiplayer_object.address, &data->multiplayer_properties);
                    if (properties->flags.test(_multiplayer_object_only_visible_in_editor_bit) && !game_engine_sandbox_is_active())
                        hide = true;
                }
                if (definition->attachments.count > 0) {
                    const long &attached_lights_size = object->attached_lights.size & ~7;
                    memset((char *)object + object->attached_lights.offset, -1, attached_lights_size);
                }
                initialized = true;
            }
            if (success) {
                success = object_type_new(object_index, data, &out_of_memory);
                created = true;
                object = object_get(object_index);
            }
            if (data->owner_object_index != -1)
                object_in_early_mover_join(inl_object_get_ultimate_parent(data->owner_object_index), object_index);
            if (success) {
                bool temporary = (bool)((object->flags >> 17) & 1);
                if ((data->flags & 2) || (data->flags & 4))
                    objects_flags_set(&object->flags, 17, false);
                objects_new_early_mover(object_index);
                objects_new_register_scenario_object(object_index);
                object_choose_variant(object_index, data->model_variant_index);
                object_choose_initial_permutation(object_index, data->region_permutation_flags,
                    (s_model_customization_region *)data->region_permutations, data->region_permutation_count);
                object_set_initial_change_colors(object_index, data->active_change_colors, data->change_colors);
                object_initialize_vitality(object_index, 0, 0);
                object_compute_change_colors(object_index);
                if (objects_new_has_animation_manager(object_index))
                    objects_new_reset_interpolation(object_index);
                object_wake(object_index);
                bool loaded = object_load_scenario_placement_matrices(object_index);
                if (!loaded)
                    object_compute_node_matrices(object_index);
                if (objects_new_can_connect_to_map()) {
                    if (!data->placed_by_script)
                        data->placed_by_script = object_try_and_get_location(object_index, &data->location);
                    object_reconnect_to_map(object_index, true, data->placed_by_script ? &data->location : 0);
                }
                havok_component_aquire(object_index, false, false);
                if (!loaded)
                    object_postprocess_node_matrices(object_index);
                if ((bool)((data->flags >> 5) & 1))
                    objects_flags_set(&object->physics_flags, 1, true);
                else
                    objects_flags_set(&object->physics_flags, 1, false);
                object_reconnect_to_physics(object_index);
                if (object_header_get(object_index)->object_type != 5)
                    widgets_new(object_index);
                attachments_new(object_index);
                object_type_definition *child_type = object_type_definitions[object_get(object_index)->identifier_type];
                for (short part_index = 0; child_type->parts[part_index]; ++part_index)
                    if (child_type->parts[part_index]->create_children)
                        child_type->parts[part_index]->create_children(object_index);
                if (definition->creation_effect.index != -1) {
                    c_tag_index temporary;
                    c_tag_index *effect_index = &temporary;
                    new (effect_index) c_tag_index(definition->creation_effect.index);
                    effect_new_from_object(*effect_index, &data->damage_owner, 0, object_index, 0.0f, 0.0f, 0, 0, 1);
                }
                object_damage_new(object_index);
                objects_flags_set(&object->flags, 17, temporary);
                if (data->parent_object_index != -1)
                    object_attach_to_marker_with_type(data->parent_object_index, data->parent_marker, object_index, data->child_marker);
                if (hide)
                    object_set_hidden(object_index, true);
                if (objects_flags_test_ptr(&object->flags, 17) && !(header->flags & 1) && objects_new_can_connect_to_map()) {
                    if (!(data->flags & 2) && (!(data->flags & 4) || object->location.cluster_reference.bsp_index != -1))
                        object_delete(object_index);
                }
            }
            objects_flags_set(&object->flags, 2, false);
            if (!success) {
                if (created)
                    object_type_delete(object_index);
                object_header_delete(object_index);
                object_index = -1;
            } else if (global_scenario->lightmap.index != -1) {
                const s_scenario_lightmap_definition *cinematic = (const s_scenario_lightmap_definition *)tag_get('sLdT', global_scenario->lightmap.index);
                if (cinematic->airprobes.count > 0) {
                    long scenario_index;
                    s_scenario_object *scenario_object = object->identifier.find_scenario_object_from_scenario(global_scenario, &scenario_index);
                    if (scenario_object && scenario_object->cinematic_name != 0) {
                        long cinematic_index = -1;
                        for (long index = 0; index < cinematic->airprobes.count; index++) {
                            if (scenario_object->cinematic_name == ((const s_lightmap_airprobe *)cinematic->airprobes.address)[index].name) {
                                cinematic_index = index;
                                break;
                            }
                        }
                        if (cinematic_index == -1 && scenario_object->cinematic_name == _string_id_all)
                            cinematic_index = 30000;
                        object->cinematic_object_index = cinematic_index;
                    }
                }
                if (cinematic->scenery_light_probes.count > 0) {
                    for (long light_index = 0; light_index < cinematic->scenery_light_probes.count; light_index++) {
                        if (object->identifier.is_equal(&((const s_lightmap_scenery_probe
                            *)cinematic->scenery_light_probes.address)[light_index].object_identifier)) {
                            object->cinematic_light_index = light_index;
                            break;
                        }
                    }
                }
                if (object->cinematic_light_index == -1) {
                    if (cinematic->machine_light_probes.count > 0) {
                        for (long effect_index = 0; effect_index < cinematic->machine_light_probes.count; effect_index++) {
                            if (object->identifier.is_equal(&((const s_lightmap_machine_probe
                                *)cinematic->machine_light_probes.address)[effect_index].object_identifier)) {
                                object->cinematic_light_index = effect_index;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return object_index;
}

t_object_render_message_queue_allocation_manager g_object_render_message_queue_allocator;
t_object_broadphase_data_allocation_manager g_object_broadphase_data_allocator;
t_object_data_allocation_manager g_object_data_allocator;
t_object_name_list_allocation_manager g_object_name_list_allocator;
t_object_render_data_allocation_manager g_object_render_data_allocator;
t_object_memory_pool_allocation_manager g_object_memory_pool_allocator;
t_collideable_first_data_allocation_manager g_collideable_first_data_allocator;
t_collideable_data_reference_allocation_manager g_collideable_data_reference_allocator;
t_collideable_cluster_reference_allocation_manager g_collideable_cluster_reference_allocator;
t_noncollideable_first_data_allocation_manager g_noncollideable_first_data_allocator;
t_noncollideable_data_reference_allocation_manager g_noncollideable_data_reference_allocator;
t_noncollideable_cluster_reference_allocation_manager g_noncollideable_cluster_reference_allocator;
t_tracked_object_globals_allocation_manager g_tracked_object_globals_allocator;

#ifndef SCENARIO_SCENARIO_TAG_H
#define SCENARIO_SCENARIO_TAG_H

#include "tag_files/tag_types.h"
#include "scenario/scenario_spawn_data.h"
#include <stddef.h>

struct s_player_spawn_influence;
struct s_netgame_goal_spawn_influence;
struct s_weapon_spawn_influence;
struct s_vehicle_spawn_influence;
struct s_projectile_spawn_influence;
struct s_equipment_spawn_influence;
struct scenario_cutscene_flag;

struct s_scenario_soft_surface_sink_depths
{
    float maximum_sink_depth[6];
};
typedef char scenario_soft_surface_sink_depths_size[(sizeof(s_scenario_soft_surface_sink_depths) == 0x18) ? 1 : -1];

// scenario cinematic_lighting block element.
struct s_scenario_cinematic_lighting
{
    long name;
    s_tag_reference lighting;
};
typedef char scenario_cinematic_lighting_size[(sizeof(s_scenario_cinematic_lighting) == 0x14) ? 1 : -1];

// The scenario tag uses four-byte tag pointers.
struct scenario
{
    short type;
    unsigned short flags;
    long campaign_id;
    long map_id;
    float local_north;
    float budget;
    s_tag_block structure_bsps;
    s_tag_reference structure_seams;
    s_tag_block skies;
    s_tag_block zone_set_pvs;
    s_tag_block zone_set_audibility;
    s_tag_block zone_sets;
    s_tag_block bsp_atlas;
    s_tag_block predicted_resources;
    s_tag_block functions;
    s_tag_data editor_scenario_data;
    s_tag_block comments;
    s_tag_block environment_objects;
    s_tag_block object_names;
    s_tag_block scenery;
    s_tag_block scenery_palette;
    s_tag_block bipeds;
    s_tag_block biped_palette;
    s_tag_block vehicles;
    s_tag_block vehicle_palette;
    s_tag_block equipment;
    s_tag_block equipment_palette;
    s_tag_block weapons;
    s_tag_block weapon_palette;
    s_tag_block device_groups;
    s_tag_block machines;
    s_tag_block machine_palette;
    s_tag_block terminals;
    s_tag_block terminal_palette;
    s_tag_block controls;
    s_tag_block control_palette;
    s_tag_block sound_scenery;
    s_tag_block sound_scenery_palette;
    s_tag_block giants;
    s_tag_block giant_palette;
    s_tag_block effect_scenery;
    s_tag_block effect_scenery_palette;
    s_tag_block light_volumes;
    s_tag_block light_volume_palette;
    s_tag_block sandbox_vehicles;
    s_tag_block sandbox_weapons;
    s_tag_block sandbox_equipment;
    s_tag_block sandbox_scenery;
    s_tag_block sandbox_machines;
    s_tag_block sandbox_crates;
    s_tag_block sandbox_sound_scenery;
    s_tag_block soft_ceilings;
    s_tag_block starting_profiles;
    s_tag_block player_starting_locations;
    s_tag_block trigger_volumes;
    s_tag_block recorded_animations;
    s_tag_block zone_set_switch_trigger_volumes;
    c_typed_tag_block<s_player_spawn_influence> player_influences[5];
    c_typed_tag_block<s_weapon_spawn_influence> weapon_influences;
    c_typed_tag_block<s_vehicle_spawn_influence> vehicle_influences;
    c_typed_tag_block<s_projectile_spawn_influence> projectile_influences;
    c_typed_tag_block<s_equipment_spawn_influence> equipment_influences;
    c_typed_tag_block<s_netgame_goal_spawn_influence> netgame_goal_influences[8];
    s_tag_block decals;
    s_tag_block decal_palette;
    s_tag_block detail_object_collection_palette;
    s_tag_block style_palette;
    s_tag_block squad_groups;
    s_tag_block squads;
    s_tag_block zones;
    s_tag_block mission_scenes;
    s_tag_block characters;
    s_tag_block ai_pathfinding_data;
    s_tag_block ai_user_hint_data;
    s_tag_block recording_points;
    s_tag_data script_string_data;
    s_tag_block scripts;
    s_tag_block globals;
    s_tag_block script_references;
    s_tag_block source_files;
    s_tag_block script_data;
    c_typed_tag_block<scenario_cutscene_flag> cutscene_flags;
    s_tag_block cutscene_camera_points;
    s_tag_block cutscene_titles;
    s_tag_reference custom_object_names;
    s_tag_reference cutscene_title_strings;
    s_tag_block scenario_resources;
    s_tag_block seat_mappings;
    s_tag_block kill_trigger_volume_indices;
    s_tag_block trigger_volume_indices;
    s_tag_block syntax_nodes;
    s_tag_block orders;
    s_tag_block triggers;
    s_tag_block acoustics_palette;
    s_tag_block old_background_sound_palette;
    s_tag_block sound_environment_palette;
    s_tag_block weather_palette;
    s_tag_block atmosphere;
    s_tag_block camera_fx_palette;
    s_tag_block unused_block0;
    s_tag_block unused_block1;
    s_tag_block unused_block2;
    s_tag_block scenario_cluster_data;
    s_tag_block acoustic_spaces;
    long object_salts[32];
    c_typed_tag_block<s_scenario_spawn_data> spawn_data;
    s_tag_reference sound_effect_collection;
    s_tag_block crates;
    s_tag_block crate_palette;
    s_tag_block flock_definitions;
    s_tag_block flocks;
    s_tag_reference cinematic_subtitles;
    s_tag_block creatures;
    s_tag_block creature_palette;
    s_tag_block editor_folders;
    s_tag_reference game_engine_strings;
    char pad65c[8];
    s_tag_block ai_mission_dialogue;
    s_tag_reference objectives_string_list;
    s_tag_block interpolators;
    s_tag_block shared_references;
    s_tag_block screen_effect_references;
    s_tag_block simulation_definition_table;
    s_tag_reference camera_fx;
    s_tag_reference screen_effect;
    s_tag_reference sky_parameters;
    s_tag_reference global_lighting;
    s_tag_reference lightmap;
    s_tag_reference game_tuning;
    s_tag_block object_reference_frames;
    s_tag_block objectives;
    s_tag_block designer_zones;
    s_tag_block zone_debugger;
    s_tag_block decorators;
    s_tag_block cinematics;
    s_tag_block cinematic_lighting;
    s_tag_block override_player_representations;
    s_tag_block time_curve;
    c_typed_tag_block<s_scenario_soft_surface_sink_depths> soft_surfaces;
    s_tag_block cubemaps;
    s_tag_block chud_references;
};

typedef char scenario_offset_soft_surfaces[(offsetof(scenario, soft_surfaces) == 0x77c) ? 1 : -1];
typedef char scenario_atmosphere_count_offset[(offsetof(scenario, atmosphere.count) == 0x4f8) ? 1 : -1];
typedef char scenario_spawn_data_offset[(offsetof(scenario, spawn_data) == 0x5cc) ? 1 : -1];

typedef char scenario_tag_pointer_size[(sizeof(void *) != 4 || ((sizeof(void *) == 4) )) ? 1 : -1];
typedef char scenario_offset_structure_bsps[(offsetof(scenario, structure_bsps) == 0x14) ? 1 : -1];
typedef char scenario_offset_zone_set_pvs[(offsetof(scenario, zone_set_pvs) == 0x3c) ? 1 : -1];
typedef char scenario_offset_skies[(offsetof(scenario, skies) == 0x30) ? 1 : -1];
typedef char scenario_offset_zone_sets[(offsetof(scenario, zone_sets) == 0x54) ? 1 : -1];
typedef char scenario_offset_audibility[(offsetof(scenario, zone_set_audibility) == 0x48) ? 1 : -1];
typedef char scenario_offset_squad_groups[(offsetof(scenario, squad_groups) == 0x380) ? 1 : -1];
typedef char scenario_offset_ai_user_hint_data[(offsetof(scenario, ai_user_hint_data) == 0x3c8) ? 1 : -1];
typedef char scenario_offset_object_names[(offsetof(scenario, object_names) == 0xb0) ? 1 : -1];
typedef char scenario_offset_sound_effect_collection[(offsetof(scenario, sound_effect_collection) == 0x5d8) ? 1 : -1];
typedef char scenario_offset_machines[(offsetof(scenario, machines) == 0x140) ? 1 : -1];
typedef char scenario_offset_sandbox_vehicles[(offsetof(scenario, sandbox_vehicles) == 0x1e8) ? 1 : -1];
typedef char scenario_offset_trigger_volumes[(offsetof(scenario, trigger_volumes) == 0x260) ? 1 : -1];
typedef char scenario_offset_player_influences[(offsetof(scenario, player_influences) == 0x284) ? 1 : -1];
typedef char scenario_offset_squads[(offsetof(scenario, squads) == 0x38c) ? 1 : -1];
typedef char scenario_offset_zones[(offsetof(scenario, zones) == 0x398) ? 1 : -1];
typedef char scenario_offset_scripts[(offsetof(scenario, scripts) == 0x3f4) ? 1 : -1];
typedef char scenario_offset_globals[(offsetof(scenario, globals) == 0x400) ? 1 : -1];
typedef char scenario_offset_kill_trigger_volume_indices[(offsetof(scenario, kill_trigger_volume_indices) == 0x48c) ? 1 : -1];
typedef char scenario_offset_script_string_data[(offsetof(scenario, script_string_data) == 0x3e0) ? 1 : -1];
typedef char scenario_offset_syntax_nodes[(offsetof(scenario, syntax_nodes) == 0x4a4) ? 1 : -1];
typedef char scenario_offset_ai_mission_dialogue[(offsetof(scenario, ai_mission_dialogue) == 0x664) ? 1 : -1];
typedef char scenario_offset_interpolators[(offsetof(scenario, interpolators) == 0x680) ? 1 : -1];
typedef char scenario_offset_objectives[(offsetof(scenario, objectives) == 0x71c) ? 1 : -1];
typedef char scenario_offset_object_reference_frames[(offsetof(scenario, object_reference_frames) == 0x710) ? 1 : -1];
typedef char scenario_offset_cinematic_subtitles[(offsetof(scenario, cinematic_subtitles) == 0x618) ? 1 : -1];
typedef char scenario_offset_cinematics[(offsetof(scenario, cinematics) == 0x74c) ? 1 : -1];
typedef char scenario_offset_chud_references[(offsetof(scenario, chud_references) == 0x794) ? 1 : -1];


typedef char scenario_decal_palette_offset[(offsetof(scenario, decal_palette) == 0x35c) ? 1 : -1];
typedef char scenario_game_tuning_offset[(offsetof(scenario, game_tuning) == 0x700) ? 1 : -1];

typedef char scenario_camera_fx_index_offset[(offsetof(scenario, camera_fx.index) == 0x6bc) ? 1 : -1];

typedef char scenario_simulation_definition_table_offset[(offsetof(scenario, simulation_definition_table) == 0x6a4) ? 1 : -1];

typedef char scenario_acoustics_palette_offset[(offsetof(scenario, acoustics_palette) == 0x4c8) ? 1 : -1];
typedef char scenario_screen_effect_offset[(offsetof(scenario, screen_effect) == 0x6c0) ? 1 : -1];
typedef char scenario_global_lighting_index_offset[(offsetof(scenario, global_lighting.index) == 0x6ec) ? 1 : -1];

typedef char scenario_acoustic_spaces_layout[(offsetof(scenario, acoustic_spaces) == 0x540) ? 1 : -1];
typedef char scenario_ai_pathfinding_data_layout[(offsetof(scenario, ai_pathfinding_data) == 0x3bc) ? 1 : -1];
typedef char scenario_camera_fx_palette_layout[(offsetof(scenario, camera_fx_palette) == 0x504) ? 1 : -1];
typedef char scenario_campaign_id_layout[(offsetof(scenario, campaign_id) == 0x4) ? 1 : -1];
typedef char scenario_comments_layout[(offsetof(scenario, comments) == 0x98) ? 1 : -1];
typedef char scenario_creature_palette_layout[(offsetof(scenario, creature_palette) == 0x634) ? 1 : -1];
typedef char scenario_creatures_layout[(offsetof(scenario, creatures) == 0x628) ? 1 : -1];
typedef char scenario_cubemaps_layout[(offsetof(scenario, cubemaps) == 0x788) ? 1 : -1];
typedef char scenario_custom_object_names_layout[(offsetof(scenario, custom_object_names) == 0x454) ? 1 : -1];
typedef char scenario_decals_layout[(offsetof(scenario, decals) == 0x350) ? 1 : -1];
typedef char scenario_decorators_layout[(offsetof(scenario, decorators) == 0x740) ? 1 : -1];
typedef char scenario_designer_zones_layout[(offsetof(scenario, designer_zones) == 0x728) ? 1 : -1];
typedef char scenario_detail_object_collection_palette_layout[(offsetof(scenario, detail_object_collection_palette) == 0x368) ? 1 : -1];
typedef char scenario_editor_folders_layout[(offsetof(scenario, editor_folders) == 0x640) ? 1 : -1];
typedef char scenario_editor_scenario_data_layout[(offsetof(scenario, editor_scenario_data) == 0x84) ? 1 : -1];
typedef char scenario_game_engine_strings_layout[(offsetof(scenario, game_engine_strings) == 0x64c) ? 1 : -1];
typedef char scenario_global_lighting_layout[(offsetof(scenario, global_lighting) == 0x6e0) ? 1 : -1];
typedef char scenario_bsp_atlas_layout[(offsetof(scenario, bsp_atlas) == 0x60) ? 1 : -1];
typedef char scenario_mission_scenes_layout[(offsetof(scenario, mission_scenes) == 0x3a4) ? 1 : -1];
typedef char scenario_object_salts_layout[(offsetof(scenario, object_salts) == 0x54c) ? 1 : -1];
typedef char scenario_old_background_sound_palette_layout[(offsetof(scenario, old_background_sound_palette) == 0x4d4) ? 1 : -1];
typedef char scenario_orders_layout[(offsetof(scenario, orders) == 0x4b0) ? 1 : -1];
typedef char scenario_override_player_representations_layout[(offsetof(scenario, override_player_representations) == 0x764) ? 1 : -1];
typedef char scenario_recording_points_layout[(offsetof(scenario, recording_points) == 0x3d4) ? 1 : -1];
typedef char scenario_scenario_cluster_data_layout[(offsetof(scenario, scenario_cluster_data) == 0x534) ? 1 : -1];
typedef char scenario_scenario_resources_layout[(offsetof(scenario, scenario_resources) == 0x474) ? 1 : -1];
typedef char scenario_screen_effect_references_layout[(offsetof(scenario, screen_effect_references) == 0x698) ? 1 : -1];
typedef char scenario_script_references_layout[(offsetof(scenario, script_references) == 0x40c) ? 1 : -1];
typedef char scenario_shared_references_layout[(offsetof(scenario, shared_references) == 0x68c) ? 1 : -1];
typedef char scenario_sound_environment_palette_layout[(offsetof(scenario, sound_environment_palette) == 0x4e0) ? 1 : -1];
typedef char scenario_structure_seams_layout[(offsetof(scenario, structure_seams) == 0x20) ? 1 : -1];
typedef char scenario_style_palette_layout[(offsetof(scenario, style_palette) == 0x374) ? 1 : -1];
typedef char scenario_triggers_layout[(offsetof(scenario, triggers) == 0x4bc) ? 1 : -1];
typedef char scenario_weather_palette_layout[(offsetof(scenario, weather_palette) == 0x4ec) ? 1 : -1];
typedef char scenario_zone_debugger_layout[(offsetof(scenario, zone_debugger) == 0x734) ? 1 : -1];

#endif // SCENARIO_SCENARIO_TAG_H

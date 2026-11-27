#ifndef GAME_GAME_GLOBALS_H
#define GAME_GAME_GLOBALS_H

#include <stddef.h>

#include "game/game_types.h"
#include "game/materials.h"
#include "math/real_math_types.h"

#include "ai/ai_globals_definition.h"
#include "tag_files/tag_types.h"
#include "text/language_pack.h"

struct sound_globals_reference;
struct s_player_control_constants;

// Default camera track for units without an assigned track.
struct s_game_globals_camera
{
    s_tag_reference default_unit_camera_track;
};

struct s_grenade_definition
{
    short maximum_count;
    s_tag_reference throwing_effect;
    char reserved[0x10];
    s_tag_reference equipment;
    s_tag_reference projectile;
};

typedef char grenade_definition_size[(sizeof(s_grenade_definition) == 0x44) ? 1 : -1];
typedef char grenade_definition_maximum_count_offset[(offsetof(s_grenade_definition, maximum_count) == 0) ? 1 : -1];
typedef char grenade_definition_effect_offset[((offsetof(s_grenade_definition, throwing_effect) + offsetof(s_tag_reference, index)) == 0x10) ? 1 : -1];
typedef char grenade_definition_item_offset[((offsetof(s_grenade_definition, equipment) + offsetof(s_tag_reference, index)) == 0x30) ? 1 : -1];
typedef char grenade_definition_projectile_offset[((offsetof(s_grenade_definition, projectile) + offsetof(s_tag_reference, index)) == 0x40) ? 1 : -1];
struct s_player_information_definition;
struct s_player_representation
{
    s_tag_reference first_person_hands;
    s_tag_reference first_person_body;
    s_tag_reference third_person_unit;
    long third_person_variant;
    s_tag_reference binoculars_zoom_in_sound;
    s_tag_reference binoculars_zoom_out_sound;
};
typedef char player_representation_size[(sizeof(s_player_representation) == 0x54) ? 1 : -1];
struct s_global_material_definition;
struct s_campaign_metagame_definition;
// Cinematic lens settings.
struct s_game_globals_cinematic_camera
{
    s_tag_reference cinematic_anchor;
    float aperture;
};
typedef char game_globals_cinematic_camera_size[(sizeof(s_game_globals_cinematic_camera) == 0x14) ? 1 : -1];
typedef char game_globals_cinematic_camera_aperture_offset[(offsetof(s_game_globals_cinematic_camera, aperture) == 0x10) ? 1 : -1];

struct s_damage_globals_definition
{
    s_tag_reference large_island_cull_effect;
};
typedef char damage_globals_cull_effect_index_offset[
    (offsetof(s_damage_globals_definition, large_island_cull_effect)
        + offsetof(s_tag_reference, index) == 0x0c) ? 1 : -1];

// Global game tag settings.
struct s_game_globals {
    char pad_000[0xac];
    long language; // Cache language index, validated by its readers.
    c_typed_tag_block<s_damage_globals_definition> damage_globals;
    c_typed_tag_block<sound_globals_reference> sound_globals;  // (64-byte elements)
    c_typed_tag_block<s_ai_globals_definition> ai_globals;  // (428-byte elements)
    s_tag_block damage_tables;                              // damage_armor_table_lookup, +0xd4
    s_tag_block unused;
    c_typed_tag_block<s_tag_reference> sounds;
    c_typed_tag_block<s_game_globals_camera> camera;      // following_camera reads element 0
    c_typed_tag_block<s_player_control_constants> player_control;  // (132-byte elements)
    s_tag_block difficulty;                                 // (644-byte elements)
    c_typed_tag_block<s_grenade_definition> grenades;       // (68-byte elements)
    s_tag_block unused_grenade_block;
    s_tag_block interface_tags;
    c_typed_tag_block<s_tag_reference> cheat_weapons;
    c_typed_tag_block<s_tag_reference> cheat_powerups;
    c_typed_tag_block<s_player_information_definition> player_information;  // (388-byte elements)
    c_typed_tag_block<s_player_representation> player_representations;
    s_tag_block falling_damage;                             // (120-byte elements)
    union {
        c_typed_tag_block<s_global_material_definition> materials;
        long material_count;
    };
    c_typed_tag_block<real_rgb_color> player_colors;
    union {
        s_tag_reference multiplayer_globals;
        struct {
            unsigned long multiplayer_globals_group_tag;
            TAG_POINTER(const char) multiplayer_globals_name;
            long multiplayer_globals_name_length;
            long multiplayer_globals_index;
        };
    };
    c_typed_tag_block<s_game_globals_cinematic_camera> cinematic_camera;
    c_typed_tag_block<s_campaign_metagame_definition> campaign_metagame;
    c_language_pack language_packs[12];                     // one per e_language
    s_tag_reference rasterizer_globals_ref;
    s_tag_reference camera_fx;
    s_tag_reference default_wind;
    s_tag_reference breakable_surface_damage;
    s_tag_reference collision_damage;
    long water_material_name;
    c_global_material_type water_material_type;
    s_tag_reference particle_system_limits;
};

typedef char game_globals_sounds_offset[(offsetof(s_game_globals, sounds) == 0xec) ? 1 : -1];
typedef char game_globals_cheat_weapons_offset[(offsetof(s_game_globals, cheat_weapons) == 0x140) ? 1 : -1];
typedef char game_globals_cheat_powerups_offset[(offsetof(s_game_globals, cheat_powerups) == 0x14c) ? 1 : -1];
typedef char game_globals_default_wind_offset[(offsetof(s_game_globals, default_wind) == 0x50c) ? 1 : -1];
typedef char game_globals_water_material_name_offset[(offsetof(s_game_globals, water_material_name) == 0x53c) ? 1 : -1];

typedef char game_globals_particle_limits_offset[(offsetof(s_game_globals, particle_system_limits) == 0x544) ? 1 : -1];
typedef char game_globals_camera_fx_offset[(offsetof(s_game_globals, camera_fx) == 0x4fc) ? 1 : -1];

typedef char game_globals_breakable_damage_offset[(offsetof(s_game_globals, breakable_surface_damage) == 0x51c) ? 1 : -1];
typedef char game_globals_collision_damage_offset[(offsetof(s_game_globals, collision_damage) == 0x52c) ? 1 : -1];

typedef char game_globals_damage_globals_offset[(offsetof(s_game_globals, damage_globals) == 0xb0) ? 1 : -1];

typedef char game_globals_off_language[(offsetof(s_game_globals, language) == 0xac) ? 1 : -1];
typedef char game_globals_off_sound_globals[(offsetof(s_game_globals, sound_globals) == 0xbc) ? 1 : -1];
typedef char game_globals_off_ai_globals[(offsetof(s_game_globals, ai_globals) == 0xc8) ? 1 : -1];
typedef char game_globals_off_camera[(offsetof(s_game_globals, camera) == 0xf8) ? 1 : -1];
typedef char game_globals_off_player_control[(offsetof(s_game_globals, player_control) == 0x104) ? 1 : -1];
typedef char game_globals_off_damage_tables[(offsetof(s_game_globals, damage_tables) == 0xd4) ? 1 : -1];

typedef char game_globals_off_difficulty[(offsetof(s_game_globals, difficulty) == 0x110) ? 1 : -1];
typedef char game_globals_off_grenades[(offsetof(s_game_globals, grenades) == 0x11c) ? 1 : -1];
typedef char game_globals_off_interface_tags[(offsetof(s_game_globals, interface_tags) == 0x134) ? 1 : -1];
typedef char game_globals_off_player_information[(offsetof(s_game_globals, player_information) == 0x158) ? 1 : -1];
typedef char game_globals_off_player_representations[(offsetof(s_game_globals, player_representations) == 0x164) ? 1 : -1];
typedef char game_globals_off_falling_damage[(offsetof(s_game_globals, falling_damage) == 0x170) ? 1 : -1];
typedef char game_globals_off_materials[(offsetof(s_game_globals, materials) == 0x17c) ? 1 : -1];
typedef char game_globals_off_mp_globals[(offsetof(s_game_globals, multiplayer_globals) == 0x194) ? 1 : -1];
typedef char game_globals_off_mp_globals_idx[(offsetof(s_game_globals, multiplayer_globals_index) == 0x1a0) ? 1 : -1];
typedef char game_globals_off_campaign_metagame[(offsetof(s_game_globals, campaign_metagame) == 0x1b0) ? 1 : -1];
typedef char game_globals_off_cinematic_camera[(offsetof(s_game_globals, cinematic_camera) == 0x1a4) ? 1 : -1];
typedef char game_globals_off_language_packs[(sizeof(void *) != 4 || (offsetof(s_game_globals, language_packs) == 0x1bc)) ? 1 : -1];
typedef char game_globals_off_rast_idx[(sizeof(void *) != 4
    || ((offsetof(s_game_globals, rasterizer_globals_ref) + offsetof(s_tag_reference, index)) == 0x4f8)) ? 1 : -1];
typedef char game_globals_size[(sizeof(void *) != 4 || (sizeof(s_game_globals) == 0x554)) ? 1 : -1];


// Functions defined in tu/blamlibxbox/game_globals.cpp that other units call.

float game_difficulty_get_value_by_difficulty(short value_type, short difficulty);
long game_get_grenade_type_index_from_item_defintion(long item_definition_index);
float game_difficulty_get_value(short value_type);
float game_difficulty_get_team_value(short value_type, e_game_team team);
extern "C" int game_get_player_character_type(long player_index);

extern "C" {
extern const short k_game_difficulty_friendly_remap[];
}

extern s_game_globals *global_game_globals;

// game_globals.obj (March 2007 build).
inline long game_globals_get_style_index(s_game_globals *globals, long index)
{
    s_ai_globals_definition *ai_globals = globals->ai_globals.begin();
    if (index >= 0 && index < ai_globals->styles_count)
        return ai_globals->styles[index].index;
    return -1;
}

typedef char game_globals_player_colors_offset[(offsetof(s_game_globals, player_colors) == 0x188) ? 1 : -1];
#endif // GAME_GAME_GLOBALS_H

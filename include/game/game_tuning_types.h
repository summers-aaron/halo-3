#ifndef GAME_GAME_TUNING_TYPES_H
#define GAME_GAME_TUNING_TYPES_H

#include <stddef.h>

struct s_global_tuning {
    union {
        struct {
            float f0, f4, f8, fc;
            float terminal_modifier;
            float projectile_modifier;
            float f18, f1c;
            float control_modifier;
            float sound_scenery_modifier;
            float f28;
            float creature_modifier;
            float giant_modifier;
            float effect_scenery_modifier;
        };
        float values[14];
    };
};
struct s_player_tuning {
    unsigned long flags;
    float f4;
    union { float f8; float decorator_distance_scale; };
    union { float fc; float delta; };
    union { float f10; float instance_fade_scale; };
    union { float f14; float distance_scale; };
    union { float f18; float lod_distance_scale; };
    union { float f1c; float decal_distance; };
    union { int i20; long lighting_pass; };
    float f24;
    union { int i28; long simple_lighting_pass; };
    float f2c;
    union { int i30; long maximum_shadow_count; };
    union { float f34; float shadow_resolution_scale; };
};
extern "C" {
extern s_global_tuning g_game_tuning_globals;
extern s_player_tuning g_game_tuning_players[4];
}

#if defined(_XBOX)
typedef char s_global_tuning_size_check[(sizeof(s_global_tuning) == 0x38) ? 1 : -1];
typedef char s_global_tuning_terminal_check[(offsetof(s_global_tuning, terminal_modifier) == 0x10) ? 1 : -1];
typedef char s_global_tuning_control_check[(offsetof(s_global_tuning, control_modifier) == 0x20) ? 1 : -1];
typedef char s_global_tuning_creature_check[(offsetof(s_global_tuning, creature_modifier) == 0x2c) ? 1 : -1];
typedef char s_player_tuning_f4_check[(offsetof(s_player_tuning, f4) == 4) ? 1 : -1];
typedef char s_player_tuning_delta_check[(offsetof(s_player_tuning, delta) == 0xc) ? 1 : -1];
typedef char s_player_tuning_lod_distance_scale_check[(offsetof(s_player_tuning, lod_distance_scale) == 0x18) ? 1 : -1];
typedef char s_player_tuning_decal_distance_check[(offsetof(s_player_tuning, decal_distance) == 0x1c) ? 1 : -1];
typedef char s_player_tuning_simple_lighting_pass_check[(offsetof(s_player_tuning, simple_lighting_pass) == 0x28) ? 1 : -1];
typedef char s_player_tuning_f34_check[(offsetof(s_player_tuning, f34) == 0x34) ? 1 : -1];
typedef char s_player_tuning_size_check[(sizeof(s_player_tuning) == 0x38) ? 1 : -1];
#endif

#endif // GAME_GAME_TUNING_TYPES_H

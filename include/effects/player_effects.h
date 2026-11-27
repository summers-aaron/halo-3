#ifndef EFFECTS_PLAYER_EFFECTS_H
#define EFFECTS_PLAYER_EFFECTS_H

#include "cache/restricted_memory_regions.h"

// Functions defined in tu/blamlibxbox/player_effects.cpp.

struct real_matrix4x3;
union real_rgb_color;

void player_effect_update();
void scripted_player_effect_screen_fade_in(float red, float green, float blue, short ticks);
void scripted_player_effect_screen_fade_out(float red, float green, float blue, short ticks);
void player_effect_get_screen_tint_multiplier(long user_index, real_rgb_color *multiplier);
void player_effect_apply_camera_effect_matrix(long user_index, real_matrix4x3 *matrix);
void player_effect_render(long user_index);

struct player_effect_datum;
union real_vector3d;
struct s_damage_owner;
struct s_damage_response_camera_impulse_definition;
struct s_damage_response_camera_shake_definition;
struct s_damage_response_screen_flash_definition;

extern "C" void player_effect_start_response(long unit_index, long user_index, long response_index,
    const s_damage_owner *owner, const real_vector3d *direction, float magnitude, float scale, long apply_flash, bool extra);
extern "C" void player_effect_update_screen_tint(long user_index, player_effect_datum *effect, const void *tint, float scale);
void player_effect_update_screen_flash(long user_index, player_effect_datum *effect,
    const s_damage_response_screen_flash_definition *definition, float magnitude);
extern "C" void player_effect_update_camera_shake(long user_index, player_effect_datum *effect,
    const s_damage_response_camera_shake_definition *definition, float scale);
void player_effect_update_camera_impulse(long user_index, player_effect_datum *effect,
    const s_damage_response_camera_impulse_definition *impulse, const real_vector3d *direction, float scale);

struct render_screen_flash;

extern "C" void player_effect_start(long unit_index, long user_index, long effect_index,
    const s_damage_owner *owner, const real_vector3d *direction, float magnitude, float duration, long flags, bool extra);
void player_effect_get_screen_flash(long user_index, render_screen_flash *flash);

template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
void __tls_set_g_player_effect_globals_allocator(void *allocation);
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_effect_globals_allocator> t_player_effect_globals_allocator;
template<>
void *t_player_effect_globals_allocator::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits);

extern t_player_effect_globals_allocator g_player_effect_globals_allocator;
extern bool g_render_enable_screen_flashes;
extern "C" real_rgb_color g_scripted_player_effect_screen_fade_color;

void player_effect_initialize(void);
void player_effect_dispose(void);
void player_effect_initialize_for_new_map(void);
void player_effect_dispose_from_old_map(void);

#endif // EFFECTS_PLAYER_EFFECTS_H

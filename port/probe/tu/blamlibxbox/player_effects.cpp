// blamlibxbox/player_effects.obj: from the records 0x11a8c0 0x12bf98 0x179248 0x18ee00 0x23e3e0 0x23e428 0x23e4b8 0x23e610 0x23e7e0 0x23e908 0x23ea30 0x23eb20 0x23ec88 0x23ed80 0x23ee80 0x23f188 0x23f258 0x23f2f8
#include "interface/chud/chud_dispose_state_types.h"
// @flags /TP /O2 /Oc
#include "game/player_control_globals.h"
// @address 0x361a8 k_random_direction_table
#include "math/random_math.h"
#include "memory/tls_data_arrays.h"
#include "effects/player_effect_tls.h"
#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
// @address 0x23e428 ?player_effect_initialize_for_new_map@@YAXXZ
#include "cseries/crt_xdk.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include <string.h>
#include <math.h>
#include "game/game_time_types.h"
#include "tag_files/tag_groups.h"

#include "memory/datum_index.h"
#include "math/random_math_range.h"
#include "effects/player_effects.h"
#include "game/player_rumble.h"
#include "math/color_math.h"
#include "math/real_constant_pool.h"
#include "cseries/global_real_argb_colors.h"
#include "interface/chud/cortana_effect_definition.h"
#include "math/periodic_functions.h"

#include "cache/restricted_memory_regions_inline.h"
#include "physics/physics_constants.h"
#include "rasterizer/rasterizer_main.h"
#include "cache/restricted_memory_regions.h"
#include "cache/restricted_memory.h"
#include "cache/restricted_memory_registration.h"
#include "math/matrix_math.h"
#include "memory/data_types.h"
#include "rasterizer/rasterizer_video_mode_types.h"
#include "effects/rumble_entry_types.h"
#include "effects/rumble_globals_types.h"
#include "effects/rumble_player_types.h"
#include "cache/cache_file_globals.h"
#include "interface/chud/chud.h"
#include "scenario/scenario.h"
#include "bitmaps/bitmap_group.h"
#include "camera/director.h"
#include "effects/player_effect_types.h"
#include "effects/player_effect_globals_types.h"
extern "C" {
// @address 0x7ea8d4 g_scripted_player_effect_screen_fade_color
}

#include "game/game_time_inline.h"

#include "game/player_mapping_types.h"

#include "objects/damage_effect_definitions.h"
#include "game/player_information_definition.h"
#include "game/game_globals.h"
#include "units/unit_types.h"

#include "game/player_mapping_inline.h"
#include "cache/restricted_memory_layout.h"
#include "objects/damage_response_definition.h"
#include "interface/chud/chud_directional_damage.h"
#include "interface/chud/chud_user_data_types.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_quad.h"
#include "rasterizer/rasterizer_global_definitions.h"
#include "game/player_control_types.h"
#if !defined(_XBOX) || defined(__clang__)
#include "camera/camera_effect_interpolation.h"
static s_camera_effect_interpolation g_damage_camera_interpolation[4];
#endif
struct s_damage_owner;
struct s_tag_instance;
struct s_globals_definition;
struct s_data_array;
struct s_game_globals;
class c_restricted_memory;
struct chud_curve_follower_2d;
struct bitmap_data;

struct render_screen_flash {
    long type;
    float intensity;
    real_argb_color color;
};

static __forceinline player_effect_datum *player_effect_get(long user_index)
{
    return &((s_player_effect_globals *)g_player_effect_globals)->effects[user_index];
}
static __forceinline void rumble_player_clear_scripted_effect()
{
    s_rumble_globals *globals = (s_rumble_globals *)g_rumble_globals;
    globals->scripted_scale = 0.0f;
    globals->scripted_left = 0.0f;
    globals->scripted_right = 0.0f;
}
static __forceinline void rumble_player_clear(long user_index)
{
    s_rumble_player *player = &((s_rumble_globals *)g_rumble_globals)->players[user_index];
    memset(player, 0, sizeof(*player));
    for (long index = 0; index < 8; ++index) {
        player->entries[index].rumble_index = NONE;
        player->entries[index].rumble_type = NONE;
        player->entries[index].intensity = 1.0f;
    }
}
// @address 0x11a8c0 ?player_effect_update@@YAXXZ
void player_effect_update()
{
    if (((s_player_effect_globals *)g_player_effect_globals)->player_effect_index != NONE &&
        game_time_get() - ((s_player_effect_globals *)g_player_effect_globals)->fade_start_time
            > ((s_player_effect_globals *)g_player_effect_globals)->player_effect_index &&
        !((s_player_effect_globals *)g_player_effect_globals)->fade_out)
        ((s_player_effect_globals *)g_player_effect_globals)->player_effect_index = NONE;
    if (((s_player_effect_globals *)g_player_effect_globals)->scripted_flags & 1) {
        s_scripted_player_effect *effect = &((s_player_effect_globals *)g_player_effect_globals)->scripted_effect;
        if (effect->ticks > 0)
            --effect->ticks;
        else if (((s_player_effect_globals *)g_player_effect_globals)->scripted_flags & 2) {
            ((s_player_effect_globals *)g_player_effect_globals)->scripted_flags &= ~1;
            rumble_player_clear_scripted_effect();
        }
    }
    for (long user_index = 0; user_index < 4; ++user_index) {
        if (players_user_is_active(user_index)) {
            player_effect_datum *effect = player_effect_get(user_index);
            effect->flags &= ~1;
            if (effect->flash_ticks > 0) --effect->flash_ticks;
            effect->flags &= ~2;
            if (effect->tint_ticks > 0) --effect->tint_ticks;
            effect->flags &= ~4;
            if (effect->impulse_ticks > 0) --effect->impulse_ticks;
            effect->flags &= ~8;
            if (effect->shake_ticks > 0) --effect->shake_ticks;
        } else {
            player_effect_datum *effect = player_effect_get(user_index);
            memset(effect, 0, sizeof(*effect));
#if !defined(_XBOX) || defined(__clang__)
            g_damage_camera_interpolation[user_index].valid = false;
#endif
            rumble_player_clear(user_index);
        }
    }
}

// @address 0x23e3e0 ?player_effect_initialize@@YAXXZ
void player_effect_initialize(void)
{
    g_player_effect_globals = (s_player_effect_globals *)g_player_effect_globals_allocator.reserve_memory(
        "player effects", 0, sizeof(s_player_effect_globals), 0);
}

void player_effect_initialize_for_new_map()
{
#if !defined(_XBOX) || defined(__clang__)
    memset(g_damage_camera_interpolation, 0, sizeof(g_damage_camera_interpolation));
#endif
    s_player_effect_globals *globals = (s_player_effect_globals *)g_player_effect_globals;
    memset(globals, 0, sizeof(s_player_effect_globals));
    globals->player_effect_index = NONE;
    globals->game_time = game_time_get();
    const real_rgb_color *black = &global_real_argb_colors[_real_argb_black].rgb;
    g_scripted_player_effect_screen_fade_color = *black;
}

// @address 0x5bec98 ?player_effect_dispose@@YAXXZ
void player_effect_dispose(void)
{
}

// @address 0x5bec98 ?player_effect_dispose_from_old_map@@YAXXZ
void player_effect_dispose_from_old_map(void)
{
}

static __forceinline s_data_array *player_effect_object_table()
{
    return (s_data_array *)tls_object_header_data();
}

static __forceinline s_damage_effect_definition *player_effect_tag_definition(long index)
{
    return (s_damage_effect_definition *)global_tag_instances[datum_index_to_absolute_index(index)].definition;
}

static __forceinline long player_effect_unit_player(long index)
{
    return ((unit_datum *)((s_object_header *)player_effect_object_table()->data)[datum_index_to_absolute_index(index)].datum)->unit.player_index;
}


// @address 0x23e4b8 player_effect_start
extern "C" void player_effect_start(long unit_index, long user_index, long effect_index, const s_damage_owner *owner,
    const real_vector3d *direction, float magnitude, float duration, long flags, bool extra)
{
    unsigned long group_tag = tag_get_group_tag(effect_index);
    long response_index = -1;
    if (group_tag == 0x64726466) {
        response_index = effect_index;
    } else {
        s_damage_effect_definition *definition = player_effect_tag_definition(effect_index);
        response_index = definition->damage_response.index;
        if (response_index == -1)
            response_index = ((s_player_information_definition *)global_game_globals->player_information.address)->damage_response.index;
    }
    if (response_index != -1) {
        if (user_index == -1) {
            long player_index = player_effect_unit_player(unit_index);
            long index = -1;
            while (true) {
                if (player_index != -1) index = ((long)::director_get_perspective(player_index, index));
                if (index == -1) break;
                player_effect_start_response(unit_index, index, response_index, owner, direction, magnitude, duration, flags, extra);
            }
        } else {
            player_effect_start_response(unit_index, user_index, response_index, owner, direction, magnitude, duration, flags, extra);
        }
    }
}


// Registers a member of field layout 1 (see restricted_memory_register_member) and publishes its
// per-thread address. One of a family of identical allocators that differ
// only in the layout index, the update callback and the thread-local pointer.

// @address 0x23e3d0 ?__tls_set_g_player_effect_globals_allocator@@YAXPAX@Z





// @address 0x12c528 ?matrix4x3_rotation_from_axis_and_angle@@YAXPAUreal_matrix4x3@@PBTreal_vector3d@@MM@Z

static __forceinline float player_effect_ticks_to_seconds(float ticks)
{
    return ticks * TLS_GAME_TIME_GLOBALS->tick_length;
}

static __forceinline long player_effect_seconds_to_ticks_round(float seconds)
{
    return (long)(seconds * (float)TLS_GAME_TIME_GLOBALS->ticks_per_second + 0.5f);
}

static __forceinline unsigned long *player_effects_random_seed_address()
{
    return &(*get_local_random_seed_address());
}


static __forceinline real_vector3d *player_effects_random_direction3d(unsigned long *seed, real_vector3d *result)
{
    short index = _random_range(seed, 0, 0, 0, 0, 1026);
    *result = k_random_direction_table[index];
    return result;
}

static __forceinline bool player_effects_time_initialized()
{
    return TLS_GAME_TIME_GLOBALS && TLS_GAME_TIME_GLOBALS->initialized != 0;
}

static __forceinline float player_effects_pin(float value, float lower, float upper)
{
    return (value > lower ? value : lower) < upper ? (value > lower ? value : lower) : upper;
}




static __forceinline s_damage_response_definition *player_effect_tag_response_definition(long index)
{
    return (s_damage_response_definition *)global_tag_instances[datum_index_to_absolute_index(index)].definition;
}

static __forceinline unit_datum *player_effect_object_get_and_verify_type(long index)
{
    return (unit_datum *)((s_object_header *)player_effect_object_table()->data)[datum_index_to_absolute_index(index)].datum;
}

static __forceinline void player_effect_start_chud_directional_damage(long user, const real_vector3d *direction, float duration, short type,
    float intensity, float first, float second, real_argb_color *color)
{
    if ((unsigned long)user <= 3 && intensity > 0.0001f && duration > 0.0001f && (first > second ? first : second) > 0.0001f)
        chud_persistent_user_data()[user].m_directional_damage.start(direction, duration, type, intensity, first, second, color);
}

// @address 0x23e610 player_effect_start_response
extern "C" void player_effect_start_response(long unit_index, long user_index, long response_index, const s_damage_owner *owner,
    const real_vector3d *direction, float magnitude, float scale, long apply_flash, bool extra)
{
    s_damage_response_definition *definition = player_effect_tag_response_definition(response_index);
    player_effect_datum *effect = player_effect_get(user_index);
    if (unit_index != -1) {
        unit_datum *object = player_effect_object_get_and_verify_type(unit_index);
        bool shielded = object->object.shield_vitality > 0.0f;
        for (long i = 0; i < definition->classes.count(); ++i) {
            s_damage_response_class *r = &definition->classes[i];
            if ((shielded && r->type != 1) || (!shielded && r->type != 0)) {
                unsigned char apply = (unsigned char)apply_flash;
                if (!apply && (r->flags & 1)) break;
                if (apply) {
                    player_effect_update_screen_flash(user_index, effect, &r->screen_flash, magnitude);
                    player_effect_start_chud_directional_damage(
                        user_index, direction, r->directional_flash.duration, r->directional_flash.fade_function,
                            r->directional_flash.size, r->directional_flash.inner_scale, r->directional_flash.outer_scale, &r->directional_flash.color);
                }
                player_effect_update_screen_tint(user_index, effect, &r->motion_blur, scale);
                rumble_player_impulse(user_index, response_index, i, scale);
                player_effect_update_camera_impulse(user_index, effect, &r->camera_impulse, direction, scale);
                player_effect_update_camera_shake(user_index, effect, &r->camera_shake, scale);
                break;
            }
        }
    }
}

// @address 0x23ed80 player_effect_update_screen_tint
extern "C" void player_effect_update_screen_tint(long user_index, player_effect_datum *effect, const void *tint, float scale)
{
    const s_damage_response_screen_tint_definition *definition = (const s_damage_response_screen_tint_definition *)tint;
    bool apply = true;
    if (effect->tint.priority > definition->priority &&
        player_effect_ticks_to_seconds((float)effect->tint_ticks) > definition->duration)
        apply = false;
    if (!apply) return;
    memcpy(&effect->tint, definition, sizeof(effect->tint));
    unsigned char flags = effect->flags;
    float duration = effect->tint.duration;
    short rate = TLS_GAME_TIME_GLOBALS->ticks_per_second;
    effect->tint.amount[0] *= scale;
    effect->tint.amount[1] *= scale;
    effect->flags = flags | 2;
    effect->tint.amount[2] *= scale;
    float ticks = (float)rate * duration;
    effect->tint.duration = ticks;
    effect->tint_ticks = (short)ticks;
}

// @address 0x23e7e0 ?scripted_player_effect_screen_fade_in@@YAXMMMF@Z
void scripted_player_effect_screen_fade_in(float red, float green, float blue, short ticks)
{
    if (player_effects_time_initialized()) {
        float seconds = (float)ticks / 30.0f;
        g_scripted_player_effect_screen_fade_color.red = player_effects_pin(red, 0.0f, 1.0f);
        g_scripted_player_effect_screen_fade_color.green = player_effects_pin(green, 0.0f, 1.0f);
        g_scripted_player_effect_screen_fade_color.blue = player_effects_pin(blue, 0.0f, 1.0f);
        ((s_player_effect_globals *)g_player_effect_globals)->player_effect_index = (short)player_effect_seconds_to_ticks_round(seconds);
        ((s_player_effect_globals *)g_player_effect_globals)->fade_out = false;
        ((s_player_effect_globals *)g_player_effect_globals)->fade_start_time = game_time_get();
    }
}

// @address 0x23e908 ?scripted_player_effect_screen_fade_out@@YAXMMMF@Z
void scripted_player_effect_screen_fade_out(float red, float green, float blue, short ticks)
{
    if (player_effects_time_initialized()) {
        float seconds = (float)ticks / 30.0f;
        g_scripted_player_effect_screen_fade_color.red = player_effects_pin(red, 0.0f, 1.0f);
        g_scripted_player_effect_screen_fade_color.green = player_effects_pin(green, 0.0f, 1.0f);
        g_scripted_player_effect_screen_fade_color.blue = player_effects_pin(blue, 0.0f, 1.0f);
        ((s_player_effect_globals *)g_player_effect_globals)->player_effect_index = (short)player_effect_seconds_to_ticks_round(seconds);
        ((s_player_effect_globals *)g_player_effect_globals)->fade_out = true;
        ((s_player_effect_globals *)g_player_effect_globals)->fade_start_time = game_time_get();
    }
}


static __forceinline float player_effect_tint_multiplier(float amount, float scale)
{
    float value = amount * scale + 1.0f;
    if (!(value >= 0.0f))
        value = 0.0f;
    return value;
}

// @address 0x23ea30 ?player_effect_get_screen_tint_multiplier@@YAXJPATreal_rgb_color@@@Z
void player_effect_get_screen_tint_multiplier(long user_index, real_rgb_color *multiplier)
{
    multiplier->red = 1.0f;
    multiplier->green = 1.0f;
    multiplier->blue = 1.0f;
    if (user_index != NONE) {
        player_effect_datum *effect = player_effect_get(user_index);
        // match: 0x23ea30: pointer __restrict qualifier prevents alias reload
        s_damage_response_screen_tint_definition * __restrict tint = &effect->tint;
        if (effect->tint_ticks > 0 || (effect->flags & 2)) {
            float scale = 1.0f;
            if (tint->duration > 0.0f)
                scale = transition_function_evaluate(tint->transition, (float)effect->tint_ticks / tint->duration);
            multiplier->red = player_effect_tint_multiplier(tint->amount[1], scale);
            multiplier->green = player_effect_tint_multiplier(tint->amount[0], scale);
            multiplier->blue = player_effect_tint_multiplier(tint->amount[2], scale);
        }
    }
}

// @address 0x179248 ?player_effect_get_screen_flash@@YAXJPAUrender_screen_flash@@@Z
// match: 0x179248: pointer __restrict qualifier prevents alias reload
void player_effect_get_screen_flash(long user_index, render_screen_flash *__restrict flash)
{
    s_player_effect_globals *globals = (s_player_effect_globals *)g_player_effect_globals;
    flash->intensity = 0.0f;
    if (globals->player_effect_index != NONE) {
        flash->type = 2;
        flash->color.rgb = g_scripted_player_effect_screen_fade_color;
        flash->color.alpha = 1.0f;
        flash->intensity = globals->player_effect_index > 0
            ? transition_function_evaluate(5, PIN((float)(game_time_get() - globals->fade_start_time) / (float)globals->player_effect_index, 0.0f, 1.0f))
            : 1.0f;
        if (!globals->fade_out)
            flash->intensity = 1.0f - flash->intensity;
        flash->intensity = PIN(flash->intensity, 0.0f, 1.0f);
    } else if (user_index != NONE) {
        player_effect_datum *effect = player_effect_get(user_index);
        const s_damage_response_screen_flash_definition *definition = &effect->flash;
        if (effect->flash_ticks > 0 || (effect->flags & 1)) {
            flash->type = definition->type;
            flash->color = definition->color;
            if (definition->duration > 0.0f)
                flash->intensity = transition_function_evaluate(definition->transition, (float)effect->flash_ticks / definition->duration) *
                    definition->intensity;
            else
                flash->intensity = definition->intensity;
            flash->intensity = PIN(flash->intensity, 0.0f, 1.0f);
        }
    }
}

// @address 0x23eb20 ?get_shake_matrix@@YAXMMPAUreal_matrix4x3@@@Z
void get_shake_matrix(float translation, float rotation, real_matrix4x3 *matrix)
{
    s_player_effect_globals *globals = (s_player_effect_globals *)g_player_effect_globals;
    if (!(globals->enabled <= 0.0f)) {
        if (rotation != 0.0f) {
            float sine = (float)sin(rotation);
            float cosine = (float)cos(rotation);
            real_vector3d axis;
            player_effects_random_direction3d(player_effects_random_seed_address(), &axis);
            matrix4x3_rotation_from_axis_and_angle(matrix, &axis, sine, cosine);
        }
        if (translation != 0.0f) {
            real_vector3d direction;
            player_effects_random_direction3d(player_effects_random_seed_address(), &direction);
            matrix->position.x = direction.i * translation;
            matrix->position.y = direction.j * translation;
            matrix->position.z = direction.k * translation;
        }
    }
}

// @address 0x23ec88 ?player_effect_update_screen_flash@@YAXJPAUplayer_effect_datum@@PBUs_damage_response_screen_flash_definition@@M@Z
void player_effect_update_screen_flash(
    long user_index, player_effect_datum *effect, const s_damage_response_screen_flash_definition *definition, float magnitude)
{
    bool apply = true;
    if ((effect->flash.priority > definition->priority &&
            player_effect_ticks_to_seconds((float)effect->flash_ticks) > definition->duration) ||
        !(definition->intensity >= 0.0001f))
        apply = false;
    if (apply) {
        effect->flash = *definition;
        effect->flash.duration *= (float)TLS_GAME_TIME_GLOBALS->ticks_per_second;
        effect->flash_ticks = (short)effect->flash.duration;
        effect->flash.intensity *= magnitude;
        effect->flags |= 1;
    }
}

// @address 0x23f188 player_effect_update_camera_shake
void player_effect_update_camera_shake(long user_index, player_effect_datum *effect, const s_damage_response_camera_shake_definition *definition, float scale)
{
    float shake_scale = scale;
    if (player_effect_ticks_to_seconds((float)effect->shake_ticks) < definition->duration || shake_scale > effect->shake_scale ||
        (!(shake_scale < effect->shake_scale) && !(player_effect_ticks_to_seconds((float)effect->shake_ticks) >= definition->duration))) {
        effect->shake = *definition;
        effect->shake_scale = shake_scale;
        effect->shake_ticks = (short)player_effect_seconds_to_ticks_round(effect->shake.duration);
        effect->flags |= 8;
    }
}

// @address 0x23f258 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_player_effect_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_effect_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = restricted_memory_register_member(&g_restricted_regions[3], name, type, size, 0,
      __tls_set_g_player_effect_globals_allocator, 0, 0);
  unsigned int offset = g_restricted_regions[3].get_member_offset(member_index);
  void *address = restricted_region_get_base_address(3) + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  g_player_effect_globals = (s_player_effect_globals *)address;
  return restricted_region_get_base_address(3) + g_restricted_regions[3].get_member_offset(member_index);
}


// @address 0x23f2f8 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_player_effect_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_effect_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

static __forceinline real_vector3d *player_effect_cross_product3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    result->i = a->j * b->k - a->k * b->j;
    result->j = a->k * b->i - a->i * b->k;
    result->k = a->i * b->j - a->j * b->i;
    return result;
}


static __forceinline float player_effect_camera_scale_random(const float &fraction)
{
    return fraction * 2.0f - 1.0f;
}
static __forceinline float player_effect_camera_random()
{
    random_seed_next(get_local_random_seed_address());
    float fraction = (float)((*get_local_random_seed_address()) >> 16) * (1.0f / 65535.0f);
    return player_effect_camera_scale_random(fraction);
}
#if !defined(_XBOX) || defined(__clang__)
// PC (port/sdk/d3d/null_device.cpp): whether the frames between two ticks are drawn interpolated (HALO3_FPS above 30).
extern "C" unsigned long port_d3d_frame_interpolation(void);

// tmp/probe (GAME-CHANGES "High-Hz camera"): camera shake and kicks at frame rates above the tick rate.
// player_effect_apply_camera_effect_matrix runs once a main-loop pass (observer_update, observer_apply_camera_effect).
// Its shakes (a damage response's camera shake: every jump's landing shakes the camera 8 ticks, up to 8 mm; the Cortana
// effect's; a script's) take a new random direction at every call, and their envelopes and the camera impulse (a
// kick: landing, explosions, melee) follow tick counters. The console draws one frame per tick, so its shake changes
// direction 30 times a second and its kicks move once a frame. Drawing every pass, the port showed a new random shake
// in every frame (240 a second at HALO3_FPS=240: a fine jitter instead of a shake) and the kicks moving in 30 Hz steps.
// With frames between ticks drawn interpolated, each shake has one random target per tick (from the game's own
// direction table). Scripted and Cortana shakes ease between directions, and damage responses blend complete tick
// samples to ease onset, replacement and expiry too. At HALO3_FPS=30 every
// shake and kick is the console's.
namespace {
enum
{
    k_shake_stream_rotation,
    k_shake_stream_translation,
    k_shake_stream_cortana_rotation,
    k_shake_stream_cortana_translation,
    k_shake_stream_scripted,            // six values: angles 0-2, position 0-2
    k_shake_stream_count = k_shake_stream_scripted + 6
};

bool player_effect_camera_eased()
{
    return port_d3d_frame_interpolation() != 0 && player_effects_time_initialized();
}

// how far this pass is between this tick and the next
float player_effect_camera_leftover()
{
    const float leftover = TLS_GAME_TIME_GLOBALS->leftover_ticks;
    return leftover < 0.0f ? 0.0f : leftover > 1.0f ? 1.0f : leftover;
}

// a tick counter (counting down once a tick) as it stands this pass
float player_effect_camera_ticks(long ticks, float leftover)
{
    const float remaining = (float)ticks - leftover;
    return ticks <= 0 ? 0.0f : remaining > 0.0f ? remaining : 0.0f;
}

// The shake's target for a tick: a direction of the random direction table (as player_effects_random_direction3d),
// picked by a hash of the user, the stream and the tick.
const real_vector3d *player_effect_shake_target(long user_index, long stream, long tick)
{
    unsigned long hash = (unsigned long)tick * 0x9e3779b1ul ^ (unsigned long)(user_index * k_shake_stream_count + stream) * 0x85ebca6bul;
    hash ^= hash >> 16;
    hash *= 0x7feb352dul;
    hash ^= hash >> 15;
    hash *= 0x846ca68bul;
    hash ^= hash >> 16;
    return &k_random_direction_table[hash % 1026];
}

// The shake this pass: this tick's target moved toward the next tick's by the leftover.
real_vector3d player_effect_shake_vector(long user_index, long stream, float leftover)
{
    const long tick = TLS_GAME_TIME_GLOBALS->time;
    const real_vector3d *from = player_effect_shake_target(user_index, stream, tick);
    const real_vector3d *to = player_effect_shake_target(user_index, stream, tick + 1);
    real_vector3d result;
    result.i = from->i + (to->i - from->i) * leftover;
    result.j = from->j + (to->j - from->j) * leftover;
    result.k = from->k + (to->k - from->k) * leftover;
    return result;
}

// A value in [-1, 1] eased the same way (the scripted effect's, as player_effect_camera_random).
float player_effect_shake_scalar(long user_index, long stream, float leftover)
{
    return player_effect_shake_vector(user_index, stream, leftover).i;
}

// get_shake_matrix with the eased shake: the rotation about the eased axis by rotation times its length, the
// translation along the eased direction. Both lengths are at most 1, and 1 at every tick.
void get_shake_matrix_eased(long user_index, long stream, float leftover, float translation, float rotation,
    real_matrix4x3 *matrix)
{
    s_player_effect_globals *globals = (s_player_effect_globals *)g_player_effect_globals;
    if (!(globals->enabled <= 0.0f)) {
        if (rotation != 0.0f) {
            real_vector3d axis = player_effect_shake_vector(user_index, stream, leftover);
            const float length = sqrtf(axis.i * axis.i + axis.j * axis.j + axis.k * axis.k);
            if (length > 1e-6f) {
                axis.i /= length;
                axis.j /= length;
                axis.k /= length;
                const float angle = rotation * length;
                matrix4x3_rotation_from_axis_and_angle(matrix, &axis, (float)sin(angle), (float)cos(angle));
            }
        }
        if (translation != 0.0f) {
            const real_vector3d direction = player_effect_shake_vector(user_index, stream + 1, leftover);
            matrix->position.x = direction.i * translation;
            matrix->position.y = direction.j * translation;
            matrix->position.z = direction.k * translation;
        }
    }
}
}
#endif

#if !defined(_XBOX) || defined(__clang__)
// Sample the damage response at the simulation tick, including its phase and direction.
// An inactive response contributes identity, including the tick after it expires.
static void player_effect_apply_damage_camera_interpolated(long user_index, float leftover, real_matrix4x3 *matrix)
{
    const player_effect_datum *datum = player_effect_get(user_index);
    real_matrix4x3 damage = k_real_math_identity_matrix_storage;
    if (datum->impulse_ticks > 0 || (datum->flags & 4)) {
        const float scale = (datum->flags & 4) ? 1.0f :
            transition_function_evaluate(datum->impulse.transition, datum->impulse_ticks / datum->impulse.duration) *
                datum->impulse_scale;
        real_vector3d axis;
        player_effect_cross_product3d(global_up3d, &datum->direction, &axis);
        const float angle = datum->impulse.rotation * scale;
        matrix4x3_rotation_from_axis_and_angle(&damage, &axis, (float)sin(angle), (float)cos(angle));
        const float push = datum->impulse.pushback * scale;
        damage.position.x = datum->displacement.i * scale + datum->direction.i * push;
        damage.position.y = datum->displacement.j * scale + datum->direction.j * push;
        damage.position.z = datum->displacement.k * scale + datum->direction.k * push;
    }
    if (datum->shake_ticks > 0 || (datum->flags & 8)) {
        const float elapsed = datum->shake.duration - TLS_GAME_TIME_GLOBALS->tick_length * datum->shake_ticks;
        const float scale = (datum->flags & 8) ? 1.0f :
            transition_function_evaluate(datum->shake.transition, 1.0f - elapsed / datum->shake.duration) *
                datum->shake_scale;
        const float phase = datum->shake.period > 0.0f ? elapsed / datum->shake.period : 0.0f;
        const float wave = periodic_function_evaluate(datum->shake.wave, phase);
        const float amount = ((1.0f - datum->shake.wobble) + wave * datum->shake.wobble) * scale;
        real_matrix4x3 shake = k_real_math_identity_matrix_storage;
        get_shake_matrix_eased(user_index, k_shake_stream_rotation, 0.0f,
            MAX(datum->shake.translation * amount, 0.0f), MAX(datum->shake.rotation * amount, 0.0f), &shake);
        matrix4x3_compute_product(&damage, &shake, &damage);
    }
    g_damage_camera_interpolation[user_index].sample(TLS_GAME_TIME_GLOBALS->time, leftover, &damage);
    if (memcmp(&damage, &k_real_math_identity_matrix_storage, sizeof(damage)) != 0)
        matrix4x3_compute_product(matrix, &damage, matrix);
}

// Scripted and Cortana effects continue to evaluate at the time of this pass.
static void player_effect_apply_camera_effect_matrix_eased(long user_index, real_matrix4x3 *matrix)
{
    const float leftover = player_effect_camera_leftover();
    s_player_effect_globals *globals = (s_player_effect_globals *)g_player_effect_globals;
    s_scripted_player_effect *script = &globals->scripted_effect;
    real_matrix4x3 effect;
    unsigned long scripted_flags = globals->scripted_flags;
    if (scripted_flags & 1) {
        g_damage_camera_interpolation[user_index].valid = false;
        float intensity = script->intensity;
        effect = k_real_math_identity_matrix_storage;
        if (script->ticks > 0) {
            const float ticks = player_effect_camera_ticks(script->ticks, leftover);
            intensity *= (scripted_flags & 2) ? ticks / script->total_ticks : 1.0f - ticks / script->total_ticks;
        }
        intensity = PIN(intensity, 0.0f, 1.0f);
        ((s_rumble_globals *)g_rumble_globals)->scripted_scale = intensity;
        matrix4x3_rotation_from_angles(&effect,
            player_effect_shake_scalar(user_index, k_shake_stream_scripted + 0, leftover) * script->angles[0] * intensity,
            player_effect_shake_scalar(user_index, k_shake_stream_scripted + 1, leftover) * script->angles[1] * intensity,
            player_effect_shake_scalar(user_index, k_shake_stream_scripted + 2, leftover) * script->angles[2] * intensity);
        set_real_point3d(&effect.position,
            player_effect_shake_scalar(user_index, k_shake_stream_scripted + 3, leftover) * script->position[1] * intensity,
            player_effect_shake_scalar(user_index, k_shake_stream_scripted + 4, leftover) * script->position[0] * intensity,
            player_effect_shake_scalar(user_index, k_shake_stream_scripted + 5, leftover) * script->position[2] * intensity);
    } else {
        player_effect_apply_damage_camera_interpolated(user_index, leftover, matrix);
        if (g_chud_cinematic_state->index == -1) return;
        effect = k_real_math_identity_matrix_storage;
        float translation = 0.0f, rotation = 0.0f;
        s_cortana_effect_definition *definition
            = (s_cortana_effect_definition *)global_tag_instances[datum_index_to_absolute_index(g_chud_cinematic_state->index)].definition;
        if (definition->postprocessing.count() && definition->postprocessing.address &&
            ((s_cortana_effect_postprocessing *)definition->postprocessing.address)->hud_shake_amount.count) {
            real_point2d point; real_point3d offset;
            chud_compute_follower_value_2d(
                (s_chud_curve_follower_2d *)((s_cortana_effect_postprocessing *)definition->postprocessing.address)
                    ->hud_shake_amount.address,
                g_chud_cinematic_state->frame, &point, &offset, &offset);
            translation = point.x; rotation = point.y;
        }
        get_shake_matrix_eased(user_index, k_shake_stream_cortana_rotation, leftover, translation, rotation, &effect);
    }
    matrix4x3_compute_product(matrix, &effect, matrix);
}
#endif

// @address 0x12bf98 ?player_effect_apply_camera_effect_matrix@@YAXJPAUreal_matrix4x3@@@Z
void player_effect_apply_camera_effect_matrix(long user_index, real_matrix4x3 *matrix)
{
    if (user_index == -1) return;
#if !defined(_XBOX) || defined(__clang__)
    if (player_effect_camera_eased()) {
        player_effect_apply_camera_effect_matrix_eased(user_index, matrix);
        return;
    }
    g_damage_camera_interpolation[user_index].valid = false;
#endif
    s_player_effect_globals *globals = (s_player_effect_globals *)g_player_effect_globals;
    s_scripted_player_effect *script = &globals->scripted_effect;
    real_matrix4x3 effect;
    unsigned long scripted_flags = globals->scripted_flags;
    if (scripted_flags & 1) {
        float intensity = script->intensity;
        effect = k_real_math_identity_matrix_storage;
        if (script->ticks > 0)
            intensity *= (scripted_flags & 2) ? (float)script->ticks / script->total_ticks
                                              : 1.0f - (float)script->ticks / script->total_ticks;
        intensity = PIN(intensity, 0.0f, 1.0f);
        ((s_rumble_globals *)g_rumble_globals)->scripted_scale = intensity;
        matrix4x3_rotation_from_angles(&effect, player_effect_camera_random() * script->angles[0] * intensity,
            player_effect_camera_random() * script->angles[1] * intensity, player_effect_camera_random() * script->angles[2] * intensity);
        set_real_point3d(&effect.position, player_effect_camera_random() * script->position[1] * intensity,
            player_effect_camera_random() * script->position[0] * intensity,
            player_effect_camera_random() * script->position[2] * intensity);
    } else {
        player_effect_datum *user_effect = player_effect_get(user_index);
        player_effect_datum *datum = user_effect;
        if (datum->impulse_ticks > 0 || (datum->flags & 4)) {
            float scale;
            if (datum->flags & 4) scale = 1.0f;
            else scale = transition_function_evaluate(
                             datum->impulse.transition, 1.0f - (datum->impulse.duration - datum->impulse_ticks) / datum->impulse.duration) *
                datum->impulse_scale;
            real_vector3d axis;
            player_effect_cross_product3d(global_up3d, &datum->direction, &axis);
            float cosine = (float)cos(datum->impulse.rotation * scale);
            float sine = (float)sin(datum->impulse.rotation * scale);
            matrix4x3_rotation_from_axis_and_angle(&effect, &axis, sine, cosine);
            float push = datum->impulse.pushback*scale;
            effect.position.x = datum->displacement.i*scale + datum->direction.i*push;
            effect.position.y = datum->displacement.j*scale + datum->direction.j*push;
            effect.position.z = datum->displacement.k*scale + datum->direction.k*push;
            matrix4x3_compute_product(matrix, &effect, matrix);
        }
        if (datum->shake_ticks > 0 || (datum->flags & 8)) {
            effect = k_real_math_identity_matrix_storage;
            float scale;
            if (datum->flags & 8) scale = 1.0f;
            else scale = transition_function_evaluate(datum->shake.transition,
                             1.0f - (datum->shake.duration - TLS_GAME_TIME_GLOBALS->tick_length * datum->shake_ticks) / datum->shake.duration) *
                datum->shake_scale;
            float wave = periodic_function_evaluate(datum->shake.wave,
                (datum->shake.duration - TLS_GAME_TIME_GLOBALS->tick_length*datum->shake_ticks)/datum->shake.period);
            float amount = ((1.0f-datum->shake.wobble) + wave*datum->shake.wobble)*scale;
            float translation = MAX(datum->shake.translation * amount, 0.0f);
            float rotation = MAX(datum->shake.rotation * amount, 0.0f);
            get_shake_matrix(translation, rotation, &effect); matrix4x3_compute_product(matrix, &effect, matrix);
        }
        if (g_chud_cinematic_state->index == -1) return;
        effect = k_real_math_identity_matrix_storage;
        float translation = 0.0f, rotation = 0.0f;
        s_cortana_effect_definition *definition
            = (s_cortana_effect_definition *)global_tag_instances[datum_index_to_absolute_index(g_chud_cinematic_state->index)].definition;
        if (definition->postprocessing.count() && definition->postprocessing.address &&
            ((s_cortana_effect_postprocessing *)definition->postprocessing.address)->hud_shake_amount.count) {
            real_point2d point; real_point3d offset;
            chud_compute_follower_value_2d(
                (s_chud_curve_follower_2d *)((s_cortana_effect_postprocessing *)definition->postprocessing.address)
                    ->hud_shake_amount.address,
                g_chud_cinematic_state->frame, &point, &offset, &offset);
            translation = point.x; rotation = point.y;
        }
        get_shake_matrix(translation, rotation, &effect);
    }
    matrix4x3_compute_product(matrix, &effect, matrix);
}

// @address 0x72d076 ?g_render_enable_screen_flashes@@3_NA
bool g_render_enable_screen_flashes = true;
// @address 0x72d077 ?player_effects_enabled@@3_NA
bool player_effects_enabled = true;

// @address 0x18ee00 ?player_effect_render@@YAXJ@Z
void player_effect_render(long user_index)
{
    render_screen_flash flash;
    player_effect_get_screen_flash(user_index, &flash);
    float intensity = flash.intensity;
    if (!(intensity > 0.0f) || g_render_enable_screen_flashes == 0)
        return;
    short width;
    short height;
    if (rasterizer_main_get_video_mode()->fIsWideScreen != 0) {
        width = (short)c_rasterizer::render_globals.width;
        height = (short)c_rasterizer::render_globals.height;
    } else {
        width = 640;
        height = 480;
    }
    long blend_mode;
    unsigned long pixel;
    {
        real_argb_color color = flash.color;
        switch (flash.type) {
        case 0:
            blend_mode = 1;
            real_rgb_color_scale(&color.rgb, intensity, &color.rgb);
            color.alpha = color.alpha * intensity;
            break;
        case 1: {
            blend_mode = 2;
            float bias = 1.0f - intensity;
            real_argb_color_scale(&color, intensity, &color);
            real_argb_color_bias(&color, bias, &color);
            break;
        }
        case 2:
            blend_mode = 3;
            color.alpha = color.alpha * intensity;
            break;
        case 3: {
            blend_mode = 4;
            float bias = 1.0f - intensity;
            real_argb_color_scale(&color, intensity, &color);
            real_argb_color_bias(&color, bias, &color);
            break;
        }
        case 4:
            blend_mode = 6;
            real_argb_color_scale(&color, intensity, &color);
            break;
        case 5: {
            blend_mode = 7;
            color.alpha = color.alpha * intensity;
            real_argb_color_scale(&color, intensity, &color);
            break;
        }
        case 6:
            blend_mode = 10;
            color.alpha = color.alpha * (1.0f - intensity);
            break;
        default:
            break;
        }
        pixel = real_argb_color_to_pixel32(&color);
    }
    if ((pixel & 0xFF000000) != 0) {
        point2d verts[4];
        verts[1].x = width;
        verts[2].x = width;
        verts[2].y = height;
        verts[3].y = height;
        verts[0].x = 0;
        verts[0].y = 0;
        verts[1].y = 0;
        verts[3].x = 0;
        c_rasterizer_texture_ref texture_ref;
        void *definition = 0;
        texture_ref.clear();
        const long &tag_index = global_game_globals->rasterizer_globals_ref.index;
        if (tag_index != -1)
            definition = global_tag_instances[datum_index_to_absolute_index(tag_index)].definition;
        s_rasterizer_default_texture *default_texture = (s_rasterizer_default_texture *)((c_rasterizer_globals *)definition)->m_default_textures_refs.address;
        const bitmap_data *entry;
        if (bitmap_group_get_xenon_hardware_format(default_texture->bitmap.index, 0, &texture_ref, &entry, (__int64 *)0)) {
            c_rasterizer::begin_high_quality_blend();
            rasterizer_quad_screenspace_explicit(verts, pixel, texture_ref, false, (c_rasterizer::e_alpha_blend_mode)blend_mode);
        }
    }
}




static __forceinline s_player_control_state *player_effects_control_get_state(long user_index)
{
    return &player_control_globals->states[user_index];
}

static __forceinline const real_euler_angles2d *player_effects_control_get_facing_angles(long user_index)
{
    s_player_control_state *player_control = player_effects_control_get_state(user_index);
    return &player_control->action.look;
}


// @address 0x23ee80
void player_effect_update_camera_impulse(long user_index, player_effect_datum *effect,
    const s_damage_response_camera_impulse_definition *impulse, const real_vector3d *direction, float scale)
{
    float impulse_scale = scale;
    if (impulse->duration > (float)effect->impulse_ticks || impulse_scale > effect->impulse_scale ||
        (!(impulse_scale < effect->impulse_scale) && !(player_effect_ticks_to_seconds((float)effect->impulse_ticks) >= impulse->duration))) {
        real_vector3d impulse_direction = *direction;
        impulse_direction.k = 0.0f;
        normalize3d(&impulse_direction);
        real_euler_angles2d facing = *player_effects_control_get_facing_angles(user_index);
        real_vector3d forward;
        vector3d_from_euler_angles2d(&forward, &facing);
        forward.k = 0.0f;
        normalize3d(&forward);
        if (fabsf(magnitude_squared3d(&impulse_direction) - 1.0f) < 0.0001f &&
            fabsf(magnitude_squared3d(&forward) - 1.0f) < 0.0001f) {
            float angle = signed_angle_between_vectors2d(&forward.xy, &impulse_direction.xy);
            effect->impulse = *impulse;
            effect->impulse_scale = impulse_scale;
            effect->impulse.duration *= (float)TLS_GAME_TIME_GLOBALS->ticks_per_second;
            effect->impulse_ticks = (short)effect->impulse.duration;
            effect->direction.i = (float)cos(angle);
            effect->direction.j = (float)sin(angle);
            effect->direction.k = 0.0f;
            float displacement = _real_random_range(
                player_effects_random_seed_address(), effect->impulse.jitter.lower, effect->impulse.jitter.upper);
            float rotation = _real_random_range(player_effects_random_seed_address(), 0.0f, 6.28318530717958647692f);
            cross_product3d(&effect->direction, global_up3d, &effect->displacement);
            normalize3d(&effect->displacement);
            float cosine = (float)cos(rotation);
            float sine = (float)sin(rotation);
            rotate_vector_about_axis(&effect->displacement, &effect->direction, sine, cosine);
            scale_vector3d(&effect->displacement, displacement, &effect->displacement);
            effect->flags |= 4;
        }
    }
}

// @address 0x11df6cc g_player_effect_globals
__declspec(thread) s_player_effect_globals *g_player_effect_globals;

t_player_effect_globals_allocator g_player_effect_globals_allocator;

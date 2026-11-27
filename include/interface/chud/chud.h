#include "main/game_tick_time_samples.h"
#include "effects/effect_runtime_storage.h"
#ifndef INTERFACE_CHUD_CHUD_H
#define INTERFACE_CHUD_CHUD_H

struct s_chud_globals_definition;
extern s_chud_globals_definition *chud_globals;
extern "C" bool g_chud_text_render_target_dirty;

// Functions defined in tu/interfacelibxbox/chud.cpp.

void chud_set_player_training_text(long player_index, wchar_t *text);
long chud_compute_unit_backpack_weapon_index(long unit_index);
void __tls_set_g_chud_persistent_user_data_allocator(void *value);
void __tls_set_g_user_widget_allocator_0(void *object);
void chud_motion_sensor_invalidate(long user_index);
void chud_draw_screen(long user_index);

union real_point3d;
struct s_equipment_slot;
struct s_chud_curvature_info;
typedef s_chud_curvature_info chud_table_entry;
union real_euler_angles2d;
union real_vector3d;

extern "C" long chud_get_user_skin_index(long id);
extern "C" long chud_find_slot_containing_point(s_equipment_slot *slots, const real_point3d *point, bool use_alternate_radius);
extern "C" chud_table_entry *chud_get_layout_entry(long id, long flag_index);
extern "C" float chud_get_scoreboard_fade(long user_index);
extern "C" long chud_get_layout_index(long user_index);
extern "C" void chud_draw_composite(int width, int height);
extern "C" void chud_post_ammo_pickup(long user_index, long definition_index, short count);
extern "C" void chud_post_grenade_pickup(long user_index, long grenade_index);
extern "C" void chud_set_cutscene_title(const wchar_t *text, float seconds);
extern "C" void chud_set_texture_cam_enabled(bool value);
extern "C" void chud_stop_screen_effect(void);
extern "C" void chud_start_screen_effect(long index);
extern "C" void chud_compute_limited_direction(const real_vector3d *desired, const real_vector3d *current, const real_euler_angles2d *reference,
    float maximum_angle, real_vector3d *result, float *turn_direction);
extern "C" float chud_get_cinematic_fov_scale(void);
extern "C" float chud_get_cinematic_fade(void);
extern "C" void chud_draw_text_widgets(long user_index);
extern "C" unsigned char chud_initialize_draw_state(long user_index, float global_hud_alpha, bool draw_cortana_effect, bool curved_hud);

class c_chud_cortana_effect;
class c_chud_persistent_user_data;
class c_chud_update_user_data;
struct chud_metagame_state;
struct chud_saved_film_progress;
struct s_chud_widget_base;
struct s_chud_widget_collection;

extern "C" long chud_get_user_skin_key(long id);
void chud_get_widget_collection_and_definition(long tag_index, long entry_index, long element_index, s_chud_widget_collection **collection,
    s_chud_widget_base **widget, bool *text);
extern "C" bool chud_compute_curve_value_3d(void *curve, unsigned long frame, bool smooth, real_point3d *out);
extern "C" void chud_get_saved_film_progress(chud_saved_film_progress *progress);
extern "C" void chud_persistent_user_data_initialize(c_chud_persistent_user_data *data, long user_index);
extern "C" bool chud_get_metagame_state(long user_index, chud_metagame_state *state);
extern "C" long chud_can_show_action_prompt(unsigned long user_index);
extern "C" c_chud_update_user_data *chud_update_user_data_build(c_chud_update_user_data *update_data, long user_index, long last_unit_index, bool force);
extern "C" void chud_cortana_effect_render(c_chud_cortana_effect *effect, long user_index);

struct s_chud_curve_follower_2d;
union real_point2d;

extern "C" void chud_compute_follower_value_2d(s_chud_curve_follower_2d *follower, unsigned long frame, real_point2d *position, real_point3d *offset,
    real_point3d *secondary);

class c_chud_state_allocator;
struct s_chud_cinematic_strip_tuning;

extern "C" c_chud_state_allocator g_chud_state_allocator;
extern "C" s_chud_cinematic_strip_tuning g_chud_cinematic_strip_tuning;
extern bool chud_cortana_debug;
extern "C" bool g_chud_metagame_debug_enabled;
extern "C" long g_chud_debug_medal_counter;
extern "C" real_point2d g_chud_projection_normalized_scale;
extern "C" real_point2d g_chud_projection_top_center;
extern "C" real_point2d g_chud_projection_bottom_right;
extern "C" real_point2d g_chud_projection_top_left;

extern "C" float g_chud_turn_yaw_wrap;
extern "C" float g_chud_turn_yaw_limit;
// Suppresses network-status collection and forces its authority indicator.
extern "C" bool g_chud_network_status_override;

unsigned long chud_globals_get_skin_color(const s_chud_globals_definition *definition, long row, long column);

// Restricted-memory allocators owned by chud.cpp.
template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
void __tls_set_g_chud_user_data_allocator_0(void *object);
void __tls_set_g_chud_user_data_allocator_1(void *object);
void __tls_set_g_chud_user_data_allocator_2(void *object);
void __tls_set_g_chud_user_data_allocator_3(void *object);
void __tls_set_g_chud_scripting_globals_allocator(void *value);
#include "cache/restricted_memory_regions.h"
// Specialization is owned by chud.cpp; declare it before allocator users.
template<>
void t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_user_data_allocator_0>::deallocate(void *allocation);

extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_user_data_allocator_0> chud_user_data_allocator_0;
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_user_data_allocator_1> chud_user_data_allocator_1;
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_user_data_allocator_2> chud_user_data_allocator_2;
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_user_data_allocator_3> chud_user_data_allocator_3;
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_persistent_user_data_allocator> g_chud_persistent_user_data_allocator;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_scripting_globals_allocator> t_chud_scripting_globals_allocator;
extern t_chud_scripting_globals_allocator chud_scripting_globals_allocator;
extern "C" void *chud_allocate_scripting_globals(t_chud_scripting_globals_allocator *allocation,
    const char *name, const char *type, unsigned int size, long alignment_bits);

extern "C" {
extern __declspec(thread) s_game_tick_time_samples *g_main_gamestate_timing_data;
}

extern "C" void chud_initialize(void);
void chud_dispose(void);
void chud_initialize_for_new_map(void);
void chud_dispose_from_old_map(void);

#endif // INTERFACE_CHUD_CHUD_H

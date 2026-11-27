#ifndef CUTSCENE_CINEMATICS_H
#define CUTSCENE_CINEMATICS_H

#include "cseries/cseries_types.h"
#include "cache/restricted_memory_regions.h"
#include "cutscene/cinematic_globals_types.h"

// Functions defined in tu/blamlibxbox/cinematics.cpp.

union rectangle2d;

struct s_cinematic_scene;
typedef void (*cinematics_shot_proc_t)(s_cinematic_scene *scene, long shot_index, long ticks, void *user_data);
extern "C" void cinematic_iterate_shots(long ticks, cinematics_shot_proc_t shot_proc, void *user_data);

bool cinematic_in_progress();

// Local inline body for inlined call sites, named apart from the out-of-line function above:
// this build emits bl for cross-unit calls, so inlined sites need this copy.
static __forceinline bool inl_cinematic_in_progress()
{
    bool result = false;
    if (cinematic_globals)
        result = cinematic_globals->in_progress;
    return result;
}
void draw_quad(union rectangle2d *rect, unsigned long flags);
void __tls_set_g_cinematic_globals_allocator(void *value);
void cinematic_set_title_delayed(short title_index, float delay_seconds);
void cinematic_move_attached_objects(long parent_object_index, long old_marker_name, long new_marker_name, long child_marker_name);
void cinematic_object_create_from_cinematic_scene(const char *name);
void cinematic_object_destroy_internal(long index);
void cinematic_destroy();
void cinematic_clear_letterbox_bars(bool clear_depth);
void cinematic_create_and_attach_players_weapon(long object_index, long parent_marker, long child_marker);

struct presentation_parameters;
union real_vector3d;

struct s_game_non_bsp_zone_set;
class c_scenario_resource_registry;
extern "C" void cinematic_initialize(void);
void cinematic_dispose();
void cinematic_initialize_for_new_map();
void cinematic_dispose_from_old_map();
extern "C" void cinematic_update_activation(const s_game_non_bsp_zone_set *old_zone_set, const s_game_non_bsp_zone_set *new_zone_set,
    c_scenario_resource_registry *pending_zone_registry);
extern "C" void cinematic_start(void);
extern "C" void cinematic_stop(void);
extern "C" void cinematic_set_paused(unsigned char pause_requested);
extern "C" long cinematic_object_get_from_scene(long handle);
extern "C" void cinematic_reset_groups(void);
extern "C" void cinematic_activate_scene_object(long cinematic_index, long scene_index, long scene_object_index);
extern "C" void cinematic_get_letterbox_coverage(presentation_parameters *parameters);
extern "C" void chud_draw_loading_setup_view(void *, const real_vector3d *position, const real_vector3d *focus);
extern "C" void cinematic_reset_scene_and_show_chud(void);
extern "C" bool main_loading_get_action(void);
extern "C" void cinematic_setup_scene_groups(long scene_index);
extern "C" void cinematic_update(void);
extern "C" void cinematic_compute_letterbox_bars(rectangle2d *bounds, rectangle2d *top_bar, rectangle2d *bottom_bar);
extern "C" void cinematic_render(bool unused, bool render_titles);
extern "C" void cinematic_predict_sounds_in_range(s_cinematic_scene *scene, int target_shot_index, int first_frame, int end_frame);
extern "C" void cinematic_predict_shot_sounds(s_cinematic_scene *scene, long shot_index, long end_frame, void *unused);
extern "C" float cinematic_get_skip_fade(void);
extern "C" bool cinematic_can_skip(void);
extern "C" void subtitle_set(string_id id, float delay_seconds);
extern "C" long cinematic_object_get(const char *name);
extern "C" long cinematic_object_get_unit(const char *name);
extern "C" long cinematic_object_get_scenery(const char *name);


// Shared declarations used by cinematics.cpp.
template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
enum e_scene_object_type;
long cinematic_object_create(const char *name, long variant_name, long definition_index, e_scene_object_type object_type);
extern "C" void __tls_set_g_cinematic_light_globals_allocator(void *value);
void __tls_set_g_cinematic_new_globals_allocator(void *value);
extern "C" t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_globals_allocator> g_cinematic_globals_allocator;
extern "C" t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_new_globals_allocator> g_cinematic_new_globals_allocator;
extern "C" t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_light_globals_allocator> g_cinematic_light_globals_allocator;
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_globals_allocator>::reserve_memory(const char *name, const char *type, unsigned int size, long alignment_bits);
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_new_globals_allocator>::reserve_memory(const char *name, const char *type, unsigned int size, long alignment_bits);
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cinematic_light_globals_allocator>::reserve_memory(const char *name, const char *type, unsigned int size, long alignment_bits);
extern "C" const float k_widescreen_aspect_ratio;
extern "C" const float k_cinematics_perspective_m22;
extern "C" const float k_cinematics_perspective_m32;

#endif // CUTSCENE_CINEMATICS_H

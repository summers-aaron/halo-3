#ifndef INTERFACE_FIRST_PERSON_WEAPONS_H
#define INTERFACE_FIRST_PERSON_WEAPONS_H

// Functions defined in tu/interfacelibxbox/first_person_weapons.cpp.

class c_model_animation;
struct object_marker;
struct real_matrix4x3;
union real_point3d;
union real_vector3d;
struct render_first_person_model;

void __tls_set_g_first_person_weapon_orientations_allocator(void *allocator);
void __tls_set_g_first_person_weapons_allocator(void *allocator);
long first_person_weapon_find_user_index_from_weapon(long weapon_index);
long first_person_weapon_get_weapon_slot(long user_index, long weapon_index);
long first_person_weapon_state_from_message(short unit_message);
void first_person_weapons_update_camera_estimate(long user_index);
void first_person_weapon_build_model(long object_index, long render_model_index, unsigned long flags, const real_matrix4x3 *transform, long node_count,
    const real_matrix4x3 *nodes, long *node_map, render_first_person_model *model);
short first_person_weapon_get_markers_by_string_id(long weapon_index, long marker_name, struct object_marker * markers, short maximum_count);
const c_model_animation *weapon_get_deterministic_first_person_animation(long weapon_index, long animation, long *graph_index);
short weapon_get_deterministic_first_person_animation_duration_ticks(long weapon_index, long animation, short mode);
void first_person_weapon_suspend_weapon_attachment(long unit_index, bool suspend);
bool first_person_weapon_set_user_state(unsigned long user_index, long speed_divisor);
bool first_person_weapon_adjust_light(long object_index, long marker_name, real_point3d *point, real_vector3d *vector, real_vector3d *vector2);
bool first_person_weapon_attach(long user_index, long unit_index, bool force);
void first_person_weapon_update(long user_index);
extern "C" void first_person_weapons_initialize_for_new_map();
extern "C" void first_person_weapon_clear_custom_animation(long unit_index);
extern "C" void first_person_weapon_apply_weapon_ik(long user_index, render_first_person_model *unit_model, render_first_person_model *weapon_model);

struct s_animation_event_data;
struct s_owner;

extern "C" void first_person_weapons_deallocate(s_owner *self);
void first_person_weapon_start_interpolation(long user_index, long slot_index, float interpolation_time);
extern "C" void first_person_weapon_reset_channels(long user_index, long slot_index);
extern "C" bool first_person_weapon_is_deterministic_animation(long animation_name);
extern "C" bool first_person_weapon_has_custom_animation(long unit_index);
void first_person_primary_animation_event_callback(const s_animation_event_data *event, long user_index);
extern "C" bool first_person_weapon_set_custom_animation(long unit_index, long graph_index, long animation_name, unsigned char start);
extern "C" void first_person_weapon_handle_animation_event(long user_index, const s_animation_event_data *event, long weapon_slot);
void first_person_weapon_build_node_matrices(long user_index, long slot_index);

struct first_person_weapon_attachment;

void first_person_weapon_attach_weapon_slot(long user_index, long slot_index, const first_person_weapon_attachment *attachment, bool force);

// Named allocator objects owned by first_person_weapons.obj.
template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_first_person_weapons_allocator> g_first_person_weapons_allocator;
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_first_person_weapon_orientations_allocator> g_first_person_weapon_orientations_allocator;

enum e_unit_messages;

void first_person_weapon_handle_message(long unit_index, long weapon_index, e_unit_messages message, long hand);

extern "C" { extern __declspec(thread) void *g_first_person_weapon_orientation_data; }

#endif // INTERFACE_FIRST_PERSON_WEAPONS_H

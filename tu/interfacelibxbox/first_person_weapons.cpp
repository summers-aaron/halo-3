#include "effects/effect_creation.h"
// @address 0x11df8d4 g_first_person_weapon_orientation_data
// interfacelibxbox/first_person_weapons.obj: from the records 0x111620 0x1538e0 0x22e628 0x2dd4c0 0x2dd548 0x2dd5c0 0x317478 0x320aa8 0x4731d8 0x4731e8 0x4731f8 0x4733b8 0x473418 0x4735a0 0x473750 0x473768 0x473780 0x4738c0 0x4739b8 0x473b48 0x473ba0 0x473da8 0x474178 0x474c00 0x474c60 0x474d58 0x474f30 0x474ff0 0x475c10 0x475ce0 0x476988 0x476ca0 0x476d28 0x476f50 0x477600 0x477868 0x477980 0x477c88 0x477ce8 0x477d50 0x477e90 0x478078 0x478170 0x4782d0 0x478448 0x4784e8 0x478500 0x478520 0x4785c0 0x4785d8 0x57a920
// @flags /TP /O2
extern "C" {
__declspec(thread) void *g_first_person_weapon_orientation_data;
}
// @address 0x11df8d8 g_first_person_weapon_data
// @address 0x11df70c g_director_globals
// @address 0x11df878 g_object_message_queue
#include "game/player_control_globals.h"
#include "camera/observer_globals_types.h"
#include "scenario/scenario.h"
#include "interface/first_person_weapons_dependencies.h"
#include "main/main_loop_globals.h"
#include "memory/tls_data_arrays.h"
#include "memory/data_types.h"
#include "math/matrix_math_types.h"
#include "math/real_math.h"
#include "objects/object_placement.h"
#include "objects/object_types.h"
// @address 0x4731d8 ?__tls_set_g_first_person_weapon_data_allocator@@YAXPAX@Z
// @address 0x4731e8 ?__tls_set_g_first_person_weapon_orientation_data_allocator@@YAXPAX@Z
// @address 0x4785d8 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_first_person_weapon_orientations_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x477868 ?first_person_weapon_build_model@@YAXJJKPBUreal_matrix4x3@@J0PAJPAUrender_first_person_model@@@Z
// @address 0x22e628 ?__tls_set_g_first_person_weapon_orientations_allocator@@YAXPAX@Z
// @address 0x317478 ?__tls_set_g_first_person_weapons_allocator@@YAXPAX@Z
// @address 0x4733b8 ?first_person_weapon_find_user_index_from_weapon@@YAJJ@Z
// @address 0x473b48 ?first_person_weapon_get_weapon_slot@@YAJJJ@Z
// @address 0x473780 ?weapon_get_deterministic_first_person_animation@@YAPBVc_model_animation@@JJPAJ@Z
// @address 0x474c00 ?first_person_weapon_reset_sound@@YAXJJ@Z
// @address 0x1538e0 ?weapon_get_deterministic_first_person_animation_duration_ticks@@YAFJJF@Z
// @address 0x2fba78 ?get_frames_remaining@c_animation_channel@@QBAMXZ
// @address 0x28ef98 ?set_playback_speed@c_animation_channel@@QAAXM@Z
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x24dac0 ?hs_arguments_evaluate@@YAPAJJFPBF_N@Z
// @address 0x73e758 ?hs_function_table@@3PAPBUhs_function_definition@@A
// @address 0xab8a0 ?hs_return@@YAXJJ@Z
#include "cseries/cseries.h"
#include "tag_files/tag_groups.h"
#include "game/game_globals.h"
#include "units/unit_types.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "cache/restricted_memory_regions.h"
#include "objects/objects.h"
#include "camera/observer.h"
#include "effects/effects.h"
#include "game/player_control.h"
#include "hs/hs_runtime.h"
#include "interface/first_person_weapons.h"
#include "interface/user_interface_memory_allocation.h"
#include "units/unit_messages.h"
#include "tag_files/string_ids.h"
#include "items/weapon_definitions.h"
#include "math/real_math_types.h"
#include "cache/restricted_memory.h"
#include "items/weapons.h"
#include "math/random_math.h"
#include "models/render_models.h"
#include "motor/motor_system.h"
#include "motor/weapon_actions.h"
#include "objects/damage_owner.h"
#include "sound/sound_manager.h"
#include "math/matrix_math.h"
#include "animations/animation_manager.h"
#include "animations/animation_definition_types.h"
#include "animations/mixing_board/mixing_board_slider.h"
#include "sound/game_sound.h"
#include "animations/animation_id.h"
#include "animations/mixing_board/channels/channel_base.h"
#include "tag_files/tag_index.h"
#include "objects/object_datum.h"
#include "game/game_time_types.h"
#include "units/slider_movement_pattern_types.h"
#include "models/render_model_marker_types.h"
#include "render/render_first_person_model_types.h"
#include "objects/weapon_first_person_interface_definition_types.h"
#include "models/render_model_marker_group_types.h"
#include "cache/cache_files.h"
#include "hs/hs_function.h"
#include "units/slider_target_position_types.h"
#include "hs/hs_value_types.h"
#include "animations/mixing_board/mixing_board_definition.h"
#include "animations/mixing_board/mixing_board_slider_types.h"
#include "models/render_model_definitions.h"
#include "animations/animation_interpolation.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_regions_inline.h"
#include "multithreading/thread_tls.h"

#include "ai/weapon_datum_types.h"
#include "game/player_control_types.h"
#include "objects/object_definitions.h"
#include "models/model_types.h"
#include "interface/first_person_weapons_externs.h"


#include "interface/first_person_weapon_runtime_types.h"
#include "camera/director_storage_types.h"
#include "hs/hs_function_definition.h"
#include "hs/hs_macro_function.h"
#include "physics/walker_leg_definitions.h"
#include "physics/walker_physics_definitions.h"
#include "animations/ik_point_iterator.h"
#include "animations/animation_event_data.h"
#include "animations/animation_graph_node_types.h"
#include "game/player_information_definition.h"
#include "objects/object_header.h"
#include "objects/object_node_matrices_inline.h"

struct first_person_weapon_attachment
{
    long object_indices[10];
};

struct fpw_first_person;

void __tls_set_g_first_person_weapon_orientations_allocator(void *allocator)
{
    g_decal_message_queue = (t_message_queue<c_decal_message_union, 256> *)allocator;
}

void __tls_set_g_first_person_weapons_allocator(void *allocator)
{
    g_object_message_queue = (t_message_queue<s_object_render_thread_message, 2048> *)allocator;
}

// @address 0x473ba0 ?first_person_weapon_state_from_message@@YAJF@Z
long first_person_weapon_state_from_message(short unit_message)
{
    int result = -1;
    switch (unit_message) {
    case _unit_message_idle:
        return _string_id_idle;
    case _unit_message_fire_1:
        return _string_id_fire_1;
    case _unit_message_fire_2:
        return _string_id_fire_2;
    case _unit_message_misfire_1:
        return _string_id_misfire_1;
    case _unit_message_misfire_2:
        return _string_id_misfire_2;
    case _unit_message_light_on:
        return _string_id_light_on;
    case _unit_message_light_off:
        return _string_id_light_off;
    case _unit_message_reload_empty:
        return _string_id_reload_empty;
    case _unit_message_reload_full:
        return _string_id_reload_full;
    case _unit_message_fully_charged:
        return _string_id_overcharged;
    case _unit_message_overheat_enter:
        return _string_id_overheating;
    case _unit_message_overheat_exit:
        return _string_id_o_h_exit;
    case _unit_message_reload_enter:
        return _string_id_reload_enter;
    case _unit_message_reload_continue_empty:
        return _string_id_reload_continue_empty;
    case _unit_message_reload_continue_full:
        return _string_id_reload_continue_full;
    case _unit_message_reload_finished:
        return _string_id_reload_exit;
    case _unit_message_melee:
        return _string_id_melee;
    case _unit_message_put_away_primary:
    case _unit_message_put_away_secondary:
        return _string_id_put_away;
    case _unit_message_ready_primary:
    case _unit_message_ready_secondary:
        return _string_id_ready;
    case _unit_message_ready_overheated_primary:
    case _unit_message_ready_overheated_secondary:
        return _string_id_ready_overheated;
    case _unit_message_throw_grenade:
        return _string_id_throw_grenade;
    case _unit_message_throw_overheated:
        return _string_id_throw_overheated;
    case _unit_message_throw_equipment:
        return _string_id_throw_equipment;
    case _unit_message_melee_1st_hit:
        return _string_id_melee_1sthit;
    case _unit_message_melee_1st_hit_to_idle:
        return _string_id_melee_1sthit2idle;
    case _unit_message_melee_2nd_hit:
        return _string_id_melee_2ndhit;
    case _unit_message_melee_2nd_hit_to_idle:
        return _string_id_melee_2ndhit2idle;
    case _unit_message_melee_3rd_hit:
        return _string_id_melee_3rdhit;
    case _unit_message_melee_strike_1:
        return _string_id_melee_strike_1;
    case _unit_message_melee_strike_2:
        return _string_id_melee_strike_2;
    case _unit_message_melee_strike_3:
        return _string_id_melee_strike_3;
    case _unit_message_melee_strike_4:
        return _string_id_melee_strike_4;
    case _unit_message_melee_dash:
        return _string_id_melee_dash;
    case _unit_message_melee_lunge:
        return _string_id_melee_lunge;
    case _unit_message_melee_dash_airborne:
        return _string_id_melee_dash_airborne;
    case _unit_message_melee_lunge_airborne:
        return _string_id_melee_lunge_airborne;
    case _unit_message_melee_lunge_unaimed:
        return _string_id_melee_lunge_unaimed;
    case _unit_message_melee_lunge_unaimed_airborne:
        result = _string_id_melee_lunge_unaimed_airborne;
    }
    return result;
}





extern "C" {

      // this thread's base of each layout
}

void __tls_set_g_first_person_weapon_data_allocator(void *address)
{
  g_first_person_weapon_data = (fpw_first_person *)address;
}

void __tls_set_g_first_person_weapon_orientation_data_allocator(void *address)
{
  g_first_person_weapon_orientation_data = address;
}

// @address 0x478448 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_first_person_weapons_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<
    3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_first_person_weapons_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = g_restricted_regions[3].add_member(name, type, size, 0, __tls_set_g_first_person_weapon_data_allocator, 0, 0);
  unsigned int offset = g_restricted_regions[3].get_member_offset(member_index);
  void *address = restricted_region_get_base_address(3) + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  g_first_person_weapon_data = (fpw_first_person *)address;
  return restricted_region_get_base_address(3) + g_restricted_regions[3].get_member_offset(member_index);
}

// @address 0x478520 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_first_person_weapon_orientations_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<
    3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_first_person_weapon_orientations_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = g_restricted_regions[3].add_member(name, type, size, alignment_bits, __tls_set_g_first_person_weapon_orientation_data_allocator, 0, 0);
  unsigned int offset = g_restricted_regions[3].get_member_offset(member_index);
  void *address = restricted_region_get_base_address(3) + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  g_first_person_weapon_orientation_data = address;
  return restricted_region_get_base_address(3) + g_restricted_regions[3].get_member_offset(member_index);
}

// @address 0x4784e8 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_first_person_weapons_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<
    3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_first_person_weapons_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}
// @address 0x478500 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_first_person_weapons_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<
    3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_first_person_weapons_allocator>::deallocate(void *allocation)
{
    int value = m_member_index;
    g_first_person_weapon_data = 0;
    restricted_region_free_member(3, value);
}
// @address 0x4785c0 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_first_person_weapon_orientations_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<
    3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_first_person_weapon_orientations_allocator>::allocate(
    unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}


// @address 0x320aa8 ?deallocate@?$t_restricted_allocation_manager@$02$1?adopt_old_object_render_message_queue@@YAXPAX@Z$0A@$1?__tls_set_g_first_person_weapons_allocator@@YAX0@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<
    3, &adopt_old_object_render_message_queue, static_cast<void (*)(void *)>(0),
    &__tls_set_g_first_person_weapons_allocator>::deallocate(void *allocation)
{
    g_object_message_queue = 0;
    restricted_region_free_member(3, m_member_index);
}

template<>
void t_restricted_allocation_manager<
    3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_first_person_weapon_orientations_allocator>::deallocate(void *allocation)
{
    long member_index = m_member_index;
    __tls_set_g_first_person_weapon_orientation_data_allocator(0);
    restricted_region_free_member(3, member_index);
}


extern "C" {
 // @address 0x79d7dc k_mixing_board_slider_targets
}
__forceinline fpw_first_person *fpw_first_person_get(long user){return &((fpw_first_person *)g_first_person_weapon_data)[user];}
__forceinline fpw_weapon *fpw_weapon_get(fpw_first_person*fp,long slot){return &fp->weapons[slot];}
long first_person_weapon_find_user_index_from_weapon(long weapon_index)
{
    long result = -1;
    for (long user_index = 0; user_index < 4; ++user_index)
    {
        fpw_first_person *user = fpw_first_person_get(user_index);
        for (long slot = 0; slot < 2; ++slot)
        {
            fpw_weapon *weapon = fpw_weapon_get(user, slot);
            if (weapon_index == weapon->object_indices[2])
            {
                result = user_index;
                break;
            }
        }
    }
    return result;
}

long first_person_weapon_get_weapon_slot(long user_index, long weapon_index)
{
    long result = -1;
    if (user_index != -1)
    {
        fpw_first_person *user = fpw_first_person_get(user_index);
        for (long slot = 0; slot < 2; ++slot)
        {
            fpw_weapon *weapon = fpw_weapon_get(user, slot);
            if (weapon->object_indices[2] == weapon_index)
            {
                result = slot;
                break;
            }
        }
    }
    return result;
}

__forceinline void transition_reset(fpw_transition *value)
{
    unsigned short *c = &value->previous;
    unsigned short *n = &value->count;
    unsigned short *e = &value->elapsed;
    unsigned char *f = &value->flags;
    *c = value->current;
    *n = 0;
    *e = 0;
    *f = 0;
}
__forceinline void transition_set_type(fpw_transition *value, const t_short_fixed<13> *types)
{
    unsigned char *f = &value->flags;
    unsigned short *c = &value->current;
    *f = 3;
    *c = types[3].m_value;
}
// @address 0x4731f8 first_person_weapons_initialize_for_new_map
extern "C" void first_person_weapons_initialize_for_new_map(){
 const t_short_fixed<13> *transition_types=k_mixing_board_slider_targets;
 for(long user=0;user<4;++user){
  fpw_first_person *fp=fpw_first_person_get(user);
  (*(void *(*)(void *, int, unsigned int))&memset)(fp,0,sizeof(*fp));
  fp->unit_index=-1;
  memset(fp->object_indices,-1,sizeof(fp->object_indices));
  fp->camera_matrix=(*global_identity4x3);
  for(long slot=0;slot<2;++slot){
   fpw_weapon *w=fpw_weapon_get(fp,slot);
   memset(w->object_indices,-1,sizeof(w->object_indices));
   w->weapon_index=-1;w->unknown_index=-1;w->hands_node_count=-1;
   w->animations.initialize(-1,-1);
   (&w->extra_channels[0])->initialize();(&w->extra_channels[1])->initialize();
   w->ids[1].clear();w->ids[2].clear();w->ids[0].clear();w->sound_index=-1;
  }
  c_interpolator_control *control = (c_interpolator_control *)&fp->elapsed;
  control->disable();
  control->set_interpolation_type(_interpolation_type_ease_in_ease_out);
  fp->unknown_index=-1;fp->unknown_index2=-1;fp->id.clear();
  fpw_transition *t=(fpw_transition *)fp->camera_transition; transition_reset(t);transition_set_type(t,transition_types);
 }
}

// match: 0x475ce0: this setup path inlines here; the shared setup method remains out of line.
static __forceinline bool first_person_weapons_internal_setup_animation_channel_by_index(
    const c_animation_manager *manager, c_animation_channel *channel, c_animation_id id, unsigned long flags)
{
    bool result = false;
    if (manager->get_graph_tag_index() != -1)
    {
        c_animation_channel_settings settings;
        settings.m_graph_index = manager->get_graph_tag_index();
        settings.m_animation_id = id;
        settings.m_playback_flags.set_unsafe((unsigned short)flags);
        result = manager->internal_setup_animation_channel(channel, &settings);
    }
    return result;
}

static __forceinline char *first_person_weapons_orientations_get(long user_index, long slot_index)
{
    return (char *)g_first_person_weapon_orientation_data + (user_index << 13) + (slot_index << 12);
}

static __forceinline long first_person_weapons_find_user_index_from_unit(long unit_index)
{
    long result = -1;
    for (long user_index = 0; user_index < 4; ++user_index) {
        if (fpw_first_person_get(user_index)->unit_index == unit_index) {
            result = user_index;
            break;
        }
    }
    return result;
}


void first_person_weapon_reset_sound(long user_index, long slot_index)
{
    fpw_weapon *weapon = fpw_weapon_get(fpw_first_person_get(user_index), slot_index);
    if (weapon->weapon_index != -1) {
        sound_stop_impulse(weapon->weapon_index);
        weapon->weapon_index = -1;
        weapon->unknown_index = -1;
    }
}

// @address 0x474c60 ?first_person_weapon_start_interpolation@@YAXJJM@Z
void first_person_weapon_start_interpolation(long user_index, long slot_index, float interpolation_time)
{
    fpw_first_person *first_person = fpw_first_person_get(user_index);
    fpw_weapon *weapon = fpw_weapon_get(first_person, slot_index);
    if (first_person->flags & 2) {
        char *orientations = first_person_weapons_orientations_get(user_index, slot_index);
        memcpy(orientations + 2048, orientations, weapon->node_count << 5);
        if (((const c_animation_manager *)&weapon->animations)->valid()) {
            c_interpolator_control *interpolator = (c_interpolator_control *)&weapon->animations.elapsed;
            if (interpolation_time >= interpolator->get_time_remaining())
                interpolator->set_status(true, interpolation_time);
        }
    }
}


static __forceinline const real_matrix4x3 *first_person_weapons_get_adjustment(long user_index)
{
    fpw_first_person *first_person = fpw_first_person_get(user_index);
    fpw_weapon *weapon = fpw_weapon_get(first_person, 0);
    const real_matrix4x3 *result = 0;
    if (weapon->hands_node_count != -1 && (weapon->flags & 16) != 0)
        result = &weapon->camera_adjustment;
    return result;
}
void first_person_weapons_update_camera_estimate(long user_index)
{
    fpw_first_person *first_person = fpw_first_person_get(user_index);
    const s_observer_result *camera = observer_try_and_get_camera(user_index);
    if (camera) {
        matrix4x3_from_point_and_vectors(&first_person->camera_matrix, &camera->position, &camera->forward, &camera->up);
        const real_matrix4x3 *adjustment = first_person_weapons_get_adjustment(user_index);
        if (adjustment)
            matrix4x3_compute_product(&first_person->camera_matrix, adjustment, &first_person->camera_matrix);
    } else {
        first_person->camera_matrix = (*global_identity4x3);
    }
}



// @address 0x476ca0 first_person_weapon_reset_channels
void first_person_weapon_reset_channels(long user_index, long slot_index)
{
    fpw_weapon *weapon = fpw_weapon_get(fpw_first_person_get(user_index), slot_index);
    weapon->extra_channels[1].reset();
    weapon->extra_channels[0].reset();
    weapon->ids[1].clear();
    weapon->ids[2].clear();
    weapon->ids[0].clear();
    weapon->animations.reset_state_channels();
}

void first_person_weapon_build_model(long object_index, long render_model_index, unsigned long flags, const real_matrix4x3 *transform,
    long node_count, const real_matrix4x3 *nodes, long *node_map, render_first_person_model *model)
{
    model->object_index = object_index;
    model->render_model_index = render_model_index;
    model->flags = flags;
    if (node_map) {
        render_model_definition *definition = (render_model_definition *)tag_get('mode', render_model_index);
        real_matrix4x3 *model_nodes = model->nodes;
        long model_node_count = definition->nodes.count;
        if (model_node_count > 64)
            model_node_count = 64;
        for (long node_index = 0; node_index < model_node_count; ++node_index) {
            long mapped_node_index = node_map[node_index];
            if (mapped_node_index != -1) {
                if (transform)
                    matrix4x3_compute_product(transform, &nodes[mapped_node_index], &model_nodes[node_index]);
                else
                    memcpy(&model_nodes[node_index], &nodes[mapped_node_index], sizeof(real_matrix4x3));
            }
        }
    } else {
        if ((unsigned long)node_count > 64)
            node_count = 64;
        if (transform) {
            for (long node_index = 0; node_index < node_count; ++node_index)
                matrix4x3_compute_product(transform, &nodes[node_index], &model->nodes[node_index]);
        } else {
            memcpy(model->nodes, nodes, node_count * sizeof(real_matrix4x3));
        }
    }
}


// @address 0x477980 first_person_weapon_is_deterministic_animation
bool first_person_weapon_is_deterministic_animation(long animation_name)
{
    bool result = false;
    switch (animation_name) {
    case _string_id_melee:
    case _string_id_reload_empty:
    case _string_id_reload_full:
    case _string_id_melee_1sthit:
    case _string_id_melee_2ndhit:
    case _string_id_melee_3rdhit:
    case _string_id_reload_enter:
    case _string_id_melee_lunge:
    case _string_id_melee_lunge_airborne:
    case _string_id_melee_lunge_unaimed:
    case _string_id_melee_lunge_unaimed_airborne:
    case _string_id_melee_strike_1:
    case _string_id_melee_strike_2:
    case _string_id_melee_strike_3:
    case _string_id_melee_strike_4:
        result = true;
        break;
    }
    return result;
}

// @address 0x477c88 first_person_weapon_has_custom_animation
bool first_person_weapon_has_custom_animation(long unit_index)
{
    bool result = false;
    if (unit_index != -1) {
        long user_index = first_person_weapons_find_user_index_from_unit(unit_index);
        if (user_index != -1)
            result = (fpw_first_person_get(user_index)->flags & 0x80) != 0;
    }
    return result;
}

extern "C" {

}

// @address 0x477ce8 first_person_weapon_clear_custom_animation
extern "C" void first_person_weapon_clear_custom_animation(long unit_index)
{
    if (unit_index == -1)
        return;
    long user_index = first_person_weapons_find_user_index_from_unit(unit_index);
    if (user_index < 0 || user_index >= 4)
        return;
    fpw_first_person *first_person = fpw_first_person_get(user_index);
    first_person->flags &= ~0x80;
    first_person_weapon_raise(user_index, 0);
}




// @address 0x2dd4c0 first_person_weapon_set_custom_animation_evaluate
extern "C" void first_person_weapon_set_custom_animation_evaluate(short function_index, long thread_index, bool initialize)
{
    hs_value result;
    result.long_value = 0;
    hs_value *arguments = (hs_value *)hs_macro_function_evaluate(function_index, thread_index, initialize);
    if (arguments) {
        result.boolean_value = first_person_weapon_set_custom_animation(
            arguments[0].long_value, arguments[1].long_value, arguments[2].long_value, arguments[3].boolean_value);
        hs_return(thread_index, result.long_value);
    }
}

// @address 0x2dd548 first_person_weapon_has_custom_animation_evaluate
extern "C" void first_person_weapon_has_custom_animation_evaluate(short function_index, long thread_index, bool initialize)
{
    hs_value result;
    result.long_value = 0;
    hs_value *arguments = (hs_value *)hs_macro_function_evaluate(function_index, thread_index, initialize);
    if (arguments) {
        result.boolean_value = first_person_weapon_has_custom_animation(arguments[0].long_value);
        hs_return(thread_index, result.long_value);
    }
}

// @address 0x2dd5c0 first_person_weapon_clear_custom_animation_evaluate
extern "C" void first_person_weapon_clear_custom_animation_evaluate(short function_index, long thread_index, bool initialize)
{
    hs_value *arguments = (hs_value *)hs_macro_function_evaluate(function_index, thread_index, initialize);
    if (arguments) {
        first_person_weapon_clear_custom_animation(arguments[0].long_value);
        hs_return(thread_index, 0);
    }
}



// @address 0x473418 ?first_person_weapon_get_markers_by_string_id@@YAFJJPAUobject_marker@@F@Z
short first_person_weapon_get_markers_by_string_id(long weapon_index, long marker_name, struct object_marker * markers, short maximum_count)
{
    short marker_count = 0;
    void *weapon = object_try_and_get_and_verify_type(weapon_index, 4);
    if (weapon)
    {
        long user_index = first_person_weapon_find_user_index_from_weapon(weapon_index);
        if (user_index != -1)
        {
            long weapon_slot = first_person_weapon_get_weapon_slot(user_index, weapon_index);
            if (weapon_slot != -1)
            {
                fpw_first_person *user = fpw_first_person_get(user_index);
                fpw_weapon *first_person_weapon = &user->weapons[weapon_slot];
                if (marker_name != 170 && (first_person_weapon->flags & 4) &&
                    first_person_weapon->object_indices[4] != -1 &&
                    first_person_weapon->object_indices[5] != -1)
                {
                    long render_model_index = first_person_weapon->object_indices[4];
                    long node_remap_count = first_person_weapon->weapon_node_remap_count;
                    long marker_group_index = render_model_find_marker_group_by_name(render_model_index, marker_name);
                    marker_count = (short)render_model_get_markers_by_group_index(render_model_index, marker_group_index, 0,
                        node_remap_count, first_person_weapon->weapon_node_remapping, 64,
                        first_person_weapon->node_matrices, false, markers, maximum_count);
                }
                if (marker_count == 0 && (first_person_weapon->flags & 8) &&
                    first_person_weapon->object_indices[6] != -1)
                {
                    long render_model_index = first_person_weapon->object_indices[6];
                    long node_remap_count = first_person_weapon->hands_node_remap_count;
                    long marker_group_index = render_model_find_marker_group_by_name(render_model_index, marker_name);
                    marker_count = (short)render_model_get_markers_by_group_index(render_model_index, marker_group_index, 0,
                        node_remap_count, first_person_weapon->hands_node_remapping, 64,
                        first_person_weapon->node_matrices, false, markers, maximum_count);
                }
                for (long marker_index = 0; marker_index < marker_count; ++marker_index)
                    matrix4x3_compute_product(&user->camera_matrix, &markers[marker_index].matrix, &markers[marker_index].matrix);
            }
        }
    }
    return marker_count;
}

extern "C" {
}
static __forceinline long first_person_weapons_seconds_to_ticks_round(float seconds)
{
    return (long)(seconds * (float)game_time_globals->ticks_per_second + 0.5f);
}


// @address 0x475c10 first_person_weapon_update_visibility
extern "C" bool first_person_weapon_update_visibility(long user_index, long slot_index)
{
    fpw_weapon *weapon = fpw_weapon_get(fpw_first_person_get(user_index), slot_index);
    bool result = false;
    if (weapon->flags & 1) {
        bool inside = g_director_globals->slots[user_index].perspective == 0 && ((const c_animation_manager *)&weapon->animations)->valid();
        result = inside;
    }
    // match: 0x475c10: reload the weapon flags at 0x475c9c after valid(). Keeping them in r31 grows the frame from 96 to 112 bytes.
    const volatile unsigned long &updated_flags = weapon->flags;
    unsigned long flags = updated_flags;
    if (result != ((flags >> 1) & 1)) {
        if (result != 0)
            flags = flags | 2;
        else
            flags = flags & -3;
        weapon->flags = flags;
    }
    return result;
}






static __forceinline bool first_person_weapons_unit_is_dual_wielding(long unit_index)
{
    unit_datum *unit = (unit_datum *)
        object_get_and_verify_type(unit_index, 8195);
    bool result = false;
    if ((unsigned char)unit->unit.current_weapon_indices[0] != 255 && (unsigned char)unit->unit.current_weapon_indices[1] != 255)
        result = true;
    return result;
}

const c_model_animation *weapon_get_deterministic_first_person_animation(long weapon_index, long animation, long *graph_index)
{
    const c_model_animation *result = 0;
    if (weapon_index != -1)
    {
        weapon_datum *weapon = (weapon_datum *)
            object_get_and_verify_type(weapon_index, 4);
        long unit_index = weapon->item.unit_object_index;
        const weapon_definition *definition = (const weapon_definition *)tag_get('weap', weapon->definition_index);
        if (unit_index != -1)
        {
            unit_datum *unit = (unit_datum *)
                object_get_and_verify_type(unit_index, 8195);
            long player_index = unit->unit.player_index;
            if (player_index != -1)
            {
                const weapon_first_person_interface_definition *interface =
                    weapon_definition_get_first_person_interface_by_character(definition, game_get_player_character_type(player_index));
                if (interface && interface->first_person_animations.index != -1)
                {
                    c_animation_manager manager;
                    if (manager.reset_graph(interface->first_person_animations.index, -1, true))
                    {
                        long weapon_class = _string_id_current;
                        long dual_wield_unit_index = weapon->item.unit_object_index;
                        if (dual_wield_unit_index != -1 && first_person_weapons_unit_is_dual_wielding(dual_wield_unit_index))
                            weapon_class = _string_id_dual;
                        result = animation_manager_get_auxilary_animation(&manager, weapon_class, _string_id_current, animation);
                    }
                }
            }
        }
    }
    return result;
}

short weapon_get_deterministic_first_person_animation_duration_ticks(long weapon_index, long animation, short mode)
{
    short duration_ticks = 0;
    const c_model_animation *model_animation = weapon_get_deterministic_first_person_animation(weapon_index, animation, 0);
    if (model_animation)
    {
        long frame_count = model_animation->get_frame_count();
        long last_frame = model_animation->find_first_key_of_type(4);
        long frame_duration = 0;
        switch (mode)
        {
        case 0:
            frame_duration = frame_count;
            break;
        case 1:
            frame_duration = last_frame != -1 ? (frame_count > last_frame ? last_frame : frame_count) : frame_count;
            break;
        case 2:
            frame_duration = model_animation->get_primary_key_frame_index();
            break;
        case 3:
            frame_duration = model_animation->get_primary_key_frame_index();
            if (frame_duration <= 0)
                frame_duration = frame_count;
            break;
        default:
            // match: 0x1538e0: __assume(0) omits the mode<=3 jump-table range check; removal adds cmplwi+bgt and swaps r9/r10
            __assume(0);
        }
        if (frame_duration == -1)
            duration_ticks = -1;
        else if (frame_duration > 0)
        {
            duration_ticks = (short)first_person_weapons_seconds_to_ticks_round((float)frame_duration / 30.0f);
            duration_ticks = (short)(duration_ticks > 1 ? duration_ticks : 1);
        }
    }
    return duration_ticks;
}

static __forceinline void first_person_weapon_reset_interpolation(long user_index, long slot_index)
{
    fpw_weapon *weapon = fpw_weapon_get(fpw_first_person_get(user_index), slot_index);
    ((c_interpolator_control *)&weapon->animations.elapsed)->disable();
}

// @address 0x474178 ?first_person_weapon_set_state@@YA_NJJJ_N@Z
bool first_person_weapon_set_state(long user_index, long slot_index, long state, bool suppress)
{
    bool result = false;
    fpw_first_person *first_person = fpw_first_person_get(user_index);
    fpw_weapon *weapon = fpw_weapon_get(first_person, slot_index);
    if (first_person->unit_index != -1) {
        if (first_person->flags & 4) {
            weapon->sound_index = state;
            weapon->pending_suppress |= suppress;
        } else if (weapon->flags & 1) {
            c_animation_manager *manager = (c_animation_manager *)&weapon->animations;
            c_animation_channel *state_channel = (c_animation_channel *)&weapon->animations.channels[0];
            long current_state = weapon->animations.current.state;
            long weapon_class = weapon->object_indices[3];
            void *object = object_get_and_verify_type(weapon->object_indices[2], 4);
            const weapon_definition *definition =
                (const weapon_definition *)tag_get('weap', *(long *)object);
            bool finished = true;
            if (state_channel->valid())
                finished = ((state_channel->m_state_flags >> (1)) & 1) != 0;
            finished = finished || manager->state_channel_finished();
            switch (state) {
            case _string_id_ready:
                if (current_state == _string_id_ready)
                    state = -1;
                break;
            case _string_id_ready_overheated:
                if (current_state == _string_id_ready_overheated)
                    state = -1;
                break;
            case _string_id_overheating:
                if (((short)definition->reload_interrupt_mode == 3 || (short)definition->reload_interrupt_mode == 7) &&
                    current_state == _string_id_fire_2 && !finished)
                    state = -1;
                break;
            case _string_id_overcharged:
                if (definition->reload_interrupt_mode == 7 && current_state == _string_id_fire_1)
                    state = -1;
                break;
            case _string_id_light_off:
            case _string_id_light_on:
                if (current_state != _string_id_idle && current_state != _string_id_posing)
                    state = -1;
                break;
            case _string_id_fire_1:
            case _string_id_fire_2:
                if (weapon->state_timer > 0)
                    state = -1;
                break;
            case _string_id_o_h_exit:
                if (current_state != _string_id_idle && current_state != _string_id_posing && current_state != _string_id_overcharged &&
                    current_state != _string_id_overheated && current_state != _string_id_overheating &&
                    current_state != _string_id_ready_overheated && (!finished || current_state != _string_id_throw_overheated))
                    state = -1;
                break;
            case _string_id_idle:
                if (definition->reload_interrupt_mode == 7 &&
                    (current_state == _string_id_throw_grenade || current_state == _string_id_throw_overheated) && !finished)
                    state = -1;
                if ((current_state == _string_id_fire_1 || current_state == _string_id_fire_2 ||
                    current_state == _string_id_throw_grenade || current_state == _string_id_throw_overheated) && !finished)
                    state = -1;
                break;
            }
            if (state != -1) {
                bool reset_interpolation = false;
                float interpolation_time = 0.26666668f;
                switch (state) {
                case _string_id_fire_1:
                case _string_id_fire_2:
                case _string_id_ready:
                case _string_id_overheated:
                case _string_id_misfire_1:
                case _string_id_misfire_2:
                case _string_id_ready_overheated:
                case _string_id_reload_exit:
                    interpolation_time = 0.0f;
                    break;
                case _string_id_melee:
                case _string_id_reload_empty:
                case _string_id_reload_full:
                case _string_id_melee_1sthit:
                case _string_id_reload_enter:
                case _string_id_melee_strike_1:
                case _string_id_melee_strike_2:
                case _string_id_melee_strike_3:
                case _string_id_melee_strike_4:
                    interpolation_time = 0.13333334f;
                    break;
                }
                switch (state) {
                case _string_id_fire_1:
                    if (current_state == _string_id_reload_empty || current_state == _string_id_reload_full || current_state == _string_id_reload_enter ||
                        current_state == _string_id_reload_continue_empty || current_state == _string_id_reload_continue_full ||
                        current_state == _string_id_reload_exit) {
                        interpolation_time = 0.0f;
                        reset_interpolation = true;
                    }
                    break;
                case _string_id_melee:
                case _string_id_ready:
                case _string_id_ready_overheated:
                case _string_id_melee_1sthit:
                case _string_id_melee_2ndhit:
                case _string_id_melee_3rdhit:
                case _string_id_melee_strike_1:
                case _string_id_melee_strike_2:
                case _string_id_melee_strike_3:
                case _string_id_melee_strike_4:
                    reset_interpolation = true;
                    break;
                }
                if (reset_interpolation)
                    first_person_weapon_reset_interpolation(user_index, slot_index);
                if (interpolation_time > 0.0f)
                    ::first_person_weapon_start_interpolation(user_index, slot_index, interpolation_time);
                unsigned long playback_flags = 29;
                switch (state) {
                case _string_id_fire_1:
                case _string_id_fire_2:
                    if (_rotl(definition->flags, 32 - 17) & 1)
                        playback_flags = 31;
                    break;
                case _string_id_idle:
                case _string_id_overheated:
                case _string_id_overcharged:
                case _string_id_overheating:
                case _string_id_reload_continue_empty:
                case _string_id_reload_continue_full:
                    playback_flags = 31;
                    break;
                }
                if (weapon_class == _string_id_dual) {
                    if (slot_index == 0)
                        playback_flags |= 0x1000;
                    else
                        playback_flags |= 0x2000;
                }
                if (suppress && weapon->weapon_index != -1 && weapon->unknown_index == _string_id_overheating)
                    suppress = false;
                if (((const c_animation_manager *)&weapon->animations)->valid() &&
                    manager->set_goal(_string_id_current, weapon_class, 1, state, 146, playback_flags)) {
                    result = true;
                    weapon->state_timer = 0;
                    if (manager != 0 && state_channel->valid() && state_channel->m_definition_index == state &&
                        (state == _string_id_fire_1 || state == _string_id_fire_2)) {
                        if (_rotl(definition->flags, 32 - 30) & 1)
                            weapon->state_timer = (short)weapon->animations.channels[0].get_frames_remaining();
                        else if (_rotl(definition->flags, 32 - 17) & 1)
                            weapon->state_timer = 4;
                    }
                    if (suppress)
                        first_person_weapon_reset_sound(user_index, slot_index);
                }
            }
        }
    }
    return result;
}


void first_person_weapon_suspend_weapon_attachment(long unit_index, bool suspend)
{
    for (long user_index = 0; user_index < 4; ++user_index)
    {
        fpw_first_person *first_person = fpw_first_person_get(user_index);
        if (first_person->unit_index == unit_index && suspend != (bool)(((unsigned long)first_person->flags >> 2) & 1))
        {
            if (suspend)
                first_person->flags |= FLAG(2);
            else
                first_person->flags &= ~FLAG(2);
            if (!suspend)
            {
                if (((first_person->flags >> (3)) & 1))
                {
                    first_person_weapon_attach(user_index, first_person->unit_index, false);
                    first_person->flags &= ~FLAG(3);
                }
                for (long slot_index = 0; slot_index < 2; ++slot_index)
                {
                    fpw_weapon *weapon = fpw_weapon_get(first_person, slot_index);
                    if (weapon->sound_index != -1)
                    {
                        first_person_weapon_set_state(user_index, slot_index, weapon->sound_index, weapon->pending_suppress);
                        weapon->sound_index = -1;
                        weapon->pending_suppress = false;
                    }
                }
            }
        }
    }
}

// @address 0x477d50 ?first_person_weapon_set_user_state@@YA_NKJ@Z
bool first_person_weapon_set_user_state(unsigned long user_index, long speed_divisor)
{
    bool result = false;
    bool ret;
    if (user_index <= 3) {
        fpw_first_person *first_person = fpw_first_person_get(user_index);
        if (!(first_person->flags & 0x40) && !(first_person->flags & 0x10) && !(first_person->flags & 0x20)) {
            result = true;
            bool any_set = false;
            for (long slot = 0; slot < 2; ++slot) {
                c_animation_channel *channel = &first_person->weapons[slot].animations.channels[0];
                if (first_person_weapon_set_state(user_index, slot, _string_id_put_away, false)) {
                    any_set = result;
                    if (speed_divisor > 0) {
                        float speed;
                        if (!(channel->get_frames_remaining() <= 1.0f))
                            speed = channel->get_frames_remaining();
                        else
                            speed = 1.0f;
                        channel->set_playback_speed(speed / (float)(double)(int)speed_divisor);
                    }
                }
            }
            if (any_set) {
                first_person->flags = (((first_person->flags & ~0x10) | (_rotl((unsigned long)result, 4) & 0x10)) & ~0x40) |
                    (_rotl((unsigned long)result, 4) & 0x40);
                ret = result;
                goto done;
            }
            first_person->flags = (((first_person->flags & ~0x10) | (_rotl((unsigned long)result, 6) & 0x10)) & ~0x40) |
                (_rotl((unsigned long)result, 6) & 0x40);
        }
    }
    ret = result;
done:
    return ret;
}


// @address 0x4735a0 ?first_person_weapon_adjust_light@@YA_NJJPATreal_point3d@@PATreal_vector3d@@1@Z
bool first_person_weapon_adjust_light(long object_index, long marker_name, real_point3d *point, real_vector3d *vector, real_vector3d *vector2)
{
    // Ultimate parent walk, expanded inline (the out-of-line copy lives in objects.obj).
    long parent_candidate = object_index;
    long parent_index = -1;
    while (parent_candidate != -1)
    {
        parent_index = parent_candidate;
        parent_candidate = ((object_datum *)object_get_and_verify_type(parent_candidate, -1))->parent_object_index;
    }
    bool result = false;
    s_object_header *header = object_header_get(parent_index);
    if (((1 << header->object_type) & 0x1003) != 0)
    {
        long player_index = ((unit_datum *)header->datum)->unit.player_index;
        long perspective = -1;
        for (;;)
        {
            if (player_index != -1)
                perspective = ((long)::director_get_perspective(player_index, perspective));
            if (perspective == -1)
                break;
            fpw_first_person *user = fpw_first_person_get(perspective);
            long found = -1;
            for (long i = 0; i < 2; i++)
            {
                if (fpw_weapon_get(user, i)->object_indices[2] == object_index)
                {
                    found = i;
                    break;
                }
            }
            long slot = found;
            if (found < 0 || found >= 2)
                slot = 0;
            fpw_weapon *weapon = fpw_weapon_get(user, slot);
            if ((weapon->flags & 2) == 0)
                continue;
            object_marker marker;
            if ((short)::first_person_weapon_get_markers_by_string_id(weapon->object_indices[2], marker_name, &marker, 1) == 0)
                continue;
            *point = marker.matrix.position;
            *vector = marker.matrix.rotation.forward;
            *vector2 = marker.matrix.rotation.up;
            result = true;
            break;
        }
    }
    return result;
}
// @address 0xf07d0 ?matrix4x3_rotation_from_vectors@@YAXPAUreal_matrix4x3@@PBTreal_vector3d@@1@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0x88cd0 ?observer_try_and_get_camera@@YAPBUs_observer_result@@J@Z
// @address 0x9cff8 ?render_model_find_marker_group_by_name@@YAJJJ@Z
// @address 0xdb528 ?reset@c_animation_channel@@QAAXXZ
// @address 0x28e1a8 ?reset_graph@c_animation_manager@@QAA_NJJ_N@Z
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0x28ff10 ?set_goal@c_animation_manager@@QAA_NJJJJJK@Z
// @address 0x11a2e8 ?set_status@c_interpolator_control@@QAAX_NM@Z
// @address 0x1a35d0 ?sound_stop_impulse@@YAXJ@Z
// @address 0x11a238 ?state_channel_finished@c_animation_manager@@QBA_NXZ
// @address 0xdc770 ?valid@c_animation_manager@@QBA_NXZ
// @address 0x11a000 ?weapon_definition_get_first_person_interface_by_character@@YAPBUweapon_first_person_interface_definition@@PBUweapon_definition@@J@Z
// @address 0x28e078 ??0c_animation_manager@@QAA@XZ
// @address 0x3497f8 ??1c_animation_manager@@QAA@XZ

// @address 0x476d28 ?first_person_weapon_setup_animations@@YAXJJ_N@Z
void first_person_weapon_setup_animations(long user_index, long slot_index, bool update_goal)
{
    fpw_first_person *first_person = fpw_first_person_get(user_index);
    fpw_weapon *weapon = fpw_weapon_get(first_person, slot_index);
    weapon->extra_channels[1].reset();
    weapon->extra_channels[0].reset();
    weapon->ids[1].clear();
    weapon->ids[2].clear();
    weapon->ids[0].clear();
    c_animation_manager *manager = (c_animation_manager *)&weapon->animations;
    manager->setup_animation_channel_by_parallel_impulse(&weapon->extra_channels[1], weapon->object_indices[3],
        _string_id_current, _string_id_overcharged_jitter, 31);
    weapon->ids[1] = manager->find_parallel_impulse_animation_id(weapon->object_indices[3], _string_id_current, _string_id_overlays);
    weapon->ids[2] = manager->find_parallel_impulse_animation_id(weapon->object_indices[3], _string_id_current, _string_id_ammunition);
    weapon->ids[0] = manager->find_parallel_impulse_animation_id(weapon->object_indices[3], _string_id_current, _string_id_pitch_and_turn);
    bool result = false;
    if (update_goal) {
        float playback_ratio = animation_channel_get_playback_ratio((const c_animation_channel *)manager);
        long weapon_class = weapon->object_indices[3];
        unsigned long playback_flags = 29;
        if (weapon_class == _string_id_dual)
            playback_flags = 0x101d + (slot_index != 0 ? 0x1000 : 0);
        if (manager->set_goal(_string_id_current, weapon_class, 1, _string_id_current, 146, playback_flags)) {
            result = true;
            if (((c_animation_channel *)manager)->valid())
                ((c_animation_channel *)manager)->set_frame_position_from_playback_ratio(playback_ratio);
            ::first_person_weapon_start_interpolation(user_index, slot_index, k_standard_interpolation_time);
        }
    }
    if (!result) {
        first_person_weapon_set_state(user_index, slot_index, _string_id_idle, true);
        if (slot_index == 0) {
            fpw_transition *transition = (fpw_transition *)first_person->camera_transition;
            transition_reset(transition);
            transition_set_type(transition, k_mixing_board_slider_targets);
        }
    }
}

// @address 0x477e90 first_person_weapon_raise
extern "C" bool first_person_weapon_raise(unsigned long user_index, long frames)
{
    bool result = false;
    if (user_index <= 3) {
        fpw_first_person *first_person = fpw_first_person_get(user_index);
        bool immediate = frames < 0;
        long flags = first_person->flags;
        if ((flags & 0x10 || flags & 0x40) && (immediate || !(flags & 0x20)) &&
            !(flags & 0x80) && !(flags & 0x100) && !(flags & 0x200)) {
            first_person->flags = flags & ~0x70;
            first_person_weapon_attach(user_index, first_person->unit_index, true);
            bool any_set = false;
            for (long slot_index = 0; slot_index < 2; ++slot_index) {
                fpw_weapon *weapon = fpw_weapon_get(first_person, slot_index);
                c_animation_channel *channel = &weapon->animations.channels[0];
                if ((weapon->flags & 2) && (weapon->flags & 1)) {
                    first_person_weapon_setup_animations(user_index, slot_index, false);
                    if (first_person_weapon_set_state(user_index, slot_index, _string_id_ready, false)) {
                        any_set = true;
                        if (immediate) {
                            if (((c_animation_channel *)channel)->valid())
                                channel->set_frame_position_from_playback_ratio(1.0f);
                        } else if (frames > 0) {
                            float speed;
                            if (!(channel->get_frames_remaining() <= 1.0f))
                                speed = channel->get_frames_remaining();
                            else
                                speed = 1.0f;
                            channel->set_playback_speed(speed / (float)(double)(int)frames);
                        }
                    }
                }
            }
            if (any_set)
                first_person->flags |= 0x20;
            result = true;
        }
    }
    return result;
}

struct s_animation_event_data;

// @address 0x473750 ?first_person_primary_animation_event_callback@@YAXPBUs_animation_event_data@@J@Z
void first_person_primary_animation_event_callback(const s_animation_event_data *event, long user_index)
{
    first_person_weapon_handle_animation_event(user_index, event, 0);
}

// @address 0x473768 ?first_person_secondary_animation_event_callback@@YAXPBUs_animation_event_data@@J@Z
void first_person_secondary_animation_event_callback(const s_animation_event_data *event, long user_index)
{
    first_person_weapon_handle_animation_event(user_index, event, 1);
}

// @address 0x4782d0 first_person_weapon_update_custom_animation
extern "C" bool first_person_weapon_update_custom_animation(unsigned long user_index)
{
    bool result = false;
    if (user_index <= 3) {
        fpw_first_person *first_person = fpw_first_person_get(user_index);
        if (first_person->flags & 0x40) {
            result = true;
            if (first_person->flags & 0x80) {
                fpw_weapon *weapon = fpw_weapon_get(first_person, 0);
                bool playing = false;
                c_animation_manager *manager = (c_animation_manager *)&weapon->animations;
                if (manager->valid()) {
                    char *orientations = first_person_weapons_orientations_get(user_index, 0);
                    if (manager->update_state_animation(first_person_primary_animation_event_callback, user_index, weapon->node_count,
                            (real_orientation *)orientations + 64, (real_orientation *)orientations))
                    {
                        c_animation_channel *state_channel = (c_animation_channel *)&weapon->animations.channels[0];
                        bool finished = true;
                        if (state_channel->valid())
                            finished = ((state_channel->m_state_flags >> (1)) & 1) != 0;
                        if (!finished && !manager->state_channel_finished())
                            playing = true;
                    }
                    animation_manager_update_interpolator(manager);
                    first_person_weapon_build_node_matrices(user_index, 0);
                }
                if (!playing) {
                    first_person->unknown_index = -1;
                    first_person->unknown_index2 = -1;
                    first_person->flags &= ~0x80;
                    first_person->id.clear();
                }
            }
            if (first_person_weapon_raise(user_index, 0))
                result = false;
        }
    }
    return result;
}

static __forceinline bool first_person_weapons_next_overheated(const weapon_datum *weapon)
{
    bool result;
    if (weapon->triggers[0].state == 1 || weapon->triggers[0].state == 2)
        result = true;
    else
        result = false;
    return result;
}
// @address 0x473da8 ?first_person_weapon_next_state@@YAXJJ@Z
void first_person_weapon_next_state(long user_index, long slot_index)
{
    fpw_first_person *first_person = fpw_first_person_get(user_index);
    fpw_weapon *weapon = fpw_weapon_get(first_person, slot_index);
    weapon_datum *object = (weapon_datum *)object_get_and_verify_type(weapon->object_indices[2], 4);
    const weapon_definition *definition = (const weapon_definition *)tag_get('weap', object->definition_index);
    long state = weapon->animations.current.state;
    long next = _string_id_idle;
    if (state <= _string_id_melee_1sthit) {
      if (state != _string_id_melee_1sthit) {
      switch (state) {
    case _string_id_fire_1: case _string_id_fire_2:
        if (weapon->state_timer > 0)
            return;
        next = _string_id_idle;
        if (((short)definition->reload_interrupt_mode == 3 || (short)definition->reload_interrupt_mode == 7) && state == _string_id_fire_2) {
            next = _string_id_overheated;
            break;
        }
        break;
    case _string_id_throw_overheated: case _string_id_overheating: case _string_id_ready_overheated:
        next = unit_weapon_in_overheated_loop(first_person->unit_index, slot_index) ? _string_id_overheated : _string_id_o_h_exit;
        first_person_weapon_set_state(user_index, slot_index, next, false);
        return;
    case _string_id_overheated:
        if (unit_weapon_in_overheated_loop(first_person->unit_index, slot_index))
            return;
        next = _string_id_o_h_exit;
        break;
    case _string_id_overcharged:
        if (first_person_weapons_next_overheated(object))
            return;
        next = _string_id_idle;
    default:
        break;
    case _string_id_put_away:
        weapon->animations.pause_playback(true);
        return;
      }
      } else
        next = _string_id_melee_1sthit2idle;
    } else {
      if (state <= _string_id_melee_dash) {
        if (state != _string_id_melee_dash) {
          if (state != _string_id_melee_2ndhit) {
            if ((unsigned long)(state - _string_id_reload_enter) <= 2) {
            if (!weapon->animations.paused()) {
                weapon->animations.pause_playback(true);
                return;
            }
            next = _string_id_reload_exit;
            }
          } else
            next = _string_id_melee_2ndhit2idle;
        } else
            next = _string_id_melee_lunge;
      } else if (state == _string_id_melee_dash_airborne)
          next = _string_id_melee_lunge_airborne;
    }
    if (next == _string_id_idle && first_person_weapons_next_overheated(object))
        next = _string_id_overcharged;
    first_person_weapon_set_state(user_index, slot_index, next, false);
}

// 0x57a920


// c_walker_physics::calculate_foot_matrix
// @address 0x57a920 walker_physics_calculate_foot_matrix
extern "C" void walker_physics_calculate_foot_matrix(const s_walker_physics_definition *definition, long object_index,
    long leg_index, bool use_marker_position, real_matrix4x3 *matrix)
{
    use_marker_position = false;
    const s_walker_leg_definition *leg = &definition->legs[leg_index];
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, 0xffffffff);
    const s_object_definition *object_definition = (const s_object_definition *)tag_get(
        'obje', object->definition_index);
    const s_model_definition *model = (const s_model_definition *)tag_get(
        'hlmt', object_definition->model.index);
    const render_model_definition *render_model =
        (const render_model_definition *)tag_get('mode', model->render_model_index);
    long marker_group_index = render_model_find_marker_group_by_name(model->render_model_index, leg->knee_marker_names[0]);
    const render_model_marker_group *marker_group = &((const render_model_marker_group *)render_model->marker_groups.address)[marker_group_index];
    const render_model_marker *marker = &((const render_model_marker *)marker_group->markers.address)[leg->runtime_marker_index];
    const real_matrix4x3 *node_matrix = object_get_node_matrix(object_index, leg->node);
    real_orientation orientation;
    orientation.rotation = marker->rotation;
    orientation.scale = marker->scale;
    if (use_marker_position || !(((long)leg->leg_flags >> (0)) & 1))
        orientation.translation = marker->translation;
    else
        orientation.translation = leg->planted_position;
    real_matrix4x3 marker_matrix;
    // match: 0x57a920: expanded inline (the image calls only the inner quaternion/product routines); the shared function stays out of line.
    matrix3x3_rotation_from_quaternion(&marker_matrix.rotation, &orientation.rotation);
    marker_matrix.position = orientation.translation;
    marker_matrix.scale = orientation.scale;
    matrix4x3_compute_product(node_matrix, &marker_matrix, matrix);
}
// @address 0x111620 ?first_person_weapons_handle_deleted_object@@YAXJ@Z
void first_person_weapons_handle_deleted_object(long object_index)
{
    for (long user_index = 0; user_index < 4; user_index++)
    {
        fpw_first_person *first_person = fpw_first_person_get(user_index);
        for (long slot_index = 0; slot_index < 2; slot_index++)
        {
            fpw_weapon *weapon = fpw_weapon_get(first_person, slot_index);
            if (weapon->object_indices[2] == object_index)
            {
                first_person_weapon_attachment attachment;
                memcpy(&attachment, weapon->object_indices, sizeof(attachment));
                attachment.object_indices[2] = NONE;
                first_person_weapon_attach_weapon_slot(user_index, slot_index, &attachment, true);
            }
        }
        if (first_person->unit_index == object_index)
            first_person_weapon_attach(user_index, NONE, true);
    }
}

static __forceinline bool first_person_weapons_restore_user(long user_index)
{
    bool result;
    fpw_first_person *first_person = fpw_first_person_get(user_index);
    if (((first_person->flags >> (7)) & 1))
    {
        for (long slot_index = 0; slot_index < 2; slot_index++)
        {
            first_person_weapon_reset_channels(user_index, slot_index);
            first_person_weapon_attachment attachment = *(const first_person_weapon_attachment *)fpw_weapon_get(first_person, slot_index)->object_indices;
            attachment.object_indices[4] = NONE;
            attachment.object_indices[5] = first_person->unknown_index;
            first_person_weapon_attach_weapon_slot(user_index, slot_index, &attachment, true);
        }
        fpw_weapon *weapon = fpw_weapon_get(first_person, 0);
        result = weapon->animations.graph != NONE;
        if (result)
        {
            result = ((c_animation_manager *)&weapon->animations)->set_custom_goal(first_person->id, 1);
            ::first_person_weapon_start_interpolation(user_index, 0, k_standard_interpolation_time);
        }
        if (!result)
        {
            fpw_first_person_get(user_index)->flags &= ~0x80;
            first_person_weapon_raise(user_index, 0);
            return result;
        }
    }
    else
    {
        for (long slot_index = 0; slot_index < 2; slot_index++)
            ((c_animation_manager *)&fpw_weapon_get(first_person, slot_index)->animations)->set_state_position_to_last_frame();
        result = true;
    }
    return result;
}

// @address 0x478170 first_person_weapon_restore_custom_animation
extern "C" bool first_person_weapon_restore_custom_animation(long user_index)
{
    bool result = false;
    if (VALID_INDEX(user_index, 4))
        result = first_person_weapons_restore_user(user_index);
    return result;
}

static __forceinline bool first_person_weapons_set_custom_animation_for_user(
    fpw_first_person *users, long user_index, long graph_index, long animation_name, unsigned char start)
{
    bool result = false;
    const c_model_animation_graph *graph = (const c_model_animation_graph *)global_tag_instances[(unsigned short)graph_index].definition;
    if (graph)
    {
        c_animation_id animation = graph->find_animation(animation_name);
        if (animation.valid())
        {
            fpw_first_person *first_person = &users[user_index];
            result = true;
            first_person->flags |= 0x80;
            first_person->unknown_index = graph_index;
            first_person->id = animation;
            if (!start || !first_person_weapon_set_user_state(user_index, 0))
                result = first_person_weapon_restore_custom_animation(user_index);
        }
    }
    return result;
}

// @address 0x478078 first_person_weapon_set_custom_animation
extern "C" bool first_person_weapon_set_custom_animation(long unit_index, long graph_index, long animation_name, unsigned char start)
{
    fpw_first_person *users = (fpw_first_person *)g_first_person_weapon_data;
    long user_index = first_person_weapons_find_user_index_from_unit(unit_index);
    bool result = false;
    if (VALID_INDEX(user_index, 4))
    {
        const c_model_animation_graph *graph = (const c_model_animation_graph *)global_tag_instances[(unsigned short)graph_index].definition;
        if (graph)
        {
            c_animation_id animation = graph->find_animation(animation_name);
            if (animation.valid())
            {
                fpw_first_person *first_person = &users[user_index];
                result = true;
                first_person->flags |= 0x80;
                first_person->unknown_index = graph_index;
                first_person->id = animation;
                if (!start || !first_person_weapon_set_user_state(user_index, 0))
                    result = first_person_weapon_restore_custom_animation(user_index);
            }
        }
    }
    switch (result)
    {
    case false:
    case true:
        return result;
    default:
        // match: 0x478078: unreachable hint omits dead branch check
        __assume(0);
    }
}
// @address 0x477600 first_person_weapon_apply_weapon_ik
extern "C" void first_person_weapon_apply_weapon_ik(long user_index, render_first_person_model *unit_model, render_first_person_model *weapon_model)
{
    fpw_first_person *first_person = fpw_first_person_get(user_index);
    fpw_weapon *primary = fpw_weapon_get(first_person, 0);
    fpw_weapon *secondary = fpw_weapon_get(first_person, 1);
    if ((primary->flags & 2) != 0 && (secondary->flags & 2) == 0 &&
        primary->object_indices[2] != -1 && primary->object_indices[3] != _string_id_dual)
    {
        s_object_header *header = object_header_get(first_person->unit_index);
        if (((1 << header->object_type >> (0)) & 1) != 0)
        {
            fpw_manager *animations = &primary->animations;
            if (((const c_animation_manager *)animations)->valid())
            {
                t_short_fixed<13> slider_value;
                memcpy(&slider_value, first_person->camera_transition, sizeof(slider_value));
                if (slider_value.m_value > 0)
                {
                    float blend = slider_value.export_real();
                    ik_point_iterator iterator;
                    iterator.marker = -1;
                    iterator.attach_to_marker = -1;
                    iterator.index = -1;
                    iterator.mode = 0;
                    iterator.mode_name = animations->current.mode;
                    const c_model_animation_graph *graph =
                        (const c_model_animation_graph *)tag_get('jmad', animations->graph);
                    iterator.weapon_class_name = animations->current.weapon_class;
                    if (graph->find_next_weapon_ik_point(&iterator))
                    {
                        do
                        {
                            long attach_marker = iterator.attach_to_marker;
                            if (attach_marker != -1 && attach_marker != 0)
                            {
                                long unit_render_model = primary->object_indices[6];
                                long weapon_render_model = primary->object_indices[4];
                                if (unit_render_model != -1 && weapon_render_model != -1)
                                {
                                    render_model_definition *unit_render =
                                        (render_model_definition *)tag_get('mode', unit_render_model);
                                    long unit_group =
                                        render_model_find_marker_group_by_name(unit_render_model, iterator.marker);
                                    long weapon_group =
                                        render_model_find_marker_group_by_name(weapon_render_model, attach_marker);
                                    if (unit_group != -1 && weapon_group != -1)
                                    {
                                        render_model_marker_group *unit_markers =
                                            (render_model_marker_group *)unit_render->marker_groups.address + unit_group;
                                        render_model_definition *weapon_render =
                                            (render_model_definition *)tag_get('mode', weapon_render_model);
                                        render_model_marker_group *weapon_markers =
                                            (render_model_marker_group *)weapon_render->marker_groups.address + weapon_group;
                                        if (unit_markers->markers.count > 0 && weapon_markers->markers.count > 0)
                                        {
                                            render_model_marker *unit_marker =
                                                (render_model_marker *)unit_markers->markers.address;
                                            render_model_marker *weapon_marker =
                                                (render_model_marker *)weapon_markers->markers.address;
                                            real_matrix4x3 unit_marker_matrix;
                                            real_matrix4x3 weapon_marker_matrix;
                                            matrix4x3_from_point_and_quaternion(
                                                &unit_marker_matrix, &unit_marker->translation, &unit_marker->rotation);
                                            matrix4x3_from_point_and_quaternion(
                                                &weapon_marker_matrix, &weapon_marker->translation, &weapon_marker->rotation);
                                            matrix4x3_compute_product(&weapon_model->nodes[weapon_marker->node_index],
                                                         &weapon_marker_matrix, &weapon_marker_matrix);
                                            biped_apply_inverse_kinematics(unit_render_model, unit_marker->node_index,
                                                         &unit_marker_matrix, &weapon_marker_matrix,
                                                         blend, unit_render->nodes.count, unit_model->nodes);
                                        }
                                    }
                                }
                            }
                            iterator.mode_name = animations->current.mode;
                            graph = (const c_model_animation_graph *)tag_get('jmad', animations->graph);
                            iterator.weapon_class_name = animations->current.weapon_class;
                        } while (graph->find_next_weapon_ik_point(&iterator));
                    }
                }
            }
        }
    }
}

struct effect_vector_field;

// @address 0x474d58 first_person_weapon_handle_animation_event
extern "C" void first_person_weapon_handle_animation_event(long user_index, const s_animation_event_data *event, long weapon_slot)
{
    if (g_director_globals->slots[user_index].perspective == 0)
    {
        fpw_weapon *weapon = fpw_weapon_get(fpw_first_person_get(user_index), weapon_slot);
        long weapon_index = weapon->object_indices[2];
        if (weapon_index != -1)
        {
            if (event->event_type == 1)
            {
                long sound_index = event->definition_index;
                if (sound_index != -1)
                {
                    bool play = true;
                    if (((event->flags >> (6)) & 1))
                    {
                        weapon_datum *weapon_data = (weapon_datum *)
                            object_get_and_verify_type(weapon_index, _object_mask_weapon);
                        if (weapon_is_age_based(weapon_data->definition_index)
                            && fabsf(weapon_data->age - 1.0f) < 0.0001f)
                            play = false;
                    }
                    if (!play)
                        return;
                    long sound_result = object_impulse_sound_new(weapon_index, sound_index, -1,
                        global_origin3d, global_forward3d, 1.0f);
                    weapon->weapon_index = sound_result;
                    weapon->unknown_index = weapon->animations.current.state;
                    return;
                }
            }
            if (event->event_type == 2)
            {
                long effect_index = event->definition_index;
                if (effect_index != -1)
                {
                    s_damage_owner owner;
                    damage_compute_damage_owner_from_object_index(weapon_index, &owner);
                    long marker = event->marker_name;
                    if (marker != -1 && marker != 0)
                    {
                        effect_new_attached_from_marker_name(effect_index, &owner, 104, weapon_index, -1, marker, 0.0f, 0.0f, 0, 0, 0, 0);
                        return;
                    }
                    effect_new_from_object(effect_index, &owner, 104, weapon_index, 0.0f, 0.0f, 0, 0, 0);
                }
            }
        }
    }
}

enum e_unit_messages;
// @address 0x4739b8 ?first_person_weapon_handle_message@@YAXJJW4e_unit_messages@@J@Z
void first_person_weapon_handle_message(long unit_index, long weapon_index, e_unit_messages message, long slot_index)
{
    long state = first_person_weapon_state_from_message((short)message);
    for (long user_index = 0; user_index < 4; ++user_index)
    {
        fpw_first_person *first_person = fpw_first_person_get(user_index);
        if (first_person->unit_index == unit_index)
        {
            fpw_first_person *user = fpw_first_person_get(user_index);
            fpw_weapon *weapon = fpw_weapon_get(user, slot_index);
            switch (message)
            {
            case _unit_message_ready_primary:
            case _unit_message_ready_secondary:
            case _unit_message_put_away_end_primary:
            case _unit_message_put_away_end_secondary:
                first_person_weapon_attach(user_index, user->object_indices[0], false);
                break;
            case _unit_message_fire_1:
            case _unit_message_fire_2:
                weapon->firing_time += 0.05f;
                break;
            }
            bool suppress = weapon->weapon_index != -1 && weapon->unknown_index != state;
            long message_value = message;
            if (message_value >= _unit_message_reload_continue_empty && message_value <= _unit_message_reload_finished)
                suppress = false;
            if (state != -1)
                first_person_weapon_set_state(user_index, slot_index, state, suppress);
        }
    }
    if (weapon_index != -1)
        weapon_setup_first_person_emulation(weapon_index, state);
}


extern "C" {
}

static __forceinline const c_model_animation_graph *first_person_weapons_get_graph(long graph_tag_index)
{
    return (const c_model_animation_graph *)tag_get('jmad', graph_tag_index);
}
static __forceinline c_animation_id first_person_weapons_channel_get_animation_id(const c_animation_channel *channel)
{
    return channel->m_animation_id;
}
static __forceinline const c_model_animation *first_person_weapons_channel_get_animation(const c_animation_channel *channel)
{
    const c_model_animation *animation = NULL;
    if (channel->m_animation_id.valid())
        animation = first_person_weapons_get_graph(channel->m_graph_index)->get_animation(channel->m_animation_id);
    return animation;
}
static __forceinline const c_model_animation *first_person_weapons_channel_find_animation(const c_animation_channel *channel)
{
    const c_model_animation *animation = NULL;
    if (first_person_weapons_channel_get_animation_id(channel).valid())
        first_person_weapons_get_graph(channel->m_graph_index)->get_graph_and_animation(channel->m_animation_id, NULL, &animation);
    return animation;
}
static __forceinline const c_model_animation *first_person_weapons_get_state_animation(const fpw_manager *animations)
{
    const c_animation_channel *state_channel = (const c_animation_channel *)&animations->channels[0];
    const c_model_animation *animation = NULL;
    if (state_channel->m_animation_id.valid())
        animation = first_person_weapons_channel_find_animation(state_channel);
    return animation;
}
static __forceinline short first_person_weapons_graph_get_node_count(const c_model_animation_graph *graph)
{
    return (short)graph->m_nodes.count;
}
static __forceinline long first_person_weapons_get_node_count(const fpw_manager *animations)
{
    const c_model_animation_graph *graph = first_person_weapons_get_graph(animations->graph);
    long node_count = 0;
    if (graph)
        node_count = first_person_weapons_graph_get_node_count(graph);
    return node_count;
}
static __forceinline bool first_person_weapons_animations_valid_goals(const fpw_manager *animations)
{
    bool valid_goals = ((animations->flags >> (6)) & 1);
    return valid_goals;
}
static __forceinline bool first_person_weapons_animations_valid(const fpw_manager *animations)
{
    return animations->graph != -1 && first_person_weapons_animations_valid_goals(animations) &&
        animations->channels[0].valid();
}
static const float k_interpolation_ratio_epsilon = 0.0001f;
static __forceinline void first_person_weapons_interpolate_node_orientations(
    const c_interpolator_control *control, short node_count, const real_orientation *orientations, real_orientation *result_orientations)
{
    if (control->enabled())
    {
        float ratio = control->get_ratio();
        if (fabsf(ratio - 1.0f) >= k_interpolation_ratio_epsilon)
        {
            for (short node_index = 0; node_index < node_count; node_index++, orientations++, result_orientations++)
                orientation_interpolate(orientations, result_orientations, ratio, result_orientations);
        }
    }
}
static __forceinline void first_person_weapons_matrix4x3_translation(real_matrix4x3 *matrix, const real_point3d *translation)
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
static __forceinline void first_person_weapons_compute_view_offset_matrix(long user_index, real_matrix4x3 *view_matrix)
{
    const s_player_block *observer = &g_observer_globals->blocks[user_index];
    float tangent = tan(0.5f * observer->camera_result.vertical_field_of_view);
    float yaw = atan(0.0f);
    float pitch = atan(tangent * observer->view_offset.vertical);
    matrix3x3_rotation_from_angles(&view_matrix->rotation, -yaw, -pitch, 0.0f);
    view_matrix->position = *global_origin3d;
    view_matrix->scale = 1.0f;
}
static __forceinline long first_person_weapons_get_rounds_total(long weapon_index, short magazine_index)
{
    const weapon_datum *weapon =
        (const weapon_datum *)object_get_and_verify_type(weapon_index, 4);
    return weapon->magazines[0].rounds_loaded + weapon_get_rounds_inventory(weapon_index, magazine_index, false);
}
template <typename t_type>
static __forceinline const t_type &first_person_weapons_pin(const t_type &value, const t_type &minimum, const t_type &maximum)
{
    const t_type &bounded = value > minimum ? value : minimum;
    return bounded < maximum ? bounded : maximum;
}


// @address 0x475ce0 ?first_person_weapon_build_node_matrices@@YAXJJ@Z
void first_person_weapon_build_node_matrices(long user_index, long slot_index)
{
    fpw_first_person *first_person = fpw_first_person_get(user_index);
    fpw_weapon *weapon = fpw_weapon_get(first_person, slot_index);
    long weapon_index = weapon->object_indices[2];
    if (((weapon->flags >> (2)) & 1) && ((weapon->flags >> (3)) & 1) && weapon_index != -1 && weapon->object_indices[6] != -1)
    {
        long weapon_model_index = weapon->object_indices[4];
        const weapon_datum *weapon_object =
            (const weapon_datum *)object_get_and_verify_type(weapon_index, 4);
        const weapon_definition *weapon_tag = (const weapon_definition *)tag_get('weap', weapon_object->definition_index);
        fpw_manager *animations = &weapon->animations;
        const render_model_definition *hands_model = (const render_model_definition *)tag_get('mode', weapon->object_indices[6]);
        const render_model_definition *weapon_model = NULL;
        c_animation_manager *animation_manager = (c_animation_manager *)animations;
        c_animation_channel channel;
        if (weapon_model_index != -1)
            weapon_model = (const render_model_definition *)tag_get('mode', weapon->object_indices[4]);

        weapon->node_count = first_person_weapons_get_node_count(&weapon->animations);
        real_orientation *orientations = (real_orientation *)first_person_weapons_orientations_get(user_index, slot_index);
        for (long node_index = 0; node_index < weapon->node_count; node_index++)
            orientations[node_index] = *global_identity_orientation;

        if (weapon_model)
        {
            for (short node_index = 0; node_index < weapon_model->nodes.count; node_index++)
            {
                short orientation_index = (short)weapon->weapon_node_remapping[node_index];
                const render_model_node *node = TAG_BLOCK_GET_ELEMENT(&weapon_model->nodes, node_index, render_model_node);
                if (orientation_index != -1)
                {
                    orientations[orientation_index].rotation = node->default_rotation;
                    orientations[orientation_index].translation = node->default_translation;
                    orientations[orientation_index].scale = 1.0f;
                }
            }
        }
        for (short node_index = 0; node_index < hands_model->nodes.count; node_index++)
        {
            short orientation_index = (short)weapon->hands_node_remapping[node_index];
            const render_model_node *node = TAG_BLOCK_GET_ELEMENT(&hands_model->nodes, node_index, render_model_node);
            if (orientation_index != -1)
            {
                orientations[orientation_index].rotation = node->default_rotation;
                orientations[orientation_index].translation = node->default_translation;
                orientations[orientation_index].scale = 1.0f;
            }
        }

        if (first_person_weapons_animations_valid(animations))
        {
            bool interpolate = true;
            animation_manager->get_node_orientations(NULL, 1.0f, NULL, weapon->node_count, orientations, 0, 0);
            if (weapon_model)
                object_apply_function_overlay_node_orientations(weapon_index, weapon_model, animation_manager, NULL, weapon->node_count, orientations);
            const c_model_animation *state_animation = first_person_weapons_get_state_animation(animations);
            if (state_animation && ((state_animation->animation_flags >> (4)) & 1))
                interpolate = false;

            ((c_interpolator_control *)&first_person->elapsed)->set_status(interpolate, interpolate ? 0.4f : 0.2f);
            if (((c_interpolator_control *)&first_person->elapsed)->enabled())
            {
                float ratio = ((c_interpolator_control *)&first_person->elapsed)->get_ratio();
                if (ratio > 0.0001f)
                {
                    if ((&weapon->extra_channels[0])->valid())
                        weapon->extra_channels[0].apply_weighted_node_orientations(NULL, ratio, NULL, weapon->node_count, orientations, NULL, 0);
                    if ((&weapon->extra_channels[1])->valid())
                        weapon->extra_channels[1].apply_weighted_node_orientations(
                            NULL, ratio * (0.5f + weapon_object->jitter), NULL, weapon->node_count, orientations, NULL, 0);

                    bool blend_screen_applied = false;
                    if (weapon->ids[0].valid() && animation_manager->setup_animation_channel_by_index(&channel, weapon->ids[0], 0))
                    {
                        channel.apply_weighted_blend_screen_node_orientations(
                            first_person->turn_yaw, first_person->aim_pitch, ratio, NULL, weapon->node_count, orientations, NULL, 0);
                        blend_screen_applied = true;
                    }
                    if (weapon->ids[1].valid() && animation_manager->setup_animation_channel_by_index(&channel, weapon->ids[1], 0))
                    {
                        if (first_person_weapons_channel_get_animation((const c_animation_channel *)&channel)->get_frame_count() >= 9)
                        {
                            if (first_person->movement_yaw > 0.0f)
                            {
                                channel.set_frame_position(0.0f);
                                channel.apply_weighted_node_orientations(
                                    NULL, first_person->movement_yaw *ratio, NULL, weapon->node_count, orientations, NULL, 0);
                            }
                            else if (first_person->movement_yaw < 0.0f)
                            {
                                channel.set_frame_position(1.0f);
                                channel.apply_weighted_node_orientations(
                                    NULL, -first_person->movement_yaw *ratio, NULL, weapon->node_count, orientations, NULL, 0);
                            }
                            if (first_person->movement_pitch > 0.0f)
                            {
                                channel.set_frame_position(3.0f);
                                channel.apply_weighted_node_orientations(
                                    NULL, first_person->movement_pitch *ratio, NULL, weapon->node_count, orientations, NULL, 0);
                            }
                            else if (first_person->movement_pitch < 0.0f)
                            {
                                channel.set_frame_position(2.0f);
                                channel.apply_weighted_node_orientations(
                                    NULL, -first_person->movement_pitch *ratio, NULL, weapon->node_count, orientations, NULL, 0);
                            }
                            if (!blend_screen_applied)
                            {
                                if (first_person->turn_yaw > 0.0f)
                                {
                                    channel.set_frame_position(4.0f);
                                    channel.apply_weighted_node_orientations(
                                        NULL, first_person->turn_yaw *ratio, NULL, weapon->node_count, orientations, NULL, 0);
                                }
                                else if (first_person->turn_yaw < 0.0f)
                                {
                                    channel.set_frame_position(5.0f);
                                    channel.apply_weighted_node_orientations(
                                        NULL, -first_person->turn_yaw *ratio, NULL, weapon->node_count, orientations, NULL, 0);
                                }
                                if (first_person->turn_pitch > 0.0f)
                                {
                                    channel.set_frame_position(7.0f);
                                    channel.apply_weighted_node_orientations(
                                        NULL, first_person->turn_pitch *ratio, NULL, weapon->node_count, orientations, NULL, 0);
                                }
                                else if (first_person->turn_pitch < 0.0f)
                                {
                                    channel.set_frame_position(6.0f);
                                    channel.apply_weighted_node_orientations(
                                        NULL, -first_person->turn_pitch *ratio, NULL, weapon->node_count, orientations, NULL, 0);
                                }
                            }
                            if (weapon->fire_overlay_weight > 0.0f)
                            {
                                channel.set_frame_position(8.0f);
                                channel.apply_weighted_node_orientations(
                                    NULL, weapon->fire_overlay_weight *ratio, NULL, weapon->node_count, orientations, NULL, 0);
                            }
                        }
                        else
                        {
                            weapon->ids[1].clear();
                        }
                    }
                }
            }

            if (weapon->ids[2].valid() && first_person_weapons_internal_setup_animation_channel_by_index(animation_manager, &channel, weapon->ids[2], 0))
            {
                const c_model_animation *animation = first_person_weapons_channel_get_animation((const c_animation_channel *)&channel);
                if (weapon_tag->reload_interrupt_mode == 2 && (animations->current.state == _string_id_reload_empty ||
                    animations->current.state == _string_id_reload_full))
                {
                    short frame = weapon_object->magazines[0].rounds_loaded;
                    short ticks = weapon_object->magazines[0].ticks_total - weapon_object->magazines[0].ticks;
                    if (ticks >= 44)
                    {
                        const s_weapon_magazine_definition *magazine = TAG_BLOCK_GET_ELEMENT(&weapon_tag->magazines, 0, s_weapon_magazine_definition);
                        float scale = MIN((float)(ticks - 44) / 5.0f, 1.0f);
                        short rounds = (short)first_person_weapons_get_rounds_total(weapon_index, 0);
                        short maximum = MIN(rounds, magazine->rounds_loaded_maximum);
                        frame += (short)((float)(maximum - frame) * scale);
                    }
                    channel.set_frame_position((float)frame);
                    channel.apply_weighted_node_orientations(NULL, 1.0f, NULL, weapon->node_count, orientations, NULL, 0);
                }
                else
                {
                    channel.set_frame_position(
                        (float)first_person_weapons_pin<long>(weapon_object->magazines[0].rounds_loaded, 0, animation->get_frame_count() - 1));
                    channel.apply_weighted_node_orientations(NULL, 1.0f, NULL, weapon->node_count, orientations, NULL, 0);
                }
            }

            if (((first_person->flags >> (1)) & 1) && ((c_interpolator_control *)&animations->elapsed)->enabled())
                first_person_weapons_interpolate_node_orientations(
                    (c_interpolator_control *)&animations->elapsed, (short)weapon->node_count, orientations + 64, orientations);
        }

        first_person->flags |= 2;
        real_matrix4x3 base_matrix;
        first_person_weapons_matrix4x3_translation(&base_matrix, &weapon_tag->first_person_weapon_offset);
        weapon->node_matrix_count = weapon->node_count;
        if (weapon->weapon_root_node_index != -1 && weapon->opposite_hand_node_index != -1)
            first_person_weapons_get_graph(animations->graph)
                ->node_matrices_from_orientations_with_gun_hand_swap(
                    weapon->node_matrices, orientations, &base_matrix, weapon->weapon_root_node_index, weapon->opposite_hand_node_index);
        else
            first_person_weapons_get_graph(animations->graph)->node_matrices_from_orientations(weapon->node_matrices, orientations, &base_matrix);

        if (weapon->hands_node_count != -1)
        {
            weapon->camera_adjustment = weapon->node_matrices[weapon->hands_node_count];
            weapon->camera_adjustment.position.x -= weapon_tag->first_person_weapon_offset.x;
            weapon->camera_adjustment.position.y -= weapon_tag->first_person_weapon_offset.y;
            weapon->camera_adjustment.position.z -= weapon_tag->first_person_weapon_offset.z;
            weapon->flags |= 16;
        }
        else
        {
            weapon->flags &= ~16;
        }

        real_matrix4x3 view_offset_matrix;
        first_person_weapons_compute_view_offset_matrix(user_index, &view_offset_matrix);
        long node_count = first_person_weapons_get_node_count(animations);
        for (long node_index = 0; node_index < node_count; node_index++)
        {
            if (node_index != weapon->hands_node_count)
                matrix4x3_compute_product(&view_offset_matrix, &weapon->node_matrices[node_index], &weapon->node_matrices[node_index]);
        }
        channel.initialize();
    }
}

// ---- 0x476f50 first_person_weapon_attach_weapon_slot ----

static __forceinline void first_person_weapons_set_weapon_visible(long user_index, long slot_index, bool visible)
{
    fpw_weapon *weapon = fpw_weapon_get(fpw_first_person_get(user_index), slot_index);
    if (visible != ((weapon->flags >> (1)) & 1))
    {
        if (visible)
            weapon->flags |= FLAG(1);
        else
            weapon->flags &= ~FLAG(1);
    }
}
static __forceinline void first_person_weapons_set_weapon_attached(long user_index, long slot_index, bool attached)
{
    fpw_weapon *weapon = fpw_weapon_get(fpw_first_person_get(user_index), slot_index);
    if (attached != (((long)weapon->flags >> (0)) & 1))
    {
        if (attached)
            weapon->flags |= FLAG(0);
        else
            weapon->flags &= ~FLAG(0);
        effects_notify_first_person(user_index, weapon->object_indices[2], attached);
    }
}
static __forceinline long first_person_weapons_graph_find_node(const c_model_animation_graph *graph, long name)
{
    for (long node_index = 0; node_index < graph->m_nodes.count; node_index++)
    {
        if (((const animation_graph_node *)graph->m_nodes.address)[node_index].name == name)
            return node_index;
    }
    return NONE;
}
static __forceinline long first_person_weapons_graph_find_node_with_flags(const c_model_animation_graph *graph, unsigned long flags)
{
    for (long node_index = 0; node_index < graph->m_nodes.count; node_index++)
    {
        if ((((const animation_graph_node *)graph->m_nodes.address)[node_index].flags & flags) == flags)
            return node_index;
    }
    return NONE;
}

// @address 0x476f50 ?first_person_weapon_attach_weapon_slot@@YAXJJPBUfirst_person_weapon_attachment@@_N@Z
// match: 0x476f50: pointer __restrict qualifier prevents alias reload
void first_person_weapon_attach_weapon_slot(long user_index, long slot_index, const first_person_weapon_attachment *__restrict attachment, bool force)
{
    fpw_first_person *first_person = fpw_first_person_get(user_index);
    fpw_weapon *weapon = fpw_weapon_get(first_person, slot_index);
    bool was_visible = ((weapon->flags >> (1)) & 1);
    bool attached = true;
    first_person_weapons_set_weapon_visible(user_index, slot_index, false);
    first_person_weapons_set_weapon_attached(user_index, slot_index, false);
    first_person_weapon_attachment previous_attachment = *(const first_person_weapon_attachment *)weapon->object_indices;
    bool unit_changed = weapon->object_indices[0] != attachment->object_indices[0];
    bool models_changed =
        weapon->object_indices[1] != attachment->object_indices[1] ||
        weapon->object_indices[4] != attachment->object_indices[4] ||
        weapon->object_indices[5] != attachment->object_indices[5] ||
        weapon->object_indices[6] != attachment->object_indices[6] ||
        weapon->object_indices[7] != attachment->object_indices[7] ||
        weapon->object_indices[8] != attachment->object_indices[8] ||
        weapon->weapon_node_remap_count != attachment->object_indices[8] ||
        weapon->object_indices[9] != attachment->object_indices[9] ||
        weapon->hands_node_remap_count != attachment->object_indices[9];
    if (force || unit_changed)
    {
        matrix4x3_identity(&weapon->camera_adjustment);
        weapon->flags &= ~FLAG(4);
        attached = attached && attachment->object_indices[0] != NONE;
    }

    bool valid = attached;
    if (!attached || force || models_changed)
    {
        valid = false;
        first_person->flags &= ~FLAG(1);
        weapon->node_matrix_count = 0;
        weapon->flags &= ~FLAG(3);
        weapon->flags &= ~FLAG(2);
        if (attachment->object_indices[0] != NONE && attachment->object_indices[2] != NONE)
        {
            if (attachment->object_indices[5] != NONE)
            {
                if (attachment->object_indices[5] != weapon->object_indices[5])
                    ((c_animation_manager *)&weapon->animations)->reset_graph(attachment->object_indices[5], NONE, false);
                weapon->hands_node_count = first_person_weapons_graph_find_node(first_person_weapons_get_graph(weapon->animations.graph), 207);
                if (attachment->object_indices[6] != NONE)
                {
                    if (((c_animation_manager *)&weapon->animations)
                            ->build_model_mapping_table(attachment->object_indices[6], &weapon->hands_node_remap_count,
                                weapon->hands_node_remapping, NUMBEROF(weapon->hands_node_remapping)))
                    {
                        weapon->flags |= FLAG(3);
                        memcpy(weapon->hands_node_secondary_remapping, weapon->hands_node_remapping, sizeof(weapon->hands_node_remapping));
                        if (slot_index != 0)
                        {
                            const c_model_animation_graph *graph = first_person_weapons_get_graph(weapon->animations.graph);
                            for (long node_index = 0; node_index < weapon->hands_node_remap_count; node_index++)
                            {
                                long graph_node_index = weapon->hands_node_remapping[node_index];
                                if (graph_node_index != NONE && !((((const animation_graph_node *)graph->m_nodes.address)[graph_node_index].flags >> (5)) & 1))
                                    weapon->hands_node_secondary_remapping[node_index] = NONE;
                            }
                        }
                    }
                }
                else
                {
                    weapon->flags |= FLAG(3);
                    memset(weapon->hands_node_remapping, NONE, sizeof(weapon->hands_node_remapping));
                    weapon->hands_node_remap_count = 0;
                }
                if (attachment->object_indices[4] != NONE)
                {
                    if (((c_animation_manager *)&weapon->animations)
                            ->build_model_mapping_table(attachment->object_indices[4], &weapon->weapon_node_remap_count,
                                weapon->weapon_node_remapping, NUMBEROF(weapon->weapon_node_remapping)))
                    {
                        weapon->flags |= FLAG(2);
                        weapon->weapon_root_node_index = NONE;
                        weapon->opposite_hand_node_index = NONE;
                        if (slot_index != 0 && weapon->weapon_node_remap_count > 0)
                        {
                            const c_model_animation_graph *graph = first_person_weapons_get_graph(weapon->animations.graph);
                            long root_node_index = weapon->weapon_node_remapping[0];
                            if (root_node_index != NONE)
                            {
                                short parent_node_index = ((const animation_graph_node *)graph->m_nodes.address)[root_node_index].parent_node_index;
                                if (parent_node_index != NONE)
                                {
                                    if (((((const animation_graph_node *)graph->m_nodes.address)[parent_node_index].flags >> (3)) & 1))
                                        weapon->opposite_hand_node_index = (short)first_person_weapons_graph_find_node_with_flags(graph, FLAG(4));
                                    else
                                        weapon->opposite_hand_node_index = (short)first_person_weapons_graph_find_node_with_flags(graph, FLAG(3));
                                    if (weapon->opposite_hand_node_index != NONE)
                                        weapon->weapon_root_node_index = (short)root_node_index;
                                }
                            }
                        }
                    }
                }
                else
                {
                    weapon->flags |= FLAG(2);
                    memset(weapon->weapon_node_remapping, NONE, sizeof(weapon->weapon_node_remapping));
                    weapon->weapon_node_remap_count = 0;
                    weapon->weapon_root_node_index = NONE;
                    weapon->opposite_hand_node_index = NONE;
                }
                valid = ((weapon->flags >> (2)) & 1) && ((weapon->flags >> (3)) & 1);
            }
        }
        else
        {
            attached = false;
        }
    }

    weapon->fire_overlay_weight = 0.0f;
    weapon->firing_time = 0.0f;
    weapon->idle_timer = 0;
    memcpy(weapon->object_indices, attachment, sizeof(*attachment));
    first_person_weapon_reset_sound(user_index, slot_index);
    first_person_weapon_reset_interpolation(user_index, slot_index);
    ((c_interpolator_control *)&first_person->elapsed)->disable();
    if (attached)
    {
        first_person_weapons_set_weapon_attached(user_index, slot_index, true);
        bool same_weapon = (previous_attachment.object_indices[3] == weapon->object_indices[3] && !models_changed) ||
            previous_attachment.object_indices[2] == weapon->object_indices[2];
        bool update_goal = valid && same_weapon && !unit_changed && !force;
        first_person_weapon_setup_animations(user_index, slot_index, update_goal);
        if (was_visible)
            first_person_weapons_set_weapon_visible(user_index, slot_index, true);
        if (force || models_changed)
            first_person_weapon_build_node_matrices(user_index, slot_index);
    }
    else
    {
        first_person_weapon_reset_channels(user_index, slot_index);
    }
}

static __forceinline long first_person_weapon_render_model_node_count(long render_model_index)
{
    return TAG_GET('mode', render_model_definition, render_model_index)->nodes.count;
}

// @address 0x476988 ?first_person_weapon_attach@@YA_NJJ_N@Z
bool first_person_weapon_attach(long user_index, long unit_index, bool force)
{
    fpw_first_person *first_person = fpw_first_person_get(user_index);
    if (first_person->unit_index != unit_index)
    {
        force = true;
        first_person->unit_index = unit_index;
        first_person->flags &= ~FLAG(0);
    }
    if (force || first_person->unit_index != NONE)
    {
        if (((first_person->flags >> (2)) & 1))
        {
            first_person->flags |= FLAG(3);
        }
        else
        {
            first_person_weapon_attachment attachment;
            memset(&attachment, NONE, sizeof(attachment));
            attachment.object_indices[0] = first_person->unit_index;
            if (attachment.object_indices[0] != NONE)
            {
                const unit_datum *unit = (const unit_datum *)object_get_and_verify_type(attachment.object_indices[0], 8195);
                attachment.object_indices[3] = first_person_weapons_unit_is_dual_wielding(attachment.object_indices[0]) ? _string_id_dual : _string_id_any;
                if (unit->unit.player_index != NONE)
                {
                    const s_game_globals *game_globals = (const s_game_globals *)global_game_globals;
                    attachment.object_indices[1] = game_get_player_character_type(unit->unit.player_index);
                    if (VALID_INDEX(attachment.object_indices[1], game_globals->player_representations.count()))
                    {
                        const s_player_representation *representation = &game_globals->player_representations[attachment.object_indices[1]];
                        if (representation->first_person_hands.index != NONE)
                        {
                            attachment.object_indices[6] = representation->first_person_hands.index;
                            attachment.object_indices[7] = representation->first_person_body.index;
                            attachment.object_indices[9] = first_person_weapon_render_model_node_count(representation->first_person_hands.index);
                        }
                    }
                }
            }
            memcpy(first_person->object_indices, &attachment, sizeof(attachment));
            if (attachment.object_indices[1] != 5)
            {
                for (long slot_index = 0; slot_index < 2; slot_index++)
                {
                    attachment.object_indices[2] = NONE;
                    attachment.object_indices[4] = NONE;
                    attachment.object_indices[5] = NONE;
                    attachment.object_indices[8] = NONE;
                    if (attachment.object_indices[0] != NONE)
                    {
                        const unit_datum *unit = (const unit_datum *)object_get_and_verify_type(attachment.object_indices[0], 8195);
                        long weapon_index = NONE;
                        signed char weapon_slot = unit->unit.current_weapon_indices[slot_index];
                        if (weapon_slot != NONE)
                            weapon_index = unit->unit.weapon_object_indices[weapon_slot];
                        attachment.object_indices[2] = weapon_index;
                        if (attachment.object_indices[2] != NONE)
                        {
                            const weapon_datum *weapon =
                                (const weapon_datum *)object_get_and_verify_type(
                                    attachment.object_indices[2], 4);
                            const weapon_first_person_interface_definition *first_person_interface =
                                weapon_definition_get_first_person_interface_by_character(
                                    TAG_GET('weap', const weapon_definition, weapon->definition_index), attachment.object_indices[1]);
                            if (first_person_interface && first_person_interface->first_person_model.index != NONE)
                            {
                                attachment.object_indices[4] = first_person_interface->first_person_model.index;
                                attachment.object_indices[5] = first_person_interface->first_person_animations.index;
                                attachment.object_indices[8] = first_person_weapon_render_model_node_count(first_person_interface->first_person_model.index);
                            }
                        }
                    }
                    if (((first_person->flags >> (6)) & 1) && ((first_person->flags >> (7)) & 1))
                    {
                        attachment.object_indices[4] = NONE;
                        attachment.object_indices[5] = first_person->unknown_index;
                    }
                    if (force || memcmp(&attachment, fpw_weapon_get(first_person, slot_index)->object_indices, sizeof(attachment)) != 0)
                        first_person_weapon_attach_weapon_slot(user_index, slot_index, &attachment, force);
                }
            }
        }
    }
    return first_person->unit_index != NONE;
}

extern "C" {
}

static __forceinline float first_person_weapons_tick_length()
{
    return game_time_globals->tick_length;
}
static __forceinline unsigned long first_person_weapons_flags_get(const fpw_first_person *first_person)
{
    return first_person->flags;
}
static __forceinline s_player_control_state *first_person_weapons_control_get(long user_index)
{
    return &player_control_globals->states[user_index];
}
static __forceinline const real_euler_angles2d *first_person_weapons_facing_get(long user_index)
{
    s_player_control_state *state = first_person_weapons_control_get(user_index);
    return &state->action.look;
}
static __forceinline float first_person_weapons_signed_angular_difference(float from, float to)
{
    float difference = to - from;
    if (difference >= biped_half_turn_angle())
        difference -= biped_full_turn_angle();
    if (difference <= -biped_half_turn_angle())
        difference += biped_full_turn_angle();
    return difference;
}
static __forceinline bool first_person_weapons_reached_last_frame(const fpw_manager *manager)
{
    bool result = true;
    const c_animation_channel *channel = (const c_animation_channel *)&manager->channels[0];
    if (channel->valid())
        result = (channel->m_state_flags >> 1) & 1;
    return result;
}
static inline void first_person_weapons_set_channel_speed(c_animation_channel *channel, float speed)
{
    const float playback_speed = (float)(double)speed;
    ((c_animation_channel *)channel)->m_playback_speed = MAX(playback_speed, 0.0001f);
}
static __forceinline long first_person_weapons_channel_impulse_name(const c_animation_channel *channel)
{
    return ((const c_animation_channel *)channel)->m_definition_index;
}
static __forceinline bool first_person_weapons_flying_through_air(long unit_index)
{
    const unit_datum *unit = (const unit_datum *)object_get_and_verify_type(unit_index, 8195);
    bool result = false;
    if (unit->object.object_identifier.get_type() == _object_type_biped)
        result = motor_system_in_airborne_state(unit_index);
    return result;
}
static __forceinline void first_person_weapons_update_interpolators(fpw_manager *manager)
{
    c_interpolator_control *control = (c_interpolator_control *)&manager->elapsed;
    control->update();
    if ((bool)(((unsigned short)manager->control_flags >> 1) & 1))
        control->disable();
}
static inline bool first_person_weapons_slider_active(const c_animation_manager *manager)
{
    const fpw_manager *board = (const fpw_manager *)manager;
    const c_mixing_board_definition *definition = NULL;
    if ((unsigned long)board->board <= 2)
        definition = &g_mixing_board_definition_list[board->board];
    t_short_fixed<13> value = manager->get_slider_value_from_chain(5, _mixing_board_parent_slider_regular);
    const unsigned char *slider = (const unsigned char *)manager + definition->component_types[0].offset
        + (c_mixing_board_definition::get_component_size(0) + 37) * 5;
    // match: 0x474ff0: u64/u32 ref pair pins register colors; plain values recolor r25/r26, r20-r23, r19/r28
    const unsigned __int64 &wide_slider_state = (unsigned char)*slider;
    // match: 0x474ff0: u64/u32 ref pair pins register colors; plain values recolor r25/r26, r20-r23, r19/r28
    const unsigned long &slider_state = (unsigned long)wide_slider_state;
    return slider_state != 1 && value > 0;
}
static __forceinline const c_animation_channel *first_person_weapons_primary_channel_get(fpw_first_person *first_person)
{
    fpw_weapon *const &weapon = fpw_weapon_get(first_person, 0);
    BLAM_ASSUME(weapon != NULL);
    return &weapon->animations.channels[0];
}
static __forceinline bool first_person_weapons_unit_slider_active(long unit_index, const long &type_bit)
{
    bool active = true;
    const s_object_header *header = object_header_get(unit_index);
    const unit_datum *unit = (const unit_datum *)header->datum;
    const c_animation_manager *manager = (const c_animation_manager *)((const char *)unit + unit->object.animation.offset);
    const unsigned long &object_type = header->object_type;
    if (((type_bit << object_type) & 1) != 0)
        active = first_person_weapons_slider_active(manager);
    return active;
}
static __forceinline bool first_person_weapons_camera_channel_valid(const c_animation_channel *channel)
{
    // match: 0x474ff0: u64 ref pins the r27/r28 colors in the channel-valid check; a plain value swaps them
    const unsigned __int64 &wide_enabled = (unsigned char)channel->m_enabled;
    const unsigned long enabled = (unsigned long)wide_enabled;
    return enabled != 0 && channel->m_graph_index != NONE && channel->m_animation_id.m_animation_index >= 0;
}

// @rdata 0x8202af7c
static const float first_person_weapons_authored_rate = 30.0f;

// @address 0x474ff0 ?first_person_weapon_update@@YAXJ@Z
void first_person_weapon_update(long user_index)
{
    fpw_first_person *first_person = fpw_first_person_get(user_index);
    const unit_datum *unit = (const unit_datum *)object_get_and_verify_type(first_person->unit_index, 8195);
    bool any_visible = false;
    first_person_weapons_update_camera_estimate(user_index);
    if (!first_person_weapon_update_custom_animation(user_index))
    {
        float movement_acceleration = 72.0f;
        float movement_deceleration = 15.0f;
        float turning_acceleration = 27.0f;
        float turning_deceleration = 6.0f;
        fpw_weapon *primary_weapon = fpw_weapon_get(first_person, 0);
        BLAM_ASSUME(primary_weapon != NULL);
        if (primary_weapon->object_indices[2] != NONE)
        {
            const weapon_datum *weapon =
                (const weapon_datum *)object_get_and_verify_type(
                    primary_weapon->object_indices[2], 4);
            const weapon_definition *definition = TAG_GET('weap', const weapon_definition, weapon->definition_index);
            if (definition->movement_acceleration != 0.0f)
                movement_acceleration = definition->movement_acceleration;
            if (definition->movement_deceleration != 0.0f)
                movement_deceleration = definition->movement_deceleration;
            if (definition->turning_acceleration != 0.0f)
                turning_acceleration = definition->turning_acceleration;
            if (definition->turning_deceleration != 0.0f)
                turning_deceleration = definition->turning_deceleration;
        }
        real_update_smooth_value(&first_person->movement_yaw, &first_person->movement_yaw_velocity, unit->unit.throttle.i,
            first_person_weapons_tick_length(), movement_acceleration, movement_deceleration, -1.0f, 1.0f, false);
        real_update_smooth_value(&first_person->movement_pitch, &first_person->movement_pitch_velocity, unit->unit.throttle.j,
            first_person_weapons_tick_length(), movement_acceleration, movement_deceleration, -1.0f, 1.0f, false);
        float yaw = PIN(first_person->facing_delta.yaw / first_person_weapons_tick_length(), -1.0f, 1.0f);
        float pitch = PIN(-(first_person->facing_delta.pitch / first_person_weapons_tick_length()), -1.0f, 1.0f);
        real_update_smooth_value(&first_person->turn_yaw, &first_person->turn_yaw_velocity, yaw, first_person_weapons_tick_length(),
            turning_acceleration, turning_deceleration, -1.0f, 1.0f, false);
        real_update_smooth_value(&first_person->turn_pitch, &first_person->turn_pitch_velocity, pitch, first_person_weapons_tick_length(),
            turning_acceleration, turning_deceleration, -1.0f, 1.0f, false);
        // match: 0x474ff0: zero-bit rotate preserves 32-bit GPR value
        const long &object_type_bit = _rotl(1, 0);
        const real_euler_angles2d *player_facing = first_person_weapons_facing_get(user_index);
        BLAM_ASSUME(player_facing != NULL);
        if (((first_person->flags >> (0)) & 1))
        {
            first_person->facing_delta.yaw = first_person_weapons_signed_angular_difference(first_person->facing_angles.yaw, player_facing->yaw);
            first_person->facing_delta.pitch = first_person_weapons_signed_angular_difference(first_person->facing_angles.pitch, player_facing->pitch);
        }
        else
        {
            first_person->facing_delta = *global_zero_angles2d;
        }
        first_person->facing_angles = *player_facing;
        first_person->flags |= FLAG(0);
        for (long slot_index = 0; slot_index < 2; slot_index++)
        {
            fpw_weapon *weapon = fpw_weapon_get(first_person, slot_index);
            long weapon_class = weapon->object_indices[3];
            if (weapon->state_timer > 0)
                weapon->state_timer--;
            any_visible |= first_person_weapon_update_visibility(user_index, slot_index);
            if (((weapon->flags >> (0)) & 1))
            {
                c_animation_manager *manager = (c_animation_manager *)&weapon->animations;
                const weapon_datum *weapon_data =
                    (const weapon_datum *)object_get_and_verify_type(
                        weapon->object_indices[2], 4);
                if (manager->valid())
                {
                    real_orientation *orientations = (real_orientation *)g_first_person_weapon_orientation_data + (user_index * 2 + slot_index) * 128;
                    void (*callback)(const s_animation_event_data *, long) = slot_index == 0
                        ? first_person_primary_animation_event_callback
                        : first_person_secondary_animation_event_callback;
                    if (!manager->update_state_animation(callback, user_index, weapon->node_count, orientations + 64, orientations) ||
                        first_person_weapons_reached_last_frame(&weapon->animations) || manager->state_channel_finished())
                    {
                        if (((first_person->flags >> (4)) & 1))
                        {
                            first_person->flags &= ~FLAG(4);
                            first_person->flags |= FLAG(6);
                            first_person_weapon_restore_custom_animation(user_index);
                        }
                        else
                        {
                            if (((first_person->flags >> (5)) & 1))
                                first_person->flags &= ~0x1f0;
                            first_person_weapon_next_state(user_index, slot_index);
                        }
                    }
                }
                float speed = magnitude3d(&unit->unit.throttle);
                bool moving = speed > 0.1f;
                if (first_person_weapons_flying_through_air(first_person->unit_index))
                    moving = false;
                if (((c_animation_channel *)&weapon->extra_channels[0])->valid())
                {
                    if (moving)
                    {
                        float playback_speed = PIN(speed, 0.5f, 1.0f);
                        first_person_weapons_set_channel_speed(&weapon->extra_channels[0], playback_speed);
                    }
                    manager->update_animation_channel(&weapon->extra_channels[0], NULL, 0);
                    bool reset = false;
                    if (!moving)
                    {
                        reset = true;
                    }
                    else
                    {
                        long impulse_name = first_person_weapons_channel_impulse_name(&weapon->extra_channels[0]);
                        if (impulse_name == 199)
                            reset = true;
                    }
                    if (reset)
                    {
                        weapon->extra_channels[0].reset();
                        if (!moving && weapon->animations.current.state == 12)
                            first_person_weapon_start_interpolation(user_index, slot_index, k_standard_interpolation_time);
                    }
                }
                if (moving)
                {
                    if (!((c_animation_channel *)&weapon->extra_channels[0])->valid() && manager->valid())
                    {
                        // match: 0x474ff0: zero-bit rotate preserves 32-bit GPR value
                        const unsigned long &channel_flags = _rotl(31, 0);
                        manager->setup_animation_channel_by_parallel_impulse(&weapon->extra_channels[0], weapon_class,
                            _string_id_current, _string_id_moving, channel_flags);
                        first_person_weapon_start_interpolation(user_index, slot_index, 0.13333334f);
                    }
                }
                if (((c_animation_channel *)&weapon->extra_channels[1])->valid())
                {
                    if (weapon->animations.current.state == _string_id_overcharged)
                    {
                        manager->setup_animation_channel_by_parallel_impulse(&weapon->extra_channels[1], weapon_class,
                            _string_id_current, _string_id_overcharged_jitter, 31);
                        float time = ((c_animation_channel *)&weapon->extra_channels[1])->m_frame_position * (1.0f / 30.0f);
                        time += (weapon_data->jitter + 1.0f) * game_time_globals->tick_length * 2.0f;
                        time = (float)fmod(time, ((c_channel_base *)&weapon->extra_channels[1])->get_authored_duration());
                        weapon->extra_channels[1].set_frame_position(time * first_person_weapons_authored_rate);
                    }
                    else
                        weapon->extra_channels[1].reset();
                }
                float firing_acceleration = 9.0f;
                float firing_deceleration = 6.0f;
                real_update_smooth_value(&weapon->fire_overlay_weight, &weapon->firing_time, 0.0f, game_time_globals->tick_length,
                    firing_acceleration, firing_deceleration, 0.0f, 1.0f, false);
                if (weapon->fire_overlay_weight == 1.0f)
                    weapon->firing_time = 0.0f;
                first_person_weapons_update_interpolators((fpw_manager *)manager);
                if (player_control_get_target_autoaim_level(user_index) == 0.0f && player_control_get_zoom_level(user_index) == NONE &&
                    weapon->fire_overlay_weight == 0.0f && first_person->movement_yaw == 0.0f && first_person->movement_pitch == 0.0f &&
                    first_person->turn_yaw == 0.0f && first_person->turn_pitch == 0.0f)
                {
                    if (weapon->animations.current.state == 12)
                    {
                        const s_player_information_definition *idle = (const s_player_information_definition *)global_game_globals->player_information.address;
                        if (weapon->idle_delay == 0)
                        {
                            float fraction = 0.0f;
                            float minimum = idle->first_person_idle_time.lower;
                            float range = idle->first_person_idle_time.upper - minimum;
                            unsigned long *seed = get_local_random_seed_address();
                            fraction = _real_random(seed);
                            weapon->idle_delay = (short)(long)((minimum + range * fraction) * (float)game_time_globals->ticks_per_second + 0.5f);
                        }
                        weapon->idle_timer++;
                        if (weapon->idle_timer > weapon->idle_delay)
                        {
                            weapon->idle_delay = 0;
                            if (_real_random(get_local_random_seed_address()) >= idle->first_person_idle_skip_fraction)
                                first_person_weapon_set_state(user_index, slot_index, _string_id_posing, true);
                        }
                    }
                    else
                        weapon->idle_timer = 0;
                }
                else
                {
                    weapon->idle_timer = 0;
                    if (weapon->animations.current.state == _string_id_posing)
                        first_person_weapon_set_state(user_index, slot_index, _string_id_idle, true);
                }
                if (((weapon->flags >> (1)) & 1))
                    first_person_weapon_build_node_matrices(user_index, slot_index);
            }
            weapon->sound_timer--;
        }
        if (!any_visible)
            first_person->flags = first_person_weapons_flags_get(first_person) & ~FLAG(0);
        if (((c_interpolator_control *)&first_person->elapsed)->enabled())
            ((c_interpolator_control *)&first_person->elapsed)->update();
        // match: 0x474ff0: zero-bit rotate preserves 32-bit GPR value
        const fpw_weapon *const &camera_weapon = fpw_weapon_get(first_person, _rotl(0, 0));
        BLAM_ASSUME(camera_weapon != NULL);
        if (camera_weapon->get_weapon_class() != _string_id_dual && first_person->unit_index != NONE)
        {
            const c_animation_channel *channel = first_person_weapons_primary_channel_get(first_person);
            bool slider_active = first_person_weapons_unit_slider_active(first_person->unit_index, object_type_bit);
            bool animation_allows = false;
            bool immediate = false;
            // match: 0x474ff0: const reference binding preserves temporary lifetime
            const unsigned long &channel_valid = (unsigned char)first_person_weapons_camera_channel_valid((const c_animation_channel *)channel);
            if (channel_valid)
            {
                const c_model_animation *animation = channel->get_dominant_animation();
                animation_allows = !animation || !((animation->animation_flags >> (3)) & 1);
                if (((c_animation_channel *)channel)->m_definition_index == _string_id_ready &&
                    channel->get_playback_time_remaining() < k_standard_interpolation_time)
                    immediate = true;
                if (((animation->animation_flags >> (7)) & 1))
                {
                    animation_allows = true;
                    immediate = true;
                }
            }
            if ((animation_allows && (unsigned char)slider_active) || immediate)
            {
                c_mixing_board_slider *slider = (c_mixing_board_slider *)first_person->camera_transition;
                slider->move_to_target(_slider_target_position_full, _slider_movement_pattern_full_ease_in, 0.13333334f);
                mixing_board_slider_update(slider);
                return;
            }
            else
            {
                c_mixing_board_slider *slider = (c_mixing_board_slider *)first_person->camera_transition;
                slider->move_to_target(_slider_target_position_one, _slider_movement_pattern_ease_in, 0.06666667f);
                mixing_board_slider_update(slider);
                return;
            }
        }
        else
        {
            fpw_transition *transition = (fpw_transition *)first_person->camera_transition;
            transition_reset(transition);
            transition->flags = 1;
            transition->current = k_mixing_board_slider_targets[1].m_value;
            mixing_board_slider_update((c_mixing_board_slider *)transition);
        }
    }
}

first_person_weapons_allocation_manager g_first_person_weapons_allocator;
first_person_weapon_orientations_allocation_manager g_first_person_weapon_orientations_allocator;

extern "C" {
// @address 0x7c1b88 g_user_interface_allocation
c_user_interface_allocation g_user_interface_allocation;
}


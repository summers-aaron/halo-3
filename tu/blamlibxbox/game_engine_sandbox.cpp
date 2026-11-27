// blamlibxbox/game_engine_sandbox.obj: from the records 0xa15c0 0x1d0ec8 0x1d18f0 0x1d25e0 0x1d41d0 0x26d048 0x26d0d8 0x26d188 0x26d250 0x26d2f8 0x26d358 0x26d3f8 0x26d5e0 0x26d710 0x26d7e8 0x26d880 0x26da98 0x26dba8 0x26dca0 0x26de28 0x26de88 0x26e160 0x26e270 0x26e350 0x26e638 0x26e9e0 0x26ec98 0x26eeb8 0x26f008 0x26f448 0x26f530 0x26f958 0x26f968 0x26fb80 0x26fc58 0x26fc70 0x26fd60 0x26fe18 0x26fe50 0x2700c8 0x270680 0x270f60 0x271120 0x271458 0x2714e8 0x2716b8 0x271890 0x271910 0x271c30 0x271de8 0x271e30 0x271ed8 0x2721b8 0x272230 0x2722b0 0x2723a0 0x272528 0x272608 0x272768 0x2728d0 0x272c50 0x272cc0 0x2731d8 0x273298 0x428ad0 0x4313c8
// @address 0x1142940 g_window_manager
// @flags /TP /O2 /Oc
#include "math/real_math_constants.h"
#include "physics/collision_test_vector.h"
#include "memory/tls_globals.h"
#include "simulation/simulation_queue_event_types.h"
#include "math/matrix_math.h"
#include <new>
#include "memory/data_types.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "math/matrix_math_types.h"
#include "cseries/cseries.h"
#include "cseries/cseries_wrapped.h"
#include "game/player_types.h"
#include "shell/shell.h"
#include "objects/object_iterator.h"
#include "objects/object_types.h"
#include "objects/object_placement.h"
#include "units/unit_types.h"
#include "game/game.h"
#include "game/game_engine.h"
#include "game/players.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "game/game_engine_sandbox.h"
#include "game/game_engine_util.h"
#include "game/game_engine_current_entry.h"
#include "networking/logic/network_life_cycle.h"
#include "objects/damage_owner.h"
#include "physics/havok.h"
#include "simulation/simulation_encoding.h"
#include "simulation/simulation_entity_types.h"
#include "units/bipeds.h"
#include "ai/behavior.h"
#include "game/game_engine_candy_monitor.h"
#include "interface/user_interface_controller.h"
#include "motor/action_system.h"
#include "objects/damage.h"
#include "objects/object_early_movers.h"
#include "physics/breakable_surfaces.h"
#include "physics/character_physics_globals.h"
#include "scenario/scenario_map_variant.h"
#include "scenario/scenario_soft_ceilings.h"
#include "tag_files/tag_types.h"
#include "objects/object_reference_types.h"
#include "interface/c_gui_screen_widget.h"
#include "scenario/scenario_trigger_volumes.h"
#include "game/game_engine_teleporters.h"
#include "game/game_engine_vip.h"
#include "objects/object_datum.h"
#include "game/game_engine_default.h"
#include "interface/c_window_manager.h"
#include "game/game_engine_event_data.h"
#include "cache/cache_files.h"
#include "camera/observer.h"
#include "input/input_abstraction.h"
#include "interface/user_interface_window_manager.h"
#include "physics/havok_component.h"
#include "physics/havok_component_class.h"
#include "game/player_mapping_types.h"
#include "input/input_abstraction_types.h"
#include "game/map_variant.h"
#include "objects/object_definitions.h"
#include "models/model_types.h"
#include "game/game_globals.h"
#include "game/game_engine_settings_definitions.h"
#include "game/multiplayer_runtime_globals.h"
#include "game/game_engine_sandbox_variant.h"
#include "game/game_globals_storage.h"
#include "game/game_engine_interface_state_types.h"
#include "game/game_engine_sandbox_globals.h"
#include "game/game_time_types.h"
#include "physics/physics_constants_tls.h"
#include "math/real_math_globals.h"
#include "cseries/global_real_argb_colors.h"
#include "camera/saved_film_director_storage.h"
#include "physics/havok_group_types.h"
#include <ppcintrinsics.h>
#include "game/game_engine_scoring_globals.h"
#include "game/game_engine_list.h"
#include "game/game_engine_sandbox_engine.h"
#include "game/game_engine_sandbox_dependencies.h"
#include "game/game_engine_kill_handler.h"
#include "objects/multiplayer_object_types.h"
#include "game/game_engine_sandbox_state_types.h"
#include "physics/havok_utilities.h"
#include "game/game_engine_end_condition.h"
#include "cseries/read_write_barrier.h"
#include "game/player_in_game_iterator.h"
#include "game/map_variant_object_properties_types.h"
#include "game/multiplayer_constants.h"
#include "scenario/scenario.h"
#include "game/player_machine.h"
#include "simulation/game_interface/simulation_game_engine_player_types.h"
#include "camera/camera_types.h"
#include "camera/director_notification_types.h"
#include "math/matrix_math_noalias.h"
#include "physics/collision_types.h"
#include "tag_files/tag_index.h"
#include "effects/effect_creation.h"
#include "memory/datum_index.h"
#include "memory/datum_index_inlines.h"
#include "objects/object_header.h"
#include "game/player_access.h"
#include "physics/havok_component_datum_access.h"

struct s_sandbox_player_event {
  union {
    struct {
      long event_type;
      short player_index;
      long variant_object_index;
      real_point3d player_position;
      real_vector3d player_forward;
      real_vector3d player_up;
      real_point3d position;
      real_vector3d forward;
      real_vector3d up;
      float time;
      long edited_object_id;
      unsigned char minimum_count;
      unsigned char maximum_count;
      bool flags;
      unsigned short game_engine_flags;
      unsigned char placement_flags;
      unsigned char team_designator;
      unsigned char spare_clips;
      unsigned char spawn_rate;
      char teleporter_channel;
      char boundary_shape;
      float boundary_width;
      float boundary_box_length;
      float boundary_positive_height;
      float boundary_negative_height;
    };
    unsigned __int64 words[15];
  };
};

union u_sandbox_datum_event_scratch {
  unsigned long datum_storage[sizeof(s_variant_object_datum) / sizeof(unsigned long)];
  s_sandbox_player_event event;
  unsigned __int64 force_size[16];
};


enum e_player_editor_request_type { _player_editor_request_type_0 = 0 };

struct s_variant_multiplayer_object_properties_definition;
class c_teleporter_area;
// @address 0xa15c0 ?game_engine_is_object_being_edited@@YA_NJPAJ@Z
// @address 0x1d0ec8 game_engine_sandbox_compute_budget_total
// @address 0x1d18f0 game_engine_sandbox_update_variant_object_properties
// @address 0x26d188 game_engine_sandbox_is_player_in_edit_mode
// @address 0x26d250 ?get_player_who_is_editing_object@@YAJJ@Z
// @address 0x26d710 game_engine_sandbox_is_object_tracked
// @address 0x26d7e8 game_engine_sandbox_is_active
// @address 0x26de28 game_engine_sandbox_toggle_player_mode
// @address 0x26e160 game_engine_sandbox_is_object_fixed
// @address 0x26f958 ?dispose_from_old_map@c_sandbox_engine@@UBAXXZ
// @address 0x26fc58 ?game_starting@c_sandbox_engine@@UBAXXZ
// @address 0x271890 ?apply_game_engine_traits_for_player@c_sandbox_engine@@UBAXJPAVc_player_traits@@@Z
// @address 0x271e30 ?player_killed_player@c_sandbox_engine@@UBAXJJJ_NV?$c_flags@W4e_game_engine_kill_flags@@J$0BI@@@J@Z
// @address 0x271de8 ?adjust_object_gravity@c_sandbox_engine@@UBAXJPATreal_vector3d@@@Z
// @address 0x2721b8 game_engine_sandbox_get_object_radius
// @address 0x272230 game_engine_sandbox_is_object_soft_ceiling_limited
// @address 0x272528 game_engine_sandbox_track_object
// @address 0x2723a0 ?sandbox_update_object_position@@YAX_NJPBTreal_point3d@@PBTreal_vector3d@@2@Z
// @address 0x271ed8 ?player_hump@c_sandbox_engine@@UBAXJ@Z
// @address 0x26eeb8 ?player_delete_object@c_sandbox_engine@@SAXJJ@Z
// @address 0x26dca0 game_engine_sandbox_set_variant_object_properties
// @address 0x26de88 game_engine_sandbox_spawn_object_for_player
// @address 0x8b0e8 user_interface_controller_get_rumble_enabled
// @address 0x8b370 player_get_user_from_controller
// @address 0x8dbd0 ?get_screen_by_name@c_window_manager@@QAAPAVc_gui_screen_widget@@W4e_window_index@@J@Z
// @address 0x26fc70 ?player_added@c_sandbox_engine@@UBAXJ@Z
// @address 0x26f448 ?handle_deleted_object@c_sandbox_engine@@UBAXJ@Z
// @address 0x1d20b8 ?get_variant_object_datum@c_map_variant@@QBA_NJPAUs_variant_object_datum@@@Z
// @address 0x1d2758 ?set_variant_object_datum@c_map_variant@@QAAXJPBUs_variant_object_datum@@_N@Z
// @address 0xe2b70 ?object_set_velocities@@YAXJPBTreal_vector3d@@0@Z
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0x250040 ?game_engine_get_game_object_definition_index@@YAJJ@Z
// @address 0x807a0 ?game_is_cooperative@@YA_NXZ
// @address 0xa0a10 ?current_game_engine@@YAPBVc_game_engine@@XZ
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0x266a50 ?set@c_player_traits@@QAAXPBV1@_N@Z
// @address 0x3a97c0 ?simulation_action_game_engine_globals_update@@YAXK@Z
// @address 0x208758 ?game_engine_end_round_with_winner@@YAXJ_NW4e_game_engine_end_condition@@@Z
// @address 0x3ac700 ?simulation_request_player_event@@YAXPBUs_player_editor_request_event_data@@@Z
// @address 0x3aee70 ?simulation_encoding_get_world_bounds@@YAXPATreal_rectangle3d@@@Z
// @address 0x9cb80 ?object_get_velocities@@YAXJPATreal_vector3d@@0@Z
// @address 0x9a6a8 ?unit_get_camera_position@@YAXJPATreal_point3d@@@Z


static __forceinline c_game_variant *game_engine_sandbox_variant_get(void)
{
  game_options *options = &game_globals->options;
  return options->game_mode == 2 ? &options->game_variant : 0;
}

static __forceinline short game_engine_sandbox_variant_type(void)
{
  short type = 0;
  c_game_variant *variant = game_engine_sandbox_variant_get();
  if (variant)
    type = (short)variant->game_engine_index;
  return type;
}

bool game_engine_is_object_being_edited(long object_index, long * result_index)
{
  bool result = false;
  if (current_game_engine_entry() && game_globals->options.game_mode == 2 && game_engine_sandbox_variant_type() == 5) {
    long index = get_player_who_is_editing_object(object_index);
    result = index != -1;
    if (result_index)
      *result_index = index;
  }
  return result;
}


static __forceinline unsigned char game_engine_sandbox_placement_is_active(unsigned short flags)
{
  return (flags >> 1) & 1;
}

static __forceinline long game_engine_sandbox_object_root(long object_index, s_object_header *headers)
{
  long root_index = -1;
  while (object_index != -1) {
    root_index = object_index;
    object_index = ((object_datum *)headers[datum_index_to_absolute_index(object_index)].datum)->parent_object_index;
  }
  return root_index;
}

// 0x1d18f0


// 0x26d188
bool game_engine_sandbox_is_player_in_edit_mode(unsigned long player_index)
{
  long absolute_index = datum_index_to_absolute_index(player_index);
  bool result = (((s_game_engine_globals *)game_engine_globals)->sandbox.active_player_flags & (1 << absolute_index)) != 0;
  if (result) {
    long unit_index = ((player_datum *)((s_data_array *)player_data)->data)[absolute_index].unit_index;
    if (unit_index == -1 ||
        ((object_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(unit_index)].datum)->definition_index !=
        game_engine_get_game_object_definition_index(0))
      return false;
  }
  return result;
}

long get_player_who_is_editing_object(long object_index)
{
  long result = -1;
  if (object_index != -1) {
    object_datum *object = object_get(object_index);
    if (object) {
      short owner_index = object->map_variant_index;
      if (owner_index != -1) {
        for (long i = 0; i < 16; i++) {
          if (((s_game_engine_globals *)game_engine_globals)->sandbox.owners[i] == owner_index) {
            result = datum_absolute_index_to_index(player_data, i);
            break;
          }
        }
      }
    }
  }
  return result;
}


// @address 0x26d2f8 ?stop_editing_object@@YAXJ@Z
void stop_editing_object(long player_index)
{
  for (long i = 0; i < 16; i++) {
    if (((s_game_engine_globals *)game_engine_globals)->sandbox.owners[i] == player_index) {
      ((s_game_engine_globals *)game_engine_globals)->sandbox.owners[i] = -1;
      simulation_action_game_engine_globals_update(0x80);
    }
  }
}


// 0x26de28
void game_engine_sandbox_toggle_player_mode(unsigned long player_index)
{
  if (player_index != -1 && player_get(player_index)->unit_index != -1) {
    const c_game_engine *engine = current_game_engine();
    if (engine)
      ((const c_sandbox_engine *)engine)->toggle_player_mode(player_index);
  }
}

// @address 0x8c426c g_sandbox_editor_variant
c_game_engine_sandbox_variant *g_sandbox_editor_variant;


int game_engine_sandbox_update_variant_object_properties(c_map_variant *variant, long object_index)
{
  int result = 0;
  s_object_header *headers = ((s_object_header *)object_header_data->data);
  object_datum *object = (object_datum *)headers[datum_index_to_absolute_index(object_index)].datum;
  long type = object->type;
  bool eligible = type == 6 || type == 10 || type == 1 || type == 2 || type == 3;
  if (eligible) {
    long start_index = variant->object_type_start[type];
    if (start_index != -1) {
      long placement_index = object->scenario_datum_index + start_index;
      s_variant_object_datum *placement = &variant->objects[placement_index];
      if (game_engine_sandbox_placement_is_active(placement->flags)) {
        if (game_engine_sandbox_object_root(object_index, headers) == object_index)
          ::object_set_position_direct(object_index, &placement->position,
              &placement->forward, &placement->up, 0, false);
        ((c_map_variant const *)variant)->apply_multiplayer_properties/*0x1d4480*/(placement_index, object_index);
        result = true;
      }
    }
  }
  return result;
}

// @address 0x744f7c g_sandbox_default_edit_distance
float g_sandbox_default_edit_distance = 5.0f;
static __forceinline short sandbox_current_game_engine_index()
{
  short result = 0;
  if (game_globals->options.game_mode != 1) {
    c_game_variant *variant = game_engine_sandbox_variant_get();
    if (variant)
      result = (short)variant->game_engine_index;
  }
  return result;
}
static __forceinline bool sandbox_game_is_playback()
{
  bool result = false;
  if (game_globals->options.game_simulation == 4)
    result = true;
  return result;
}
static __forceinline unsigned long game_engine_sandbox_multiplayer_is_selected(unsigned long flags)
{
  return _rotl(flags, 21) & 1;
}
static __forceinline s_multiplayer_object_properties *game_engine_sandbox_multiplayer_properties(object_datum *object)
{
  short offset = object->multiplayer_data_offset;
  return offset == -1 ? 0 : (s_multiplayer_object_properties *)((char *)object + offset);
}
static __forceinline bool game_engine_sandbox_object_has_parent(long object_index)
{
  bool result = false;
  if (sandbox_game_is_playback()) {
    long root = game_engine_sandbox_object_root(object_index, ((s_object_header *)object_header_data->data));
    if (root != object_index)
      result = true;
  }
  return result;
}
// @address 0x1d25e0 game_engine_sandbox_update_variant_objects
extern "C" void game_engine_sandbox_update_variant_objects(c_map_variant *variant)
{
  for (long index = 0; index < variant->variant_object_count; ++index) {
    s_variant_object_datum *entry = &variant->objects[index];
    if (index >= variant->scenario_object_count) {
      if (entry->reuse_timeout > 0)
        --entry->reuse_timeout;
      else
        entry->reuse_timeout = 0;
    }
    long object_index = entry->object_index;
    if (object_index != -1 && game_is_predicted()) {
      object_datum *object = (object_datum *)object_try_and_get_and_verify_type(object_index, -1);
      if (object) {
        s_multiplayer_object_properties *properties = game_engine_sandbox_multiplayer_properties(object);
        if (properties && game_engine_sandbox_multiplayer_is_selected((unsigned short)properties->flags.get_unsafe()) &&
            game_engine_sandbox_is_active()) {
          if (!game_engine_sandbox_object_has_parent(object_index))
            snapshot_object_position(false, object_index);
        }
      }
    }
    compute_total_damage(index);
  }
}

// @address 0x26d3f8 ?game_engine_sandbox_start_editing_object@@YAXJJ@Z
static __forceinline long game_engine_sandbox_variant_object_get_object_index(long variant_object_index)
{
    s_variant_object_datum variant_object;
    ((s_game_engine_globals *)game_engine_globals)->map_info.get_variant_object_datum(variant_object_index, &variant_object);
    return variant_object.object_index;
}
void game_engine_sandbox_start_editing_object(long player_index, long variant_object_index)
{
    if (sandbox_current_game_engine_index() == 5 && player_index != -1) {
        long absolute_player_index = player_index & 0xffff;
        player_datum *player = &((player_datum *)((s_data_array *)player_data)->data)[absolute_player_index];
        long object_index = game_engine_sandbox_variant_object_get_object_index(variant_object_index);
        if (object_index != -1) {
            object_datum *object = object_get(object_index);
            if (object->type == 1) {
                long child_unit_index = -1;
                if (unit_get_child_actor(object_index, &child_unit_index, true) != -1)
                    action_submit_event(child_unit_index, 13);
            }
            c_sandbox_engine::player_lock_object_for_edit(player_index, object_index);
            real_point3d aim_position;
            unit_get_aim_position(player->unit_index, &aim_position);
            if (absolute_player_index != -1) {
                real_vector3d offset;
                ::get_offset_to_bounding_sphere_center(object_index, &offset);
                s_game_engine_globals *globals = (s_game_engine_globals *)game_engine_globals;
                real_point3d *edit_position = &globals->sandbox.edit_position[absolute_player_index];
                real_point3d position;
                position.x = edit_position->x + offset.i;
                position.y = edit_position->y + offset.j;
                position.z = edit_position->z + offset.k;
                float dx = position.x - aim_position.x;
                float dy = position.y - aim_position.y;
                float dz = position.z - aim_position.z;
                float *distance = &globals->sandbox.edit_times[absolute_player_index];
                *distance = __fsqrts(dx * dx + dy * dy + dz * dz);
                globals->sandbox.edit_times[absolute_player_index] = PIN(globals->sandbox.edit_times[absolute_player_index], 2.0f, 10.0f);
            }
            game_engine_sandbox_set_object_connected(object_index, 1);
        }
    }
}
// @address 0x26d5e0 ?game_engine_sandbox_stop_editing_object@@YAXJM_N@Z
void game_engine_sandbox_stop_editing_object(long variant_object_index, float time, bool force_attached)
{
  if (sandbox_current_game_engine_index() == 5) {
    s_variant_object_datum variant_object;
    ((s_game_engine_globals *)game_engine_globals)->map_info.get_variant_object_datum(variant_object_index, &variant_object);
    if (variant_object.object_index != -1) {
      object_datum *object = object_get(variant_object.object_index);
      if ((unsigned short)object->map_variant_index != 65535) {
        long player_index = get_player_who_is_editing_object(variant_object.object_index);
        if (player_index != -1) {
          c_sandbox_engine::player_unlock_object(player_index, time, force_attached);
          if (!sandbox_game_is_playback()) {
            stop_editing_object(object->map_variant_index);
            ((s_game_engine_globals *)game_engine_globals)->sandbox.edit_times[datum_index_to_absolute_index(player_index)] = g_sandbox_default_edit_distance;
          }
        }
      }
    }
  }
}

// 0x26d710
unsigned char game_engine_sandbox_is_object_tracked(long object_index)
{
  bool result = false;
  if (current_game_engine_entry() && game_globals->options.game_mode == 2 && game_engine_sandbox_variant_type() == 5) {
    s_game_engine_globals *globals = (s_game_engine_globals *)game_engine_globals;
    for (long i = 0; i < globals->sandbox.object_count; i++) {
      if (globals->sandbox.object_indices[i] == object_index) {
        result = true;
        break;
      }
    }
  }
  return result;
}

// 0x26d7e8
extern "C" bool game_engine_sandbox_is_active(void)
{
  return current_game_engine_entry() && game_globals->options.game_mode == 2 && game_engine_sandbox_variant_type() == 5;
}


static __forceinline bool game_engine_sandbox_havok_rigid_body_is_movable(const c_havok_component *component, long index)
{
  unsigned char motion_type = component->m_rigid_bodies[index].m_rigid_body->m_motion.m_type;
  return !(motion_type == 7 || motion_type == 6);
}

static __forceinline bool game_engine_sandbox_havok_component_is_flag7(const c_havok_component *component)
{
  return (unsigned char)((component->m_flags >> 7) & 1);
}

// 0x26e160
bool game_engine_sandbox_is_object_fixed(long object_index)
{
  object_datum *object = object_get(object_index);
  bool result = false;
  if (((1 << object->type) & 0x1cdf) && (unsigned char)((object->physics_flags >> 7) & 1) && object->havok_component_index != -1) {
    c_havok_component *component = havok_component_get(object->havok_component_index);
    bool all_fixed = true;
    for (long index = 0; index < component->m_rigid_bodies.getSize(); index++)
      all_fixed = all_fixed && game_engine_sandbox_havok_rigid_body_is_movable(component, index) &&
          !game_engine_sandbox_havok_component_is_flag7(component);
    if (!all_fixed)
      result = true;
  }
  else {
    result = true;
  }
  return result;
}

void c_sandbox_engine::dispose_from_old_map(void) const
{
  g_sandbox_editor_variant = 0;
}


void c_sandbox_engine::game_starting(void) const
{
  simulation_encoding_get_world_bounds(&game_engine_globals->map_info.m_bounds);
}

void c_sandbox_engine::player_added(long player_index) const
{
  if (game_is_cooperative())
    return;
  player_hump(-1);
  bool active = false;
  if (game_globals->options.game_simulation == 4)
    active = true;
  if (active != 0)
    return;
  c_map_variant *players = &((s_game_engine_globals *)game_engine_globals)->map_info;
  for (int i = 0; i < players->variant_object_count; i++) {
    unsigned short flags = players->objects[i].flags.m_storage;
    unsigned char a = (flags >> 6) & 1;
    unsigned char b = (flags >> 1) & 1;
    if (a != 0 && b != 0)
      simulation_action_sandbox_group_update(i, 1, 0);
    simulation_action_sandbox_group_update(i, 8, 0);
  }
}

void c_sandbox_engine::apply_game_engine_traits_for_player(long player_index, c_player_traits *traits) const
{
  if (game_engine_sandbox_variant_get() && game_engine_sandbox_is_player_in_edit_mode(player_index))
    traits->set(&g_sandbox_editor_variant->m_player_traits, false);
}


// 0x271de8
void c_sandbox_engine::adjust_object_gravity(long object_index, real_vector3d *gravity) const
{
  if (::game_engine_is_object_being_edited(object_index, (long *)(0))) {
    gravity->i = 0.0f;
    gravity->j = 0.0f;
    gravity->k = 0.0f;
  }
}


void c_sandbox_engine::handle_deleted_object(long object_index) const
{
  long player_index = get_player_who_is_editing_object(object_index);
  if (player_index != -1 && sandbox_game_is_playback())
    ((s_game_engine_globals *)game_engine_globals)->sandbox.owners[datum_index_to_absolute_index(player_index)] = -1;
  for (short i = 0; i < 16; i++) {
    if (((s_game_engine_globals *)game_engine_globals)->sandbox.handle_slots[i] == object_index)
      ((s_game_engine_globals *)game_engine_globals)->sandbox.handle_slots[i] = -1;
  }
  if (!sandbox_game_is_playback())
    game_engine_sandbox_dispose_tracked_object(object_index, -1, 1);
}

typedef char sandbox_event_size[(sizeof(s_sandbox_player_event) == 0x78) ? 1 : -1];

// Delete the edited object, or request deletion when predicted.
void c_sandbox_engine::player_delete_object(long player_index, long object_index)
{
  if (object_index == -1)
    return;
  bool predicted = false;
  if (game_globals->options.game_simulation == 4)
    predicted = true;
  if (predicted == 0) {
    stop_editing_object(object_index);
    s_variant_object_datum datum;
    s_game_engine_globals *edit_globals = (s_game_engine_globals *)game_engine_globals;
    c_map_variant *map_variant = &edit_globals->map_info;
    map_variant->get_variant_object_datum(object_index, &datum);
    if ((datum.flags & 0x3ff) == 0)
      return;
    if ((((unsigned char)(((unsigned short)datum.flags >> 5) & 0x7ff)) & 1) != 0)
      return;
    long edited_object_index = edit_globals->map_info.objects[object_index].object_index;
    if (edited_object_index != -1) {
      map_variant->remove_object(object_index, _object_removal_origin_2);
      object_delete(edited_object_index);
    }
    simulation_action_sandbox_group_update(object_index, 8, 0);
    return;
  }
  s_sandbox_player_event event;
  float zero = k_real_zero_storage;
  event.game_engine_flags = 0;
  event.boundary_shape = 0;
  event.spawn_rate = 0;
  event.spare_clips = 0;
  event.teleporter_channel = 0;
  event.boundary_negative_height = zero;
  event.boundary_positive_height = zero;
  event.boundary_box_length = zero;
  event.boundary_width = zero;
  event.placement_flags = 12;
  event.team_designator = 9;
  for (int i = 15; i != 0; i--)
    event.words[15 - i] = 0;
  event.player_index = player_index;
  event.variant_object_index = object_index;
  event.event_type = 3;
  simulation_request_player_event((const s_player_editor_request_event_data *)&event);
}

// Transition the sandbox screens for the selected player.
static __forceinline void game_engine_sandbox_update_player_screens(long player_index)
{
  c_gui_screen_widget *screen = g_window_manager.get_screen_by_name((e_window_index)player_index, 0x10533);
  if (screen)
    screen->transition_out_with_transition_type(0, _screen_transition_normal);
  screen = g_window_manager.get_screen_by_name((e_window_index)player_index, 0x10534);
  if (screen)
    screen->transition_out_with_transition_type(0, _screen_transition_normal);
  screen = g_window_manager.get_screen_by_name((e_window_index)player_index, 0x10535);
  if (screen)
    screen->transition_out_with_transition_type(0, _screen_transition_normal);
  screen = g_window_manager.get_screen_by_name((e_window_index)player_index, 0x10536);
  if (screen)
    screen->transition_out_with_transition_type(0, _screen_transition_normal);
  screen = g_window_manager.get_screen_by_name((e_window_index)player_index, 0x10531);
  if (screen)
    screen->transition_out_with_transition_type(0, _screen_transition_normal);
  screen = g_window_manager.get_screen_by_name((e_window_index)player_index, 0x10532);
  if (screen)
    screen->transition_out_with_transition_type(0, _screen_transition_normal);
}

void c_sandbox_engine::player_hump(long user_index) const
{
  if (user_index != -1) {
    long controller_index = user_interface_controller_get_rumble_enabled(user_index);
    long player_index = player_get_user_from_controller(controller_index);
    if (player_index == 4)
      return;
    game_engine_sandbox_update_player_screens(player_index);
  } else {
    for (long player = 0; player < 4; player++)
      game_engine_sandbox_update_player_screens(player);
  }
}

// 0x272230

static __forceinline s_multiplayer_object_properties *multiplayer_block_from_object(char *object)
{
  short offset = ((object_datum *)object)->multiplayer_data_offset;
  return offset != -1 ? (s_multiplayer_object_properties *)(object + offset) : 0;
}

bool game_engine_sandbox_is_object_soft_ceiling_limited(long object_index)
{
  char *object = (char *)object_try_and_get_and_verify_type(object_index, 0xffffffff);
  s_multiplayer_object_properties *block = object ? multiplayer_block_from_object(object) : 0;
  bool result = false;
  if (block) {
    char type = block->object_type;
    if (type == 9 || type == 10 || type == 11 || type == 12)
      result = true;
  }
  return result;
}



// @address 0x26d048 ?get_offset_to_bounding_sphere_center@@YAXJPATreal_vector3d@@@Z
void get_offset_to_bounding_sphere_center(long object_index, union real_vector3d * offset)
{
    real_point3d center;
    float radius;
    real_point3d origin;

    object_get_bounding_sphere(object_index, &center, &radius);
    object_get_origin(object_index, &origin);
    vector_from_points3d(&origin, &center, offset);
}

// tag instance table: global_tag_instances, 8 bytes per entry, the data pointer at +4
extern "C" {

}

// 0x26d358
// @address 0x26d358 game_engine_sandbox_set_object_connected
void game_engine_sandbox_set_object_connected(unsigned long object_index, long is_connected)
{
  object_datum *object = object_get(object_index);
  unsigned long index = ((const s_object_definition *)global_tag_instances[(unsigned short)object->definition_index].definition)->model.index;
  if (index != 0xffffffff &&
      ((const s_model_definition *)global_tag_instances[datum_index_to_absolute_index(index)].definition)->physics_model_index != -1 &&
#if !defined(_XBOX) || defined(__clang__)
      (object_set_movable(object_index, static_cast<unsigned char>(is_connected)), object->havok_component_index == -1))
#else
      // Xbox enters this call with object_index and is_connected still in r3 and r4.
      (object_set_movable(), object->havok_component_index == -1))
#endif
  {
    ::havok_component_aquire(object_index, 1, 1);
    object_set_movable(object_index, is_connected);
  }
}

// 0x272c50

// @address 0x272c50 ?snapshot_object_position@@YAX_NJ@Z
void snapshot_object_position(bool update, long object_index)
{
  if (object_index != -1) {
    real_vector3d up;
    real_vector3d forward;
    real_point3d origin;
    ::object_get_orientation(object_index, &forward, &up);
    object_get_origin(object_index, &origin);
    ::sandbox_update_object_position(
        update, object_index, &origin, &forward, &up);
  }
}
static __forceinline real_vector3d *game_engine_sandbox_monitor_cross_product(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
  float k = a->i * b->j - a->j * b->i;
  float j = a->k * b->i - a->i * b->k;
  result->i = a->j * b->k - a->k * b->j;
  result->j = j;
  result->k = k;
  return result;
}
// @address 0x272cc0 game_engine_sandbox_spawn_monitor_for_player
extern "C" bool game_engine_sandbox_spawn_monitor_for_player(long player_index)
{
  long unit_index = player_get(player_index)->unit_index;
  bool result = false;
  if (unit_index != -1) {
    unit_datum *unit = (unit_datum *)object_get(unit_index);
    float saved_shield_vitality = unit->object.shield_vitality;
    float saved_body_vitality = unit->object.body_vitality;
    real_point3d camera;
    real_point3d offset;
    real_point3d new_camera;
    object_placement_data placement;
    object_placement_data_new(&placement, game_engine_get_game_object_definition_index(0), -1, 0);
    unit_get_camera_position(unit_index, &camera);
    unit_datum *camera_unit = (unit_datum *)object_get(unit_index);
    float zero = k_real_zero_storage;
    placement.forward = camera_unit->unit.aiming_vector;
    placement.up.i = zero;
    placement.up.k = placement.forward.i * zero - placement.forward.j * zero;
    placement.up.i = placement.forward.j - placement.forward.k * zero;
    placement.up.j = placement.forward.k * zero - placement.forward.i;
    normalize3d(&placement.up);
    game_engine_sandbox_monitor_cross_product(&placement.forward, &placement.up, &placement.up);
    normalize3d(&placement.up);
    object_get_origin(unit_index, &placement.position);
    long new_object = object_new(&placement);
    if (new_object != -1) {
      action_submit_event(unit_index, 20);
      if (unit->object.parent_object_index != -1)
        object_detach(unit_index);
      unit->unit.map_editor_flags.m_storage |= 1;
      simulation_action_object_update(unit_index, 0x40000000);
      object_deactivate(unit_index, false);
      object_disconnect_from_physics(unit_index);
      object_set_hidden(unit_index, true);
      player_set_unit_index(player_index, new_object);
      simulation_action_object_create(new_object);
      unit_get_camera_position(new_object, &new_camera);
      real_vector3d camera_offset;
      vector_from_points3d(&new_camera, &placement.position, &camera_offset);
      point_from_line3d(&camera, &camera_offset, 1.0f, &offset);
      object_set_position_direct(new_object, &offset, &placement.forward, &placement.up, 0, 0);
      simulation_action_object_update(new_object, 2);
      if (!sandbox_game_is_playback()) {
        unit_datum *new_unit = (unit_datum *)object_get(new_object);
        new_unit->object.shield_vitality = saved_shield_vitality;
        new_unit->object.body_vitality = saved_body_vitality;
        simulation_action_object_update(unit_index, 192);
      }
      result = true;
    }
  }
  return result;
}
// 0x1d0ec8: the quota price cap, unless the game mode is 4


static __forceinline unsigned char game_engine_sandbox_quota_maximum(unsigned char a, unsigned char b)
{
  return a > b ? a : b;
}
extern "C" {
}

void game_engine_sandbox_compute_budget_total(c_map_variant *state)
{
  unsigned int tr11 = 0;
  if (game_globals->options.game_simulation == 4)
    tr11 = 1;
  if ((tr11 & 255) == 0) {
    float threshold = k_real_zero_storage;
    float total = threshold;
    if (state->quota_count > 0) {
      for (int index = 0; index < state->quota_count; index++) {
        s_variant_quota *quota = &state->quotas[index];
        float price = quota->price;
        if (price > threshold) {
          tr11 = game_engine_sandbox_quota_maximum(quota->maximum_count, quota->placed_on_map);
          total = (float)(double)(tr11 & 255) * price + total;
        }
      }
    }
    if (total > state->budget_limit)
      total = state->budget_limit;
    state->budget_spent = total;
  }
}

// 0x271458: clear the state block, call its initialize slot, then set the sentinel fields

typedef char sandbox_update_size[(sizeof(game_engine_sandbox_update_data) == 0x88) ? 1 : -1];
typedef char sandbox_update_owners_offset[(offsetof(game_engine_sandbox_update_data, owners) == 0x38) ? 1 : -1];
// @address 0x271458 ?build_simulation_baseline@c_sandbox_engine@@UBAXJPAX@Z
void c_sandbox_engine::build_simulation_baseline(long unused, void *state) const
{
  memset(state, 0, sizeof(game_engine_sandbox_update_data));
  build_global_baseline((s_game_engine_state_data *)state);
  ((game_engine_sandbox_update_data *)state)->active_player_flags = 0;
  for (int i = 0; i < 16; i++)
    ((game_engine_sandbox_update_data *)state)->owners[i] = -1;
  ((game_engine_sandbox_update_data *)state)->edited_object_index = -1;
  ((game_engine_sandbox_update_data *)state)->round_time = 0;
  ((game_engine_sandbox_update_data *)state)->game_engine_subtype = 10;
}

// 0x2721b8: the larger of the object's own radius and its extra's radius, floored at the constant

float game_engine_sandbox_get_object_radius(unsigned long object_index)
{
  s_object_definition *object = (s_object_definition *)global_tag_instances[datum_index_to_absolute_index(object_index)].definition;
  float result = object->bounding_radius;
  unsigned long index = object->model.index;
  if (index != 0xffffffff) {
    s_model_definition *other = (s_model_definition *)global_tag_instances[datum_index_to_absolute_index(index)].definition;
    if (other->object_data.count() > 0) {
      s_model_object_data *extra = (s_model_object_data *)other->object_data.address;
      if (extra != 0) {
        float limit = extra->radius;
        if (result <= limit)
          result = limit;
      }
    }
  }
  if (result >= 0.0001f)
    return result;
  return 1.0f;
}


// @address 0x2722b0 game_engine_sandbox_find_placement_point
extern "C" bool game_engine_sandbox_find_placement_point(long definition_index, real_point3d *point, long ignore_object_index)
{
  float radius = game_engine_sandbox_get_object_radius(definition_index);
  real_point3d fixed_point;
  bool result = havok_try_and_get_sphere_fixed_position(radius, radius, _havok_group_environment, 4, ignore_object_index, point, 0, -1, &fixed_point, false);
  if (result)
    *point = fixed_point;
  long quota_index = map_variant_find_quota_index(&((s_game_engine_globals *)game_engine_globals)->map_info, definition_index);
  if (quota_index != -1) {
    s_variant_quota *quota = &((s_game_engine_globals *)game_engine_globals)->map_info.quotas[quota_index];
    if (quota && quota->placed_on_map >= quota->maximum_allowed)
      result = false;
  } else {
    result = false;
  }
  return result;
}

// @address 0x428ad0 ?get_type@c_sandbox_engine@@UBAJXZ
long c_sandbox_engine::get_type(void) const
{
  return _game_engine_type_sandbox;
}

// @address 0x5bec98 ?player_activated@c_sandbox_engine@@UBAXJ@Z
void c_sandbox_engine::player_activated(long player_index) const
{
}

// @address 0x2ea090 ?score_header_string@c_sandbox_engine@@UBAXPAV?$c_static_wchar_string@$0BAA@@@@Z
void c_sandbox_engine::score_header_string(c_static_wchar_string<256> *string) const
{
  game_engine_get_universal_string(0x115, string);
}

// @address 0x4313c8 ?get_simulation_entity_type@c_sandbox_engine@@UBA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_sandbox_engine::get_simulation_entity_type(void) const
{
  return _simulation_entity_type_sandbox_engine_globals;
}

// @address 0x40728 ??_7c_sandbox_engine@@6B@
// @address 0x783378 ?internal_sandbox_engine@@3Vc_sandbox_engine@@A
c_sandbox_engine internal_sandbox_engine;

// Update the object's variant transform and gravity velocity, then notify the simulation.


void sandbox_update_object_position(
    bool update, long object_index, union real_point3d const *origin, union real_vector3d const *forward, union real_vector3d const *up)
{
  object_datum *object = object_get(object_index);
  s_variant_object_datum variant_object;
  long variant_index = object->map_variant_index;
  c_map_variant *map_variant = &((s_game_engine_globals *)game_engine_globals)->map_info;
  map_variant->get_variant_object_datum(variant_index, &variant_object);
  if (origin)
    variant_object.position = *origin;
  if (forward)
    variant_object.forward = *forward;
  if (up)
    variant_object.up = *up;
  map_variant->set_variant_object_datum(variant_index, &variant_object, true);
  if (update) {
    real_vector3d velocity;
    velocity.i = 0.0f;
    velocity.j = 0.0f;
    velocity.k = game_time_globals->tick_length * g_physics_constants->gravity;
    object_set_velocities(object_index, &velocity, &k_real_math_zero_vector3d_storage);
  }
  ::object_set_position_direct(object_index, origin, forward,
      up, 0, true);
  bool simulation_mode_4 = false;
  if (game_globals->options.game_simulation == 4)
    simulation_mode_4 = true;
  if (!simulation_mode_4)
    ::simulation_action_object_update(object_index, 2);
}

// 0x272528


void game_engine_sandbox_track_object(int p0)
{
    if ((unsigned char)game_engine_sandbox_is_object_tracked(p0) == 0) {
        s_game_engine_globals *globals = (s_game_engine_globals *)game_engine_globals;
        // match: 0x272528: the count address is materialized before loading the count.
        long *object_count = &globals->sandbox.object_count;
        long count = *object_count;
        if (count < 64) {
            int index = globals->sandbox.object_count++;
            globals->sandbox.object_indices[index] = p0;
            set_real_vector3d(&globals->sandbox.positions[index], 65536.0f, 65536.0f, 65536.0f);
            object_datum *object = (object_datum *)object_try_and_get_and_verify_type(p0, -1);
            if (object) {
                s_multiplayer_object_properties *properties = game_engine_sandbox_multiplayer_properties(object);
                if (properties) {
                    properties->flags.set_unsafe(properties->flags.get_unsafe() | 2048);
                    ::simulation_action_object_update(p0, 4096);
                }
            }
        }
    }
    return;
}

// @address 0x273298 game_engine_sandbox_adjust_quota_counts
static __forceinline bool game_engine_sandbox_game_simulation_is_4(const game_globals_storage *game_globals)
{
  bool result = false;
  if (game_globals->options.game_simulation == 4)
    result = true;
  return result;
}
static __forceinline void game_engine_sandbox_quota_clamp(s_variant_quota *quota)
{
  unsigned char b = quota->placed_on_map;
  unsigned char a = quota->maximum_count;
  quota->maximum_count = MIN(MAX(a, b), quota->maximum_allowed);
  quota->minimum_count = MIN(MAX(quota->minimum_count, 0), b);
}
void game_engine_sandbox_adjust_quota_counts(int quota_index, int delta)
{
  if (game_engine_sandbox_is_active()) {
    s_game_engine_globals *globals = (s_game_engine_globals *)game_engine_globals;
    game_globals_storage *game = game_globals;
    s_variant_quota *quota = &globals->map_info.quotas[quota_index];
    unsigned char placed_after = quota->placed_on_map + delta;
    quota->maximum_count += delta;
    quota->placed_on_map = placed_after;
    if (!game_engine_sandbox_game_simulation_is_4(game) && quota->placed_on_map == 0)
      quota->maximum_count = 0;
    game_engine_sandbox_quota_clamp(&globals->map_info.quotas[quota_index]);
    if (!game_engine_sandbox_game_simulation_is_4(game)) {
      game_engine_sandbox_compute_budget_total(&globals->map_info);
      simulation_action_game_engine_globals_update(0x100);
      simulation_action_game_engine_globals_update(0x200);
    }
  }
}


// @address 0x26dba8 game_engine_sandbox_end_round_with_value
extern "C" void game_engine_sandbox_end_round_with_value(int player_index, int value)
{
  if (!game_engine_sandbox_game_simulation_is_4(game_globals)) {
    ((s_game_engine_globals *)game_engine_globals)->map_info.game_engine_subtype = value;
    simulation_action_game_engine_globals_update(0x400);
    game_engine_end_round_with_winner(-1, false, game_engine_sandbox_end_condition_22);
    return;
  }
  if (player_index != -1) {
    s_sandbox_player_event event;
    event.placement_flags = 12;
    event.game_engine_flags = 0;
    event.boundary_shape = 0;
    event.spawn_rate = 0;
    event.spare_clips = 0;
    event.teleporter_channel = 0;
    event.boundary_negative_height = 0.0f;
    event.boundary_positive_height = 0.0f;
    event.boundary_box_length = 0.0f;
    event.boundary_width = 0.0f;
    event.team_designator = 9;
    for (int i = 15; i != 0; i--)
      event.words[15 - i] = 0;
    event.player_index = (short)player_index;
    event.edited_object_id = value;
    event.event_type = 4;
    simulation_request_player_event((const s_player_editor_request_event_data *)&event);
  }
}


// @address 0x26d880 ?game_engine_sandbox_update_budget_quotas@@YAXJJJJ@Z
void game_engine_sandbox_update_budget_quotas(long player_index, long definition_index, long maximum_count, long minimum_count)
{
  s_game_engine_globals *globals = (s_game_engine_globals *)game_engine_globals;
  c_map_variant *map_variant = &globals->map_info;
  long quota_index = map_variant_find_quota_index(map_variant, definition_index);
  s_variant_quota *quota = &globals->map_info.quotas[quota_index];
  if (quota->minimum_count == minimum_count && quota->maximum_count == maximum_count)
    return;
  if (!game_engine_sandbox_game_simulation_is_4(game_globals)) {
    unsigned char current_maximum = quota->maximum_count;
    if (maximum_count > current_maximum) {
      if ((float)(maximum_count - current_maximum) * quota->price + map_variant->budget_spent >= map_variant->budget_limit)
        maximum_count = current_maximum;
    }
    quota->maximum_count = PIN(maximum_count, quota->placed_on_map, quota->maximum_allowed);
    quota->minimum_count = PIN(minimum_count, 0, quota->placed_on_map);
    long variant_index = -1;
    for (long i = 0; i < map_variant->variant_object_count; i++) {
      if (map_variant->objects[i].quota_index == quota_index)
        variant_index = i;
    }
    if (variant_index != -1)
      simulation_action_sandbox_group_update(variant_index, 8, 0);
    game_engine_sandbox_compute_budget_total(&((s_game_engine_globals *)game_engine_globals)->map_info);
    simulation_action_game_engine_globals_update(0x100);
  } else {
    s_sandbox_player_event event;
    event.game_engine_flags = 0;
    event.boundary_shape = 0;
    event.spawn_rate = 0;
    event.spare_clips = 0;
    event.teleporter_channel = 0;
    event.boundary_negative_height = 0.0f;
    event.boundary_positive_height = 0.0f;
    event.boundary_box_length = 0.0f;
    event.boundary_width = 0.0f;
    event.placement_flags = 12;
    event.team_designator = 9;
    for (int i = 15; i != 0; i--)
      event.words[15 - i] = 0;
    event.player_index = (short)player_index;
    event.variant_object_index = definition_index;
    event.maximum_count = (unsigned char)maximum_count;
    event.minimum_count = (unsigned char)minimum_count;
    event.event_type = 7;
    simulation_request_player_event((const s_player_editor_request_event_data *)&event);
  }
}


// @address 0x271e30 ?player_killed_player@c_sandbox_engine@@UBAXJJJ_NV?$c_flags@W4e_game_engine_kill_flags@@J$0BI@@@J@Z
void c_sandbox_engine::player_killed_player(long killer, long dead_unit, long victim, bool betrayal,
    c_flags<e_game_engine_kill_flags, long, 24> flags, long streak) const
{
  bool predicted;
  int killer_value;
  int victim_value;
  game_engine_handle_player_killed((void *)this, killer, dead_unit, victim, betrayal, flags, streak);
  predicted = false;
  if (game_globals->options.game_simulation == 4)
    predicted = true;
  if (predicted != 0)
    return;
  if (killer == -1)
    return;
  if (killer == victim)
    return;
  killer_value = get_player_team_designator(killer);
  victim_value = get_player_team_designator(victim);
  if (killer_value == victim_value)
    return;
  game_engine_adjust_player_score(killer, 1);
}

struct s_variant_multiplayer_object_properties_definition;

// 0x26dca0: authoritative property sync, or a placement request event when predicted
extern "C" void game_engine_sandbox_set_variant_object_properties(short player_index, long variant_index, const int *properties)
{
  bool predicted = false;
  if (game_globals->options.game_simulation == 4)
    predicted = true;
  u_sandbox_datum_event_scratch local;
  if (predicted == 0) {
    s_variant_object_datum &datum = *new (local.datum_storage) s_variant_object_datum;
    // match: 0x26dca0: reuse the stack address for the datum and properties instead of saving the constructor result and field pointer.
    __assume(&datum == static_cast<void *>(local.datum_storage));
    c_map_variant *map_variant = &((s_game_engine_globals *)game_engine_globals)->map_info;
    map_variant->get_variant_object_datum(variant_index, &datum);
    memcpy(&datum.properties, properties, sizeof(datum.properties));
    map_variant->set_variant_object_datum(variant_index, &datum, true);
    map_variant = &((s_game_engine_globals *)game_engine_globals)->map_info;
    new (local.datum_storage) s_variant_object_datum;
    map_variant->get_variant_object_datum(variant_index, &datum);
    long object_index = datum.object_index;
    if (object_index != -1)
      map_variant->apply_multiplayer_properties(&datum.properties, object_index);
    simulation_action_sandbox_group_update(variant_index, 4, 0);
    return;
  }
  local.event.game_engine_flags = 0;
  local.event.boundary_shape = 0;
  local.event.spawn_rate = 0;
  local.event.spare_clips = 0;
  local.event.teleporter_channel = 0;
  local.event.boundary_negative_height = 0.0f;
  local.event.boundary_positive_height = 0.0f;
  local.event.boundary_box_length = 0.0f;
  local.event.boundary_width = 0.0f;
  local.event.placement_flags = 12;
  local.event.team_designator = 9;
  _ReadWriteBarrier();
  for (int i = 15; i != 0; i--)
    local.event.words[15 - i] = 0;
  local.event.player_index = player_index;
  local.event.variant_object_index = variant_index;
  local.event.event_type = 6;
  int *edst = (int *)&local.event.game_engine_flags;
  memcpy(edst, properties, sizeof(s_variant_multiplayer_object_properties_definition));
  simulation_request_player_event((const s_player_editor_request_event_data *)&local.event);
}

// @address 0x2731d8 game_engine_sandbox_find_player_original_unit
extern "C" int game_engine_sandbox_find_player_original_unit(int p0)
{
    int result = -1;
    c_object_iterator<unit_datum> iterator;
    iterator.begin(1, 0);
    while (iterator.next()) {
        unit_datum *object = iterator.get_datum();
        if (object->unit.player_index == -1 &&
            object->unit.controller_player_index == p0 &&
            object->unit.time_of_death == -1) {
            result = iterator.get_index();
            break;
        }
    }
    return result;
}


// @address 0x26da98 game_engine_sandbox_request_properties_change
void game_engine_sandbox_request_properties_change(long player_index, long variant_object_index, const int *properties)
{
  s_variant_object_datum datum;
  ((s_game_engine_globals *)game_engine_globals)->map_info.get_variant_object_datum(variant_object_index, &datum);
  if (memcmp(properties, &datum, sizeof(s_variant_multiplayer_object_properties_definition)) != 0) {
    typedef char editor_action_storage_size[(sizeof(datum) >= sizeof(game_engine_sandbox_editor_action)) ? 1 : -1];
    typedef char editor_action_storage_alignment[(__alignof(s_variant_object_datum) >= __alignof(game_engine_sandbox_editor_action)) ? 1 : -1];
    // The editor action reuses the datum slot after the comparison.
    game_engine_sandbox_editor_action &action = *new (&datum) game_engine_sandbox_editor_action;
    action.properties.game_engine_flags = 0;
    action.properties.placement_flags = 12;
    action.properties.boundary_shape = _multiplayer_object_boundary_shape_unused;
    action.properties.spawn_rate = 0;
    action.properties.spare_clips = 0;
    action.properties.boundary_negative_height = k_real_zero_storage;
    action.properties.teleporter_channel = 0;
    action.properties.boundary_positive_height = k_real_zero_storage;
    action.properties.boundary_box_length = k_real_zero_storage;
    action.properties.boundary_width = k_real_zero_storage;
    action.properties.team_designator = _variant_team_designator_neutral;
    __int64 *p = (__int64 *)&action;
    unsigned int count = 8;
    do { *p++ = 0; } while (--count);
    action.action = 5;
    action.owner_index = variant_object_index;
    action.player_index = player_index & 0xffff;
    int *dst = (int *)&action.properties.game_engine_flags;
    memcpy(dst, properties, sizeof(s_variant_multiplayer_object_properties_definition));
    simulation_queue_editor_action_insert(&action);
  }
}

// @address 0x26e270 ?prepare_for_new_state@c_sandbox_engine@@UBAXJ@Z

void c_sandbox_engine::prepare_for_new_state(long phase) const
{
  if (!sandbox_game_is_playback() && phase == 2) {
    c_player_in_game_iterator iterator;
    iterator.begin();
    while (iterator.next()) {
      long object_index = -1;
      if (game_engine_is_player_in_edit_mode(iterator.get_index(), &object_index, 0) && object_index != -1)
        game_engine_sandbox_request_object_move(object_index, 0.0f);
    }
  }
}


// match: noalias declaration preserves caller live registers

static __forceinline bool game_engine_sandbox_variant_flag_test(unsigned short flags, long bit)
{
  return ((flags >> bit) & 1) != 0;
}
// @address 0x1d41d0 game_engine_sandbox_update_variant_objects_for_engine
extern "C" void game_engine_sandbox_update_variant_objects_for_engine(c_map_variant *map_variant)
{
  for (long index = 0; index < map_variant->variant_object_count; index++) {
    s_variant_object_datum *variant_object = &map_variant->objects[index];
    if ((variant_object->flags & 0x3ff) == 0)
      continue;
    if (game_engine_sandbox_variant_flag_test(variant_object->flags, 5) || game_engine_sandbox_variant_flag_test(variant_object->flags, 2))
      continue;
    if (variant_object->object_index != -1 && !game_engine_sandbox_variant_flag_test(variant_object->flags, 6)) {
      if (game_engine_sandbox_variant_flag_test(variant_object->flags, 8) && game_engine_sandbox_variant_flag_test(variant_object->flags, 9))
        object_detach(variant_object->object_index);
      if ((object_get(variant_object->object_index))->parent_object_index == -1) {
        if (game_engine_sandbox_variant_flag_test(variant_object->flags, 8))
          map_variant_update_parented_object_position(map_variant, index);
        else
          object_set_position_direct(variant_object->object_index, &variant_object->position, &variant_object->forward, &variant_object->up, 0, false);
      }
    }
    const s_variant_multiplayer_object_properties_definition *properties =
        (const s_variant_multiplayer_object_properties_definition *)&variant_object->properties;
    unsigned short game_engine_flags = properties->game_engine_flags.get_unsafe();
    if ((game_engine_flags & 0x3ff) == 0)
      continue;
    if (game_engine_flags != 0x3ff && (map_variant->game_engine_subtype == 10 || !((1 << map_variant->game_engine_subtype) & game_engine_flags))) {
      if (game_engine_sandbox_variant_flag_test(variant_object->flags, 6))
        continue;
      long object_index = variant_object->object_index;
      if (object_index == -1)
        continue;
      object_datum *object = object_get(object_index);
      s_variant_object_datum datum;
      c_map_variant *edit_map_variant = &((s_game_engine_globals *)game_engine_globals)->map_info;
      long variant_object_index = object->map_variant_index;
      edit_map_variant->get_variant_object_datum(variant_object_index, &datum);
      datum.flags.set_unsafe(datum.flags.get_unsafe() | 0x40);
      edit_map_variant->set_variant_object_datum(variant_object_index, &datum, false);
      ((s_game_engine_globals *)game_engine_globals)->map_info.remove_object(object->map_variant_index, _object_removal_origin_2);
      object_delete(object_index);
    } else {
      unsigned short flags = variant_object->flags;
      if (!game_engine_sandbox_variant_flag_test(flags, 6) && variant_object->object_index != -1)
        continue;
      long scenario_index = -1;
      e_object_type object_type = _object_type_none;
      if (game_engine_sandbox_variant_flag_test(flags, 3))
        map_variant->get_object_scenario_properties(index, &object_type, &scenario_index);
      variant_object->flags.set_unsafe(flags & ~0x41);
      map_variant_new_object(map_variant, map_variant->quotas[variant_object->quota_index].object_definition_index, 1, index,
          &variant_object->position, &variant_object->forward, &variant_object->up, scenario_index, object_type, properties, 0);
    }
  }
}


// @address 0x744f88 g_sandbox_edit_effect_index
long g_sandbox_edit_effect_index = -1;
static __forceinline void game_engine_sandbox_set_all_real(float (&array)[16], const float &value)
{
  for (long i = 0; i < 16; i++)
    array[i] = value;
}


struct vip_s_implicit_geometry_shape;


// @address 0x2714e8 ?build_simulation_update@c_sandbox_engine@@UBAXPAKJPAX@Z
void c_sandbox_engine::build_simulation_update(unsigned long *flags, long state_data_size, void *state_data) const
{
  game_engine_sandbox_update_data *data = (game_engine_sandbox_update_data *)state_data;
  unsigned long changed = 0;
  if (*flags & 0x3f)
    build_global_update(*flags & 0x3f, &changed, (s_game_engine_state_data *)data);
  if (*flags & 0x40) {
    short active_player_flags = ((s_game_engine_globals *)game_engine_globals)->sandbox.active_player_flags;
    if (data->active_player_flags != active_player_flags) {
      changed |= 0x40;
      data->active_player_flags = active_player_flags;
    }
  }
  if (*flags & 0x80) {
    for (short i = 0; i < 16; i++) {
      long owner = ((s_game_engine_globals *)game_engine_globals)->sandbox.owners[i];
      if (data->owners[i] != owner) {
        changed |= 0x80;
        data->owners[i] = owner;
      }
    }
  }
  if (*flags & 0x100) {
    short round_time = (short)((s_game_engine_globals *)game_engine_globals)->map_info.budget_spent;
    if (data->round_time != round_time) {
      changed |= 0x100;
      data->round_time = round_time;
    }
  }
  if (*flags & 0x200) {
    long value = (short)((s_game_engine_globals *)game_engine_globals)->map_info.variant_object_count;
    if (data->variant_object_count != value) {
      changed |= 0x200;
      data->variant_object_count = value;
    }
  }
  if (*flags & 0x400) {
    long round_value = ((s_game_engine_globals *)game_engine_globals)->map_info.game_engine_subtype;
    if (data->game_engine_subtype != round_value) {
      changed |= 0x400;
      data->game_engine_subtype = round_value;
    }
  }
  *flags = changed;
}


// @address 0x2716b8 ?apply_simulation_update@c_sandbox_engine@@UBA_NKJPBX@Z
bool c_sandbox_engine::apply_simulation_update(unsigned long flags, long state_data_size, const void *state_data) const
{
  const game_engine_sandbox_update_data *data = (const game_engine_sandbox_update_data *)state_data;
  bool result = true;
  unsigned long base_flags = flags & 0x3f;
  if (base_flags != 0) {
    unsigned char applied = apply_global_update(base_flags, (const s_game_engine_state_data *)data);
    result = result && applied != 0;
  }
  if (flags & 0x40) {
    if (data->active_player_flags != ((s_game_engine_globals *)game_engine_globals)->sandbox.active_player_flags)
      ((s_game_engine_globals *)game_engine_globals)->sandbox.active_player_flags = data->active_player_flags;
  }
  if (flags & 0x80) {
    s_game_engine_globals *globals = (s_game_engine_globals *)game_engine_globals;
    for (short i = 0; i < 16; i++) {
      if (globals->sandbox.owners[i] != data->owners[i]) {
        if (data->owners[i] == -1) {
          long player_index = datum_absolute_index_to_index(player_data, i);
          if (player_index != -1 && player_is_on_local_machine(player_index))
            globals->sandbox.edit_times[i] = g_sandbox_default_edit_distance;
        }
        globals->sandbox.owners[i] = data->owners[i];
      }
    }
  }
  if (flags & 0x100) {
    if (data->round_time != (short)((s_game_engine_globals *)game_engine_globals)->map_info.budget_spent)
      ((s_game_engine_globals *)game_engine_globals)->map_info.budget_spent = (float)data->round_time;
  }
  if (flags & 0x200)
    ((s_game_engine_globals *)game_engine_globals)->map_info.variant_object_count = (short)data->variant_object_count;
  if (flags & 0x400)
    ((s_game_engine_globals *)game_engine_globals)->map_info.game_engine_subtype = data->game_engine_subtype;
  return result;
}


static __forceinline long game_engine_sandbox_tracked_object_index(long object_index)
{
  s_game_engine_globals *globals = (s_game_engine_globals *)game_engine_globals;
  long count = globals->sandbox.object_count;
  for (long i = 0; i < count; i++) {
    if (globals->sandbox.object_indices[i] == object_index)
      return i;
  }
  return -1;
}

// @address 0x26d0d8 game_engine_sandbox_request_object_move
void game_engine_sandbox_request_object_move(long object_index, float time)
{
  game_engine_sandbox_editor_action action;
  action.properties.game_engine_flags = 0;
  action.properties.boundary_negative_height = k_real_zero_storage;
  action.properties.boundary_positive_height = k_real_zero_storage;
  action.properties.boundary_shape = _multiplayer_object_boundary_shape_unused;
  action.properties.boundary_box_length = k_real_zero_storage;
  action.properties.spawn_rate = 0;
  action.properties.boundary_width = k_real_zero_storage;
  action.properties.spare_clips = 0;
  action.properties.placement_flags = 12;
  action.properties.teleporter_channel = 0;
  action.properties.team_designator = _variant_team_designator_neutral;
  __int64 *p = (__int64 *)&action;
  unsigned int count = 8;
  do { *p++ = 0; } while (--count);
  action.action = 4;
  action.value = k_real_zero_storage;
  action.owner_index = (object_get(object_index))->map_variant_index;
  simulation_queue_editor_action_insert(&action);
}


// @address 0x26f968 ?initialize_for_new_map@c_sandbox_engine@@UBA_NXZ
bool c_sandbox_engine::initialize_for_new_map() const
{
  s_sandbox_globals *sandbox = &((s_game_engine_globals *)game_engine_globals)->sandbox;
  if (sandbox)
    sandbox->edited_object_properties.s_variant_multiplayer_object_properties_definition::s_variant_multiplayer_object_properties_definition();
  memset(sandbox, 0, sizeof(*sandbox));
  c_game_variant *object = game_engine_sandbox_variant_get();
  ((const c_game_engine_base_variant *)object->m_variant_storage)->can_be_cast_to(_game_engine_sandbox, (const void **)&g_sandbox_editor_variant);
  ((s_game_engine_globals *)game_engine_globals)->sandbox.active_player_flags = 0;
  long *owner = ((s_game_engine_globals *)game_engine_globals)->sandbox.owners;
  long count = 16;
  do { *owner++ = -1; } while (--count);
  long *handle = ((s_game_engine_globals *)game_engine_globals)->sandbox.handle_slots;
  count = 16;
  do { *handle++ = -1; } while (--count);
  bool *rotated = ((s_game_engine_globals *)game_engine_globals)->sandbox.object_rotated;
  count = 16;
  do { *rotated++ = false; } while (--count);
  long *index = ((s_game_engine_globals *)game_engine_globals)->sandbox.object_indices;
  count = 64;
  do { *index++ = -1; } while (--count);
  ((s_game_engine_globals *)game_engine_globals)->sandbox.object_count = 0;
  ((s_game_engine_globals *)game_engine_globals)->sandbox.variant_file_exists = false;
  ((s_game_engine_globals *)game_engine_globals)->sandbox.boundary_object_index = -1;
  game_engine_sandbox_set_all_real(((s_game_engine_globals *)game_engine_globals)->sandbox.edit_times, g_sandbox_default_edit_distance);
  if (((s_game_globals *)global_game_globals) != 0) {
    const cache_file_tag_instance *instance = &global_tag_instances[(unsigned short)((s_game_globals *)global_game_globals)->multiplayer_globals_index];
    s_multiplayer_globals_definition *globals = (s_multiplayer_globals_definition *)instance->definition;
    if (globals != 0) {
      s_multiplayer_runtime_globals_definition *group = (s_multiplayer_runtime_globals_definition *)globals->runtime.address;
      if (group != 0) {
        s_multiplayer_constants *settings = (s_multiplayer_constants *)group->multiplayer_constants.address;
        if (settings != 0)
          g_sandbox_edit_effect_index = settings->editor_effect.index;
      }
    }
  }
  return true;
}


// @address 0x271120 ?update@c_sandbox_engine@@UBAXXZ
void c_sandbox_engine::update(void) const
{
  game_engine_sandbox_update_variant_objects(&((s_game_engine_globals *)game_engine_globals)->map_info);
  if (sandbox_game_is_playback()) {
    for (long i = 0; i < 16; i++) {
      if (((s_game_engine_globals *)game_engine_globals)->sandbox.owners[i] == -1)
        continue;
      long player_index = datum_absolute_index_to_index(player_data, i);
      if (player_index == -1)
        continue;
      long variant_index = ((s_game_engine_globals *)game_engine_globals)->sandbox.owners[datum_index_to_absolute_index(player_index)];
      if (variant_index == -1)
        continue;
      long object_index = ((s_game_engine_globals *)game_engine_globals)->map_info.objects[variant_index].object_index;
      if (object_index == -1)
        continue;
      object_datum *object = object_get(object_index);
      bool connected = (object->physics_flags >> 20) & 1;
      if (!connected)
        game_engine_sandbox_set_object_connected(object_index, 1);
      if (player_is_on_local_machine(player_index))
        continue;
      real_vector3d forward;
      real_point3d origin;
      real_vector3d up;
      object_get_origin(object_index, &origin);
      ::object_get_orientation(object_index, &forward, &up);
      long object_variant_index = object->map_variant_index;
      if (object_variant_index == -1)
        continue;
      s_variant_object_datum *variant_object = 0;
      if (object_variant_index >= 0 && object_variant_index < 640)
        variant_object = &((s_game_engine_globals *)game_engine_globals)->map_info.objects[object_variant_index];
      float radius = game_engine_sandbox_get_object_radius(object->definition_index);
      real_vector3d delta;
      vector_from_points3d(&origin, &variant_object->position, &delta);
      float forward_dot = dot_product3d(&variant_object->forward, &forward);
      float up_dot = dot_product3d(&up, &variant_object->up);
      real_vector3d linear_velocity;
      real_vector3d angular_velocity;
      object_get_velocities(object_index, &linear_velocity, &angular_velocity);
      if ((float)sqrt(delta.i * delta.i + delta.j * delta.j + delta.k * delta.k) > radius * 0.5f) {
        linear_velocity.k = 0.0f;
        linear_velocity.j = 0.0f;
        linear_velocity.i = 0.0f;
      }
      if (forward_dot < 0.95f || up_dot < 0.95) {
        angular_velocity.k = 0.0f;
        angular_velocity.j = 0.0f;
        angular_velocity.i = 0.0f;
      }
      object_set_velocities(object_index, &linear_velocity, &angular_velocity);
    }
  }
  s_game_engine_globals *globals = (s_game_engine_globals *)game_engine_globals;
  long *preview_object_index = &globals->sandbox.boundary_object_index;
  long object_index = *preview_object_index;
  if (object_index != -1) {
    object_datum *object = (object_datum *)object_try_and_get_and_verify_type(object_index, -1);
    if (object) {
      if ((unsigned short)(object_get(object_index))->multiplayer_data_offset != 0xffff) {
        if ((unsigned short)object->map_variant_index != 0xffff) {
          s_variant_object_datum variant_object;
          globals->map_info.get_variant_object_datum(object->map_variant_index, &variant_object);
          if (variant_object.properties.boundary_shape.get() != 0) {
            calculate_influence_boundary_geometry(object_index, &variant_object, 0);
            multiplayer_object_build_boundary_primitive((const vip_s_implicit_geometry_shape *)&variant_object,
                &global_real_argb_colors[_real_argb_white], object_index);
          }
        }
      }
      else
        *preview_object_index = -1;
    }
  }
}


// @address 0x26e9e0 ?player_unlock_object@c_sandbox_engine@@SAXJM_N@Z
void c_sandbox_engine::player_unlock_object(long player_index, float time, bool force_attached)
{
  if (player_index == -1)
    return;
  if (!sandbox_game_is_playback()) {
    long variant_index = ((s_game_engine_globals *)game_engine_globals)->sandbox.owners[datum_index_to_absolute_index(player_index)];
    if (variant_index != -1) {
      long object_index = ((s_game_engine_globals *)game_engine_globals)->map_info.objects[variant_index].object_index;
      if (object_index != -1) {
        bool attached = false;
        if (force_attached || (object_get(object_index))->parent_object_index != -1)
          attached = true;
        if (!attached) {
          snapshot_object_position(false, object_index);
          ::object_set_at_rest(object_index, false);
          if (time > 0.0001f) {
            player_datum *player = player_get(player_index);
            real_point3d origin;
            object_get_origin(object_index, &origin);
            if (player->unit_index) {
              real_point3d camera;
              unit_get_camera_position(player->unit_index, &camera);
              float speed = 0.0f;
              real_vector3d direction;
              direction.i = origin.x - camera.x;
              direction.j = origin.y - camera.y;
              direction.k = origin.z - camera.z;
              normalize3d(&direction);
              speed = time * 15.0f;
              direction.i *= speed;
              direction.j *= speed;
              direction.k *= speed;
              real_vector3d linear_velocity;
              real_vector3d angular_velocity;
              object_get_velocities(object_index, &linear_velocity, &angular_velocity);
              linear_velocity = direction;
              object_set_velocities(object_index, &linear_velocity, &angular_velocity);
              ::simulation_action_object_update(object_index, 16);
            }
          }
          game_engine_sandbox_track_object(object_index);
        }
      }
    }
    ((s_game_engine_globals *)game_engine_globals)->sandbox.owners[datum_index_to_absolute_index(player_index)] = -1;
    simulation_action_game_engine_globals_update(0x80);
    return;
  }
  s_sandbox_player_event event;
  float zero = 0.0f;
  event.game_engine_flags = 0;
  event.boundary_shape = 0;
  event.spawn_rate = 0;
  event.spare_clips = 0;
  event.teleporter_channel = 0;
  event.boundary_negative_height = zero;
  event.boundary_positive_height = zero;
  event.boundary_box_length = zero;
  event.boundary_width = zero;
  event.placement_flags = 12;
  event.team_designator = 9;
  for (int i = 15; i != 0; i--)
    event.words[15 - i] = 0;
  event.time = time;
  event.player_index = (short)player_index;
  event.event_type = 2;
  long variant_index = ((s_game_engine_globals *)game_engine_globals)->sandbox.owners[datum_index_to_absolute_index(player_index)];
  if (variant_index != -1) {
    long object_index = ((s_game_engine_globals *)game_engine_globals)->map_info.objects[variant_index].object_index;
    if (object_index != -1) {
      object_get_origin(object_index, &event.position);
      ::object_get_orientation(object_index, &event.forward, &event.up);
      simulation_request_player_event((const s_player_editor_request_event_data *)&event);
    }
  }
}
static __forceinline bool game_engine_sandbox_physics_flag_test(const object_datum *object, long bit)
{
  return (object->physics_flags >> bit) & 1;
}
static __forceinline long game_engine_sandbox_object_get_early_mover(const object_datum *object)
{
  long result = -1;
  if (game_engine_sandbox_physics_flag_test(object, 1) || game_engine_sandbox_physics_flag_test(object, 2))
    result = object->local_physics_object_index;
  return result;
}
// @address 0x26ec98 ?player_lock_object_for_edit@c_sandbox_engine@@SAXJJ@Z
void c_sandbox_engine::player_lock_object_for_edit(long player_index, long object_index)
{
  if (object_index == -1)
    return;
  if (!game_engine_sandbox_is_player_in_edit_mode(player_index))
    return;
  object_datum *object = object_get(object_index);
  short variant_object_index = object->map_variant_index;
  bool can_lock = true;
  if (variant_object_index == -1)
    can_lock = false;
  s_game_engine_globals *engine_globals = (s_game_engine_globals *)game_engine_globals;
  for (short i = 0; i < 16; i++) {
    if (engine_globals->sandbox.owners[i] == variant_object_index) {
      can_lock = false;
      break;
    }
  }
  if (!can_lock)
    return;
  if (!sandbox_game_is_playback()) {
    s_variant_object_datum variant_object;
    ((s_game_engine_globals *)engine_globals)->map_info.get_variant_object_datum(variant_object_index, &variant_object);
    long root_index = game_engine_sandbox_object_root(object_index, ((s_object_header *)object_header_data->data));
    object_datum *root_object = object_get(object_index);
    if (game_engine_sandbox_object_get_early_mover(root_object) != -1 || object_early_mover_is_owner(root_index, false)) {
      if (root_index != object_index) {
        object_detach(object_index);
        object->flags &= ~0x4000000UL;
        ::simulation_action_object_update(object_index, 0x800);
      }
      variant_object.flags.set_unsafe(variant_object.flags.get_unsafe() & ~0x300);
      variant_object.parent_object_identifier.clear();
      ((s_game_engine_globals *)game_engine_globals)->map_info.set_variant_object_datum(variant_object_index, &variant_object, true);
      simulation_action_sandbox_group_update(variant_object_index, 2, false);
    }
    snapshot_object_position(false, object_index);
    ((s_game_engine_globals *)game_engine_globals)->sandbox.owners[datum_index_to_absolute_index(player_index)] = variant_object_index;
    simulation_action_game_engine_globals_update(0x80);
    game_engine_sandbox_remove_tracked_object(object_index, -1);
  } else {
    simulation_request_player_editor(player_index, (e_player_editor_request_type)1, true, variant_object_index);
  }
}

// @address 0x26fd60 ?player_just_spawned@c_sandbox_engine@@UBAXJ@Z
void c_sandbox_engine::player_just_spawned(long player_index) const
{
  if ((((s_game_engine_globals *)game_engine_globals)->sandbox.active_player_flags & (1 << (player_index & 0xffff))) != 0) {
    if (!sandbox_game_is_playback() && !game_engine_sandbox_spawn_monitor_for_player(player_index))
      ::teleporter_update((c_teleporter_area *)this, player_index, 1);
  }
}
// @address 0x26fe18 ?player_left@c_sandbox_engine@@UBAXJ@Z
void c_sandbox_engine::player_left(long player_index) const
{
  if (!sandbox_game_is_playback())
    ::teleporter_update((c_teleporter_area *)this, player_index, 1);
}


// @address 0x26f008 ?toggle_player_mode@c_sandbox_engine@@QBAXJ@Z
void c_sandbox_engine::toggle_player_mode(long player_index) const
{
  bool predicted = false;
  if (game_globals->options.game_simulation == 4)
    predicted = true;
  if (predicted == 0) {
    if (game_engine_sandbox_is_player_in_edit_mode(player_index))
      ::teleporter_update((c_teleporter_area *)(const c_game_engine *)this, player_index, 1);
    else
      ::teleporter_update((c_teleporter_area *)(const c_game_engine *)this, player_index, 0);
    return;
  }
  long absolute_index = datum_index_to_absolute_index(player_index);
  long unit_index = ((player_datum *)((s_data_array *)player_data)->data)[absolute_index].unit_index;
  if (unit_index != -1) {
    s_sandbox_player_event event;
    event.game_engine_flags = 0;
    event.boundary_shape = 0;
    event.spawn_rate = 0;
    event.spare_clips = 0;
    event.teleporter_channel = 0;
    event.boundary_negative_height = 0.0f;
    event.boundary_positive_height = 0.0f;
    event.boundary_box_length = 0.0f;
    event.boundary_width = 0.0f;
    event.placement_flags = 12;
    event.team_designator = 9;
    const object_datum *unit = object_get(unit_index);
    for (int i = 15; i != 0; i--)
      event.words[15 - i] = 0;
    for (int i = 15; i != 0; i--)
      event.words[15 - i] = 0;
    event.player_index = (short)player_index;
    event.flags = !game_engine_sandbox_is_player_in_edit_mode(player_index);
    event.event_type = 0;
    object_get_origin(unit_index, &event.player_position);
    event.player_forward = unit->forward;
    event.player_up = unit->up;
    long variant_index = ((s_game_engine_globals *)game_engine_globals)->sandbox.owners[absolute_index];
    if (variant_index != -1) {
      long object_index = ((s_game_engine_globals *)game_engine_globals)->map_info.objects[variant_index].object_index;
      if (object_index != -1) {
        object_get_origin(object_index, &event.position);
        ::object_get_orientation(object_index, &event.forward, &event.up);
      }
    }
    simulation_request_player_event((const s_player_editor_request_event_data *)&event);
  }
}


static __forceinline float game_engine_sandbox_get_budget_fraction(const c_map_variant *budget, float *used, float *limit)
{
  if (used)
    *used = budget->budget_spent;
  if (limit)
    *limit = budget->budget_limit;
  float result;
  if (!(budget->budget_limit <= 0.0001f))
    result = budget->budget_spent / budget->budget_limit;
  else
    result = 0.0f;
  return result;
}


// @address 0x271910 ?promote_to_simulation_authority@c_sandbox_engine@@UBAXXZ
void c_sandbox_engine::promote_to_simulation_authority(void) const
{
  for (long i = 0; i < 80; i++) {
    if (((s_game_engine_globals *)game_engine_globals)->map_info.m_simulation_indices[i] == -1)
      simulation_action_sandbox_group_create(i);
  }
  game_engine_sandbox_update_variant_objects_for_engine(&((s_game_engine_globals *)game_engine_globals)->map_info);
  for (long i = 0; i < ((s_game_engine_globals *)game_engine_globals)->map_info.variant_object_count; i++)
    simulation_action_sandbox_group_update(i, 8, 0);
  {
    c_player_in_game_iterator players;
    players.begin();
    while (players.next()) {
      long player_index = players.get_index();
      int owner = game_engine_sandbox_find_player_original_unit(player_index);
      if (owner == -1) {
        if (game_engine_sandbox_is_player_in_edit_mode(player_index)) {
          long unit_index = players.get_datum()->unit_index;
          if (unit_index != -1) {
            player_set_unit_index(player_index, -1);
            object_delete(unit_index);
            simulation_action_object_delete(unit_index);
          }
          {
            s_game_engine_globals *globals = (s_game_engine_globals *)game_engine_globals;
            globals->sandbox.active_player_flags &= ~(1 << (short)player_index);
          }
          simulation_action_game_engine_globals_update(64);
        }
      } else if (!game_engine_sandbox_is_player_in_edit_mode(player_index)) {
        object_delete(owner);
        long unit_index = players.get_datum()->unit_index;
        if (unit_index != -1) {
          player_set_unit_index(player_index, -1);
          object_delete(unit_index);
        }
        {
          s_game_engine_globals *globals = (s_game_engine_globals *)game_engine_globals;
          globals->sandbox.active_player_flags &= ~(1 << (short)player_index);
        }
        simulation_action_game_engine_globals_update(64);
      } else {
        ::teleporter_update((c_teleporter_area *)this, player_index, 1);
      }
    }
  }
  {
    c_object_iterator<unit_datum> objects;
    objects.begin(1, 0);
    if (objects.next()) {
      do {
        unit_datum *object = objects.get_datum();
        if (object->unit.player_index == -1 && object->unit.controller_player_index == -1) {
          long handle = objects.get_index();
          if (object_is_hidden(handle))
            object_delete(handle);
        }
      } while (objects.next());
    }
  }
  {
    s_game_engine_globals *globals = (s_game_engine_globals *)game_engine_globals;
    for (long i = 0; i < 16; i++) {
      if (globals->sandbox.owners[i] != -1) {
        long player_index = datum_absolute_index_to_index(player_data, i);
        if (player_index == -1 || player_get(player_index)->unit_index == -1 ||
            (player_get(player_index)->flags & 2) != 0)
          globals->sandbox.owners[i] = -1;
      }
    }
  }
  simulation_action_game_engine_globals_update(128);
  ((s_game_engine_globals *)game_engine_globals)->sandbox.boundary_object_index = -1;
}

// @address 0x271c30 ?get_hud_interface_state@c_sandbox_engine@@UBAXJPAUgame_engine_interface_state@@@Z
void c_sandbox_engine::get_hud_interface_state(long player_index, struct game_engine_interface_state *state) const
{
  if (game_engine_sandbox_is_player_in_edit_mode(player_index)) {
    c_map_variant *variant = &((s_game_engine_globals *)game_engine_globals)->map_info;
    const c_map_variant *budget = &((s_game_engine_globals *)game_engine_globals)->map_info;
    ((game_engine_interface_state *)state)->budget_fraction = game_engine_sandbox_get_budget_fraction(
        budget, &((game_engine_interface_state *)state)->budget_used, &((game_engine_interface_state *)state)->budget);
    s_game_engine_globals *sandbox = (s_game_engine_globals *)game_engine_globals;
    if (sandbox->sandbox.owners[datum_index_to_absolute_index(player_index)] != -1) {
      ((game_engine_interface_state *)state)->editing_state = 2;
    } else {
      long object_index = -1;
      if (player_index != -1)
        object_index = sandbox->sandbox.handle_slots[datum_index_to_absolute_index(player_index)];
      if (object_index == -1) {
        ((game_engine_interface_state *)state)->editing_state = 0;
      } else {
        bool invalid = false;
        object_datum *object = object_get(object_index);
        long variant_index = object->map_variant_index;
        if (variant_index != -1) {
          s_variant_object_datum datum;
          variant_index = object->map_variant_index;
          variant->get_variant_object_datum(variant_index, &datum);
          if (object_index == datum.helper_object_index) {
            if (datum.object_index == -1)
              invalid = true;
            else if ((object_get(datum.object_index))->parent_object_index != -1)
              invalid = true;
          }
        } else {
          invalid = true;
        }
        if ((bool)((((object_datum *)object)->damage_flags >> 2) & 1))
          invalid = true;
        long editing_state;
        if (!invalid && get_player_who_is_editing_object(object_index) == -1 && variant_index != -1)
          editing_state = 1;
        else
          editing_state = 3;
        ((game_engine_interface_state *)state)->editing_state = editing_state;
      }
    }
  }
}


// @address 0x272608 game_engine_sandbox_update_tracked_objects
extern "C" void game_engine_sandbox_update_tracked_objects(void)
{
  for (long tracked_index = 0; tracked_index < ((s_game_engine_globals *)game_engine_globals)->sandbox.object_count; tracked_index++) {
    s_game_engine_globals *globals = (s_game_engine_globals *)game_engine_globals;
    long object_index = globals->sandbox.object_indices[tracked_index];
    bool remove = false;
    object_datum *object = object_get(object_index);
    real_point3d origin;
    object_get_origin(object_index, &origin);
    if (!scenario_trigger_volumes_test_point(&origin))
      remove = true;
    if ((unsigned char)((object_get(object_index)->physics_flags >> 9) & 1) || object->parent_object_index != -1)
      remove = true;
    if (remove) {
      game_engine_sandbox_dispose_tracked_object(object_index, tracked_index, false);
      if (object->map_variant_index != -1)
        simulation_action_sandbox_group_update(object->map_variant_index, 1, false);
      tracked_index--;
    } else {
      if (game_engine_sandbox_is_object_soft_ceiling_limited(object_index))
        object_update_soft_ceiling(object_index, &globals->sandbox.positions[tracked_index]);
      if ((unsigned short)object->map_variant_index != 0xffff)
        snapshot_object_position(false, object_index);
    }
  }
}
// @address 0x272768 game_engine_sandbox_remove_tracked_object
bool game_engine_sandbox_remove_tracked_object(long object_index, long tracked_index)
{
  bool result = false;
  if (tracked_index == -1)
    tracked_index = game_engine_sandbox_tracked_object_index(object_index);
  if (tracked_index != -1) {
    s_game_engine_globals *globals = (s_game_engine_globals *)game_engine_globals;
    long last = --globals->sandbox.object_count;
    if (tracked_index != last) {
      s_sandbox_globals *sandbox_pointer = &((s_game_engine_globals *)game_engine_globals)->sandbox;
      s_sandbox_globals *sandbox = tracked_index != last ? sandbox_pointer : sandbox_pointer;
      globals->sandbox.object_indices[tracked_index] = globals->sandbox.object_indices[last];
      globals->sandbox.positions[tracked_index] = sandbox->positions[last];
    }
    object_datum *object = (object_datum *)object_try_and_get_and_verify_type(object_index, -1);
    if (object) {
      short *flags = (short *)game_engine_sandbox_multiplayer_properties(object);
      if (flags) {
        *flags &= ~0x800;
        ::simulation_action_object_update(object_index, 0x1000);
      }
    }
    result = true;
  }
  return result;
}


// @address 0x2728d0 game_engine_sandbox_dispose_tracked_object
extern "C" void game_engine_sandbox_dispose_tracked_object(long object_index, long tracked_index, bool immediate)
{
  if (game_engine_sandbox_remove_tracked_object(object_index, tracked_index) && !game_engine_object_is_marked_item(object_index)) {
    if (!immediate)
      game_engine_sandbox_set_object_connected(object_index, 0);
    game_engine_candy_monitor_attach_variant_object(object_index, -1);
    if (object_index != -1 && !immediate) {
      real_point3d origin;
      object_get_origin(object_index, &origin);
      if (breakable_surfaces_is_in_soft_ceiling(object_index) || !scenario_trigger_volumes_test_point(&origin)) {
        object_datum *object = object_get(object_index);
        ((s_game_engine_globals *)game_engine_globals)->map_info.remove_object(object->map_variant_index, _object_removal_origin_2);
        object_delete(object_index);
      }
    }
  }
}


// @address 0x26fb80 ?initialize_for_new_round@c_sandbox_engine@@UBA_NXZ
bool c_sandbox_engine::initialize_for_new_round(void) const
{
  if (!game_engine_sandbox_game_simulation_is_4(game_globals)) {
    game_engine_sandbox_update_variant_objects_for_engine(&((s_game_engine_globals *)game_engine_globals)->map_info);
    game_engine_sandbox_compute_budget_total(&((s_game_engine_globals *)game_engine_globals)->map_info);
    simulation_action_game_engine_globals_update(0x100);
  }
  map_variant_set_placement_marker_visibility(&((s_game_engine_globals *)game_engine_globals)->map_info, false);
  long *index = ((s_game_engine_globals *)game_engine_globals)->sandbox.object_indices;
  long count = 64;
  do { *index++ = -1; } while (--count);
  ((s_game_engine_globals *)game_engine_globals)->sandbox.object_count = 0;
  ((s_game_engine_globals *)game_engine_globals)->sandbox.boundary_object_index = -1;
  return true;
}

// 0x26de88: spawn an object of the given definition for a player at a point (authoritative), or ask the host to (predicted)
static __forceinline void game_engine_sandbox_initialize_event(long type, long event, s_game_engine_event_data *event_data)
{
  event_data->type = type;
  event_data->event = event;
  event_data->identifier = -1;
  event_data->flags = -1;
  event_data->cause_player_index = -1;
  event_data->cause_team_index = -1;
  event_data->effect_player_index = -1;
  event_data->effect_team_index = -1;
  event_data->data = 0;
  event_data->data_word = -1;
}


extern "C" void game_engine_sandbox_spawn_object_for_player(long definition_index, long player_index, const real_point3d *point)
{
  if (!game_engine_sandbox_game_simulation_is_4(game_globals)) {
    ((s_game_engine_globals *)game_engine_globals)->sandbox.spawn_definition_index = definition_index;
    real_point3d position;
    if (point)
      position = *point;
    if (game_engine_sandbox_find_placement_point(definition_index, &position, player_get(player_index)->unit_index)) {
      c_map_variant *map_variant = &((s_game_engine_globals *)game_engine_globals)->map_info;
      long object_index = map_variant_new_object(map_variant, definition_index, 0, -1, &position, 0, 0, -1, _object_type_none, 0, 0);
      if (object_index != -1) {
        object_datum *object = object_get(object_index);
        if (object->map_variant_index != -1) {
          game_engine_sandbox_start_editing_object(player_index, object->map_variant_index);
          c_map_variant *map_variant = &((s_game_engine_globals *)game_engine_globals)->map_info;
          long quota_index = map_variant_find_quota_index(map_variant, object->definition_index);
          if (quota_index != -1) {
            map_variant->adjust_active_object_count(quota_index, 1);
            simulation_action_sandbox_group_update(object->map_variant_index, 8, 0);
          }
        }
      } else {
        s_game_engine_event_data event_data;
        game_engine_sandbox_initialize_event(1, 56, &event_data);
        event_data.cause_player_index = player_index;
        game_engine_send_event(&event_data);
      }
    } else {
      s_game_engine_event_data event_data;
      game_engine_sandbox_initialize_event(1, 55, &event_data);
      event_data.cause_player_index = player_index;
      game_engine_send_event(&event_data);
    }
  } else {
    s_sandbox_player_event event;
    event.game_engine_flags = 0;
    event.boundary_shape = 0;
    event.spawn_rate = 0;
    event.spare_clips = 0;
    event.teleporter_channel = 0;
    event.boundary_negative_height = 0.0f;
    event.boundary_positive_height = 0.0f;
    event.boundary_box_length = 0.0f;
    event.boundary_width = 0.0f;
    event.placement_flags = 12;
    event.team_designator = 9;
    for (int i = 15; i != 0; i--)
      event.words[15 - i] = 0;
    event.player_index = (short)player_index;
    event.event_type = 5;
    event.variant_object_index = definition_index;
    event.position = *point;
    simulation_request_player_event((const s_player_editor_request_event_data *)&event);
  }
  if (player_index != -1) {
    long user_index = player_mapping_globals->player_input_user_mapping[datum_index_to_absolute_index(player_index)];
    if (user_index != -1 && !game_is_cooperative()) {
      long controller_index = user_interface_controller_get_rumble_enabled(user_index);
      if (controller_index != -1) {
        s_game_input_state *gamepad = &input_abstraction_globals.input_states[controller_index];
        gamepad->buttons[10].latch();
        gamepad->buttons[11].latch();
      }
    }
  }
}


// --- 0x26f530: consume a simulation player-editor request event ---


// @address 0x26f530 ?handle_incoming_simulation_event@c_sandbox_engine@@UBA_NW4e_simulation_event_type@@PBX@Z
bool c_sandbox_engine::handle_incoming_simulation_event(e_simulation_event_type event_type, const void *payload) const
{
  bool result = false;
  if (event_type == _simulation_event_type_player_editor_request) {
    const s_player_editor_request_event_data *data = (const s_player_editor_request_event_data *)payload;
    long player_index = player_try_and_get_player_index_from_absolute_player_index(data->request_player_index);
    if (player_index != -1) {
      switch (data->event_type) {
      case 0:
        {
          bool editing = game_engine_sandbox_is_player_in_edit_mode(player_index);
          if (editing == data->flags)
            break;
          if (editing) {
            long variant_index = ((s_game_engine_globals *)game_engine_globals)->sandbox.owners[datum_index_to_absolute_index(player_index)];
            if (variant_index != -1) {
              const real_vector3d *velocity = &data->forward;
              if (velocity->i * velocity->i + velocity->j * velocity->j + velocity->k * velocity->k > 0.0001f) {
                long object_index = ((s_game_engine_globals *)game_engine_globals)->map_info.objects[variant_index].object_index;
                if (object_index != -1)
                  sandbox_update_object_position(false, object_index, &data->position, velocity, &data->up);
              }
            }
          }
          player_datum *player = player_get(player_index);
          if (player->unit_index != -1 && !data->flags) {
            real_point3d position;
            real_point3d camera_position;
            real_point3d origin;
            real_vector3d up;
            real_vector3d forward;
            position = data->player_position;
            forward = data->player_forward;
            up = data->player_up;
            unit_get_camera_position(player->unit_index, &camera_position);
            object_get_origin(player->unit_index, &origin);
            position.x += (origin.x - camera_position.x) * 0.5f;
            position.y += (origin.y - camera_position.y) * 0.5f;
            position.z += (origin.z - camera_position.z) * 0.5f;
            object_set_position_direct(player->unit_index, &position, &forward, &up, 0, false);
            biped_fix_position(player->unit_index, -1, &position, &position, 0, 0.0f, 1.0f, true, false);
          }
          teleporter_update((c_teleporter_area *)this, player_index, data->flags == 0);
        }
        break;
      case 1:
        if (game_engine_sandbox_is_player_in_edit_mode(player_index))
          game_engine_sandbox_start_editing_object(player_index, data->variant_index);
        break;
      case 2:
        if (game_engine_sandbox_is_player_in_edit_mode(player_index)) {
          long variant_index = ((s_game_engine_globals *)game_engine_globals)->sandbox.owners[datum_index_to_absolute_index(player_index)];
          if (variant_index != -1) {
            long object_index = ((s_game_engine_globals *)game_engine_globals)->map_info.objects[variant_index].object_index;
            if (object_index != -1) {
              const real_vector3d *velocity = &data->forward;
              if (velocity->i * velocity->i + velocity->j * velocity->j + velocity->k * velocity->k > 0.0001f) {
                sandbox_update_object_position(false, object_index, &data->position, velocity, &data->up);
                c_sandbox_engine::player_unlock_object(player_index, data->distance, false);
              }
            }
          }
        }
        break;
      case 3:
        if (game_engine_sandbox_is_player_in_edit_mode(player_index))
          c_sandbox_engine::player_delete_object(player_index, data->variant_index);
        break;
      case 4:
        game_engine_sandbox_end_round_with_value(player_index, data->edited_object_id);
        break;
      case 5:
        if (game_engine_sandbox_is_player_in_edit_mode(player_index))
          game_engine_sandbox_spawn_object_for_player(data->variant_index, player_index, &data->position);
        break;
      case 6:
        if (game_engine_sandbox_is_player_in_edit_mode(player_index))
          game_engine_sandbox_request_properties_change(player_index, data->variant_index, (const int *)&data->properties);
        break;
      case 7:
        if (game_engine_sandbox_is_player_in_edit_mode(player_index))
          game_engine_sandbox_update_budget_quotas(player_index, data->variant_index, data->maximum_count, data->minimum_count);
        break;
      default:
        break;
      }
      result = true;
    }
  }
  return result;
}

// 0x2700c8: sandbox editor input: the edit button grabs/drops, the two manipulation buttons nudge the edited object, the
// delete button asks the simulation to delete the held or edited object
typedef char sandbox_editor_event_properties_offset[(offsetof(game_engine_sandbox_editor_action, properties) == 0x28) ? 1 : -1];
typedef char sandbox_editor_action_position_offset[(offsetof(game_engine_sandbox_editor_action, position) == 0x18) ? 1 : -1];
static __forceinline void game_engine_sandbox_editor_action_clear(game_engine_sandbox_editor_action *event)
{
  __int64 *p = (__int64 *)event;
  unsigned int count = 8;
  do { *p++ = 0; } while (--count);
}
static __forceinline s_sandbox_globals *game_engine_sandbox_globals_get()
{
  return &((s_game_engine_globals *)game_engine_globals)->sandbox;
}
static __forceinline bool game_engine_sandbox_player_is_editing(long player_index)
{
  return game_engine_sandbox_globals_get()->owners[datum_index_to_absolute_index(player_index)] != -1;
}
static __forceinline long game_engine_sandbox_edited_object_get(long player_index)
{
  long object_index = -1;
  long variant_object_index = game_engine_sandbox_globals_get()->owners[datum_index_to_absolute_index(player_index)];
  if (variant_object_index != -1)
    object_index = ((s_game_engine_globals *)game_engine_globals)->map_info.objects[variant_object_index].object_index;
  return object_index;
}
static __forceinline long game_engine_sandbox_player_get_controller(long player_index)
{
  return player_mapping_globals->player_input_controller_mapping[datum_index_to_absolute_index(player_index)];
}
static __forceinline long game_engine_sandbox_player_get_user(long player_index)
{
  return player_mapping_globals->player_input_user_mapping[datum_index_to_absolute_index(player_index)];
}
static __forceinline c_director *director_for_user(long user_index)
{
  return ((s_controllers *)g_director_globals)->directors[user_index].get_director();
}
static __forceinline void game_engine_sandbox_director_camera_reset(c_director *director)
{
  if (director) {
    director->get_camera()->enable_orientation(true);
    director->get_camera()->enable_movement(true);
  }
}
// @address 0x2700c8 game_engine_sandbox_handle_player_input
void game_engine_sandbox_handle_player_input(const c_game_engine *unused, long player_index)
{
  c_window_manager *window_manager = &g_window_manager;
  long controller_index = game_engine_sandbox_player_get_controller(player_index);
  long user_index = game_engine_sandbox_player_get_user(player_index);
  c_director *director = director_for_user(user_index);
  player_datum *player = player_get(player_index);
  s_game_input_state *input_state = &input_abstraction_globals.input_states[controller_index];
  c_abstract_button *delete_button = &input_state->buttons[13];
  c_abstract_button *edit_button = &input_state->buttons[2];
  c_abstract_button *grab_button = &input_state->buttons[10];
  c_abstract_button *rotate_button = &input_state->buttons[11];
  bool ui_locked = window_manager_is_controller_input_suppressed(window_manager, (e_controller_index)controller_index);
  bool locked = !game_engine_in_round() | game_is_cooperative() | ui_locked;
  if (edit_button->get_down_frames() >= 1) {
    if (network_life_cycle_squad_local_peer_is_leader() || g_sandbox_editor_variant->m_edit_mode != 1) {
      edit_button->latch();
      if (!locked && player->unit_index != -1) {
        game_engine_sandbox_editor_action action;
        game_engine_sandbox_editor_action_clear(&action);
        action.action = 2;
        action.player_index = datum_index_to_absolute_index(player_index);
        simulation_queue_editor_action_insert(&action);
      }
    }
  }
  if (game_engine_sandbox_is_player_in_edit_mode(player_index)) {
    long held_object_index = game_engine_sandbox_globals_get()->handle_slots[datum_index_to_absolute_index(player_index)];
    if (!game_engine_sandbox_player_is_editing(player_index) && grab_button->down_frames() >= 1 && !locked && held_object_index != -1) {
      grab_button->latch();
      game_engine_sandbox_editor_action action;
      game_engine_sandbox_editor_action_clear(&action);
      action.action = 3;
      action.player_index = datum_index_to_absolute_index(player_index);
      action.owner_index = (object_get(held_object_index))->map_variant_index;
      simulation_queue_editor_action_insert(&action);
    }
    if (game_engine_sandbox_player_is_editing(player_index)) {
      long object_index = game_engine_sandbox_edited_object_get(player_index);
      if (object_index != -1) {
        bool moved = false;
        object_datum *object = object_get(object_index);
        unsigned long grab_frames = grab_button->down_frames();
        if (grab_frames || rotate_button->down_frames()) {
          if (!locked) {
            if (grab_frames >= 1) {
              moved = true;
              grab_button->latch();
            }
#if !defined(_XBOX) || defined(__clang__)
            if (input_abstraction_button_down_input_frames(controller_index, *rotate_button) >=
                (long)((float)game_time_globals->ticks_per_second * 0.4f + 0.5f))
#else
            if (rotate_button->down_frames() >= (long)((float)game_time_globals->ticks_per_second * 0.4f + 0.5f))
#endif
            {
              moved = true;
              rotate_button->latch();
            }
          }
        }
        if ((bool)((object->damage_flags >> 2) & 1))
          moved = true;
        if (moved)
          game_engine_sandbox_request_object_move(object_index, 0.0f);
        if (!player->look_override)
          game_engine_sandbox_director_camera_reset(director);
      }
    } else {
      game_engine_sandbox_director_camera_reset(director);
    }
    if ((held_object_index != -1 || game_engine_sandbox_player_is_editing(player_index)) &&
      delete_button->down_frames() >= 1 && !locked) {
      long object_index = held_object_index == -1 ? game_engine_sandbox_edited_object_get(player_index) : held_object_index;
      long editor_index = get_player_who_is_editing_object(object_index);
      if (object_index != -1 && (editor_index == -1 || editor_index == player_index)) {
        object_datum *object = object_get(object_index);
        delete_button->latch();
        game_engine_sandbox_editor_action action;
        game_engine_sandbox_editor_action_clear(&action);
        action.action = 1;
        action.player_index = datum_index_to_absolute_index(player_index);
        action.owner_index = object->map_variant_index;
        simulation_queue_editor_action_insert(&action);
        c_gui_screen_widget *screen = window_manager->get_screen_by_name(_console_window, 0x10535);
        if (screen)
          screen->transition_out_with_transition_type(0, _screen_transition_normal);
      }
    }
  }
}

// @address 0x270f60 ?player_update@c_sandbox_engine@@UBAXJ@Z
void c_sandbox_engine::player_update(long player_index) const
{
  c_game_engine::player_update(player_index);
  if (player_get(player_index)->unit_index != -1) {
    update_local_player(player_index);
    if (game_engine_sandbox_player_get_user(player_index) != -1)
      game_engine_sandbox_handle_player_input(this, player_index);
  }
  bool simulated = false;
  if (game_globals->options.game_simulation == 4)
    simulated = true;
  if (simulated == 0 && game_engine_sandbox_is_player_in_edit_mode(player_index)) {
    long object_index = game_engine_sandbox_find_player_original_unit(player_index);
    if (object_index != -1) {
      s_object_header *headers = ((s_object_header *)object_header_data->data);
      long child_index = object_index;
      long root_index;
      do {
        root_index = child_index;
        child_index = ((object_datum *)headers[datum_index_to_absolute_index(child_index)].datum)->parent_object_index;
      } while (child_index != -1);
      if (root_index != object_index) {
        if (headers[datum_index_to_absolute_index(root_index)].object_type == 1)
          action_submit_event(object_index, 20);
        else
          object_detach(object_index);
      }
      object_datum *object = object_get(object_index);
      if (((unsigned char)((unit_datum *)object_get(object_index))->unit.map_editor_flags.m_storage & 1) == 0) {
        ((unit_datum *)object)->unit.map_editor_flags.m_storage |= 1;
        simulation_action_object_update(object_index, 0x40000000);
      }
      if ((object->flags & 1) == 0)
        object_set_hidden(object_index, true);
      if (((*object_header_get(object_index)).flags & 1) != 0)
        object_deactivate(object_index, false);
      if (game_engine_sandbox_physics_flag_test(object, 7))
        object_disconnect_from_physics(object_index);
    }
  }
}


static __forceinline s_sandbox_globals *game_engine_sandbox_edit_state_get()
{
  return &game_engine_globals->sandbox;
}
static __forceinline void cross_product_from_components(float ai, float aj, float ak, float bi, float bj, float bk, real_vector3d *result)
{
  float i = aj * bk - ak * bj;
  float k = ai * bj - aj * bi;
  float j = ak * bi - ai * bk;
  result->i = i;
  result->j = j;
  result->k = k;
}
// @rdata 0x820397e8
static const float game_engine_sandbox_maximum_speed = 7.0f;

// @address 0x26e638 ?manipulate_object@c_sandbox_engine@@SAXJJ@Z
void c_sandbox_engine::manipulate_object(long player_index, long object_index)
{
  object_datum *object = object_get(object_index);
  float new_speed = 0.0f;
  real_vector3d angular_velocity;
  real_point3d origin;
  real_vector3d forward;
  real_vector3d up;
  real_vector3d offset;
  real_point3d target;
  real_vector3d linear_velocity;

  angular_velocity.i = 0.0f;
  angular_velocity.j = 0.0f;
  float scale = 0.0f;
  angular_velocity.k = 0.0f;
  object_get_origin(object_index, &origin);
  ::object_get_orientation(object_index, &forward, &up);
  ::get_offset_to_bounding_sphere_center(object_index, &offset);
  target = game_engine_sandbox_edit_state_get()->edit_position[(unsigned short)player_index];
  target.x -= offset.i;
  target.y -= offset.j;
  target.z -= offset.k;
  if (game_engine_sandbox_is_object_soft_ceiling_limited(object_index))
    scenario_soft_ceilings_push_point_out(&target, game_engine_sandbox_get_object_radius(object->definition_index),
        game_engine_sandbox_edit_state_get()->edit_times[(unsigned short)player_index]);
  offset.i = target.x - origin.x;
  offset.j = target.y - origin.y;
  offset.k = target.z - origin.z;
  if (!game_engine_sandbox_is_object_fixed(object_index)) {
    float distance = magnitude3d(&offset);
    s_variant_object_datum variant_object;
    ((s_game_engine_globals *)game_engine_globals)->map_info.get_variant_object_datum(object->map_variant_index, &variant_object);
    float up_i = 0.0f, up_j = 0.0f, up_k = 0.0f;
    float variant_up_i = 0.0f, variant_up_j = 0.0f, variant_up_k = 0.0f;
    float forward_i = 0.0f, variant_forward_i = 0.0f;
    float variant_forward_j = 0.0f, forward_j = 0.0f;
    float forward_k = 0.0f, variant_forward_k = 0.0f;
    up_i = up.i;
    up_j = up.j;
    up_k = up.k;
    variant_up_i = variant_object.up.i;
    variant_up_j = variant_object.up.j;
    variant_up_k = variant_object.up.k;
    forward_i = forward.i;
    variant_forward_i = variant_object.forward.i;
    variant_forward_j = variant_object.forward.j;
    forward_j = forward.j;
    forward_k = forward.k;
    variant_forward_k = variant_object.forward.k;
    real_vector3d forward_torque;
    cross_product_from_components(variant_up_i, variant_up_j, variant_up_k, up_i, up_j, up_k, &angular_velocity);
    angular_velocity.i = -angular_velocity.i;
    angular_velocity.j = -angular_velocity.j;
    angular_velocity.k = -angular_velocity.k;
    cross_product_from_components(forward_i, forward_j, forward_k, variant_forward_i, variant_forward_j, variant_forward_k, &forward_torque);
    angular_velocity.i = angular_velocity.i + forward_torque.i;
    angular_velocity.j = angular_velocity.j + forward_torque.j;
    angular_velocity.k = angular_velocity.k + forward_torque.k;
    float magnitude = magnitude3d(&angular_velocity);
    normalize3d(&angular_velocity);
    float sign = (float)((magnitude != 0.0f) ? ((magnitude < 0.0f) ? -1 : 1) : 0);
    float alignment;
    if ((float)fabs(up_i * variant_up_i + up_j * variant_up_j + up_k * variant_up_k) >
        (float)fabs(forward_i * variant_forward_i + forward_j * variant_forward_j + forward_k * variant_forward_k))
      alignment = (float)fabs(forward_i * variant_forward_i + forward_j * variant_forward_j + forward_k * variant_forward_k);
    else
      alignment = (float)fabs(up_i * variant_up_i + up_j * variant_up_j + up_k * variant_up_k);
    const float &desired_speed = (distance + 1.0f) * distance;
    scale = (1.0f - alignment) * sign;
    scale *= 10.0f;
    angular_velocity.i = scale * angular_velocity.i;
    angular_velocity.j = scale * angular_velocity.j;
    angular_velocity.k = scale * angular_velocity.k;
    real_vector3d current_angular_velocity;
    object_get_velocities(object_index, &linear_velocity, &current_angular_velocity);
    float speed_x = linear_velocity.i;
    float speed_z = linear_velocity.k;
    float speed_y = linear_velocity.j;
    float speed = __fsqrts(speed_x * speed_x + speed_y * speed_y + speed_z * speed_z);
    new_speed = (desired_speed - speed) * 0.2f + speed;
    if (new_speed > game_engine_sandbox_maximum_speed)
      new_speed = game_engine_sandbox_maximum_speed;
    normalize3d(&offset);
    offset.i *= new_speed;
    offset.j *= new_speed;
    offset.k *= new_speed;
    object_set_velocities(object_index, &offset, &angular_velocity);
  } else {
    origin.x = offset.i * 0.3f + origin.x;
    origin.y = offset.j * 0.3f + origin.y;
    origin.z = offset.k * 0.3f + origin.z;
  }
  ::sandbox_update_object_position(false, object_index, &origin, 0, 0);
}

static __forceinline s_sandbox_globals *game_engine_sandbox_rotation_globals_get()
{
    return &((s_game_engine_globals *)game_engine_globals)->sandbox;
}
// @address 0x26e350 ?rotate_object@c_sandbox_engine@@QBAXJJMMM@Z
void c_sandbox_engine::rotate_object(long p1, long object_index, float p3, float p4, float p5) const
{
    real_vector3d axis1;
    real_vector3d axis2;
    real_vector3d axis3;
    real_vector3d forward;
    real_vector3d up;
    real_matrix4x3 matrix_temp;
    real_matrix4x3 matrix_120;
    real_matrix4x3 matrix_e0;
    real_matrix4x3 matrix_a0;
    manipulate_object(p1, object_index);
    object_get_orientation(object_index, &forward, &up);
    real_vector3d *player_axis = &game_engine_sandbox_rotation_globals_get()->player_axes[(unsigned short)p1];
    float zero = 0.0f;
    real_vector3d vertical = { zero, zero, 1.0f };
    cross_product3d(&vertical, player_axis, &axis1);
    normalize3d(&axis1);
    cross_product3d(&axis1, &game_engine_sandbox_rotation_globals_get()->player_axes[(unsigned short)p1], &axis2);
    normalize3d(&axis2);
    axis3 = game_engine_sandbox_rotation_globals_get()->player_axes[(unsigned short)p1];
    normalize3d(&axis3);
    if (!game_engine_sandbox_is_object_fixed(object_index))
    {
        real_vector3d velocity;
        real_vector3d angular_velocity;
        object_get_velocities(object_index, &velocity, &angular_velocity);
        p4 = fabsf(p4) * p4;
        p3 = fabsf(p3) * p3;
        p5 = fabsf(p5) * p5;
        scale_vector3d(&axis1, p4 * 5.0f, &axis1);
        scale_vector3d(&axis2, p3 * 5.0f, &axis2);
        scale_vector3d(&axis3, p5 * 5.0f, &axis3);
        add_vectors3d(&axis1, &axis2, &angular_velocity);
        add_vectors3d(&angular_velocity, &axis3, &angular_velocity);
        object_set_velocities(object_index, &velocity, &angular_velocity);
        return;
    }
    float z_control = p5;
    p5 = 0.1f;
    float angle1 = p4 * p5;
    float cos_angle1 = (float)cos(angle1);
    float sin_angle1 = (float)sin(angle1);
    matrix4x3_rotation_from_axis_and_angle(&matrix_e0, &axis1, sin_angle1, cos_angle1);
    float angle2 = p3 * p5;
    float cos_angle2 = (float)cos(angle2);
    float sin_angle2 = (float)sin(angle2);
    matrix4x3_rotation_from_axis_and_angle(&matrix_120, &axis2, sin_angle2, cos_angle2);
    float angle3 = z_control * p5;
    float cos_angle3 = (float)cos(angle3);
    float sin_angle3 = (float)sin(angle3);
    matrix4x3_rotation_from_axis_and_angle(&matrix_a0, &axis3, sin_angle3, cos_angle3);
    matrix4x3_compute_product(&matrix_e0, &matrix_120, &matrix_temp);
    matrix4x3_compute_product(&matrix_temp, &matrix_a0, &matrix_temp);
    matrix4x3_transform_vector(&matrix_temp, &forward, &forward);
    matrix4x3_transform_vector(&matrix_temp, &up, &up);
    sandbox_update_object_position(false, object_index, 0, &forward, &up);
}


static __forceinline s_sandbox_globals *game_engine_sandbox_probe_globals_get()
{
  return &game_engine_globals->sandbox;
}
static __forceinline void game_engine_sandbox_probe_unit_get_aiming_vector(long unit_index, real_vector3d *aiming)
{
  *aiming = ((unit_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(unit_index)].datum)->unit.aiming_vector;
}
// @address 0x26fe50 ?shoot_probe_for_player@c_sandbox_engine@@SA_NJPAJ@Z
bool c_sandbox_engine::shoot_probe_for_player(long player_index, long *hit_object)
{
  bool result = false;
  player_datum *player = player_get(player_index);
  collision_result collision;
  s_collision_test_flags flags = { 0x100009, 0x99d };
  real_point3d camera;
  const long &direction_index = player_index & 0xffff;
  unit_get_camera_position(player->unit_index, &camera);
  real_vector3d aiming;
  game_engine_sandbox_probe_unit_get_aiming_vector(player->unit_index, &aiming);
  real_vector3d vector;
  scale_vector3d(&aiming, ((s_game_engine_globals *)game_engine_globals)->sandbox.edit_times[datum_index_to_absolute_index(player_index)], &vector);
  game_engine_sandbox_probe_globals_get()->player_axes[direction_index] = aiming;
  long first_ignore_object_index = game_engine_sandbox_edited_object_get(player_index);
  long second_ignore_object_index = -1;
  if (first_ignore_object_index != -1) {
    object_datum *object = object_get(first_ignore_object_index);
    s_variant_object_datum variant_object;
    if (object->map_variant_index != -1) {
      ((s_game_engine_globals *)game_engine_globals)->map_info.get_variant_object_datum(object->map_variant_index, &variant_object);
      second_ignore_object_index = variant_object.helper_object_index;
    }
  }
  if (collision_test_vector(flags, false, &camera, &vector, first_ignore_object_index, second_ignore_object_index, -1, &collision)) {
    game_engine_sandbox_probe_globals_get()->edit_position[datum_index_to_absolute_index(player_index)] = collision.point;
    game_engine_sandbox_probe_globals_get()->probe_normals[datum_index_to_absolute_index(player_index)] = collision.plane.normal;
    result = true;
    if (hit_object)
      *hit_object = collision.object_index;
  } else {
    point_from_line3d(&camera, &vector, 1.0f, &game_engine_sandbox_probe_globals_get()->edit_position[datum_index_to_absolute_index(player_index)]);
    game_engine_sandbox_probe_globals_get()->probe_normals[datum_index_to_absolute_index(player_index)] = *global_up3d;
  }
  return result;
}


static __forceinline long game_engine_sandbox_local_player_root(long object_index)
{
  long root_index = -1;
  while (object_index != -1) {
    root_index = object_index;
    object_index = (object_get(object_index))->parent_object_index;
  }
  return root_index;
}
static __forceinline float game_engine_sandbox_local_player_distance_squared(const real_point3d *a, const real_point3d *b)
{
  float dy = 0.0f;
  float dz = 0.0f;
  float dx = 0.0f;
  dx = b->x - a->x;
  dy = b->y - a->y;
  dz = b->z - a->z;
  return dx * dx + dy * dy + dz * dz;
}

// @address 0x270680 ?update_local_player@c_sandbox_engine@@QBAXJ@Z
void c_sandbox_engine::update_local_player(long player_index) const
{
  player_datum *player = player_get(player_index);
  if (game_engine_sandbox_is_player_in_edit_mode(player_index)) {
    long object_index = -1;
    if (shoot_probe_for_player(player_index, &object_index) && object_index != -1) {
      object_datum *object = object_get(object_index);
      if (object->parent_object_index != -1) {
        long root_index = game_engine_sandbox_local_player_root(object_index);
        if (!object_early_mover_is_owner(root_index, false)) {
          object_index = root_index;
          object = object_get(object_index);
        }
      }
      bool valid = true;
      if ((unsigned short)object->map_variant_index != 0xffff) {
        s_variant_object_datum variant_object;
        s_game_engine_globals *globals = (s_game_engine_globals *)game_engine_globals;
        globals->map_info.get_variant_object_datum(object->map_variant_index, &variant_object);
        if (object_index == variant_object.helper_object_index) {
          if (variant_object.object_index == -1) {
            valid = false;
          } else {
            object_index = variant_object.object_index;
            object = object_get(object_index);
          }
        }
        if (object->parent_object_index != -1) {
          long root_index = game_engine_sandbox_local_player_root(object_index);
          if (!object_early_mover_is_owner(root_index, false))
            valid = false;
        }
        if (valid && variant_object.quota_index != -1 && globals->map_info.quotas[variant_object.quota_index].price < 0.0f)
          valid = false;
        if (variant_object.properties.boundary_shape.get() != 0 && (object_get(object_index))->multiplayer_data_offset != -1)
          ((s_game_engine_globals *)globals)->sandbox.boundary_object_index = object_index;
      } else {
        valid = false;
      }
      if ((bool)((object->damage_flags >> 2) & 1))
        valid = false;
      long type = object->type;
      bool collect = type == 6 || type == 10 || type == 1 || type == 2 || type == 3;
      if (!collect || !valid)
        object_index = -1;
    }
    game_engine_sandbox_globals_get()->handle_slots[datum_index_to_absolute_index(player_index)] = object_index;
    if (player->unit_index != -1 && !sandbox_game_is_playback()) {
      unit_datum *unit = (unit_datum *)object_get(player->unit_index);
      for (long weapon = 0; weapon < 4; weapon++) {
        if (unit->unit.weapon_object_indices[weapon] != -1)
          unit_force_drop_inventory_weapon(player->unit_index, unit->unit.weapon_object_indices[weapon], false, 0);
      }
    }
  } else {
    game_engine_sandbox_globals_get()->handle_slots[datum_index_to_absolute_index(player_index)] = -1;
  }
  long object_index = game_engine_sandbox_edited_object_get(player_index);
  if (object_index != -1) {
    object_datum *object = object_get(object_index);
    if ((unsigned short)object->map_variant_index != 0xffff) {
      s_variant_object_datum variant_object;
      s_game_engine_globals *globals = (s_game_engine_globals *)game_engine_globals;
      globals->map_info.get_variant_object_datum(object->map_variant_index, &variant_object);
      if (variant_object.properties.boundary_shape.get() != 0)
        ((s_game_engine_globals *)globals)->sandbox.boundary_object_index = object_index;
    }
    if (!sandbox_game_is_playback() && player->unit_index != -1) {
      s_damage_owner owner;
      damage_compute_damage_owner_from_object_index(player->unit_index, &owner);
      object_set_damage_owner(object_index, &owner, false);
    }
    if (player_is_on_local_machine(player_index) && (!sandbox_game_is_playback() || object->parent_object_index == -1)) {
      if (!game_engine_sandbox_is_object_fixed(object_index)) {
        real_point3d center;
        float radius;
        object_get_bounding_sphere(object_index, &center, &radius);
        real_vector3d forward;
        real_vector3d up;
        object_get_orientation(object_index, &forward, &up);
        real_point3d target = game_engine_sandbox_edit_state_get()->edit_position[datum_index_to_absolute_index(player_index)];
        float maximum_distance_squared = MAX(radius * radius * 9.0f, 1.0f);
        if (game_engine_sandbox_is_object_soft_ceiling_limited(object_index))
          scenario_soft_ceilings_push_point_out(&target, radius,
              game_engine_sandbox_edit_state_get()->edit_times[datum_index_to_absolute_index(player_index)]);
        float distance_squared = game_engine_sandbox_local_player_distance_squared(&center, &target);
        if (distance_squared > maximum_distance_squared) {
          bool reposition = true;
          real_vector3d linear_velocity;
          real_vector3d angular_velocity;
          object_get_velocities(object_index, &linear_velocity, &angular_velocity);
          float velocity_k = linear_velocity.k;
          float velocity_j = linear_velocity.j;
          float velocity_i = linear_velocity.i;
          if (__fsqrts(velocity_i * velocity_i + velocity_j * velocity_j + velocity_k * velocity_k) > 0.2f) {
            real_vector3d direction;
            vector_from_points3d(&center, &target, &direction);
            normalize3d(&direction);
            normalize3d(&linear_velocity);
            float direction_k = direction.k;
            float direction_j = direction.j;
            float direction_i = direction.i;
            if (direction_i * linear_velocity.i + direction_j * linear_velocity.j + direction_k * linear_velocity.k > 0.6f &&
                distance_squared < g_sandbox_default_edit_distance * g_sandbox_default_edit_distance * 2.0f)
              reposition = false;
          }
          if (reposition && game_engine_sandbox_find_placement_point(object->definition_index, &target, -1))
            sandbox_update_object_position(false, object_index, &target, &forward, &up);
        }
      }
      manipulate_object(player_index, object_index);
      if (player->look_override) {
        rotate_object(player_index, object_index, player->editor_rotation_x, player->editor_rotation_y, -player->editor_rotation_z);
        game_engine_sandbox_edit_state_get()->edit_times[datum_index_to_absolute_index(player_index)] += player->editor_distance * 0.3f;
        game_engine_sandbox_edit_state_get()->edit_times[datum_index_to_absolute_index(player_index)] = PIN(
            game_engine_sandbox_edit_state_get()->edit_times[datum_index_to_absolute_index(player_index)],
                2.0f, 10.0f);
        game_engine_sandbox_globals_get()->object_rotated[datum_index_to_absolute_index(player_index)] = true;
      } else if (game_engine_sandbox_globals_get()->object_rotated[datum_index_to_absolute_index(player_index)]) {
        snapshot_object_position(true, object_index);
        game_engine_sandbox_globals_get()->object_rotated[datum_index_to_absolute_index(player_index)] = false;
      }
    }
  }
  if (game_engine_sandbox_is_player_in_edit_mode(player_index) && g_sandbox_edit_effect_index != -1) {
    real_point3d point = game_engine_sandbox_edit_state_get()->edit_position[datum_index_to_absolute_index(player_index)];
    real_vector3d normal;
    negate_vector3d(&game_engine_sandbox_probe_globals_get()->player_axes[datum_index_to_absolute_index(player_index)], &normal);
    point_from_line3d(&point, &normal, 0.1f, &point);
    normalize3d(&normal);
    effect_new_unattached_from_point(g_sandbox_edit_effect_index, &point, global_up3d, &normal, 1, 1, 0, 0);
  }
  if (sandbox_game_is_playback() && game_engine_sandbox_is_player_in_edit_mode(player_index) && player_is_on_local_machine(player_index)) {
    s_game_engine_globals *globals = (s_game_engine_globals *)game_engine_globals;
    if (!globals->map_info.m_flag)
      map_variant_set_placement_marker_visibility(&globals->map_info, true);
  }
  if (!sandbox_game_is_playback())
    game_engine_sandbox_update_tracked_objects();
}

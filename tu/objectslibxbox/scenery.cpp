#include "memory/datum_index.h"
#include "tag_files/tag_groups.h"
// objectslibxbox/scenery.obj: from the records 0xdc180 0xec248 0x10ed60 0x10ee20 0x360f88 0x360fb0 0x3610a0 0x361150 0x361278
// @flags /TP /O2 /Oc
#include "objects/object_reference_types.h"
#include "memory/tls_data_arrays.h"
#include "memory/data_types.h"
#include "objects/object_header_access.h"
#include "math/matrix_math_types.h"
#include "math/real_math.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
// @address 0x10ed60 ?scenery_preprocess_root_node_matrix@@YAXJPAUreal_matrix4x3@@@Z
// @address 0x10ee20 scenery_animation_has_active_overlay
#include <ppcintrinsics.h>
#include <stdlib.h>
#include "objects/objects.h"
#include "objects/object_early_movers.h"
#include "objects/scenery.h"
#include "scenario/scenario_objects.h"
#include "math/matrix_math.h"
#include "cache/cache_files.h"
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "animations/animation_manager.h"
#include "animations/animation_search_path_iterator.h"


#include "animations/animation_id.h"

typedef char scenery_layout_33[(sizeof(s_scenery_object) == 0x170) ? 1 : -1];
typedef char scenery_layout_34[(offsetof(s_scenery_object, instance_index) == 0x168) ? 1 : -1];
typedef char scenery_layout_35[(offsetof(s_scenery_object, linked_object_index) == 0x16c) ? 1 : -1];
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z

#include "models/model_types.h"

void scenery_preprocess_root_node_matrix(long scenery_index, real_matrix4x3 *matrix)
{
  s_object_header *headers = (s_object_header *)object_header_data->data;
  s_scenery_object *scenery = (s_scenery_object *)headers[datum_index_to_absolute_index(scenery_index)].datum;
  unsigned int linked_object_index = scenery->linked_object_index;
  if (linked_object_index != 0xffffffff && object_try_and_get_and_verify_type(linked_object_index, 0xffffffff) != 0)
  {
    s_scenery_object *linked_object = (s_scenery_object *)headers[datum_index_to_absolute_index(linked_object_index)].datum;
    const real_matrix4x3 *root_node_matrix = (const real_matrix4x3 *)((char *)linked_object + linked_object->object.node_matrices.offset);
    matrix4x3_compute_product(root_node_matrix, matrix, matrix);
  }
}

extern "C" bool scenery_animation_has_active_overlay(long scenery_index)
{
  s_scenery_object *scenery = (s_scenery_object *)object_get(scenery_index);
  c_animation_manager *animation_manager = (c_animation_manager *)((char *)scenery + scenery->object.animation.offset);
  return animation_manager->m_transition_channel.m_enabled &&
         animation_manager->m_transition_channel.m_graph_index != -1 &&
         (unsigned short)animation_manager->m_transition_channel.m_animation_id.m_animation_index < 0x8000;
}

// @address 0x360f88 ?scenery_animation_clear_pending_index@@YAXJ@Z
void scenery_animation_clear_pending_index(long scenery_index)
{
  s_scenery_object *scenery = (s_scenery_object *)object_get(scenery_index);
  scenery->instance_index = -1;
}

// @address 0x360fb0 ?scenery_place@@YAXJPAUs_scenario_scenery@@@Z
void scenery_place(long scenery_index, s_scenario_scenery *scenario_scenery)
{
  short pathfinding_policy = 3;
  s_scenery_object *scenery = (s_scenery_object *)object_get_and_verify_type(scenery_index, _object_mask_scenery);
  scenery->object.flag_bits.set(_object_data_flag_22, false);
  scenery->object.flag_bits.set(_object_data_flag_23, false);
  if (scenery->object.definition_index != -1)
  {
    switch (scenario_scenery->pathfinding_policy)
    {
    case 0:
      pathfinding_policy = TAG_GET('scen', s_scenery_definition, scenery->object.definition_index)->pathfinding_policy;
      break;
    case 1:
      pathfinding_policy = 2;
      break;
    case 2:
      pathfinding_policy = 0;
      break;
    case 3:
      pathfinding_policy = 1;
      break;
    default:
      pathfinding_policy = 3;
      break;
    }
  }
  scenery->pathfinding_policy = pathfinding_policy;
  switch (pathfinding_policy)
  {
  case 1:
    scenery->object.flag_bits.set(_object_data_flag_22, true);
    // fall through
  case 0:
  case 3:
    scenery->object.flag_bits.set(_object_data_flag_23, true);
    break;
  }
  object_update_collision_culling(scenery_index);
}

// @address 0xebb20 ?object_animation_callback@@YAXPBUs_animation_event_data@@J@Z

// @rdata 0x8202af7c
static const float k_scenery_animation_frames_per_second = 30.0f;

static __forceinline c_animation_manager *scenery_get_animation_manager(long scenery_index, const object_header_block_reference *reference)
{
  s_scenery_object *scenery = (s_scenery_object *)object_get(scenery_index);
  unsigned short offset = (unsigned short)reference->offset;
  return (c_animation_manager *)((char *)scenery + (short)offset);
}

// @address 0xec248 ?scenery_update@@YA_NJ@Z
bool scenery_update(long scenery_index)
{
  bool requires_motion = false;
  s_scenery_object *scenery = (s_scenery_object *)object_get(scenery_index);
  short manager_offset = ((s_scenery_object *)object_get(scenery_index))->object.animation.offset;
  if ((unsigned short)manager_offset != 0xffff)
  {
    c_animation_manager *manager = scenery_get_animation_manager(scenery_index, &scenery->object.animation);
    ((c_animation_manager *)manager)->update_state_animation(object_animation_callback, scenery_index, 0, 0, 0);
    if (((const c_animation_manager *)manager)->valid())
    {
      const c_model_animation *animation = manager->get_state_animation();
      if (animation->frame_count > 1)
      {
        object_set_requires_motion(scenery_index);
        requires_motion = true;
      }
    }
  }
  if ((bool)(_rotl(scenery->object.physics_flags, 12) & 1) && (bool)(_rotl(scenery->object.flags, 24) & 1))
    object_in_early_mover_physics_update(scenery_index);
  return requires_motion;
}

// @address 0x361150 ?scenery_animation_get_ticks_remaining_for_scripting@@YAFJ@Z
short scenery_animation_get_ticks_remaining_for_scripting(long scenery_index)
{
  short result = 0;
  if (scenery_index != -1)
  {
      s_object_header *header = object_header_get(scenery_index);
    if (header->object_type == 6)
    {
      s_scenery_object *scenery = (s_scenery_object *)header->datum;
      unsigned short animation_manager_offset = (unsigned short)scenery->object.animation.offset;
      if (animation_manager_offset != 0xffff)
      {
        c_animation_manager *manager = (c_animation_manager *)((char *)scenery + (short)animation_manager_offset);
        if (((const c_animation_manager *)manager)->valid())
        {
          const c_model_animation *animation = 0;
          if ((unsigned short)manager->m_state_channel.m_animation_id.m_animation_index < 0x8000)
            animation = ((const c_model_animation_graph *)tag_get(0, (unsigned short)manager->m_state_channel.m_graph_index))
                ->get_animation(manager->m_state_channel.m_animation_id);
          float duration = (float)animation->frame_count * (1.0f / 30.0f);
          float authored_time = ((const c_animation_manager *)manager)->get_authored_state_time();
          result = (short)(int)((duration - authored_time) * k_scenery_animation_frames_per_second + 0.5f);
          result = (short)((result - 2 > 0) ? result - 2 : 0);
        }
      }
    }
  }
  return result;
}

static __forceinline s_scenery_object *scenery_get(long scenery_index)
{
  return (s_scenery_object *)object_get(scenery_index);
}
static __forceinline c_animation_manager *scenery_manager(long scenery_index, const object_header_block_reference *reference)
{
  s_scenery_object *scenery = scenery_get(scenery_index);
  unsigned short offset = (unsigned short)reference->offset;
  return (c_animation_manager *)((char *)scenery + (short)offset);
}
// @address 0xdc180 ?scenery_new@@YA_NJPAUobject_placement_data@@PA_N@Z
bool scenery_new(long scenery_index, object_placement_data *placement_data, bool *)
{
  s_scenery_object *scenery = (s_scenery_object *)object_get(scenery_index);
  s_scenery_definition *definition =
      (s_scenery_definition *)tag_get(0, scenery->object.definition_index);
  scenery->instance_index = -1;
  scenery->linked_object_index = -1;
  scenery->pathfinding_policy = 3;
  ::object_set_at_rest(scenery_index, true);
  scenery_animation_start_idle(scenery_index);
  scenery->flags = 0;
  if ((placement_data->flags & 64) != 0)
    scenery->flags = scenery->flags | 4;
  else
    scenery->flags = scenery->flags & 0xfffffffb;
  long linked_definition_index = definition->model.index;
  if (linked_definition_index != -1)
  {
    s_model_definition *linked_definition =
        (s_model_definition *)tag_get(0, linked_definition_index);
    if ((definition->scenery_flags & 1) != 0 || linked_definition->physics_model_index != -1)
      scenery->flags = scenery->flags | 8;
  }
  return true;
}


// @address 0x3610a0 ?scenery_animation_start_idle@@YAXJ@Z
void scenery_animation_start_idle(long scenery_index)
{
  s_scenery_object *scenery = (s_scenery_object *)object_get(scenery_index);
  scenery->flags &= ~1;
  if ((unsigned short)((s_scenery_object *)object_get(scenery_index))->object.animation.offset != 0xffff)
  {
    c_animation_manager *manager = (c_animation_manager *)((char *)scenery + scenery->object.animation.offset);
    long graph_index = manager->m_graph_tag_index;
    if (graph_index != -1 && scenery_animation_start_private(scenery_index, graph_index, 12, 0, -1, false, true))
      scenery->flags |= 1;
  }
}

class c_animation_channel;


static __forceinline bool scenery_channel_valid(const c_animation_channel *channel)
{
  bool valid = channel->m_enabled && channel->m_graph_index != -1 && (unsigned short)channel->m_animation_id.m_animation_index < 0x8000;
  return valid;
}

static __forceinline long scenery_get_graph_tag_index(c_animation_manager *manager)
{
  long graph_tag_index = manager->m_graph_tag_index;
  return graph_tag_index;
}
static __forceinline const c_model_animation_graph *scenery_get_graph(c_animation_manager *manager)
{
  long graph_tag_index = scenery_get_graph_tag_index(manager);
  // match: 0x361278: preserve the frame-time temporary operand rank through the graph pointer reference.
  const c_model_animation_graph *const &graph = (const c_model_animation_graph *)tag_get(0, graph_tag_index);
  return graph;
}
static __forceinline bool scenery_animation_id_valid(const c_animation_id *animation)
{
  bool valid = animation->m_animation_index >= 0;
  return valid;
}
static __forceinline void scenery_set_state_position(c_animation_manager *manager, short frame_index)
{
  const c_animation_channel *channel = &manager->m_state_channel;
  signed char active;
  long graph_index;
  unsigned short animation_index;
  bool valid;
  short signed_frame = frame_index;
  long frame = signed_frame;
  double wide_frame = (double)frame;
  float real_frame = (float)wide_frame;
  float seconds_per_tick = 1.0f / 30.0f;
  const float &authored_time = real_frame * seconds_per_tick;
  valid = (active = channel->m_enabled) && (graph_index = channel->m_graph_index) != -1 &&
          (animation_index = (unsigned short)channel->m_animation_id.m_animation_index) < 0x8000;
  if (valid)
    ((c_animation_channel *)manager)->set_frame_position(authored_time * k_scenery_animation_frames_per_second);
}

// @address 0x361278 ?scenery_animation_start_private@@YA_NJJJFJ_N0@Z
bool scenery_animation_start_private(
    long scenery_index, long graph_index, long animation_name, short frame_index, long linked_object_index, bool repeat, bool interpolate)
{
  bool success = false;
  if (scenery_index != -1 && graph_index != -1 && animation_name != 0)
  {
    s_scenery_object *scenery = (s_scenery_object *)object_get(scenery_index);
    unsigned long animation_manager_offset = (unsigned short)scenery->object.animation.offset;
    if (animation_manager_offset != 0xffff)
    {
      c_animation_manager *manager =
          (c_animation_manager *)((char *)scenery + (short)(unsigned short)scenery_get(scenery_index)->object.animation.offset);
      const s_scenery_definition *definition =
          (const s_scenery_definition *)tag_get(0, scenery->object.definition_index);
      if (scenery_get_graph_tag_index(manager) == graph_index || ((c_animation_manager *)manager)->reset_graph(graph_index, definition->model.index, true))
      {
        c_animation_id animation = scenery_get_graph(manager)->find_animation(animation_name);
        if (scenery_animation_id_valid(&animation))
        {
          long goal_flags = repeat ? 0x4001 : 31;
          if (interpolate)
            goal_flags |= 2;
          else
            goal_flags &= ~2;
          if (((c_animation_manager *)manager)->set_custom_goal(animation, goal_flags))
          {
            success = true;
            scenery_set_state_position(manager, frame_index);
            scenery->flags &= ~1;
            object_wake(scenery_index);
            scenery->linked_object_index = linked_object_index;
            if (linked_object_index != -1)
            {
              if (scenery->object.parent_object_index != -1)
                object_detach(scenery_index);
              if ((bool)(_rotl(scenery->object.physics_flags, 25) & 1))
                object_disconnect_from_physics(scenery_index);
              ::object_set_position_direct(scenery_index, global_origin3d,
                  global_forward3d, global_up3d, 0, false);
              object_wake(scenery_index);
              object_set_in_limbo(scenery_index, false);
            }
            ::object_set_at_rest(scenery_index, true);
            object_compute_node_matrices(scenery_index);
            object_activate(scenery_index);
          }
        }
      }
    }
  }
  return success;
}

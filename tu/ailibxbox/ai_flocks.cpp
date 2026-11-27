// ailibxbox/ai_flocks.obj: from the records 0xd30c8 0xd3218 0xd3370 0xd3430 0xd35e0 0xd36e0 0xd3998 0x55aec0 0x55afe8 0x55b108 0x55b280 0x55b5e0 0x55bae8 0x55bb98 0x55bd38 0x55be80 0x55c048 0x55c398 0x55c4c8 0x55c5b8 0x55cd08 0x55dd08 0x55de38 0x55df98 0x55e0e8 0x55e1b8 0x55e2f8 0x55e398
// @address 0x11df734 g_flocks_data
#include "math/real_math_constants.h"
#include "units/biped_globals.h"
#include "ai/ai_tls_data.h"
// @address 0x2251c k_real_zero_storage
// @flags /TP /O2 /Oc
#include "objects/object_reference_types.h"
#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
#include "tag_files/tag_groups.h"
#include "objects/object_header.h"
#include <stdlib.h>
#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "math/point_math.h"
#include "memory/data_types.h"
#include "game/game_time_types.h"
#include "objects/object_datum.h"
#include "ai/actor_globals.h"
#include <float.h>
#include <math.h>
#include "cache/restricted_memory_regions.h"
#include "game/game.h"
#include "game/player_types.h"
#include "objects/objects.h"
#include "ai/ai.h"
#include "ai/ai_flocks.h"
#include "math/real_constant_pool.h"
#include "ai/ai_globals.h"
#include "ai/props.h"
#include "ai/flock_runtime_types.h"
#include "creatures/creatures.h"
#include "creatures/creature_definitions.h"
#include "objects/widgets/widgets.h"
#include "scenario/scenario_trigger_volumes.h"
#include "memory/data.h"
#include "math/random_math_range.h"
#include "cache/restricted_memory_registration.h"
#include "physics/collision_types.h"
#include "scenario/scenario_trigger_volume.h"
#include "scenario/scenario.h"
#include "physics/collision_test_vector.h"
#include "memory/tls_data_arrays.h"
#include "scenario/scenario_tag.h"


typedef char boid_state_size[(sizeof(boid_state) == 0x44) ? 1 : -1];
typedef char boid_state_update_timer_offset[(offsetof(boid_state, update_timer) == 0x12) ? 1 : -1];
typedef char boid_state_target_offset[(offsetof(boid_state, target_object_index) == 0x20) ? 1 : -1];
typedef char boid_state_perlin_offset[(offsetof(boid_state, perlin) == 0x28) ? 1 : -1];
#include "creatures/creature_datum.h"
// one flock (0x4c bytes): the boid list head and the bounds of its root boids






struct boid_perch_entry_b;

#include "ai/flock_instance_types.h"






#include "objects/object_lifecycle.h"
#include <string.h>
#include "tag_files/tag_types.h"
#include "scenario/scenario_light_types.h"
#include "objects/object_placement.h"
#include "physics/collision_test_flags.h"
#include "cseries/cseries.h"

__declspec(thread) s_data_array *g_flocks_data;

namespace {
struct flock_iterator {
  flock_datum *datum;
  s_data_iterator iterator;
  long current_index;
};
}

struct boid_perch_entry_b {
  float x;
  float y;
  float z;
  float radius;
};

static __forceinline boid_state *flock_boid_link_block(creature_datum *creature)
{
  boid_state *result = 0;
  if (creature->creature.link_type == 1)
    result = (boid_state *)((char *)creature + creature->creature.creature_reference.offset);
  return result;
}
static __forceinline creature_datum *flock_boid_iterator_advance(flock_boid_iterator *iterator)
{
  creature_datum *creature = 0;
  iterator->current_object_index = iterator->next_object_index;
  if (iterator->next_object_index != -1) {
    creature = (creature_datum *)object_get_and_verify_type(iterator->next_object_index, NONE);
    boid_state *link = flock_boid_link_block(creature);
    iterator->next_object_index = link ? link->next_object_index : -1;
  }
  return creature;
}

// @address 0xd35e0 ?flock_boid_iterator_next@@YAPAUcreature_datum@@PAUflock_boid_iterator@@@Z
creature_datum *flock_boid_iterator_next(flock_boid_iterator *iterator)
{
  creature_datum *creature = 0;
  iterator->current_object_index = iterator->next_object_index;
  if (iterator->current_object_index != -1) {
    creature = (creature_datum *)object_get_and_verify_type(iterator->current_object_index, NONE);
    boid_state *link = flock_boid_link_block(creature);
    iterator->next_object_index = link ? link->next_object_index : -1;
  }
  return creature;
}

static __forceinline long ai_flocks_seconds_to_ticks_round(float seconds)
{
  return (long)(seconds * (float)tls_game_time_globals()->ticks_per_second + 0.5f);
}
template<class T> static __forceinline const T &ai_flocks_max(const T &a, const T &b)
{
  return a > b ? a : b;
}
static __forceinline float flock_cosine(float angle)
{
  return (float)cos((double)angle);
}
// @address 0xd36e0 ?perlin_state_update@@YAXPAUs_perlin_state@@MMM_NPATreal_point3d@@@Z
void perlin_state_update(s_perlin_state *state, float magnitude, float minimum_period, float maximum_period, bool planar, real_point3d *offset)
{
  ++state->elapsed;
  if (state->elapsed > state->period) {
    state->previous = state->target;
    state->target.x = _real_random_range(tls_random_seed(), -magnitude, magnitude);
    state->target.y = _real_random_range(tls_random_seed(), -magnitude, magnitude);
    if (planar)
      state->target.z = 0.0f;
    else
      state->target.z = _real_random_range(tls_random_seed(), -magnitude, magnitude);
    float period = _real_random_range(tls_random_seed(), minimum_period, maximum_period);
    state->period = (short)ai_flocks_max(1L, ai_flocks_seconds_to_ticks_round(period));
    state->elapsed = 0;
  }
  float blend;
  if (state->period > 0) {
    blend = flock_cosine(3.1415927f * ((float)state->elapsed / (float)state->period));
    blend *= 0.5f;
    blend += 0.5f;
  } else
    blend = 0.0f;
  float delta_x = state->target.x - state->previous.x;
  float delta_y = state->target.y - state->previous.y;
  offset->x = state->previous.x + delta_x * (1.0f - blend);
  offset->y = state->previous.y + delta_y * (1.0f - blend);
  if (planar)
    offset->z = 0.0f;
  else {
    float delta_z = state->target.z - state->previous.z;
    offset->z = state->previous.z + delta_z * (1.0f - blend);
  }
}

// @address 0x55bae8 ?flock_remove_creature@@YA_NJJ@Z
bool flock_remove_creature(long owner_index, long object_index)
{
  flock_datum *owner = datum_get<flock_datum>(g_flocks_data, owner_index);
  long *reference = &owner->first_boid_index;
  bool removed = false;
  while (*reference != -1) {
    long current_index = *reference;
    creature_datum *object = (creature_datum *)((s_object_header *)tls_object_header_data()->data)[(unsigned short)current_index].datum;
    boid_state *link;
    if (object->creature.link_type == 1)
      link = (boid_state *)((char *)object + object->creature.creature_reference.offset);
    else
      link = 0;
    if (current_index == object_index) {
      *reference = link->next_object_index;
      link->flock_index = -1;
      owner->boid_count--;
      removed = true;
      break;
    }
    reference = &link->next_object_index;
  }
  return removed;
}

// @address 0x55bd38 ?flock_update_bounding_box@@YAXJ@Z
void flock_update_bounding_box(long index)
{
  flock_datum *f = datum_get<flock_datum>(g_flocks_data, index);
  real_rectangle3d bounds = {FLT_MAX, -FLT_MAX, FLT_MAX, -FLT_MAX, FLT_MAX, -FLT_MAX};
  flock_boid_iterator it;
  it.current_object_index = -1;
  it.next_object_index = f->first_boid_index;
  creature_datum *c;
  while ((c = flock_boid_iterator_advance(&it)) != 0) {
    object_datum *boid = (object_datum *)c;
    if (boid->parent_object_index == -1) {
      real_point3d *p = &boid->position;
      bounds.x0 = bounds.x0 > p->x ? p->x : bounds.x0;
      bounds.x1 = bounds.x1 > p->x ? bounds.x1 : p->x;
      bounds.y0 = bounds.y0 > p->y ? p->y : bounds.y0;
      bounds.y1 = bounds.y1 > p->y ? bounds.y1 : p->y;
      bounds.z0 = bounds.z0 > p->z ? p->z : bounds.z0;
      bounds.z1 = bounds.z1 > p->z ? bounds.z1 : p->z;
    }
  }
  f->bounds = bounds;
}



// @address 0x84740 ?data_iterator_next@@YAPAXPAUs_data_iterator@@@Z

namespace {
static __forceinline void flock_iterator_new(flock_iterator *iterator)
{
  if (tls_actor_globals()->enabled) {
    iterator->iterator.absolute_index = -1;
    iterator->iterator.index = -1;
    iterator->iterator.data = (s_data_array *)g_flocks_data;
  }
}
static __forceinline flock_datum *flock_iterator_next(flock_iterator *iterator)
{
  flock_datum *result = 0;
  if (tls_actor_globals()->enabled) {
    iterator->datum = (flock_datum *)data_iterator_next(&iterator->iterator);
    if (iterator->datum)
      result = iterator->datum;
    iterator->current_index = iterator->iterator.index;
  }
  return result;
}
}

// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0xfc560 ?object_delete@@YAXJ@Z
static __forceinline void ai_flocks_data_make_invalid(s_data_array *data)
{
  data->valid = 0;
}


// @address 0xd3370 flocks_dispose_from_old_structure_bsp
extern "C" void flocks_dispose_from_old_structure_bsp(long flock_index)
{
  flock_boid_iterator boids;
  boids.current_object_index = -1;
  boids.next_object_index = datum_get<flock_datum>(g_flocks_data, flock_index)->first_boid_index;
  creature_datum *creature;
  while ((creature = flock_boid_iterator_advance(&boids)) != 0) {
    boid_state *link = flock_boid_link_block(creature);
    if (link)
      link->flock_index = -1;
    object_delete(boids.current_object_index);
  }
  ::datum_delete(g_flocks_data, flock_index);
}

// @address 0xd3430 flocks_dispose_from_bsp_mask
extern "C" void flocks_dispose_from_bsp_mask(unsigned long mask)
{
  scenario *globals = (scenario *)global_scenario;
  flock_iterator iterator;
  flock_iterator_new(&iterator);
  flock_datum *flock;
  while ((flock = flock_iterator_next(&iterator)) != 0) {
    bool dispose = false;
    short bsp_index = flock->definition_index;
    if (bsp_index >= 0 && bsp_index < globals->flocks.count) {
      short value = ((s_flock_instance *)globals->flocks.address)[bsp_index].bsp_index;
      if (value >= 0 && value < globals->structure_bsps.count) {
        if ((mask & (1 << value)) != 0)
          dispose = true;
      } else
        dispose = true;
    } else
      dispose = true;
    if (!dispose)
      continue;
    flock_boid_iterator boids;
    boids.current_object_index = -1;
    boids.next_object_index = datum_get<flock_datum>(g_flocks_data, iterator.current_index)->first_boid_index;
    creature_datum *creature;
    while ((creature = flock_boid_iterator_advance(&boids)) != 0) {
      boid_state *link = flock_boid_link_block(creature);
      if (link) {
        flock->first_boid_index = link->next_object_index;
        link->flock_index = -1;
      }
      objects_purge_deleted_object(boids.current_object_index);
    }
    ::datum_delete(g_flocks_data, iterator.current_index);
  }
}

namespace { const float k_flocks_danger = 3.0f; const float k_flocks_zero = 0.0f; const float k_flocks_offset = 0.2f; }
static __forceinline float ai_flocks_axis_distance(float value, const float *bounds, float zero)
{
  float distance;
  if (!(value > bounds[1])) {
    if (!(value >= bounds[0]))
      distance = bounds[0] - value;
    else
      distance = zero;
  } else {
    distance = value - bounds[1];
  }
  return distance;
}

// @address 0xd3218 ?flocks_handle_weapon_detonation@@YAXPATreal_point3d@@@Z
void flocks_handle_weapon_detonation(real_point3d *point)
{
  flock_iterator iterator;
  flock_iterator_new(&iterator);
  flock_datum *flock;
  while ((flock = flock_iterator_next(&iterator)) != 0) {
    if (flock->active) {
      float danger_radius = 3.0f;
      float tag_radius = ((boid_flock_definition *)tag_get(NONE, flock->tag_index))->danger_radius;
      if (tag_radius > 0.0f)
        danger_radius = tag_radius;
      if (flock_volume_test_point(iterator.current_index, point, danger_radius + 1.0f)) {
        flock->death_timer = (short)ai_flocks_seconds_to_ticks_round(3.0f);
        flock->last_death_position = *point;
        flock->death_scale = 2.0f;
      }
    }
  }
}

// @address 0x55be80 ?flock_update_danger@@YAXJ@Z
void flock_update_danger(long flock_index)
{
  short index = 0;
  flock_datum *flock = datum_get<flock_datum>(g_flocks_data, flock_index);
  float danger_radius = k_flocks_danger;
  float zero = k_flocks_zero;
  float tag_radius = ((boid_flock_definition *)tag_get(NONE, flock->tag_index))->danger_radius;
  if (!(tag_radius <= zero))
    danger_radius = tag_radius + k_flocks_offset;
  c_data_iterator<player_datum> players;
  players.m_iterator.absolute_index = -1;
  flock->danger_mask = 0;
  players.m_iterator.index = -1;
  players.m_iterator.data = tls_player_data();
  while (players.next()) {
    player_datum *player = players.get_datum();
    if (player->unit_index != -1) {
      real_point3d position;
      object_get_origin(player->unit_index, &position);
      float dx = ai_flocks_axis_distance(position.x, &flock->bounds.x0, zero);
      float dy = ai_flocks_axis_distance(position.y, &flock->bounds.y0, zero);
      float dz = ai_flocks_axis_distance(position.z, &flock->bounds.z0, zero);
      bool inside = true;
      if (!(dx * dx + (dz * dz + dy * dy) < danger_radius * danger_radius))
        inside = false;
      if (inside)
        flock->danger_mask |= 1 << index;
    }
    index = (short)(index + 1);
  }
  short timer = flock->death_timer;
  if (timer > 0)
    flock->death_timer = timer - 1;
}

// @address 0x55dd08 flock_handle_boid_death
extern "C" void flock_handle_boid_death(long creature_index)
{
  creature_datum *creature = (creature_datum *)object_get_and_verify_type(creature_index, NONE);
  boid_state *link = flock_boid_link_block(creature);
  if (link && link->flock_index != -1) {
    long flock_index = link->flock_index;
    real_point3d position;
    object_get_origin(creature_index, &position);
    if (flock_remove_creature(flock_index, creature_index)) {
      flock_datum *flock = datum_get<flock_datum>(g_flocks_data, flock_index);
      if (flock->death_timer <= 0) {
        flock->death_timer = (short)ai_flocks_seconds_to_ticks_round(3.0f);
        flock->last_death_position = position;
        flock->death_scale = 1.0f;
      }
    }
  }
}

// @address 0x55de38 ?flock_handle_creature_delete@@YAXJ@Z
void flock_handle_creature_delete(long creature_index)
{
  creature_datum *creature = (creature_datum *)object_get_and_verify_type(creature_index, NONE);
  boid_state *link = flock_boid_link_block(creature);
  if (link && link->flock_index != -1)
    flock_remove_creature(link->flock_index, creature_index);
  flock_iterator iterator;
  flock_iterator_new(&iterator);
  flock_datum *flock;
  while ((flock = flock_iterator_next(&iterator)) != 0) {
    flock_boid_iterator boids;
    boids.current_object_index = -1;
    boids.next_object_index = datum_get<flock_datum>(g_flocks_data, iterator.current_index)->first_boid_index;
    creature_datum *boid;
    while ((boid = flock_boid_iterator_advance(&boids)) != 0) {
      boid_state *boid_link = flock_boid_link_block(boid);
      if (boid_link && boid_link->target_object_index == creature_index)
        boid_link->target_object_index = -1;
    }
  }
}

// @address 0x55aec0 flocks_dispose_from_old_map
extern "C" void flocks_dispose_from_old_map()
{
  flock_iterator iterator;
  flock_iterator_new(&iterator);
  flock_datum *flock;
  while ((flock = flock_iterator_next(&iterator)) != 0) {
    flock_boid_iterator boids;
    boids.current_object_index = -1;
    boids.next_object_index = datum_get<flock_datum>(g_flocks_data, iterator.current_index)->first_boid_index;
    creature_datum *creature;
    while ((creature = flock_boid_iterator_advance(&boids)) != 0) {
      boid_state *link = flock_boid_link_block(creature);
      if (link)
        link->flock_index = -1;
      object_delete(boids.current_object_index);
    }
  }
  ai_flocks_data_make_invalid(g_flocks_data);
}


// @address 0xac2e8 ?trigger_volume_get_matrix@@YA_NPBUscenario_trigger_volume@@PAUreal_matrix4x3@@@Z
namespace {
}

// @address 0x55df98 ?flock_volume_test_point@@YA_NJPATreal_point3d@@M@Z
bool flock_volume_test_point(long flock_index, real_point3d *point, float radius)
{
  flock_datum *flock = datum_get<flock_datum>(g_flocks_data, flock_index);
  scenario *scenario = global_scenario;
  s_flock_instance *definition = &((s_flock_instance *)scenario->flocks.address)[flock->definition_index];
  bool result = false;
  real_matrix4x3 matrix;
  if (definition->trigger_volume_index >= 0 && definition->trigger_volume_index < scenario->trigger_volumes.count &&
      trigger_volume_get_matrix(&((scenario_trigger_volume *)scenario->trigger_volumes.address)[definition->trigger_volume_index], &matrix)) {
    const real_point3d *extents = &((scenario_trigger_volume *)scenario->trigger_volumes.address)[definition->trigger_volume_index].extents;
    real_vector3d offset;
    vector_from_points3d(&matrix.position, point, &offset);
    result = true;
    for (short axis = 0; axis < 3; ++axis) {
      float distance = dot_product3d(&matrix.rotation.basis[axis], &offset);
      if (distance < -radius || distance > extents->n[axis] + radius) {
        result = false;
        break;
      }
    }
  }
  return result;
}

// @address 0x55e0e8 ?flock_get_by_name@@YAJJ@Z
long flock_get_by_name(long name)
{
  scenario *scenario = (struct scenario *)global_scenario;
  long result = -1;
  flock_iterator iterator;
  flock_iterator_new(&iterator);
  flock_datum *flock;
  while ((flock = flock_iterator_next(&iterator)) != 0) {
    if (flock->definition_index >= 0 && flock->definition_index < scenario->flocks.count) {
      s_flock_instance *definition = &((s_flock_instance *)scenario->flocks.address)[flock->definition_index];
      if (definition->name == name) {
        result = iterator.current_index;
        break;
      }
    }
  }
  return result;
}

// @address 0xc1510 ?game_test_cluster_activation@@YA_NPBUs_cluster_reference@@@Z
// @address 0x55bb98 ?flock_update_activation_status@@YA_NJ@Z
bool flock_update_activation_status(long index)
{
  flock_datum *f = datum_get<flock_datum>(g_flocks_data, index);
  scenario *s = (scenario *)global_scenario;
  bool result = false;
  flock_boid_iterator it;
  it.current_object_index = -1;
  it.next_object_index = datum_get<flock_datum>(g_flocks_data, index)->first_boid_index;
  creature_datum *boid;
  while ((boid = flock_boid_iterator_advance(&it)) != 0) {
    object_header_datum *h = object_header_get(it.current_object_index);
    boid_state *ai = boid->creature.link_type == 1 ? (boid_state *)((char *)boid + boid->creature.creature_reference.offset) : 0;
    bool active = (bool)(h->flags & 1);
    result |= active;
    if (ai) ai->active = active;
  }
  if (f->check_sources) {
    f->source_mask = 0;
    if (f->definition_index >= 0 && f->definition_index < s->flocks.count) {
      s_flock_instance *def = &((s_flock_instance *)s->flocks.address)[f->definition_index];
      for (short source_index = 0; source_index < def->source_count; ++source_index) {
        ai_flocks_spawn_source *src = &def->sources[source_index];
        if (game_test_cluster_activation(&src->cluster)) {
          f->source_mask |= 1 << source_index;
          result = true;
        }
      }
    }
  }
  f->active = result;
  return result;
}

// @address 0x55e1b8 flock_reset_boid_update_timers
extern "C" void flock_reset_boid_update_timers(long flock_index)
{
  flock_boid_iterator iterator;
  iterator.current_object_index = -1;
  iterator.next_object_index = datum_get<flock_datum>(g_flocks_data, flock_index)->first_boid_index;
  creature_datum *creature;
  while ((creature = flock_boid_iterator_advance(&iterator)) != 0) {
    boid_state *link = creature->creature.link_type == 1 ? (boid_state *)((char *)creature + creature->creature.creature_reference.offset) : 0;
    if (link->needs_update && link->update_timer == 0) {
      link->update_timer = (short)ai_flocks_seconds_to_ticks_round(
          _real_random_range(tls_random_seed(), 0.0f, 1.0f));
    }
  }
}

// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0x1ec830 ?data_set_new_base_address@@YAXPAPAUs_data_array@@PAU1@@Z
// @address 0x55aea8 ?__tls_set_g_widget_data_allocator@@YAXPAX@Z

typedef t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &__tls_set_g_widget_data_allocator> t_ai_flocks_allocation_manager;

// @address 0x55e2f8 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_widget_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template <>
void *t_ai_flocks_allocation_manager::allocate(unsigned int size, const char *name)
{
  long member_index = restricted_memory_register_member(&g_restricted_regions[1], name, "", size, 0, __tls_set_g_widget_data_allocator, 0, 0);
  unsigned int offset = restricted_region_get_member_offset(1, member_index);
  data_set_new_base_address(&g_flocks_data, (s_data_array *)(restricted_region_get_base_address(1) + offset));
  m_member_index = member_index;
  m_thread_id = get_current_thread_index();
  return restricted_region_get_base_address(1) + restricted_region_get_member_offset(1, member_index);
}

// @address 0x55e398 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_widget_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template <>
void t_ai_flocks_allocation_manager::deallocate(void *memory)
{
    g_flocks_data = 0;
    restricted_region_free_member(1, m_member_index);
}

// @address 0x3aa90 k_real_math_origin3d

static __forceinline unsigned long ai_flocks_random_next(unsigned long *seed)
{
  random_seed_next(seed);
  return *seed >> 16;
}
static __forceinline short ai_flocks_short_random_range(short minimum, short maximum)
{
  return minimum + (short)((ai_flocks_random_next(tls_random_seed()) * (maximum - minimum)) >> 16);
}

// @address 0x55afe8 ?boid_state_initialize@@YAXJPAUboid_state@@FM@Z
void boid_state_initialize(long flock_index, boid_state *state, short p2, float p3)
{
  state->w14 = p2;
  state->f24 = p3;
  const real_point3d *zero = &k_real_math_origin3d;
  state->flock_index = -1;
  state->w16 = -1;
  state->dead = 0;
  state->b9 = 0;
  state->update_count = 0;
  state->needs_update = 0;
  state->update_timer = 0;
  state->w36 = -1;
  state->l28 = -1;
  state->target_object_index = -1;
  state->active = 1;
  s_perlin_state *tail = &state->perlin;
  tail->elapsed = 0;
  tail->period = -1;
  tail->previous = *zero;
  long definition_index = datum_get<flock_datum>(g_flocks_data, flock_index)->definition_index;
  s_flock_instance *definition = (s_flock_instance *)&((s_flock_instance *)global_scenario->flocks.address)[definition_index];
  if (definition->random_count > 0) {
    state->w16 = ai_flocks_short_random_range(0, (short)definition->random_count);
  }
  unsigned short mflags = definition->flags;
  if ((((unsigned short)mflags >> 3) & 1) != 0)
    state->needs_update = 1;
}


// The guard goto keeps the inner source-count check before accumulating weights.
// The weighted-pick goto keeps the 1 used by (1 << i) hoisted above the flag dispatch.
// @address 0x55b280 flock_pick_random_source
extern "C" short flock_pick_random_source(flock_datum *a, s_flock_instance *b)
{
  short result = -1;
  long count = b->source_count;
  if (0 >= count) goto return_result;
  if ((short)a->source_mask > 0) {
    if (((unsigned short)b->flags & (1 << 4)) != 0) {
      for (short round = 0; round < 2; round = (short)(round + 1)) {
        short fresh = 0;
        for (short i = 0; i < b->source_count; i = (short)(i + 1)) {
          if (((1 << i) & a->source_mask) != 0 && (a->used_source_mask & (1 << i)) == 0)
            fresh = (short)(fresh + 1);
        }
        if (fresh > 0) {
          unsigned long *seed = tls_random_seed();
          random_seed_next(seed);
          short pick = (unsigned short)(((unsigned)(*seed >> 16) * fresh) >> 16);
          short seen = 0;
          for (short i = 0; i < b->source_count; i = (short)(i + 1)) {
            if (((1 << i) & a->source_mask) != 0 && (a->used_source_mask & (1 << i)) == 0) {
              if (pick == seen) {
                result = i;
                break;
              }
              seen = (short)(seen + 1);
            }
          }
        }
        if (result != -1)
          break;
        a->used_source_mask = 0;
      }
    } else {
      short distinct = 0;
      float zero = k_real_zero_storage;
      float sum = zero;
      if (b->source_count > 0) {
        short i = 0;
        do {
          if (((1 << i) & a->source_mask) != 0) {
            distinct = (short)(distinct + 1);
            sum = b->sources[i].weight + sum;
          }
          i = (short)(i + 1);
        } while (i < b->source_count);
        if (!(sum <= zero)) {
          unsigned long *seed = tls_random_seed();
          random_seed_next(seed);
          float worked = (float)(*seed >> 16);
          float fraction = worked * 1.5259022e-05f;
          float pick = fraction * sum;
          float acc = zero;
          for (short i = 0; i < b->source_count; i = (short)(i + 1)) {
            if (((1 << i) & a->source_mask) != 0) {
              acc = b->sources[i].weight + acc;
              if (pick <= acc) {
                result = i;
                break;
              }
            }
          }
          goto return_result;
        }
      }
      if (distinct > 0) {
        unsigned long *seed = tls_random_seed();
        random_seed_next(seed);
        short pick = (unsigned short)(((unsigned)(*seed >> 16) * distinct) >> 16);
        short seen = 0;
        for (short i = 0; i < b->source_count; i = (short)(i + 1)) {
          if (((1 << i) & a->source_mask) != 0) {
            if (pick == seen) {
              result = i;
              break;
            }
            seen = (short)(seen + 1);
          }
        }
      }
    }
  }
return_result:
  return result;
}



// @address 0x55c4c8 flock_set_active
extern "C" void flock_set_active(flock_datum *flock, const s_flock_instance *instance, bool active)
{
  if (active) {
    if (instance->flags & (1 << 4))
      flock->spawn_source = -1;
    else
      flock->spawn_source = flock_pick_random_source((flock_datum *)flock, (s_flock_instance *)instance);
    flock->target_boid_count = -1;
    flock->spawning = active;
  } else {
    const short &minimum = instance->minimum_boid_count;
    if (minimum > 0)
      flock->target_boid_count = ai_flocks_short_random_range(minimum,
          (short)((instance->maximum_boid_count + minimum) / 2));
    else
      flock->target_boid_count = 0;
    flock->spawning = active;
    flock->spawn_source = -1;
  }
}

// @address 0x8a1ecc ?g_active_structure_bsp_mask@@3KA

static __forceinline bool ai_flocks_creature_planar(const creature_definition *definition)
{
  return (definition->physics.flags & (1 << 4)) != 0;
}
extern "C" {
}


static __forceinline float ai_flocks_spawn_random_range(unsigned long *seed, float minimum, float maximum)
{
  random_seed_next(seed);
  float value = minimum + (maximum - minimum) * (1.5259022e-5f * (float)(*seed >> 16));
  return value;
}

static __forceinline bool ai_flocks_spawn_add_creature(long flock_index, long object_index)
{
  bool result = false;
  flock_datum *flock = datum_get<flock_datum>(g_flocks_data, flock_index);
  creature_datum *creature = (creature_datum *)object_get_and_verify_type(object_index, NONE);
  boid_state *link = flock_boid_link_block(creature);
  if (link) {
    link->next_object_index = flock->first_boid_index;
    link->flock_index = flock_index;
    flock->first_boid_index = object_index;
    flock->boid_count++;
    result = true;
  }
  return result;
}

// @address 0x55b5e0 flock_spawn_boid
extern "C" long flock_spawn_boid(long flock_index, unsigned short source_index)
{
  scenario *scenario = global_scenario;
  flock_datum *flock = datum_get<flock_datum>(g_flocks_data, flock_index);
  long object_index = -1;
  if (flock->definition_index >= 0 && flock->definition_index < scenario->flocks.count) {
    s_flock_instance *instance = (s_flock_instance *)&((s_flock_instance *)scenario->flocks.address)[flock->definition_index];
    object_placement_data placement;
    if (instance->creature >= 0 && instance->creature < scenario->creature_palette.count) {
      if ((short)source_index == -1)
        source_index = flock_pick_random_source((flock_datum *)flock, (s_flock_instance *)instance);
      if ((short)source_index >= 0 && (short)source_index < instance->source_count) {
        scenario_object_palette_entry *creature = &((scenario_object_palette_entry *)scenario->creature_palette.address)[instance->creature];
        if (creature->object.index != -1) {
          creature_definition *definition =
              (creature_definition *)tag_get(NONE, creature->object.index);
          ai_flocks_spawn_source *source = &instance->sources[(short)source_index];
          // match: 0x55b5e0: identity rotate keeps the source +0x0c/+0x18 float loads in fr13/fr0 order for placement.position.y
          bool planar = ai_flocks_creature_planar(definition) || ((long)_rotl(_rotl(source->flags, 0), 0) & 1) != 0;
          object_placement_data_new(&placement, creature->object.index, -1, 0);
          placement.destroyed_constraints = 1;
          placement.ai_state_size = 68;
          placement.ai_state_alignment_bits = 2;
          if (instance->minimum_scale > 0.0f && instance->maximum_scale > 0.0f)
            placement.scale = ai_flocks_spawn_random_range(tls_random_seed(), instance->minimum_scale, instance->maximum_scale);
          placement.position.x = source->position.x + ai_flocks_spawn_random_range(tls_random_seed(), -source->radius, source->radius);
          placement.position.y = source->position.y + ai_flocks_spawn_random_range(tls_random_seed(), -source->radius, source->radius);
          if (planar) {
            collision_result collision;
            placement.position.z = source->position.z + source->radius;
            real_vector3d vector;
            scale_vector3d(global_down3d, 2.0f * source->radius, &vector);
            if (collision_test_vector(k_flying_vehicle_collision_flags, false, &placement.position, &vector, -1, -1, -1, &collision))
              placement.position = collision.point;
            else
              placement.position.z = source->position.z;
          } else {
            placement.position.z = source->position.z + ai_flocks_spawn_random_range(tls_random_seed(), -source->radius, source->radius);
          }
          float orientation = source->orientation == 0.0f
              ? ai_flocks_spawn_random_range(tls_random_seed(), 0.0f, 6.2831855f)
              : source->orientation;
          placement.forward = *global_forward3d;
          rotate_vector_about_axis(&placement.forward, global_up3d, (float)sin((double)orientation), flock_cosine(orientation));
          object_index = object_new(&placement);
          if (object_index != -1) {
            creature_datum *object = (creature_datum *)object_get_and_verify_type(object_index, NONE);
            boid_state *state = (boid_state *)flock_boid_link_block(object);
            if (state) {
              boid_state_initialize(flock_index, state, (short)source_index, placement.scale);
              ai_flocks_spawn_add_creature(flock_index, object_index);
              flock->used_source_mask |= (short)(1 << (short)source_index);
            } else {
              object_delete(object_index);
              object_index = -1;
            }
          }
        }
      }
    }
  }
  return object_index;
}

// @address 0x55c5b8 ?flock_update@@YAXJ@Z
void flock_update(long index)
{
    flock_datum *flock = datum_get<flock_datum>(g_flocks_data, index);
    scenario *s = global_scenario;
    short definition = flock->definition_index;
    if (definition < 0 || definition >= s->flocks.count)
        return;
    s_flock_instance *instance = &((s_flock_instance *)s->flocks.address)[definition];
    if (((1 << instance->bsp_index) & (short)g_active_structure_bsp_mask) != 0) {
    if (flock_update_activation_status(index)) {
        if (flock->spawn_timer > 0)
            flock->spawn_timer--;
        if ((short)flock->source_mask > 0 && flock->spawn_timer <= 0 && flock->check_sources) {
            if (flock->boid_count < instance->maximum_boid_count) {
                if (!flock->spawning && (flock->target_boid_count == -1 || flock->boid_count < flock->target_boid_count))
                    flock_set_active((flock_datum *)flock, (const s_flock_instance *)instance, true);
            } else if (flock->spawning) {
                flock_set_active((flock_datum *)flock, (const s_flock_instance *)instance, false);
            }
            if (flock->spawning && flock_spawn_boid(index, flock->spawn_source) != -1) {
                float minimum_period = 1.0f;
                float maximum_period = 1.0f;
                if (instance->minimum_period > 0.0f)
                    minimum_period = instance->minimum_period;
                if (instance->maximum_period > 0.0f)
                    maximum_period = instance->maximum_period;
                if (minimum_period > maximum_period) {
                    float temp = maximum_period;
                    maximum_period = minimum_period;
                    minimum_period = temp;
                }
                long minimum_ticks = (long)((float)tls_game_time_globals()->ticks_per_second / maximum_period + 0.5f);
                long maximum_ticks = (long)((float)tls_game_time_globals()->ticks_per_second / minimum_period + 0.5f);
                flock->spawn_timer = ai_flocks_short_random_range((short)minimum_ticks, (short)maximum_ticks);
            }
        }
    }
    if (flock->boid_count <= 0)
        return;
    flock_update_bounding_box(index);
    flock_update_danger(index);
    flock_update_oldest_boid_target(index);
    flock_boid_iterator iterator;
    iterator.current_object_index = -1;
    iterator.next_object_index = datum_get<flock_datum>(g_flocks_data, index)->first_boid_index;
    creature_datum *creature = flock_boid_iterator_next(&iterator);
    if (creature == 0)
        return;
    do {
        if (creature->creature.link_type == 1) {
            boid_state *link = (boid_state *)((char *)creature + creature->creature.creature_reference.offset);
            if (link != 0) {
                if (link->dead) {
                    object_delete(iterator.current_object_index);
                } else if (link->active) {
                    boid_update(index, iterator.current_object_index);
                    link->update_count = 0;
                } else {
                    short count = (short)(link->update_count + 1);
                    link->update_count = count;
                    if (count > (long)((float)tls_game_time_globals()->ticks_per_second * 3.0f + 0.5f))
                        link->dead = 1;
                }
            }
        }
        creature = flock_boid_iterator_next(&iterator);
    } while (creature != 0);
    return;
    }
    flock->active = false;
}
static __forceinline long ai_flocks_record_new(short scenario_index,long definition) {
 long i=datum_new(g_flocks_data);
 if(i!=-1) {
  flock_datum *f=datum_get<flock_datum>(g_flocks_data,i);
  f->boid_count=0;
  f->first_boid_index=-1;
  f->spawn_timer=-1;
  f->definition_index=scenario_index;
  f->tag_index=definition;
  f->death_timer=0;
  f->active=false;
  f->check_sources=true;
  f->used_source_mask=0;
  flock_set_active((flock_datum *)f,
      (const s_flock_instance *)tag_block_get_element_with_size(&global_scenario->flocks, scenario_index, sizeof(s_flock_instance)), true);
  memset(&f->bounds,0,sizeof(f->bounds));
 }
 return i;
}
// @address 0x55b108 flock_new_from_scenario
bool flock_new_from_scenario(long scenario_index) {
 scenario *s=global_scenario;
 s_flock_instance *instance=(s_flock_instance *)tag_block_get_element_with_size(&s->flocks,scenario_index, sizeof(s_flock_instance));
 bool result=false;
 if(instance->creature>=0 && instance->creature<s->creature_palette.count) {
  scenario_object_palette_entry *creature=(scenario_object_palette_entry *)tag_block_get_element_with_size(
      &s->creature_palette, instance->creature, sizeof(scenario_object_palette_entry));
  if(creature->object.index!=-1) {
   if(instance->definition>=0 && instance->definition<s->flock_definitions.count) {
    s_tag_reference *definition=(s_tag_reference *)tag_block_get_element_with_size(&s->flock_definitions,instance->definition,sizeof(s_tag_reference));
    if(definition->index!=-1) {
     long i=ai_flocks_record_new((short)scenario_index,definition->index);
     if(i!=-1) {
      if(instance->flags&4) { flock_datum *f=datum_get<flock_datum>(g_flocks_data,i); f->check_sources=false; }
      result=true;
     }
    }
   }
  }
 }
 return result;
}

// @address 0x55c398 flock_update_oldest_boid_target
extern "C" void flock_update_oldest_boid_target(long flock_index)
{
  long selected = -1;
  long longest_wait = 0;
  flock_boid_iterator iterator;
  iterator.current_object_index = -1;
  iterator.next_object_index = datum_get<flock_datum>(g_flocks_data, flock_index)->first_boid_index;
  creature_datum *creature;
  while ((creature = flock_boid_iterator_advance(&iterator)) != 0) {
    boid_state *link = flock_boid_link_block(creature);
    if (link) {
      if (++link->target_timer > (short)longest_wait) {
        longest_wait = (unsigned short)link->target_timer;
        selected = iterator.current_object_index;
      }
    }
  }
  if (selected != -1) {
    creature = (creature_datum *)object_get_and_verify_type(selected, NONE);
    boid_state *link;
    if (creature->creature.link_type == 1)
      link = (boid_state *)((char *)creature + creature->creature.creature_reference.offset);
    else
      link = 0;
    link->target_timer = 0;
    boid_update_target(flock_index, selected);
  }
}

// @address 0xd30c8 flocks_update
extern "C" void flocks_update(void)
{
  scenario *scenario = global_scenario;
  flock_iterator iterator;
  flock_iterator_new(&iterator);
  flock_datum *flock;
  while ((flock = flock_iterator_next(&iterator)) != 0) {
    if (flock->spawning == 0 && flock->boid_count == 0) {
      short definition = flock->definition_index;
      if (definition >= 0 && definition < scenario->flocks.count && ((s_flock_instance *)scenario->flocks.address)[definition].minimum_boid_count == 0)
        flocks_dispose_from_old_structure_bsp(iterator.current_index);
    }
  }
  flock_iterator_new(&iterator);
  while ((flock = flock_iterator_next(&iterator)) != 0)
    flock_update(iterator.current_index);
}

static const float ai_flocks_boundary_force_scale = 3.0f;
static __forceinline real_vector3d *ai_flocks_vector_from_line3d(const real_vector3d *p, const real_vector3d *v, float t, real_vector3d *result)
{
  result->i = p->i + v->i * t;
  result->j = p->j + v->j * t;
  result->k = p->k + v->k * t;
  return result;
}
// @address 0xd3998 ?boundary_force_bounding_volume@@YAXPAUs_flock_instance@@PATreal_point3d@@PAMPATreal_vector3d@@_NPA_N@Z
// match: 0xd3998: strength restrict keeps the 1.0f load and fmr fr31 below the flags test
void boundary_force_bounding_volume(s_flock_instance *instance, real_point3d * point, float *__restrict strength,
    // match: 0xd3998: moved restrict keeps the flags halfword load below the pool loads and zero-vector load in r11
    real_vector3d *force, bool horizontal_only, bool *__restrict moved)
{
  scenario *scenario = global_scenario;
  float range = 1.0f;
  bool clamp = (instance->flags & 2) != 0;
  *strength = 0.0f;
  *moved = false;
  *force = *global_zero_vector3d;
  if (instance->boundary_distance > 0.0f)
    range = instance->boundary_distance;
  real_matrix4x3 matrix;
  if (instance->trigger_volume_index >= 0 && instance->trigger_volume_index < scenario->trigger_volumes.count &&
      trigger_volume_get_matrix(&((scenario_trigger_volume *)scenario->trigger_volumes.address)[instance->trigger_volume_index], &matrix)) {
    real_vector3d offset;
    vector_from_points3d(&matrix.position, point, &offset);
    float maximum = 0.0f;
    long axis_count = horizontal_only ? 2 : 3;
    const real_point3d *extents = &((scenario_trigger_volume *)scenario->trigger_volumes.address)[instance->trigger_volume_index].extents;
    for (short axis = 0; axis < axis_count; ++axis) {
      float distance = dot_product3d(&offset, &matrix.rotation.basis[axis]);
      if (distance <= range) {
        float amount = range - distance;
        amount = PIN(amount, 0.0f, range);
        float weight = amount / range * ai_flocks_boundary_force_scale;
        ai_flocks_vector_from_line3d(force, &matrix.rotation.basis[axis], weight, force);
        if (weight > maximum)
          maximum = weight;
        if (clamp && distance < 0.0f) {
          point_from_line3d(point, &matrix.rotation.basis[axis], -distance, point);
          *moved = true;
        }
      }
      if (distance >= extents->n[axis] - range) {
        float weight = (1.0f - PIN(extents->n[axis] - distance, 0.0f, range) / range) * ai_flocks_boundary_force_scale;
        ai_flocks_vector_from_line3d(force, &matrix.rotation.basis[axis], -weight, force);
        if (weight > maximum)
          maximum = weight;
        if (clamp && distance > extents->n[axis]) {
          point_from_line3d(point, &matrix.rotation.basis[axis], extents->n[axis] - distance, point);
          *moved = true;
        }
      }
    }
    normalize3d(force);
    *strength = maximum;
  }
}
static __forceinline float ai_flocks_target_hysteresis_scale()
{
    return global_lean_pivot_offset;
}

// @rdata 0x8202251c
static const float k_flock_zero = 0.0f;
// @rdata 0x820221fc
static const float k_flock_one = 1.0f;
// @rdata 0x8202490c
static const float k_flock_epsilon = 0.0001f;
static __forceinline float ai_flocks_target_magnitude_yz_squared(const real_vector3d *v) { return v->k*v->k + v->j*v->j; }
// @address 0x55c048 boid_update_target
extern "C" void boid_update_target(long flock_index, long object_index)
{
    flock_datum *flock = datum_get<flock_datum>(g_flocks_data, flock_index);
    creature_datum *object = (creature_datum *)object_get_and_verify_type(object_index, NONE);
    boid_flock_definition *definition = (boid_flock_definition *)tag_get(NONE, flock->tag_index);
    boid_state *link = object->creature.link_type == 1 ? (boid_state *)((char *)object + object->creature.creature_reference.offset) : 0;
    scenario *scenario = global_scenario;
    s_flock_instance *instance = (s_flock_instance *)&((s_flock_instance *)scenario->flocks.address)[flock->definition_index];
    if (instance->target_flock == -1) return;
    real_point3d origin;
    real_vector3d forward;
    object_get_origin(object_index, &origin);
    object_get_orientation(object_index, &forward, 0);
    if (link->target_object_index != -1) {
        creature_datum *target = (creature_datum *)object_get_and_verify_type(link->target_object_index, NONE);
        bool dead = (target->object.damage_flags & 4) != 0;
        if (dead) link->target_object_index = -1;
    }
    flock_iterator iterator;
    flock_iterator_new(&iterator);
    flock_datum *other;
    while ((other = flock_iterator_next(&iterator)) != 0) {
        if (!other->active || other->definition_index != instance->target_flock) continue;
        flock_boid_iterator boids;
        boids.current_object_index = -1;
        boids.next_object_index = datum_get<flock_datum>(g_flocks_data, iterator.current_index)->first_boid_index;
        long selected = -1;
        float best = k_flock_zero;
        while (flock_boid_iterator_next(&boids)) {
            long candidate = boids.current_object_index;
            bool previous = candidate == link->target_object_index;
            real_point3d target;
            object_get_origin(candidate, &target);
            real_vector3d direction; vector_from_points3d(&origin, &target, &direction);
            float distance = sqrtf(direction.i*direction.i + ai_flocks_target_magnitude_yz_squared(&direction));
            if (fabsf(distance) >= k_flock_epsilon) {
                float inverse = k_flock_one / distance;
                direction.i *= inverse; direction.j *= inverse; direction.k *= inverse;
            } else distance = k_flock_zero;
            float dot = forward.i*direction.i + (forward.k*direction.k + forward.j*direction.j);
            float weight = k_flock_zero;
            float limit = definition->attraction_distance;
            float inner = limit * k_real_third;
            if (distance < limit || (previous && distance < limit * ai_flocks_target_hysteresis_scale())) {
                if (distance > limit) weight = k_flock_zero;
                else if (distance < inner) weight = k_flock_one;
                else { weight = (limit-distance) / (limit-inner); weight = PIN(weight, k_flock_zero, k_flock_one); }
            }
            float score = weight * 2.0f + fabsf(dot);
            if (previous) score *= 1.2f;
            if (score > best) { best = score; selected = candidate; }
        }
        if (link->target_object_index != selected) link->attack_timer = 0;
        link->target_object_index = selected;
        break;
    }
}

// ---- 0x55cd08 boid_update views ----
static __forceinline void boid_state_clamp_perlin(boid_state *state, const real_vector3d *direction)
{
  if (state->perlin.target.x * direction->i +
          (state->perlin.target.z * direction->k + state->perlin.target.y * direction->j) < 0.0f) {
    state->perlin.target = *global_origin3d;
  }
  if (state->perlin.previous.x * direction->i +
          (state->perlin.previous.z * direction->k + state->perlin.previous.y * direction->j) < 0.0f) {
    state->perlin.previous = *global_origin3d;
  }
}


static __forceinline void ai_flocks_boid_set_control(
    unsigned char perched, const real_vector3d *desired, const real_vector3d *forward, s_creature_control_data *control)
{
  control->throttle = *desired;
  control->forward = *forward;
  if (perched)
    control->animation = 0x1de;
}
// @rdata 0x820242d4
static const float ai_flocks_boid_force_limit = 3.0f;
// @rdata 0x82021694
static const float ai_flocks_boid_infinite_distance = FLT_MAX;
// @rdata 0x82032938
static const float ai_flocks_boid_reverse = -1.0f;
// @rdata 0x820334a8
static const float ai_flocks_boid_scale_threshold = 0.01f;
// @rdata 0x8203385c
static const float ai_flocks_boid_target_minimum = 0.2f;
// @rdata 0x8203aa78
static const float ai_flocks_boid_radius_epsilon = 0.001f;
static __forceinline void ai_flocks_boid_add_yz(real_point3d *sum, const real_point3d *point)
{
  sum->y += point->y;
  sum->z += point->z;
}
static __forceinline float ai_flocks_boid_dot(const real_vector3d *a, const real_vector3d *b)
{
  return a->i * b->i + (a->k * b->k + a->j * b->j);
}
static __forceinline float ai_flocks_boid_dot(const real_point3d *a, const real_vector3d *b)
{
  return a->i * b->i + (a->k * b->k + a->j * b->j);
}
// @address 0x55cd08 ?boid_update@@YAXJJ@Z
void boid_update(long flock_index, long boid_index)
{
  flock_datum *flock = datum_get<flock_datum>(g_flocks_data, flock_index);
  creature_datum *object = (creature_datum *)object_get_and_verify_type(boid_index, NONE);
  boid_flock_definition *definition = (boid_flock_definition *)tag_get(NONE, flock->tag_index);
  s_flock_instance *instance = (s_flock_instance *)&((s_flock_instance *)global_scenario->flocks.address)[flock->definition_index];
  if (object->creature.link_type != 1)
    return;
  boid_state *state = (boid_state *)((char *)object + object->creature.creature_reference.offset);
  if (!state)
    return;
  if (object->object.parent_object_index != -1)
    return;
  float closest_distance = ai_flocks_boid_infinite_distance;
  float total_weight = 0.0f;
  float danger_weight = 0.0f;
  s_creature_control_data control;
  creature_control_clear(&control);
  real_point3d position;
  object_get_origin(boid_index, &position);
  float perch_scale_a = instance->scale_a;
  float scale = 1.0f;
  float perch_scale_b = instance->scale_b;
  real_vector3d throttle = *global_zero_vector3d;
  creature_definition *object_definition = (creature_definition *)tag_get(NONE, object->object.definition_index);
  bool planar = (((~object_definition->physics.flags) >> 4) & 1) != 0;
  if (perch_scale_a > ai_flocks_boid_scale_threshold) {
    short perch = state->w14;
    if (perch >= 0 && perch < instance->source_count) {
      ai_flocks_spawn_source *entry = &instance->sources[perch];
      float dx = position.x - entry->position.x;
      float dy = position.y - entry->position.y;
      float dz = position.z - entry->position.z;
      float distance = sqrtf(dx * dx + dz * dz + dy * dy);
      if (distance < perch_scale_a) {
        float t = distance / perch_scale_a;
        if (scale > t)
          scale = t;
      }
    }
  }
  if (perch_scale_b > ai_flocks_boid_scale_threshold) {
    short perch = state->w16;
    if (perch >= 0 && perch < instance->random_count) {
      boid_perch_entry_b *entry = &instance->perch_b[perch];
      float dx = position.x - entry->x;
      float dy = position.y - entry->y;
      float dz = position.z - entry->z;
      float distance = sqrtf(dx * dx + dz * dz + dy * dy);
      if (distance < perch_scale_b) {
        float t = distance / perch_scale_b;
        if (scale > t)
          scale = t;
      }
    }
  }
  if (scale < 0.9999f) {
    if (scale <= 0.005f)
      scale = 0.005f;
    object_set_scale(boid_index, state->f24 * scale, 0.0f, false);
  }
  float steering = definition->fallback_weight;
  float neighbor_radius = definition->neighbor_radius;
  float half_radius = neighbor_radius * 0.5f;
  short neighbor_count = 0;
  real_vector3d velocity;
  const real_vector3d *forward = &object->object.forward;
  velocity.i = forward->i * definition->speed_factor;
  velocity.j = forward->j * definition->speed_factor;
  velocity.k = forward->k * definition->speed_factor;
  real_point3d average = *global_origin3d;
  real_vector3d alignment = *global_zero_vector3d;
  real_vector3d separation = *global_zero_vector3d;
  float alignment_weight = 0.0f;
  float separation_weight = 0.0f;
  float min_throttle = 0.0f;
  real_point3d closest_position;
  throttle.j = 0.0f;
  throttle.k = 0.0f;
  if (neighbor_radius > 0.0f) {
    flock_boid_iterator boids;
    boids.next_object_index = datum_get<flock_datum>(g_flocks_data, flock_index)->first_boid_index;
    boids.current_object_index = -1;
    for (creature_datum *other = flock_boid_iterator_next(&boids); other; other = flock_boid_iterator_next(&boids)) {
      if ((creature_datum *)other == object)
        continue;
      boid_state *other_state = other->creature.link_type == 1 ? (boid_state *)((char *)other + other->creature.creature_reference.offset) : 0;
      if (other_state->needs_update != 0)
        continue;
      real_point3d other_position;
      object_get_origin(boids.current_object_index, &other_position);
      real_vector3d to_neighbor;
      vector_from_points3d(&position, &other_position, &to_neighbor);
      float distance = normalize3d(&to_neighbor);
      float neighbor_i = to_neighbor.i;
      bool visible;
      if (state->needs_update != 0) {
        visible = true;
      } else {
        float facing = ai_flocks_boid_dot(&to_neighbor, &object->object.forward);
        visible = facing > flock_cosine(definition->view_angle);
      }
      if (!visible)
        continue;
      if (distance < neighbor_radius && distance > 0.0f) {
        float distance_squared = distance * distance;
        float weight = definition->separation_force / distance_squared;
        creature_datum *neighbor = (creature_datum *)other;
        alignment_weight += weight;
        alignment.i += neighbor->object.forward.i * weight;
        alignment.j += neighbor->object.forward.j * weight;
        alignment.k += neighbor->object.forward.k * weight;
        if (distance_squared < definition->separation_radius * definition->separation_radius) {
          float push = 1.0f - distance_squared / (definition->separation_radius * definition->separation_radius);
          push = PIN(push, 0.0f, 1.0f);
          float push_weight = definition->separation_multiplier * push;
          separation_weight += push_weight;
          float repel = -push_weight;
          separation.i += repel * neighbor_i;
          separation.j += repel * to_neighbor.j;
          separation.k += repel * to_neighbor.k;
        }
        neighbor_count++;
        average.x += other_position.x;
        ai_flocks_boid_add_yz(&average, &other_position);
      }
      if (distance < closest_distance) {
        closest_distance = distance;
        closest_position = other_position;
      }
    }
  }

  if (state->needs_update == 0) {
    union {
      real_point3d perlin_point;
      real_vector3d perlin_offset;
    };
    perlin_state_update(&state->perlin, definition->perlin_magnitude, definition->perlin_minimum,
        definition->perlin_maximum, planar, &perlin_point);
    if (planar) {
      float magnitude = normalize3d(&perlin_offset);
      if (magnitude > 0.0f) {
        if (magnitude > ai_flocks_boid_force_limit)
          magnitude = ai_flocks_boid_force_limit;
        normalize3d(&perlin_offset);
        total_weight = magnitude;
        ai_flocks_vector_from_line3d(&velocity, &perlin_offset, magnitude, &velocity);
      }
    } else {
      ai_flocks_vector_from_line3d(&velocity, &perlin_offset, 1.0f, &velocity);
    }
  }
  if (state->needs_update == 0) {
    float boundary_weight;
    real_vector3d direction;
    bool pushed = false;
    boundary_force_bounding_volume(instance, &position, &boundary_weight, &direction, planar, &pushed);
    ai_flocks_vector_from_line3d(&velocity, &direction, boundary_weight, &velocity);
    total_weight += boundary_weight;
    if (pushed)
      object_set_position_direct(boid_index, &position, 0, 0, 0, false);
    if (boundary_weight > 0.0f) {
      boid_state_clamp_perlin(state, &direction);
    } else if (!planar) {
      velocity.k -= object->object.forward.k * definition->sink_factor;
    }
  }
  real_vector3d direction;
  prop_compute_avoidance_vector(flock, instance, definition, state, &position, &danger_weight, &direction, planar, &min_throttle);
  if (danger_weight > 0.0f) {
    total_weight += danger_weight;
    ai_flocks_vector_from_line3d(&velocity, &direction, danger_weight, &velocity);
    real_point3d *target = &state->perlin.target;
    if (ai_flocks_boid_dot(target, &direction) < 0.0f)
      *target = *global_origin3d;
    real_point3d *previous = &state->perlin.previous;
    if (ai_flocks_boid_dot(previous, &direction) < 0.0f)
      *previous = *global_origin3d;
  }
  if (neighbor_count > 0 && state->needs_update == 0 && total_weight < ai_flocks_boid_force_limit) {
    if (separation_weight + total_weight > ai_flocks_boid_force_limit)
      separation_weight = ai_flocks_boid_force_limit - total_weight;
    normalize3d(&separation);
    ai_flocks_vector_from_line3d(&velocity, &separation, separation_weight, &velocity);
    total_weight += separation_weight;
  }
  float reverse = -1.0f;
  bool attacking = false;
  if (state->target_object_index != -1 && state->needs_update == 0) {
    float attraction = 0.0f;
    real_point3d target_position;
    object_get_origin(state->target_object_index, &target_position);
    real_vector3d to_target;
    vector_from_points3d(&position, &target_position, &to_target);
    float distance = normalize3d(&to_target);
    float facing = ai_flocks_boid_dot(&object->object.forward, &to_target);
    if (facing < 0.0f || distance < 5.0f) {
      to_target.i *= reverse;
      to_target.j *= reverse;
      to_target.k *= reverse;
    }
    float range = definition->attraction_distance;
    float closeness;
    float inner = range * k_real_third;
    if (distance > inner)
    {
      closeness = (range - distance) / (range - inner);
      closeness = ((closeness <= 0.0f ? 0.0f : closeness) < 1.0f)
          ? MAX(closeness, 0.0f) : 1.0f;
    } else {
      closeness = 1.0f;
    }
    attraction = (definition->target_base - ai_flocks_boid_target_minimum) * closeness + ai_flocks_boid_target_minimum;
    if (total_weight < ai_flocks_boid_force_limit) {
      if (attraction + total_weight > ai_flocks_boid_force_limit)
        attraction = ai_flocks_boid_force_limit - total_weight;
      normalize3d(&to_target);
      ai_flocks_vector_from_line3d(&velocity, &to_target, attraction, &velocity);
      total_weight += attraction;
    }
    if (facing > 0.96f) {
      float duration = definition->attack_duration;
      float chance = definition->attack_chance;
      control.flags |= 0x20;
      if (chance <= 0.0f)
        chance = 0.25f;
      if (duration <= 0.0f)
        duration = 0.5f;
      short attack_ticks = state->attack_timer + 1;
      attacking = true;
      state->attack_timer = attack_ticks;
      if (attack_ticks > ai_flocks_seconds_to_ticks_round(duration)) {
        float threshold = ai_chance_this_tick(chance);
        if ((float)ai_flocks_random_next(tls_random_seed()) * 1.5259022e-05f < threshold) {
          long target_index = state->target_object_index;
          if (target_index != -1) {
            creature_datum *target_object =
                (creature_datum *)object_get_and_verify_type(target_index, NONE);
            target_object->object.damage_flags |= 0x40;
            object_wake(target_index);
          }
        }
      }
    }
  }
  if (!attacking)
    state->attack_timer = 0;
  if (state->needs_update == 0) {
    if (neighbor_count > 0) {
      if (total_weight < ai_flocks_boid_force_limit) {
        if (alignment_weight + total_weight > ai_flocks_boid_force_limit)
          alignment_weight = ai_flocks_boid_force_limit - total_weight;
        normalize3d(&alignment);
        total_weight += alignment_weight;
        ai_flocks_vector_from_line3d(&velocity, &alignment, alignment_weight, &velocity);
        if (total_weight < ai_flocks_boid_force_limit) {
          float inverse = 1.0f / (float)neighbor_count;
          scale_point3d(&average, inverse, &average);
          real_vector3d to_center;
          vector_from_points3d(&position, &average, &to_center);
          float cohesion = definition->cohesion_weight;
          float centroid_distance = normalize3d(&to_center);
          float cohesion_minimum = definition->cohesion_minimum;
          if (centroid_distance > cohesion_minimum) {
            if (centroid_distance < definition->cohesion_maximum)
              cohesion = (centroid_distance - cohesion_minimum) /
                  (definition->cohesion_maximum - cohesion_minimum) * cohesion;
            if (cohesion + total_weight > ai_flocks_boid_force_limit)
              cohesion = ai_flocks_boid_force_limit - total_weight;
            normalize3d(&to_center);
            ai_flocks_vector_from_line3d(&velocity, &to_center, cohesion, &velocity);
            total_weight += cohesion;
          }
          if (centroid_distance > definition->neighbor_radius) {
            steering = definition->far_weight;
          } else if (centroid_distance > half_radius) {
            steering = (definition->far_weight - steering) * (centroid_distance - half_radius) /
                    (definition->neighbor_radius - half_radius) + steering;
          }
        }
      }
    } else if (closest_distance < ai_flocks_boid_infinite_distance) {
      float cohesion = 0.0f;
      real_vector3d direction;
      direction.i = closest_position.x - position.x;
      direction.j = closest_position.y - position.y;
      direction.k = closest_position.z - position.z;
      normalize3d(&direction);
      cohesion = definition->cohesion_weight;
      ai_flocks_vector_from_line3d(&velocity, &direction, cohesion, &velocity);
      steering = definition->far_weight;
    } else {
      steering = definition->fallback_weight;
    }
  }
  short perch_target = state->w16;
  if (perch_target >= 0 && perch_target < instance->random_count && state->needs_update == 0) {
    float pull = 0.0f;
    boid_perch_entry_b *target_entry = &instance->perch_b[perch_target];
    real_vector3d to_perch;
    to_perch.i = target_entry->x - position.x;
    to_perch.j = target_entry->y - position.y;
    to_perch.k = target_entry->z - position.z;
    float perch_distance = normalize3d(&to_perch);
    if (perch_distance > 0.0f) {
      float radius = 1.0f;
      if (target_entry->radius > 0.0f)
        radius = target_entry->radius;
      if (perch_distance < radius) {
        state->dead = 1;
      } else {
        pull = definition->perch_weight;
        if (total_weight < ai_flocks_boid_force_limit) {
          if (pull + total_weight > ai_flocks_boid_force_limit)
            pull = ai_flocks_boid_force_limit - total_weight;
          normalize3d(&to_perch);
          ai_flocks_vector_from_line3d(&velocity, &to_perch, pull, &velocity);
          total_weight += pull;
        }
      }
    }
  }
  steering = MAX(steering, min_throttle);
  throttle.i = PIN(steering, reverse, 1.0f);
  if (planar) {
    throttle.k = 0.0f;
    velocity.k = 0.0f;
  }
  float maximum_speed = definition->max_speed;
  bool moving;
  if (state->b9 != 0)
    moving = total_weight >= maximum_speed * 0.7f;
  else
    moving = total_weight > maximum_speed;
  if (moving) {
    if (normalize3d(&velocity) == 0.0f)
      object_get_orientation(boid_index, &velocity, 0);
    state->b9 = 1;
    state->needs_update = 0;
  } else {
    throttle = *global_zero_vector3d;
    object_get_orientation(boid_index, &velocity, 0);
    state->b9 = 0;
    if (state->update_timer > 0) {
      state->update_timer--;
      if (state->update_timer == 0)
        state->needs_update = 0;
    } else if (state->needs_update != 0 && closest_distance < ai_flocks_boid_infinite_distance) {
      float radius = definition->neighbor_radius;
      state->update_timer++;
      if (radius < ai_flocks_boid_radius_epsilon)
        radius = 10.0f;
      float delay = closest_distance / radius;
      if (delay <= 0.0f)
        delay = 0.0f;
      state->update_timer = (short)ai_flocks_seconds_to_ticks_round(delay);
    }
  }
  ai_flocks_boid_set_control(state->needs_update, &throttle, &velocity, &control);
  generate_up_vector3d(&velocity, &control.up);
  creature_set_actively_controlled(boid_index, true);
  ((creature_datum *)object_get_and_verify_type(boid_index, NONE))->creature.control = control;
}

extern "C" {
t_ai_flocks_allocation_manager g_ai_flocks_allocator;
}


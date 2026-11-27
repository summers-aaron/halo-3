// blamlibxbox/contrail_states.obj: from the records 0x14bc18 0x14bcf0 0x2b42b0 0x2b4450
// @flags /TP /O2 /Oc
// @address 0x14bc18 contrail_states_set_definition
// @address 0x14bcf0 contrail_states_update_values
// @address 0x2b42b0 ?update_states@c_contrail_states@@QAAXK@Z
// @address 0x2b4450 ?set_contrail_system@c_contrail_states@@QAAXPBVc_contrail_system@@@Z
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "effects/contrail_states.h"
#include "effects/contrail_states_types.h"
#include "tag_files/tag_groups.h"
#include "memory/tls_globals.h"

#include "effects/contrail_system.h"
#include "effects/contrail.h"
#include "effects/contrail_location.h"
#include "effects/contrail_profile.h"
#include "effects/contrail_definition_types.h"
#include "effects/effect_gpu_states.h"

static __forceinline unsigned long object_element_flags(const c_contrail_system *system)
{
  unsigned long flags = 0;
  if (system) {
    const c_contrail_system *object = system;
    s_contrail_system_definition *definition = TAG_GET('cntl', s_contrail_system_definition, object->definition_index);
    for (long i = 0; i < definition->count; i++)
      flags |= definition->contrails[i].used_states;
  }
  return flags;
}

void contrail_states_set_definition(s_object_14 *object, s_definition_14 *definition)
{
  s_definition_14 *old = object->definition;
  if (definition == old)
    return;

  unsigned long old_flags = 0;
  const cache_file_tag_instance *instances = global_tag_instances;
  if (old) {
    s_contrails_property_block *block = (s_contrails_property_block *)instances[(unsigned short)old->effect_index].definition;
    int count = block->count;
    if (count > 0) {
      s_contrails_property_row *entry = block->rows;
      do {
        old_flags |= entry->used_states;
        entry++;
      } while (--count != 0U);
    }
  }

  unsigned long kept = object->dirty & ~old_flags;
  object->definition = definition;
  unsigned long new_flags = 0;
  object->dirty = kept;
  object->valid = object->valid & ~4 & ~0x1c0;
  if (definition) {
    s_contrails_property_block *block = (s_contrails_property_block *)instances[(unsigned short)definition->effect_index].definition;
    int count = block->count;
    if (count > 0) {
      s_contrails_property_row *entry = block->rows;
      do {
        new_flags |= entry->used_states;
        entry++;
      } while (--count != 0U);
    }
  }
  object->dirty |= new_flags;
  contrail_states_update_values(object, 0x3c6);
}

void contrail_states_update_values(s_object_14 *object, unsigned long mask)
{
  unsigned long update = object->dirty & ~object->valid & mask;
  if (update) {
    if (update & 0x30) {
      s_particle_instance *source = object->source;
      object->first_random_value = source->random_values[0];
      object->second_random_value = source->random_values[1];
    }
    if (update & 0x1c4) {
      s_definition_14 *definition = object->definition;
      object->age = definition->age;
      object->references = (float)definition->references;
      object->lifespan = definition->lifespan;
      object->interpolation = definition->interpolation;
    }
    if (update & 2) {
      game_time_globals_definition *game_time = tls_game_time_globals();
      float seconds = game_time->tick_length;
      long ticks = game_time->time;
      short rate = game_time->ticks_per_second;
      object->time = (float)(ticks % rate) * seconds;
    }
  }
  object->valid |= update;
}

void c_contrail_states::update_states(unsigned long update_mask)
{
    unsigned long previous_flags;
    unsigned long pending_update;
    const c_contrail_profile *contrail_profile;
    const c_contrail *definition;
    const c_contrail_location *contrail_location;
    float velocity_y;
    float velocity_x;
    float velocity_z;
    const c_contrail_system *contrail_system;
    game_time_globals_definition *time_globals;
    short tick_rate;
    float tick_seconds;
    int tick_count;
    pending_update = used_flags & ~flags & update_mask;
    previous_flags = flags;
    if (pending_update) {
        if (pending_update & 61) {
            contrail_profile = profile;
            values[0] = contrail_profile->age;
            { int first_random_seed = contrail_profile->random[0]; values[2] = (float)first_random_seed; }
            { int second_random_seed = contrail_profile->random[1]; values[3] = (float)second_random_seed; }
            { int third_random_seed = contrail_profile->random[2]; values[4] = (float)third_random_seed; }
            { int fourth_random_seed = contrail_profile->random[3]; values[5] = (float)fourth_random_seed; }
        }
        if (pending_update & 0x600) {
            definition = contrail;
            values[9] = definition->random[0];
            values[10] = definition->random[1];
        }
        if (pending_update & 0x800) {
            contrail_location = location;
            velocity_y = contrail_location->velocity.j;
            velocity_x = contrail_location->velocity.i;
            velocity_z = contrail_location->velocity.k;
            values[11] = sqrtf(((velocity_z * velocity_z) + ((velocity_x * velocity_x) + (velocity_y * velocity_y))));
        }
        if (pending_update & 0x7080) {
            contrail_system = system;
            values[7] = contrail_system->age;
            values[12] = (float)contrail_system->lod;
            values[13] = contrail_system->effect_scale_a;
            values[14] = contrail_system->effect_scale_b;
        }
        if (pending_update & 64) {
            time_globals = tls_game_time_globals();
            tick_rate = time_globals->ticks_per_second;
            tick_seconds = time_globals->tick_length;
            tick_count = time_globals->time;
            flags = previous_flags | pending_update;
            values[6] = (float)(tick_count % tick_rate) * tick_seconds;
            return;
        }
    }
    flags = previous_flags | pending_update;
}

void c_contrail_states::set_contrail_system(class c_contrail_system const * _sym_p1)
{
  if (_sym_p1 != system) {
    unsigned long old_flags = object_element_flags(system);
    system = _sym_p1;
    used_flags &= ~old_flags;
    flags &= ~0x80;
    flags &= ~0x7000;
    used_flags |= object_element_flags(_sym_p1);
    update_states(0xf0c0);
  }
}

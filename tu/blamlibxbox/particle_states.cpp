#include "effects/particle_array_globals.h"
// blamlibxbox/particle_states.obj: from the records 0x142b98 0x142c68 0x142cd8 0x143438 0x2b3738 0x2b3a00
// @flags /TP /O2 /Oc
#include "cseries/cseries_types.h"
#include "tag_files/tag_types.h"
#include "game/game_time_types.h"
#include "effects/particle_states.h"
#include "effects/particle_system.h"
#include "effects/particle_emitter_definitions.h"
#include "memory/data_types.h"
#include "memory/datum_index.h"
// @address 0x142c68 ?get_particle_system_used_states@c_particle_state_list@@QBAKXZ
// @address 0x142cd8 ?update_states_internal@c_particle_state_list@@AAAXK@Z
// @address 0x143438 ?set_particle_location@c_particle_state_list@@QAAXPBVc_particle_location@@@Z
// @address 0x2b3738 particle_state_list_set_particle
// @address 0x8c43ac g_particle_default_used_states
// @address 0x2b3a00 particle_state_list_get_state_values
// @address 0x11df78c g_particle_location_data_array

#include "effects/particle_location_runtime_api.h"
#include "effects/particle_definitions.h"
#include "effects/particle_emitter.h"
#include "effects/effect_definitions.h"
#include "effects/particle.h"
#include "tag_files/tag_groups.h"
#include "memory/tls_globals.h"
#include "memory/data.h"
#include "game/game_time_inline.h"


unsigned long c_particle_state_list::get_particle_system_used_states() const
{
  unsigned long used_states = 0;
  const c_particle_system_definition *particle_system = m_particle_system_definition;
  if (particle_system) {
    long definition_index = particle_system->get_particle_definition_index();
    const c_particle_definition *definition = definition_index != NONE
      ? (const c_particle_definition *)global_tag_instances[datum_index_to_absolute_index(definition_index)].definition
      : NULL;
    unsigned long system_used_states = definition->used_states;
    for (long entry_index = 0; entry_index < particle_system->get_emitter_count(); entry_index++)
      system_used_states = system_used_states | particle_system->get_emitter(entry_index)->get_used_particle_states();
    used_states = system_used_states;
  }
  return used_states;
}

void c_particle_state_list::update_states_internal(unsigned long states)
{
  if (states & k_particle_states_mask)
  {
    real scale = 1.0f / 65535.0f;
    m_state_values[_particle_age] = m_particle->m_age;
    m_state_values[_particle_correlation_1] = m_particle->m_correlation_1;
    m_state_values[_particle_random_1] = m_particle->m_random_seed[0] * scale;
    m_state_values[_particle_random_2] = m_particle->m_random_seed[1] * scale;
    m_state_values[_particle_random_3] = m_particle->m_random_seed[2] * scale;
    m_state_values[_particle_random_4] = m_particle->m_random_seed[3] * scale;
    m_state_values[_particle_random_5] = m_particle->m_random_seed[4] * scale;
    m_state_values[_particle_random_6] = m_particle->m_random_seed[5] * scale;
    m_state_values[_particle_random_7] = m_particle->m_random_seed[6] * scale;
    m_state_values[_particle_random_8] = m_particle->m_random_seed[7] * scale;
  }
  if (states & k_system_states_mask)
  {
    m_state_values[_system_age] = m_particle_system->get_age();
    m_state_values[_system_random_1] = m_particle_system->get_random_seed(0);
    m_state_values[_system_random_2] = m_particle_system->get_random_seed(1);
    m_state_values[_system_random_3] = m_particle_system->get_random_seed(2);
    m_state_values[_system_random_4] = m_particle_system->get_random_seed(3);
  }
  if (states & k_location_states_mask)
  {
    m_state_values[_location_lod] = m_particle_location->get_lod_amount();
    m_state_values[_location_random] = m_particle_location->get_random_seed();
  }
  if (states & (1 << _game_time))
  {
    m_state_values[_game_time] = (game_time_get() % game_tick_rate()) * game_tick_length();
  }
  if (states & (1 << _effect_a_scale))
    m_state_values[_effect_a_scale] = m_particle_system->get_exported_function(0);
  if (states & (1 << _effect_b_scale))
    m_state_values[_effect_b_scale] = m_particle_system->get_exported_function(1);
  if (states & (1 << _physics_rotation))
    m_state_values[_physics_rotation] = 0.0f;
  if (states & (1 << _distance_from_emitter))
    m_state_values[_distance_from_emitter] = 0.0f;
  if (states & (1 << _particle_scale))
    m_state_values[_distance_from_emitter] = 0.0f;
}

void c_particle_state_list::set_particle_location(class c_particle_location const * location)
{
  if (location != this->m_particle_location)
  {
    unsigned long flags = this->m_flags & 0xfffef7ff;
    unsigned long states = this->m_used_states;
    this->m_particle_location = location;
    states &= ~flags;
    states &= 0x10800;
    this->m_flags = flags;
    if (states)
    {
      this->update_states_internal(states);
      this->m_flags |= states;
    }
  }
}

static __forceinline const c_particle_system_definition *particle_states_system_definition_get(const c_particle_system *system)
{
  const s_effect_definition *effect = TAG_GET('effe', const s_effect_definition, system->m_tag_index);
  const s_particle_location_effect_event *events = (const s_particle_location_effect_event *)effect->events.address;
  return &events[system->m_def_index].particle_systems[system->m_sub_index];
}
// @address 0x142b98 ?set_particle_system@c_particle_state_list@@QAAXPBVc_particle_system@@@Z
void c_particle_state_list::set_particle_system(const c_particle_system *particle_system) {
    if(particle_system!=m_particle_system) {
      m_used_states &= ~get_particle_system_used_states();
      m_particle_system=particle_system;
      m_particle_system_definition=particle_states_system_definition_get(particle_system);
      m_flags &= 0xF9FF9CFD;
      m_used_states |= get_particle_system_used_states();
      update_states(0x06107302);
    }
  }

// @address 0x2b3738 particle_state_list_set_particle
extern "C" void particle_state_list_set_particle(c_particle_state_list *list, const c_particle *particle)
{
  list->set_particle(particle);
}



const unsigned long k_particle_states_available_mask = 0xef803;

// @address 0x2b3a00 particle_state_list_get_state_values
extern "C" void particle_state_list_get_state_values(
    const c_particle *particle, long unused, unsigned char state_a, unsigned char state_b, real *value_a, real *value_b)
{
  unsigned long mask_a = 1 << state_a;
  unsigned long mask_b = 1 << state_b;
  if ((mask_a | mask_b) & k_particle_states_available_mask)
  {
    c_particle_state_list list;
    c_particle_emitter *emitter = datum_get<c_particle_emitter>(g_particle_emitter_data, particle->get_emitter_index());
    c_particle_location *location = datum_get<c_particle_location>(g_particle_location_data_array, emitter->m_location_index);
    list.set_particle_system(datum_get<c_particle_system>(g_particle_system_data_array, location->get_system_index()));
    (&list)->set_particle_location(location);
    particle_state_list_set_particle(&list, particle);
    if (mask_a & k_particle_states_available_mask)
      *value_a = list.get_state_value(state_a);
    if (mask_b & k_particle_states_available_mask)
      *value_b = list.get_state_value(state_b);
  }
}

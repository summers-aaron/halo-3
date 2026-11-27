#ifndef EFFECTS_PARTICLE_STATES_H
#define EFFECTS_PARTICLE_STATES_H

#include "cseries/cseries_types.h"

// Functions defined in tu/blamlibxbox/particle_states.cpp.

class c_particle;
class c_particle_state_list;

extern "C" void particle_state_list_set_particle(c_particle_state_list *list, const c_particle *particle);

extern "C" unsigned long g_particle_default_used_states;

class c_particle_system;
class c_particle_system_definition;
class c_particle_location;

enum e_particle_state
{
    _particle_age = 0,
    _system_age = 1,
    _particle_random_1 = 4,
    _particle_random_2 = 5,
    _particle_random_3 = 6,
    _particle_random_4 = 7,
    _system_random_1 = 8,
    _system_random_2 = 9,
    _particle_correlation_1 = 10,
    _location_lod = 11,
    _game_time = 12,
    _effect_a_scale = 13,
    _effect_b_scale = 14,
    _physics_rotation = 15,
    _location_random = 16,
    _distance_from_emitter = 18,
    _particle_scale = 19,
    _particle_states_zero = 20,
    _particle_random_5 = 21,
    _particle_random_6 = 22,
    _particle_random_7 = 23,
    _particle_random_8 = 24,
    _system_random_3 = 25,
    _system_random_4 = 26,

    k_total_particle_states = 27
};

const unsigned long k_particle_states_mask = 0x1ee84f1;
const unsigned long k_system_states_mask = 0x6006302;
const unsigned long k_location_states_mask = (1 << _location_lod) | (1 << _location_random);

class c_particle_state_list {
  real m_state_values[k_total_particle_states];
  unsigned long m_used_states;
  unsigned long m_flags;
  const c_particle_system *m_particle_system;
  const c_particle_system_definition *m_particle_system_definition;
  const ::c_particle_location *m_particle_location;
  const c_particle *m_particle;
  friend void particle_state_list_set_particle(c_particle_state_list *list, const c_particle *particle);
  void update_states_internal(unsigned long states);
public:
  __forceinline c_particle_state_list() :
    m_used_states(g_particle_default_used_states),
    m_flags(1 << _particle_states_zero),
    m_particle_system(0),
    m_particle_system_definition(0),
    m_particle_location(0),
    m_particle(0)
  {
    m_state_values[_particle_states_zero] = 0.0f;
  }
  __forceinline real get_state_value(long state) const { return m_state_values[state]; }
  unsigned long get_particle_system_used_states() const;
  __forceinline void update_states(unsigned long mask) {
    unsigned long states=m_used_states & ~m_flags & mask;
    if(states) {update_states_internal(states); m_flags |= states;}
  }
  __forceinline void set_null_particle()
  {
    m_particle = 0;
    m_flags |= 0x1fe84f1;
    m_state_values[20] = 0.0f;
    m_state_values[0] = 0.0f;
    m_state_values[10] = 0.0f;
    m_state_values[4] = 0.0f;
    m_state_values[5] = 0.0f;
    m_state_values[6] = 0.0f;
    m_state_values[7] = 0.0f;
    m_state_values[21] = 0.0f;
    m_state_values[22] = 0.0f;
    m_state_values[23] = 0.0f;
    m_state_values[24] = 0.0f;
  }
  void set_particle_system(const c_particle_system *particle_system);
  __forceinline void set_particle(const c_particle *particle) {
    m_particle = particle;
    m_flags &= ~k_particle_states_mask;
    update_states(k_particle_states_mask);
  }
  void set_particle_location(const ::c_particle_location *location);
  __forceinline void update_particle_location(const ::c_particle_location *location)
  {
    if (location != m_particle_location) {
      unsigned long flags = m_flags & ~k_location_states_mask;
      m_particle_location = location;
      m_flags = flags;
      unsigned long states = m_used_states & ~flags & k_location_states_mask;
      if (states != 0) {
        update_states_internal(states);
        m_flags |= states;
      }
    }
  }
};

#endif // EFFECTS_PARTICLE_STATES_H

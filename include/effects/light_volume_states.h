#ifndef EFFECTS_LIGHT_VOLUME_STATES_H
#define EFFECTS_LIGHT_VOLUME_STATES_H

#include <stddef.h>

class c_light_volume_system;
class c_light_volume_location;
class c_light_volume;

extern "C" unsigned long k_light_volume_states_initial_flags;

// Twelve indexed state values.
class c_light_volume_states
{
public:
    c_light_volume_states()
    {
        m_values[11] = 0.0f;
        m_system = 0;
        m_location = 0;
        m_light_volume = 0;
        m_dirty = 0x800;
        m_flags = k_light_volume_states_initial_flags;
    }

    // The caller supplies the initial value for the trailing state.
    __forceinline c_light_volume_states(float zero)
    {
        m_values[11] = zero;
        m_system = 0;
        m_location = 0;
        m_light_volume = 0;
        m_dirty = 0x800;
        m_flags = k_light_volume_states_initial_flags;
    }

    float get_state_value(long state) const { return m_values[state]; }
    void invalidate_states(unsigned long mask) { m_dirty &= ~mask; }
    void set_light_volume_location(const c_light_volume_location *location)
    {
        if (location != m_location)
        {
            m_location = location;
            invalidate_states(0);
            update_states(0);
        }
    }
    void set_light_volume(const c_light_volume *light_volume)
    {
        if (light_volume != m_light_volume)
        {
            m_light_volume = light_volume;
            invalidate_states(96);
            update_states(96);
        }
    }
    unsigned long get_light_volume_system_used_states();
    void update_states(unsigned long mask);
    void set_light_volume_system(const c_light_volume_system *system);

private:
    const c_light_volume_system *m_system;
    const c_light_volume_location *m_location;
    const c_light_volume *m_light_volume;
    float m_values[12];
    unsigned long m_dirty;
    unsigned long m_flags;
};

typedef char light_volume_states_size[(sizeof(void *) != 4 || ((sizeof(c_light_volume_states) == 0x44) )) ? 1 : -1];

#endif // EFFECTS_LIGHT_VOLUME_STATES_H

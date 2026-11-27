#ifndef EFFECTS_CONTRAIL_STATES_TYPES_H
#define EFFECTS_CONTRAIL_STATES_TYPES_H

#include <stddef.h>

class c_contrail_system;
class c_contrail_location;
class c_contrail;
class c_contrail_profile;

extern "C" unsigned long k_contrail_default_used_flags;

// Sixteen cached state values per contrail.
// March contrail_states.h:107 attests get_state_value; the remaining methods are
// emitted by contrails.obj and contrail_states.obj in the March debug build.
class c_contrail_states
{
public:
    c_contrail_states()
    {
        system = 0;
        location = 0;
        contrail = 0;
        profile = 0;
        values[15] = 0.0f;
        flags = 0x8000;
        used_flags = k_contrail_default_used_flags;
    }

    float get_state_value(long state) const { return values[state]; }
    void invalidate_states(unsigned long mask) { flags &= ~mask; }
    void set_contrail_profile(const ::c_contrail_profile *new_profile)
    {
        profile = new_profile;
        invalidate_states(0x3d);
        update_states(0x3d);
    }
    void set_contrail_location(const c_contrail_location *new_location)
    {
        if (new_location != location)
        {
            location = new_location;
            invalidate_states(0x800);
            update_states(0x800);
        }
    }
    void set_contrail(const ::c_contrail *new_contrail)
    {
        if (new_contrail != contrail)
        {
            contrail = new_contrail;
            invalidate_states(0x600);
            update_states(0x600);
        }
    }
    unsigned long get_contrail_system_used_states();
    void set_contrail_system(const c_contrail_system *new_system);
    void update_states(unsigned long mask);

    const c_contrail_system *system;
    const c_contrail_location *location;
    const ::c_contrail *contrail;
    const ::c_contrail_profile *profile;
    float values[16];
    unsigned long flags;
    unsigned long used_flags;
};

typedef char contrail_states_size[(sizeof(void *) != 4 || ((sizeof(c_contrail_states) == 0x58) )) ? 1 : -1];
typedef char contrail_states_location_offset[(sizeof(void *) != 4 || ((offsetof(c_contrail_states, location) == 4) )) ? 1 : -1];
typedef char contrail_states_contrail_offset[(sizeof(void *) != 4 || ((offsetof(c_contrail_states, contrail) == 8) )) ? 1 : -1];
typedef char contrail_states_profile_offset[(sizeof(void *) != 4 || ((offsetof(c_contrail_states, profile) == 12) )) ? 1 : -1];
typedef char contrail_states_values_offset[(sizeof(void *) != 4 || ((offsetof(c_contrail_states, values) == 16) )) ? 1 : -1];
typedef char contrail_states_flags_offset[(sizeof(void *) != 4 || ((offsetof(c_contrail_states, flags) == 80) )) ? 1 : -1];
typedef char contrail_states_used_flags_offset[(sizeof(void *) != 4 || ((offsetof(c_contrail_states, used_flags) == 84) )) ? 1 : -1];

#endif // EFFECTS_CONTRAIL_STATES_TYPES_H

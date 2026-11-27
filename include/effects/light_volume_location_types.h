#ifndef EFFECTS_LIGHT_VOLUME_LOCATION_TYPES_H
#define EFFECTS_LIGHT_VOLUME_LOCATION_TYPES_H
#include <stddef.h>
#include "memory/data_types.h"
#include "effects/light_volume_types.h"
#include "math/real_math_types.h"

class c_light_volume_location
{
public:
    static void dispose_from_game();
    void add_sibling(long sibling_index);
    static void destroy(long location_index);
    static __forceinline c_light_volume_location *get(long index);
public:
    unsigned short m_identifier;
    unsigned short m_flags;
    long m_sibling_index;
    long m_system_index;
    long m_light_volume_index;
    unsigned long first_person_flags;
    real_point3d m_position;
    real_vector3d m_forward;
    float m_length;
    __forceinline void set_first_person(bool value) { if (value) first_person_flags |= 1; else first_person_flags &= ~1; }
    __forceinline const real_point3d *get_origin() const { return &m_position; }
    __forceinline const c_light_volume_system *get_parent_system() const
    {
        return c_light_volume_system::get(m_system_index);
    }
};


typedef char light_volume_location_size[(sizeof(c_light_volume_location) == 0x30) ? 1 : -1];

#endif

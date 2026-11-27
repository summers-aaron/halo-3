#ifndef UNITS_VEHICLE_TYPE_COMPONENT_H
#define UNITS_VEHICLE_TYPE_COMPONENT_H

#include "math/real_math_types.h"

// Functions defined in tu/objectslibxbox/vehicle_type_component.cpp that other units call.

union real_argb_color;

const real_argb_color *objects_get_default_argb_white();

struct s_vehicle_auto_control;

extern "C" s_vehicle_auto_control *vehicle_get_auto_control(void *storage, long object_index);

struct s_havok_vehicle_physics_instance;
class c_vehicle_type_human_tank;
class c_vehicle_type_human_jeep;
class c_vehicle_type_human_plane;
class c_vehicle_type_alien_scout;
class c_vehicle_type_alien_fighter;
class c_vehicle_type_turret;
class c_vehicle_type_mantis;
class c_vehicle_type_guardian;
class c_vehicle_type_chopper;
class c_vehicle_type_vtol;

class c_vehicle_type_component
{
public:
    unsigned char m_storage[96];
    signed char m_type;
    __forceinline bool vector_is_upsides_down(long vehicle_index, const real_vector3d *up);
    bool is_e_braking(long object_index);
    void reset(long object_index);
    bool kills_riders_at_terminal_velocity(long object_index);
    bool compute_function_value(long vehicle_index, long function, float *value, bool *active);
    bool should_override_deactivation(long object_index);
    void update_control(long object_index);
    void update_physics(long vehicle_index, s_havok_vehicle_physics_instance *instance);
    void adjust_gravity(long object_index, real_vector3d *gravity);
    __forceinline c_vehicle_type_human_tank *get_type_human_tank() { return (c_vehicle_type_human_tank *)this; }
    __forceinline c_vehicle_type_human_jeep *get_type_human_jeep() { return (c_vehicle_type_human_jeep *)this; }
    __forceinline c_vehicle_type_human_plane *get_type_human_plane() { return (c_vehicle_type_human_plane *)this; }
    __forceinline c_vehicle_type_alien_scout *get_type_alien_scout() { return (c_vehicle_type_alien_scout *)this; }
    __forceinline c_vehicle_type_alien_fighter *get_type_alien_fighter() { return (c_vehicle_type_alien_fighter *)this; }
    __forceinline c_vehicle_type_turret *get_type_turret() { return (c_vehicle_type_turret *)this; }
    __forceinline c_vehicle_type_mantis *get_type_mantis() { return (c_vehicle_type_mantis *)this; }
    __forceinline c_vehicle_type_guardian *get_type_guardian() { return (c_vehicle_type_guardian *)this; }
    __forceinline c_vehicle_type_chopper *get_type_chopper() { return (c_vehicle_type_chopper *)this; }
    __forceinline c_vehicle_type_vtol *get_type_vtol() { return (c_vehicle_type_vtol *)this; }
    void deplete_function_variables(long object_index);
};

#endif // UNITS_VEHICLE_TYPE_COMPONENT_H

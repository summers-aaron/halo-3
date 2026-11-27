#ifndef EFFECTS_PARTICLE_MOVEMENT_INTERNALS_H
#define EFFECTS_PARTICLE_MOVEMENT_INTERNALS_H

#include "math/real_math_types.h"
#include "physics/collision_types.h"
#include "tag_files/tag_groups.h"

class c_particle_location;
class c_particle_state_list;
class c_particle_system;

#include "effects/editable_property.h"

class c_particle_collision_event
{
public:
    void initialize(const real_point3d *position, const real_vector3d *direction, const real_vector3d *normal, c_global_material_type material_type);

    real_point3d m_position;
    real_vector3d m_forward;
    real_vector3d m_normal;
    c_global_material_type m_material_type;
};

class c_particle_controller_parameter
{
public:
    __forceinline const c_editable_property<c_particle_state_list> &get_property() const
    {
        return m_property;
    }

private:
    long m_parameter_id;
    c_editable_property<c_particle_state_list> m_property;
};

enum e_particle_controller_type
{
    _particle_controller_type_physics = 0,
    _particle_controller_type_collider,
    _particle_controller_type_flocking,
    _particle_controller_type_wind,
    k_particle_controller_type_count
};

class c_particle_controller
{
public:
    __forceinline unsigned short get_type() const { return m_type; }
    __forceinline const c_particle_controller_parameter &get_parameter_by_index(long index) const
    {
        return *TAG_BLOCK_GET_ELEMENT(&m_parameters, index, const c_particle_controller_parameter);
    }
    __forceinline void update_particles(
        const c_particle_system *system, const c_particle_location *location, long particle_count, float dt, unsigned long flags) const
    {
        switch (get_type())
        {
        case _particle_controller_type_physics:
            update_particles_physics(system, location, particle_count, dt);
            break;
        case _particle_controller_type_collider:
            update_particles_collider(system, location, particle_count, dt, flags);
            break;
        case _particle_controller_type_flocking:
            update_particles_flocking(system, location, particle_count, dt);
            break;
        case _particle_controller_type_wind:
            update_particles_wind(system, location, particle_count, dt);
            break;
        }
    }

private:
    void update_particles_physics(const c_particle_system *, const c_particle_location *, long, float) const;
    void update_particles_collider(const c_particle_system *, const c_particle_location *, long, float, unsigned long) const;
    void update_particles_flocking(const c_particle_system *, const c_particle_location *, long, float) const;
    void update_particles_wind(const c_particle_system *, const c_particle_location *, long, float) const;
    unsigned short m_type;
    unsigned short m_flags;
    s_tag_block m_parameters;
    long m_runtime_data[2];
};

class c_particle_movement_definition
{
public:
    __forceinline const c_particle_controller *get_controller_by_index(long index) const
    {
        return TAG_BLOCK_GET_ELEMENT(&m_controllers, index, const c_particle_controller);
    }
    __forceinline const c_particle_controller *get_controller_by_type(long type) const
    {
        const c_particle_controller *result = 0;
        for (long index = 0; index < m_controllers.count; ++index)
        {
            const c_particle_controller *controller = TAG_BLOCK_GET_ELEMENT(&m_controllers, index, const c_particle_controller);
            if (controller->get_type() == type)
            {
                result = controller;
                break;
            }
        }
        return result;
    }
    const c_editable_property<c_particle_state_list> *get_property_by_index(long type, long index) const;
    void update_particles(const c_particle_system *system, const c_particle_location *location, long particle_count, float dt) const;

    char m_definition_header[16];
    unsigned long m_flags;
    s_tag_block m_controllers;
};

#include "effects/particle_system.h"
#include "effects/particle_system_definitions.h"
#include "effects/effect_definitions.h"
#include "effects/effect_event_definitions.h"

struct s_particle_movement_effect_event
{
    long name;
    unsigned long flags;
    unsigned char limit_type;
    float skip_fraction;
    real_bounds delay_bounds;
    real_bounds duration_bounds;
    s_tag_block parts;
    s_tag_block accelerations;
    c_typed_tag_block<c_particle_system_definition> particle_systems;
};

typedef char particle_movement_effect_event_size[(sizeof(s_particle_movement_effect_event) == 0x44) ? 1 : -1];

__forceinline const c_particle_system_definition *c_particle_system::get_particle_system_definition() const
{
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, m_tag_index);
    const effect_event_definition *event_definition = TAG_BLOCK_GET_ELEMENT(&definition->events, m_def_index, const effect_event_definition);
    return TAG_BLOCK_GET_ELEMENT(&event_definition->particle_systems, m_sub_index, const c_particle_system_definition);
}

#endif // EFFECTS_PARTICLE_MOVEMENT_INTERNALS_H

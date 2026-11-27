#ifndef EFFECTS_PARTICLE_EMITTER_H
#define EFFECTS_PARTICLE_EMITTER_H

#include "math/matrix_math_types.h"

class c_particle_system;
class c_particle_system_definition;
class c_particle_emitter_definition;
class c_particle_state_list;
class c_particle_emitter;
class c_particle;

// Functions defined in tu/blamlibxbox/particle_emitter.cpp.

extern "C" {
extern bool g_particle_emitter_type_created[4];
}

extern "C" long particle_emitter_new(unsigned char type, long location_index, long unit_index);
extern "C" void particle_emitter_register_allocator_init(void);
extern "C" void particle_emitter_install_allocator(void);
extern "C" long particle_emitter_spawn_particles(c_particle_emitter *emitter, float delta, c_particle_state_list *state_list);

class c_particle_emitter
{
public:
    struct s_property_values
    {
        union
        {
            real_point3d emitter_offset;
            real_vector3d emitter_offset_vector;
        };
        real_vector3d emitter_direction;
        float emission_radius;
        float emission_angle;
        float emission_axis_angle;
        float particle_lifespan;
        float particle_velocity;
        float particle_angular_velocity;
        float emission_rate;
        float starting_count;
        float max_count;
    };

    static void initialize_for_map();
    static void dispose_from_map();
    static void dispose_from_game();
    __forceinline s_property_values *get_property_values() { return &m_property_values; }
    const real_point3d *get_position_world(real_point3d *point, const c_particle_system *system,
        const c_particle_system_definition *definition) const;
    float calc_matrix(const c_particle_emitter_definition *definition, const c_particle_system *system,
        const c_particle_state_list *states);
    __forceinline void set_rendered_last_frame(bool rendered)
    {
        if (rendered)
            m_flags |= 1;
        else
            m_flags |= 2;
    }

    short m_identifier;
    unsigned short m_count;
    unsigned short m_flags;
    unsigned short m_header;
    long m_particle_index;
    long m_next_emitter;
    long m_gpu_index;
    long m_location_index;
    long m_definition_index;
    s_property_values m_property_values;
    float m_accumulated_count;
    real_matrix4x3 m_matrix;

private:
    friend long particle_emitter_spawn_particles(c_particle_emitter *emitter, float delta, c_particle_state_list *state_list);
    void spawn_particle(c_particle_state_list *state_list, c_particle_system *system,
        const c_particle_emitter_definition *definition, const real_point3d &point, float t, float scale, float velocity_scale);
    void set_matrix_initialized(bool initialized)
    {
        if (initialized)
            m_flags |= 4;
        else
            m_flags &= ~4;
    }
};

typedef char particle_emitter_properties_size[(sizeof(c_particle_emitter::s_property_values) == 0x3c) ? 1 : -1];
typedef char particle_emitter_size[(sizeof(c_particle_emitter) == 0x90) ? 1 : -1];


extern "C" c_particle g_particle_emitter_gpu_scratch_particle;
extern "C" float particle_emitter_compute_matrix(c_particle_emitter *emitter, const c_particle_emitter_definition *definition,
    const c_particle_system *system, const c_particle_state_list *states);

extern "C" int particle_emitter_update_particles(c_particle_emitter *emitter, c_particle_state_list *state_list, float delta);

extern "C" void particle_emitter_render(c_particle_emitter *emitter, long pass);

#endif // EFFECTS_PARTICLE_EMITTER_H

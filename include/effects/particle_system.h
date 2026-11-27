#ifndef EFFECTS_PARTICLE_SYSTEM_H
#define EFFECTS_PARTICLE_SYSTEM_H

#include "objects/object_reference_types.h"
#include "math/real_math_types.h"
#include "tag_files/tag_index.h"

extern "C" void particle_system_create_data_arrays(void);
struct s_game_non_bsp_zone_set;
class c_scenario_resource_registry;
extern "C" void effects_delete_unused_for_new_map(const s_game_non_bsp_zone_set *old_zone_set, const s_game_non_bsp_zone_set *new_zone_set,
    c_scenario_resource_registry *pending_zone_registry);

class c_particle_state_list;
class c_particle_system;
class c_particle_definition;
class c_particle_system_definition;
class c_particle;
struct s_particle_location_datum;

namespace particle_system_lod { float compute_distance_fade(const c_particle_system *system, float distance); }
extern "C" float particle_location_compute_fade(const s_particle_location_datum *location, long user_index);
extern "C" int particle_location_update_emitters(s_particle_location_datum *location, c_particle_state_list *state_list, float delta);
extern "C" void particle_system_render(c_particle_system *system, long user_index, bool first_person, long pass);
extern "C" void particle_emitter_gpu_initialize_storage(void);

class c_particle_system
{
public:
    static long create(const c_particle_system_definition *definition, long effect_index, c_tag_index tag_index,
        unsigned long sub_index, unsigned short def_index, unsigned short location_index, unsigned long seed);
    static void initialize_for_map();
    static void dispose_from_map();
    static void dispose_from_game();
    static void initialize_for_new_structure_bsp();
    static void frame_advance_all(float delta);

    const c_particle_system_definition *get_particle_system_definition() const;
    const c_particle_definition *get_particle_definition() const;
    __forceinline float get_age() const { return m_age; }
    __forceinline float get_random_seed(unsigned long index) const { return m_random_values[index]; }
    __forceinline float get_exported_function(unsigned long index) const
    {
        switch (index)
        {
        case 0: return m_effect_scale_a;
        case 1: return m_effect_scale_b;
        }
        return 0.0f;
    }
    __forceinline long get_effect_index() const { return m_effect_index; }
    __forceinline long get_first_location_index() const { return m_first_location; }
    void initialize_particle(const c_particle_state_list *state_list, c_particle *particle) const;
    unsigned short internal_random() const;
    float real_internal_random_range(float lower, float upper) const;
    void internal_random_direction3d(real_vector3d *direction) const;
    void query_effect();
    long frame_advance(float delta);
    long add_particle_location();
    void remove_further_particle_locations(long location_index);

    unsigned short m_identifier;
    unsigned short m_header;
    float m_age;
    unsigned short m_flags;
    unsigned short m_sub_index;
    long m_tag_index;
    unsigned char m_def_index;
    unsigned char m_type;
    unsigned short m_location_index;
    long m_effect_index;
    long m_lightprobe_index;
    long m_breakable_surface_event_index;
    s_location m_location;
    float m_effect_scale_a;
    float m_effect_scale_b;
    mutable unsigned long m_random_seed;
    float m_random_values[4];
    long m_first_location;
    long m_last_location;
    long m_next_system;
    long m_previous_system;
    long m_object_index;
    short m_player_mask;
    unsigned short m_count;

private:
    friend void effects_delete_unused_for_new_map(const s_game_non_bsp_zone_set *, const s_game_non_bsp_zone_set *, c_scenario_resource_registry *);
    void release_particles();
    __forceinline void set_flag(unsigned short mask, bool value)
    {
        if (value)
            m_flags |= mask;
        else
            m_flags &= ~mask;
    }
};

typedef char particle_system_size[(sizeof(c_particle_system) == 0x58) ? 1 : -1];


extern "C" bool g_particle_system_type_created[];
extern "C" float g_particle_inv_render_delta;
extern "C" unsigned long g_particle_default_used_states;
extern "C" char k_particle_emitter_array_name[];
template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
void __tls_set_g_object_list_data_allocator(void *object);
void __tls_set_g_particle_system_data_array_allocator(void *allocation);
extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_list_data_allocator> g_particle_allocator;
extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_particle_system_data_array_allocator> g_particle_system_allocator;


extern "C" void effect_update_particle_query_record(c_particle_system *system);

extern "C" long particle_system_compute_visibility(const c_particle_system *system, long player_index, bool *out_first_person);
#endif // EFFECTS_PARTICLE_SYSTEM_H

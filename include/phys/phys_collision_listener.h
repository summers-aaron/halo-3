#ifndef INCLUDE_PHYS_COLLISION_LISTENER_H
#define INCLUDE_PHYS_COLLISION_LISTENER_H

#include "phys/phys_base_types.h"

class phys_collision_proxy;
class phys_collider;
class phys_entity;
class phys_contact_point;
class phys_contact_point_material;
class phys_contact_point_props;
class phys_simplex_cache;
class phys_dynamics_contact_manager;
class phys_constraint_owner;
class phys_simple_contact_data;
struct phys_process_collision_input;
struct phys_process_collision_output;
struct phys_process_collision_data;

enum phys_contact_point_accept {
    PHYS_CONTACT_POINT_ACCEPT = 0,
    PHYS_CONTACT_POINT_REJECT = 1
};

struct phys_contact_added_event {
    enum Type { COLLISION_EVENT_TOI = 0, TYPE_MANIFOLD = 1 };

    const phys_collision_proxy *m_body_a;
    const phys_collision_proxy *m_body_b;
    Type m_type;
    phys_entity *m_callback_fired_from;
    const phys_contact_point *m_contact_point;
    const phys_simplex_cache *m_simplex_cache;
    phys_contact_point_material *m_contact_point_material;
    float m_projected_vel;
    phys_contact_point_accept m_status;
    phys_dynamics_contact_manager *m_internal_contact_manager;
    const phys_process_collision_input *m_collision_input;
    phys_process_collision_output *m_collision_output;

    __forceinline phys_contact_added_event(phys_dynamics_contact_manager *manager,
        const phys_process_collision_input *input, phys_process_collision_output *output,
        const phys_collision_proxy *agent_body_a, const phys_collision_proxy *agent_body_b, phys_contact_point *point,
        const phys_simplex_cache *cache, phys_contact_point_material *material, float contact_projected_vel, Type type)
    {
        m_body_a = agent_body_a;
        m_body_b = agent_body_b;
        m_type = type;
        m_contact_point = point;
        m_simplex_cache = cache;
        m_contact_point_material = material;
        m_projected_vel = contact_projected_vel;
        m_internal_contact_manager = manager;
        m_collision_input = input;
        m_collision_output = output;
        m_status = PHYS_CONTACT_POINT_ACCEPT;
    }
};

struct phys_contact_confirmed_event {
    const phys_collider *m_collider_a;
    const phys_collider *m_collider_b;
    phys_entity *m_callback_fired_from;
    phys_contact_point *m_contact_point;
    phys_contact_point_material *m_contact_point_material;
    float m_rotate_normal;
    float m_projected_vel;
    phys_contact_added_event::Type m_type;
    const phys_simple_contact_data *m_contact_data;

    __forceinline phys_contact_confirmed_event(phys_contact_added_event::Type type,
        const phys_collider *pair_collider_a, const phys_collider *pair_collider_b,
        const phys_simple_contact_data *contact_data, phys_contact_point *point,
        phys_contact_point_material *material, float rotate_normal, float contact_projected_vel)
    {
        m_collider_a = pair_collider_a;
        m_collider_b = pair_collider_b;
        m_contact_point = point;
        m_contact_point_material = material;
        m_rotate_normal = rotate_normal;
        m_projected_vel = contact_projected_vel;
        m_type = type;
        m_contact_data = contact_data;
    }
    __forceinline phys_bool is_toi_event() const { return phys_bool(m_type == phys_contact_added_event::COLLISION_EVENT_TOI); }
    unsigned short get_contact_point_id() const;
    const phys_dynamics_contact_manager *get_contact_manager() const;
};

struct phys_contact_removed_event {
    unsigned short m_process_contact_id;
    phys_contact_point_material *m_contact_point_material;
    phys_entity *m_entity_a;
    phys_entity *m_entity_b;
    phys_entity *m_callback_fired_from;
    phys_dynamics_contact_manager *m_internal_contact_manager;
    phys_constraint_owner *m_constraint_owner;

    __forceinline phys_contact_removed_event(unsigned short id, phys_dynamics_contact_manager *manager,
        phys_constraint_owner *owner, phys_contact_point_material *material, phys_entity *listener_entity_a, phys_entity *listener_entity_b)
    {
        m_process_contact_id = id;
        m_contact_point_material = material;
        m_entity_a = listener_entity_a;
        m_entity_b = listener_entity_b;
        m_internal_contact_manager = manager;
        m_constraint_owner = owner;
    }
};

struct phys_contact_process_event {
    const phys_collider *m_collider_a;
    const phys_collider *m_collider_b;
    phys_entity *m_callback_fired_from;
    phys_process_collision_data *m_collision_data;
    phys_contact_point_props *m_contact_point_props[256];
    phys_dynamics_contact_manager *m_internal_contact_manager;

    __forceinline phys_contact_process_event(phys_dynamics_contact_manager *manager, const phys_collider *pair_collider_a,
        const phys_collider *pair_collider_b, phys_process_collision_data *data)
    {
        m_collider_a = pair_collider_a;
        m_collider_b = pair_collider_b;
        m_collision_data = data;
        m_internal_contact_manager = manager;
    }
};

class phys_collision_listener {
public:
    virtual void contact_point_added_callback(phys_contact_added_event &event) = 0;
    virtual void contact_point_confirmed_callback(phys_contact_confirmed_event &event) = 0;
    virtual void contact_point_removed_callback(phys_contact_removed_event &event) = 0;
    virtual void contact_process_callback(phys_contact_process_event &event) {}
    virtual ~phys_collision_listener();
};

typedef char phys_contact_added_event_size[(sizeof(void *) != 4 || ((sizeof(phys_contact_added_event) == 48) )) ? 1 : -1];
typedef char phys_contact_confirmed_event_size[(sizeof(void *) != 4 || ((sizeof(phys_contact_confirmed_event) == 36) )) ? 1 : -1];
typedef char phys_contact_removed_event_size[(sizeof(void *) != 4 || ((sizeof(phys_contact_removed_event) == 28) )) ? 1 : -1];
typedef char phys_contact_process_event_size[(sizeof(void *) != 4 || ((sizeof(phys_contact_process_event) == 1044) )) ? 1 : -1];
typedef char phys_contact_added_callback_offset[(sizeof(void *) != 4 || ((offsetof(phys_contact_added_event, m_callback_fired_from) == 12) )) ? 1 : -1];
typedef char phys_contact_confirmed_callback_offset[(sizeof(void *) != 4 || ((offsetof(phys_contact_confirmed_event,
    m_callback_fired_from) == 8) )) ? 1 : -1];
typedef char phys_contact_removed_callback_offset[(sizeof(void *) != 4 || ((offsetof(phys_contact_removed_event, m_callback_fired_from) == 16) )) ? 1 : -1];
typedef char phys_contact_process_callback_offset[(sizeof(void *) != 4 || ((offsetof(phys_contact_process_event, m_callback_fired_from) == 8) )) ? 1 : -1];
typedef char phys_contact_process_manager_offset[(sizeof(void *) != 4 || ((offsetof(phys_contact_process_event,
    m_internal_contact_manager) == 1040) )) ? 1 : -1];

#endif

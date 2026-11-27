#ifndef PHYSICS_HAVOK_ENTITY_LISTENER_H
#define PHYSICS_HAVOK_ENTITY_LISTENER_H

#include "phys/phys_broadphase_border.h"
#include "phys/phys_entity_activation_listener.h"
#include "phys/phys_collide.h"
#include "phys/phys_collision_listener.h"
#include "phys/phys_critical_section.h"
#include "phys/phys_dynamics.h"
#include "game/materials.h"

struct phys_contact_process_event;
struct phys_contact_removed_event;
class phys_dynamics_contact_manager;


class c_broad_phase_border : public phys_broadphase_border {
public:
    static __forceinline void *operator new(size_t size) throw()
    {
        phys_thread_memory *memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
        phys_ref_object *object = (phys_ref_object *)memory->allocate_chunk((int)size, (phys_memory_class)12);
        object->m_mem_size_and_flags = (unsigned short)size;
        return object;
    }
    static __forceinline void operator delete(void *storage)
    {
        phys_ref_object *object = (phys_ref_object *)storage;
        phys_thread_memory *memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
        memory->free_chunk(storage, object->m_mem_size_and_flags, (phys_memory_class)12);
    }

    __forceinline c_broad_phase_border(phys_world *world) : phys_broadphase_border(world, phys_world_desc::BROADPHASE_BORDER_ASSERT) {}
    virtual void on_max_position_exceeded(phys_entity *entity);
    virtual void collider_added_callback(const phys_collider_added_event &event);
};

#pragma pack(push, 4)
class c_entity_listener : public phys_entity_activation_listener, public phys_collision_listener {
public:
    __forceinline c_entity_listener() : critical_section(0)
    {
        deactivated_entity_index = -1;
        queue_deactivated_entities = false;
        clear_deactivated_entity_queue();
    }
    virtual void on_entity_deactivated(phys_entity *entity);
    virtual void on_entity_activated(phys_entity *entity) {}
    virtual void contact_point_added_callback(phys_contact_added_event &event);
    virtual void contact_point_confirmed_callback(phys_contact_confirmed_event &event) {}
    virtual void contact_point_removed_callback(phys_contact_removed_event &event);
    virtual void contact_process_callback(phys_contact_process_event &event);
    phys_array<phys_entity *> deactivated_entities;
    int deactivated_entity_index;
    static __forceinline c_entity_listener *get_global_entity_listener();
    bool is_deactivated_entity_queue_empty();
    bool queue_deactivated_entities;
    __declspec(align(8)) phys_critical_section critical_section;
    void clear_deactivated_entity_queue();
    __forceinline phys_entity *dequeue_deactivated_entity() {
        deactivated_entity_index++;
        phys_entity *entity = deactivated_entities[deactivated_entity_index];
        entity->object_remove_ref();
        return entity;
    }
    __forceinline void queue_deactivated_entity(phys_entity *entity) {
        if (queue_deactivated_entities) {
            entity->add_object_reference();
            deactivated_entities.array_push_back(entity);
        }
    }
private:
    void add_contact_point_to_havok_component(bool fired_from_b, phys_dynamics_contact_manager *contact_manager, unsigned short contact_point_id,
        const phys_collision_proxy *body, const phys_collision_proxy *other_body, phys_rigid_body *rigid_body, phys_rigid_body *other_rigid_body,
        const phys_collision_proxy *other_cd_body);
    void calculate_contact_point_properties(float *friction, float *restitution, c_global_material_type *material_a,
        c_global_material_type *material_b, const phys_collision_proxy *body_a, const phys_collision_proxy *body_b, long component_a, long component_b,
        const phys_rigid_body *rigid_body_a, const phys_rigid_body *rigid_body_b, unsigned int key_a, unsigned int key_b,
        const real_point3d *position);
};
#pragma pack(pop)

typedef char havok_entity_listener_section_offset[
    (sizeof(void *) != 4 || offsetof(c_entity_listener, critical_section) == 0x20) ? 1 : -1];
typedef char havok_entity_listener_size[(sizeof(void *) != 4 || sizeof(c_entity_listener) == 0x50) ? 1 : -1];

class c_world_entity_listener : public phys_collision_listener {
public:
    virtual void contact_point_added_callback(phys_contact_added_event &event) {}
    virtual void contact_point_confirmed_callback(phys_contact_confirmed_event &event) {}
    virtual void contact_point_removed_callback(phys_contact_removed_event &event) {}
};

typedef char havok_world_entity_listener_size[(sizeof(void *) != 4 || sizeof(c_world_entity_listener) == 4) ? 1 : -1];

// Storage the listeners are constructed into when havok initializes for a new map.
extern "C" {
extern __declspec(align(8)) char g_entity_listener_storage[sizeof(c_entity_listener)];
extern __declspec(align(4)) char g_world_entity_listener_storage[sizeof(c_world_entity_listener)];
}

#endif

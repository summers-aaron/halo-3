// blamlibxbox/havok_listeners.obj: from the records 0x104ce8 0x105df0 0x105e70 0x106158 0x106280 0x107328 0x107350 0x2604f0 0x2606b8 0x2607b0 0x260860 0x2608a0 0x260998 0x260a70 0x260b70 0x708350 0x70be98
// @flags /TP /O2 /Oc
// @address 0x104ce8 ?contact_process_callback@c_entity_listener@@UAAXAAUphys_contact_process_event@@@Z
// @address 0x106158 ?contact_point_added_callback@c_entity_listener@@UAAXAAUphys_contact_added_event@@@Z
// @address 0x2606b8 ?on_entity_deactivated@c_entity_listener@@UAAXPAVphys_entity@@@Z
// @address 0x260860 ?collider_added_callback@c_broad_phase_border@@UAAXABUphys_collider_added_event@@@Z
// @address 0x2608a0 ?error_set_enabled@c_havok_error@@UAAXHVphys_bool@@@Z
// @address 0x260998 ?is_error_enabled@c_havok_error@@UAA?AVphys_bool@@H@Z
#include "phys/phys_collide.h"
#include "phys/phys_broadphase_border.h"
#include "phys/phys_critical_section.h"
#include "phys/phys_critical_section_entry.h"
#include "phys/phys_manifold_point_added_event.h"
#include "phys/phys_entity_activation_listener.h"
#include "physics/havok_contact_point.h"
#include "physics/havok_collision_surface_shape.h"
#include "math/real_math.h"
#include "memory/data.h"
#include "physics/havok_component.h"
#include "physics/havok_utilities.h"
#include "phys/phys_volume_overlap_listener.h"
#include "game/materials.h"
#include "physics/havok_contact_points.h"
#include "physics/havok_listeners.h"
#include "phys/phys_dynamics_contact_manager.h"
#include "phys/phys_world_deletion_listener.h"

#include "physics/havok_component_class.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_statistics_collector.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_memory_class.h"
#include "memory/data_types.h"

enum e_havok_listener_warning
{
    _havok_warning_invalid_response_velocities = (int)0xad45d441,
    _havok_warning_large_list_shape = 0x2ff8c16c,
    _havok_warning_large_bv_tree_shape = 0x2ff8c16d,
    _havok_warning_root_transform_shape = 0x2ff8c16f,
    _havok_warning_create_convex_geometry = 0x34df5494,
    _havok_warning_raycast_outside_broadphase = 0x38f1276d,
    _havok_warning_collection_without_bv_tree = 0x578cef50,
    _havok_warning_runtime_mopp_build = 0x6e8d163b
};
// @address 0x105df0 ?contact_point_removed_callback@c_entity_listener@@UAAXAAUphys_contact_removed_event@@@Z
// @address 0x107328 ?is_deactivated_entity_queue_empty@c_entity_listener@@QAA_NXZ
// @address 0x2607b0 ?on_max_position_exceeded@c_broad_phase_border@@UAAXPAVphys_entity@@@Z
// @address 0x260a70 ?message@c_havok_error@@UAAHW4error_message@phys_error@@HPBD1H@Z

struct phys_contact_process_event;
class c_havok_contact_point;

#include "physics/havok_entity_listener.h"
static __forceinline bool listener_rigid_body_is_dynamic(const c_havok_component *component, long index)
{
    bool result = component->m_rigid_bodies[index].m_rigid_body->get_motion_type() != phys_motion::MOTION_FIXED &&
        component->m_rigid_bodies[index].m_rigid_body->get_motion_type() != phys_motion::MOTION_KEYFRAMED;
    return result;
}

static __forceinline void listener_set_rigid_body_exceeded_broadphase(c_havok_component *component, long index, bool enable)
{
    if (enable) component->m_rigid_bodies[index].m_flags |= 8;
    else component->m_rigid_bodies[index].m_flags &= ~8;
}

static __forceinline c_havok_component *listener_component_get(long index)
{
    return datum_get<c_havok_component>(g_havok_component_data, index);
}

struct phys_contact_removed_event;



bool c_entity_listener::is_deactivated_entity_queue_empty()
{
    return deactivated_entity_index + 1 >= deactivated_entities.getSize();
}

void c_entity_listener::clear_deactivated_entity_queue()
{
    while (!is_deactivated_entity_queue_empty()) {
        dequeue_deactivated_entity();
    }
    deactivated_entity_index = -1;
    deactivated_entities.clear();
}

// @address 0x2604f0 ??_Gphys_entity_activation_listener@@UAAPAXI@Z
phys_entity_activation_listener::~phys_entity_activation_listener()
{
}



void c_entity_listener::on_entity_deactivated(phys_entity *entity)
{
    havok_critical_section_enter(&critical_section);
    queue_deactivated_entity(entity);
    critical_section.leave();
}

void c_broad_phase_border::on_max_position_exceeded(phys_entity *entity) {
 if (entity) {
  long component_index = ::havok_entity_get_havok_component_index(entity);
  long rigid_body_index = ::havok_entity_get_havok_component_rigid_body_index(entity);
  if (component_index != -1) {
   c_havok_component *component = listener_component_get(component_index);
   if (listener_rigid_body_is_dynamic(component, rigid_body_index)) listener_set_rigid_body_exceeded_broadphase(component, rigid_body_index, true);
  }
 }
}

void c_broad_phase_border::collider_added_callback(const phys_collider_added_event &event)
{
    on_max_position_exceeded((phys_entity *)phys_get_rigid_body(event.m_collider));
}

void c_havok_error::error_set_enabled(int id, phys_bool enable)
{
    if (enable) {
        for (long i = 0; i < count; i++) {
            if (disabled[i] == id) {
                count--;
                for (long j = i; j < count; j++) {
                    disabled[i] = disabled[i + 1];
                }
                break;
            }
        }
    } else {
        if (is_error_enabled(id)) {
            disabled[count] = id;
            count++;
        }
    }
}

phys_bool c_havok_error::is_error_enabled(int id)
{
    phys_bool enabled(true);
    switch (id) {
    case _havok_warning_invalid_response_velocities:
    case _havok_warning_large_list_shape:
    case _havok_warning_large_bv_tree_shape:
    case _havok_warning_root_transform_shape:
    case _havok_warning_create_convex_geometry:
    case _havok_warning_raycast_outside_broadphase:
    case _havok_warning_collection_without_bv_tree:
    case _havok_warning_runtime_mopp_build:
        enabled = phys_bool(false);
        break;
    }
    for (long i = 0; i < count; i++) {
        if (disabled[i] == id) {
            enabled = phys_bool(false);
            break;
        }
    }
    return enabled;
}

int c_havok_error::message(phys_error::error_message severity, int id, const char *description, const char *file, int line)
{
    if (is_error_enabled(id)) {
        switch (severity) {
        case phys_error::MESSAGE_REPORT:
            return 1;
        case phys_error::MESSAGE_WARNING:
            return 1;
        case phys_error::MESSAGE_ASSERT:
            return 1;
        case phys_error::MESSAGE_ERROR:
            return 1;
        // match: 0x260a70: without it the severity jump table gains a `cmplwi r31,3; bgt` range check
        default: __assume(0);
        }
    }
    return 0;
}

// @address 0x260b70 ??_Gphys_collision_listener@@UAAPAXI@Z
phys_collision_listener::~phys_collision_listener()
{
}



__forceinline void c_havok_component::notify_contact_point_process(
    phys_contact_process_event *e, long bsp, long self, long other, long body, long otherbody)
{
    if (records_contact_points()) m_rigid_bodies[body].process_contact_point(e, bsp, self, other, body, otherbody);
}
void c_entity_listener::contact_process_callback(phys_contact_process_event &event)
{
    const phys_rigid_body *a = phys_get_rigid_body(event.m_collider_a);
    const phys_rigid_body *b = phys_get_rigid_body(event.m_collider_b);
    if (a && b) {
        long ai = ::havok_entity_get_havok_component_index(a),
             bi = ::havok_entity_get_havok_component_index(b);
        long ar = ::havok_entity_get_havok_component_rigid_body_index(a),
             br = ::havok_entity_get_havok_component_rigid_body_index(b);
        if (event.m_callback_fired_from == a)
        {
            long bsp = ::havok_entity_get_structure_bsp_index(b);
            listener_component_get(ai)->notify_contact_point_process(&event, bsp, ai, bi, ar, br);
        }
        else
        {
            long bsp = ::havok_entity_get_structure_bsp_index(a);
            listener_component_get(bi)->notify_contact_point_process(&event, bsp, bi, ai, br, ar);
        }
    }
}

void c_entity_listener::contact_point_removed_callback(phys_contact_removed_event &event)
{
    phys_rigid_body *first = static_cast<phys_rigid_body *>(event.m_entity_b);
    phys_rigid_body *second = static_cast<phys_rigid_body *>(event.m_entity_a);
    int id = ::havok_entity_get_havok_component_index(first);
    if (event.m_callback_fired_from == second) {
        id = ::havok_entity_get_havok_component_index(second);
        listener_component_get(id)
            ->notify_contact_point_removed(&event, second, first);
    } else {
        listener_component_get(id)
            ->notify_contact_point_removed(&event, first, second);
    }
}

void c_entity_listener::contact_point_added_callback(phys_contact_added_event &event)
{
    if (event.m_status != PHYS_CONTACT_POINT_REJECT) {
        phys_rigid_body *rigid_body_a = phys_get_rigid_body(event.m_body_a->get_root_collider());
        phys_rigid_body *rigid_body_b = phys_get_rigid_body(event.m_body_b->get_root_collider());
        if (rigid_body_a && rigid_body_b && event.m_type == phys_contact_added_event::TYPE_MANIFOLD) {
            if (event.m_callback_fired_from == rigid_body_a) {
                add_contact_point_to_havok_component(false, event.m_internal_contact_manager,
                    static_cast<phys_manifold_point_added_event &>(event).m_process_contact_id,
                    event.m_body_a, event.m_body_b, rigid_body_a, rigid_body_b, event.m_body_b);
            } else {
                add_contact_point_to_havok_component(true, event.m_internal_contact_manager,
                    static_cast<phys_manifold_point_added_event &>(event).m_process_contact_id,
                    event.m_body_b, event.m_body_a, rigid_body_b, rigid_body_a, event.m_body_a);
            }
        }
    }
}

// @address 0x105e70 ?notify_contact_point_removed@c_havok_component@@QAAXPAUphys_contact_removed_event@@PAVphys_rigid_body@@1@Z
void c_havok_component::notify_contact_point_removed(phys_contact_removed_event *event, phys_rigid_body *first, phys_rigid_body *second)
{
    if (!records_contact_points()) return;
    long second_rigid_body = ::havok_entity_get_havok_component_rigid_body_index(second);
    long first_rigid_body = ::havok_entity_get_havok_component_rigid_body_index(first);
    long second_component = ::havok_entity_get_havok_component_index(second);
    long bsp = ::havok_entity_get_structure_bsp_index(second);
    unsigned short contact_id = event->m_process_contact_id;
    c_havok_component::c_rigid_body *body =
        &m_rigid_bodies[::havok_entity_get_havok_component_rigid_body_index(first)];
    body->remove_contact_point(contact_id, bsp, second_component, first_rigid_body, second_rigid_body);
}

static unsigned int havok_get_environment_shape_key_from_cd_body(const phys_collision_proxy *body);

// @address 0x106280 ?add_contact_point_to_havok_component@c_entity_listener@@AAAX_NPAVphys_dynamics_contact_manager@@GPBVphys_collision_proxy@@2PAVphys_rigid_body@@32@Z
void c_entity_listener::add_contact_point_to_havok_component(bool fired_from_b, phys_dynamics_contact_manager *contact_manager,
    unsigned short contact_point_id, const phys_collision_proxy *body, const phys_collision_proxy *other_body, phys_rigid_body *rigid_body,
    phys_rigid_body *other_rigid_body, const phys_collision_proxy *contact_body)
{
    long component_index_a = ::havok_entity_get_havok_component_index((::phys_world_object const *)rigid_body);
    long component_index_b = ::havok_entity_get_havok_component_index((::phys_world_object const *)other_rigid_body);
    long bsp_index = ::havok_entity_get_structure_bsp_index((::phys_world_object const *)other_rigid_body);
    c_havok_component *component_a = listener_component_get(component_index_a);
    s_havok_contact_point_reference reference = k_invalid_contact_point_reference;
    long rigid_body_index_a = ::havok_entity_get_havok_component_rigid_body_index((::phys_world_object const *)rigid_body);
    if ((component_a->m_rigid_bodies[rigid_body_index_a].m_flags & 8) != 0)
        return;
    if (bsp_index == NONE && component_index_b == NONE)
        return;
    c_havok_component *component_b = component_index_b == NONE ? NULL : listener_component_get(component_index_b);
    long unknown = component_b ? component_b->get_object_index() : NONE;
    byte surface_flags = 0;
    long surface_index = NONE;
    c_global_material_type material_a = k_global_material_type_default;
    c_global_material_type material_b = k_global_material_type_default;
    float friction_value = 1.0f;
    float restitution_value = 0.0f;
    phys_contact_point *contact_point = contact_manager->get_process_contact_point(contact_point_id);
    const real_point3d *position = real_point3d_from_hkVector4_in_place(&contact_point->get_contact_position());
    unsigned int shape_key_b = havok_get_environment_shape_key_from_cd_body(other_body);
    unsigned int shape_key_a = havok_get_environment_shape_key_from_cd_body(body);
    calculate_contact_point_properties(&friction_value, &restitution_value, &material_a, &material_b, body, other_body, component_index_a,
        component_index_b, rigid_body, other_rigid_body, shape_key_a, shape_key_b, position);
    const phys_shape *contact_shape = contact_body->get_shape();
    if (contact_shape->get_type_id() == PHYS_SHAPE_USER0) {
        surface_flags = ((const c_collision_surface_shape *)contact_shape)->get_collision_surface_flags();
        surface_index = ((const c_collision_surface_shape *)contact_shape)->get_surface_index();
    }
    unsigned int environment_shape_key = havok_get_environment_shape_key_from_cd_body(other_body);
    reference = ((c_havok_component *)component_a)
                    ->notify_contact_point_added(contact_point_id, bsp_index, (unsigned int)environment_shape_key, surface_index, unknown,
                        surface_flags, rigid_body, other_rigid_body, friction_value, restitution_value, material_a, material_b);
    if (reference.rigid_body_index != -1) {
        long rigid_body_index = ::havok_entity_get_havok_component_rigid_body_index((::phys_world_object const *)rigid_body);
        c_havok_contact_point *contact = component_a->get_contact_point(reference.rigid_body_index, reference.contact_point_index);
        phys_contact_point *contact_point2 = contact_manager->get_process_contact_point(contact_point_id);
        contact->update(true, &contact_point2->get_contact_position(), &contact_point2->get_separating_normal(), contact_manager,
            contact_body->get_shape(), fired_from_b, component_index_a, rigid_body_index);
    }
}

static unsigned int havok_get_environment_shape_key_from_cd_body(const phys_collision_proxy *body)
{
    unsigned int result = PHYS_INVALID_SHAPE_KEY;
    while (body->get_parent_proxy() && body->get_parent_proxy()->get_shape()->get_user_data() != 0xCABCABB0)
        body = body->get_parent_proxy();
    if (body->get_parent_proxy())
        result = body->get_shape_key();
    return result;
}

static inline float listener_get_combined_friction(float friction_a, float friction_b)
{
    return sqrtf(friction_a * friction_b);
}

static inline float listener_get_combined_restitution(float restitution_a, float restitution_b)
{
    return sqrtf(restitution_a * restitution_b);
}

void c_entity_listener::calculate_contact_point_properties(float *friction, float *restitution, c_global_material_type *material_a,
    c_global_material_type *material_b, const phys_collision_proxy *body_a, const phys_collision_proxy *body_b, long component_a, long component_b,
    const phys_rigid_body *rigid_body_a, const phys_rigid_body *rigid_body_b, unsigned int key_a, unsigned int key_b, const real_point3d *position)
{
    long body_count = 2;
    c_global_material_type *materials[2] = { material_a, material_b };
    const phys_collision_proxy *bodies[2] = { body_a, body_b };
    unsigned int keys[2] = { key_a, key_b };
    float restitutions[2];
    float frictions[2];
    for (long i = 0; i < body_count; ++i)
    {
        const phys_collision_proxy *body = bodies[i];
        const phys_shape *shape = body->get_shape();
        calculate_havok_shape_properties(shape, phys_get_rigid_body(body->get_root_collider()), position, keys[i], &frictions[i],
            &restitutions[i], materials[i]);
    }
    *friction = listener_get_combined_friction(frictions[0], frictions[1]);
    *restitution = listener_get_combined_restitution(restitutions[0], restitutions[1]);
}

extern "C" {
c_havok_error g_hk_error;
// @address 0x7f18b8 g_entity_listener_storage
__declspec(align(8)) char g_entity_listener_storage[];
// @address 0x7ead0c g_world_entity_listener_storage
__declspec(align(4)) char g_world_entity_listener_storage[];
}


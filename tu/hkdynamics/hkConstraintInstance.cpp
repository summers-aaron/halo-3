// hkdynamics/hkConstraintInstance.obj: from the records 0x64e008 0x64e020 0x64e028 0x64e030 0x64e058 0x64e128 0x64e148 0x64e158 0x64e1f8 0x64e230 0x64e250 0x64e270 0x64e2c8 0x64e338
// @flags /TP /O2
#include "phys/phys_dynamics.h"
#include "physics/havok_halo_external.h"
#include "phys/phys_constraint.h"
// @address 0x64e008 ?destroy_loaded_constraint@@YAXPAX@Z
// @address 0x64e020 ?on_entity_added@phys_constraint@@UAAXPAVphys_entity@@@Z
// @address 0x64e028 ?on_entity_deleted@phys_constraint@@UAAXPAVphys_entity@@@Z
// @address 0x64e030 ?on_entity_removed@phys_constraint@@UAAXPAVphys_entity@@@Z
// @address 0x64e058 ??1phys_constraint@@UAA@XZ
// @address 0x64e128 ?fixup_loaded_constraint@@YAXPAX@Z
// @address 0x64e148 ?get_constraint_vtable@@YAPBXXZ
// @address 0x64e158 ??0phys_constraint@@QAA@PAVphys_entity@@0PAVphys_constraint_data@@W4constraint_priority@0@@Z
// @address 0x64e1f8 ??0phys_constraint@@IAA@W4constraint_priority@0@@Z
// @address 0x64e230 ?set_constraint_priority@phys_constraint@@QAAXW4constraint_priority@1@@Z
// @address 0x64e250 ?get_island@phys_constraint@@QAAPAVphys_island@@XZ
// @address 0x64e270 ?clone@phys_constraint@@QBAPAV1@PAVphys_entity@@0@Z
// @address 0x64e2c8 ?bind_nulls_to_fixed_body@phys_constraint@@QAAXXZ
// @address 0x64e338 ??_Gphys_constraint@@UAAPAXI@Z
class phys_constraint_modifier;
#include "phys/phys_world_op_util.h"
#include "phys/phys_constraint_owner.h"
#include "phys/phys_memory_class.h"
#include "phys/phys_thread_memory.h"
void destroy_loaded_constraint(void *object)
{
    static_cast<phys_constraint *>(object)->~phys_constraint();
}
void phys_constraint::on_entity_added(phys_entity *entity) {}
void phys_constraint::on_entity_deleted(phys_entity *entity) {}
void phys_constraint::on_entity_removed(phys_entity *entity)
{
    if (m_owner) {
        phys_world_op_util::remove_constraint_now(entity->get_action_world(), this, phys_world_op_util::OP_CALLBACKS_FIRE);
    }
}
phys_constraint::~phys_constraint()
{
    if (m_island_entities[0]) m_island_entities[0]->object_remove_ref();
    if (m_island_entities[1]) m_island_entities[1]->object_remove_ref();
    if (m_data) m_data->object_remove_ref();
}
namespace {
}
void fixup_loaded_constraint(void *storage)
{
    phys_fixup_tag flag;
    new (storage) phys_constraint(flag);
}
const void *get_constraint_vtable()
{
    __declspec(align(16)) char storage[sizeof(phys_constraint)];
    phys_fixup_tag flag;
    phys_constraint *object = new (storage) phys_constraint(flag);
    return *reinterpret_cast<const void *const *>(object);
}
phys_constraint::phys_constraint(phys_entity *listener_entity_a, phys_entity *listener_entity_b, phys_constraint_data *data, constraint_priority priority)
    : m_owner(0), m_data(data), m_priority(priority), m_want_runtime(false), m_name(0), m_user_data(0), m_constraint_internal(0)
{
    m_island_entities[0] = listener_entity_a;
    m_island_entities[1] = listener_entity_b;
    m_constraint_modifiers = 0;
    m_island_entities[0]->add_reference_critical();
    if (m_island_entities[1]) m_island_entities[1]->add_reference_critical();
    m_data->add_object_reference();
}
phys_constraint::phys_constraint(constraint_priority priority)
    : m_owner(0), m_constraint_modifiers(0), m_priority(priority), m_want_runtime(false), m_name(0), m_user_data(0), m_constraint_internal(0)
{}
void phys_constraint::set_constraint_priority(constraint_priority priority)
{
    m_priority = priority;
    if (m_constraint_internal) m_constraint_internal->m_priority = priority;
}
phys_island *phys_constraint::get_island()
{
    if (!m_island_entities[0]->body_is_fixed()) return m_island_entities[0]->get_island();
    else return m_island_entities[1]->get_island();
}
phys_constraint *phys_constraint::clone(phys_entity *listener_entity_a, phys_entity *listener_entity_b) const
{
    phys_constraint *result = new phys_constraint(listener_entity_a, listener_entity_b, m_data, (constraint_priority)m_priority);
    return result;
}
void phys_constraint::bind_nulls_to_fixed_body()
{
    for (int i = 0; i < 2; ++i) {
        if (!m_island_entities[i]) {
            phys_entity *other = get_other_entity(m_island_entities[i]);
            if (other && other->get_action_world() != 0) {
                m_island_entities[i] = other->get_action_world()->get_fixed_rigid_body();
                m_island_entities[i]->add_object_reference();
            }
        }
    }
}

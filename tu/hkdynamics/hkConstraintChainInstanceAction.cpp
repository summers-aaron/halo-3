// hkdynamics/hkConstraintChainInstanceAction.obj: from the records 0x65e3a0 0x65e3b8 0x65e3c0 0x65e3c8 0x65e3e0 0x65e400 0x65e410
// @flags /TP /O2
#include "phys/phys_constraint.h"
#include "phys/phys_base.h"
// @address 0x65e3a0 ?destroy_loaded_constraint_chain_action@@YAXPAX@Z
// @address 0x65e3b8 ?apply_action@phys_constraint_chain_action@@UAAXABVphys_step_info@@@Z
// @address 0x65e3c0 ?clone@phys_constraint_chain_action@@UBAPAVphys_action@@ABV?$phys_array@PAVphys_entity@@@@ABV?$phys_array@PAVphys_volume@@@@@Z
// @address 0x65e3c8 ?on_entity_removed@phys_constraint_chain_action@@UAAXPAVphys_entity@@@Z
// @address 0x65e3e0 ?fixup_loaded_chain_action@@YAXPAX@Z
// @address 0x65e400 ?get_constraint_chain_action_vtable@@YAPBXXZ
// @address 0x65e410 ?get_entities@phys_constraint_chain_action@@UAAXAAV?$phys_array@PAVphys_entity@@@@@Z

#include "phys/phys_constraint_chain.h"
#include "phys/phys_constraint_chain_action.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_memory_class.h"

void destroy_loaded_constraint_chain_action(void *object)
{
    static_cast<phys_constraint_chain_action *>(object)->~phys_constraint_chain_action();
}

void phys_constraint_chain_action::apply_action(const phys_step_info &step_info)
{
}

phys_action *phys_constraint_chain_action::clone(const phys_array<phys_entity *> &entities, const phys_array<phys_volume *> &phantoms) const
{
    return 0;
}

void phys_constraint_chain_action::on_entity_removed(phys_entity *entity)
{
    phys_constraint_chain *constraint = get_chain_constraint();
    constraint->on_entity_removed(entity);
}

void phys_constraint_chain_action::get_entities(phys_array<phys_entity *> &entities)
{
    entities = get_chain_constraint()->m_chained_entities;
}

void fixup_loaded_chain_action(void *object)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (object) phys_constraint_chain_action(flag);
}
const void *get_constraint_chain_action_vtable()
{
    union { __declspec(align(16)) void *ptr; char buffer[sizeof(phys_constraint_chain_action)]; } object;
    phys_fixup_tag flag;
    new (object.buffer) phys_constraint_chain_action(flag);
    return object.ptr;
}

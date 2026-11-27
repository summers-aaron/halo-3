// hkdynamics/hkConstraintChainInstance.obj: from the records 0x64f188 0x64f1a0 0x64f228 0x64f2d0 0x64f2f0 0x64f300 0x64f3c0 0x64f4b0 0x64f510
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x64f188 ?destroy_loaded_constraint_chain@@YAXPAX@Z
// @address 0x64f1a0 ?on_entity_removed@phys_constraint_chain@@UAAXPAVphys_entity@@@Z
// @address 0x64f228 ?add_chain_entity@phys_constraint_chain@@QAAXPAVphys_entity@@@Z
// @address 0x64f2d0 ?fixup_loaded_constraint_chain@@YAXPAX@Z
// @address 0x64f2f0 ?get_constraint_chain_vtable@@YAPBXXZ
// @address 0x64f300 ??0phys_constraint_chain@@QAA@PAVphys_chain_data@@@Z
// @address 0x64f3c0 ??1phys_constraint_chain@@UAA@XZ
// @address 0x64f4b0 ??_Gphys_constraint_chain_action@@UAAPAXI@Z
// @address 0x64f510 ??_Gphys_constraint_chain@@UAAPAXI@Z
#define PHYS_CONFIG_SIMD 1
#include "phys/phys_constraint_chain.h"
#include "phys/phys_constraint_chain_action.h"
#include "phys/phys_world_op_util.h"

phys_constraint_chain::phys_constraint_chain(phys_chain_data* data)
: phys_constraint(PRIORITY_PSI)
{

	m_data = data;
	data->add_object_reference();

	m_island_entities[0] = 0;
	m_island_entities[1] = 0;

	m_chain_action = new phys_constraint_chain_action(this);
}

void phys_constraint_chain::add_chain_entity(phys_entity* entity)
{

	if (m_chained_entities.getSize() < 2 )
	{
		const int idx = m_chained_entities.getSize();
		if (m_island_entities[idx] == 0)
		{
			m_island_entities[idx] = entity;
			entity->add_object_reference();
		}
	}

	m_chained_entities.array_push_back( entity );
	entity->add_object_reference();
}

phys_constraint_chain::~phys_constraint_chain()
{
	for (int i = 0; i < m_chained_entities.getSize(); i++)
	{
		m_chained_entities[i]->object_remove_ref();
	}

	m_chain_action->object_remove_ref();
}

void phys_constraint_chain::on_entity_removed(phys_entity* entity)
{


	phys_world* world = entity->get_action_world();

	world->lock_critical_ops();
	{

		world->remove_action_now(m_chain_action);

		phys_world_op_util::remove_constraint_locked_island(world, this);

	}
	world->unlock_and_flush_ops();
}


void fixup_loaded_constraint_chain(void *object)
{
	phys_fixup_tag flag;
	flag.m_fixup_finishing = 1;
	new (object) phys_constraint_chain(flag);
}
void destroy_loaded_constraint_chain(void *object)
{
	static_cast<phys_constraint_chain *>(object)->~phys_constraint_chain();
}
const void *get_constraint_chain_vtable()
{
	union { __declspec(align(16)) void *ptr; char buffer[sizeof(phys_constraint_chain)]; } object;
	phys_fixup_tag flag;
	new (object.buffer) phys_constraint_chain(flag);
	return object.ptr;
}

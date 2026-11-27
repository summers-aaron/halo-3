// hkdynamics/hkPhysicsSystem.obj: from the records 0x6585f8 0x658610 0x658670 0x6586d0 0x658770 0x658810 0x658898 0x658920 0x6589b0 0x658a40 0x658a60 0x658a70 0x658ad0 0x658d28 0x658ff0 0x6595b0
// @compiler xdk6130
// @flags /TP /O2 /Oc /GF /EHs-c- /GR-
// @address 0x6585f8 ?destroy_loaded_physics_system@@YAXPAX@Z
// @address 0x658610 ?remove_rigid_body@phys_physics_system@@QAAXH@Z
// @address 0x658670 ?remove_volume@phys_physics_system@@QAAXH@Z
// @address 0x6586d0 ?remove_constraint@phys_physics_system@@QAAXH@Z
// @address 0x658770 ?remove_action@phys_physics_system@@QAAXH@Z
// @address 0x658810 ?add_system_rigid_body@phys_physics_system@@QAAXPAVphys_rigid_body@@@Z
// @address 0x658898 ?add_volume@phys_physics_system@@QAAXPAVphys_volume@@@Z
// @address 0x658920 ?add_world_constraint@phys_physics_system@@QAAXPAVphys_constraint@@@Z
// @address 0x6589b0 ?add_world_action@phys_physics_system@@QAAXPAVphys_action@@@Z
// @address 0x658a40 ?fixup_loaded_physics_system@@YAXPAX@Z
// @address 0x658a60 ?get_physics_system_vtable@@YAPBXXZ
// @address 0x658a70 ??0phys_physics_system@@QAA@XZ
// @address 0x658ad0 ??1phys_physics_system@@UAA@XZ
// @address 0x658d28 ?copy@phys_physics_system@@QAAXABV1@@Z
// @address 0x658ff0 ?clone@phys_physics_system@@QBAPAV1@XZ
// @address 0x6595b0 ??_Gphys_physics_system@@UAAPAXI@Z
#include "phys/phys_physics_system.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_action.h"
#include "phys/phys_constraint.h"

// match: 0x658ff0: clone inlines the constructor body; called out of line, clone shrinks 76 bytes
// __forceinline is Xbox-only: the PC link needs an out-of-line copy for hkWorld's new hkPhysicsSystem calls.
// The declarator is spelled twice so __forceinline stays glued to it: the xdk6130 link optimizer drops the inline
// when a #if/#endif gap (and its #line) splits them across lines (lane-check showed -76B at 0x658ff0).
#if defined(_MSC_VER) && !defined(__clang__)
__forceinline phys_physics_system::phys_physics_system()
#else
phys_physics_system::phys_physics_system()
#endif
: m_name(0), m_user_data(0), m_active(true)
{
}

phys_physics_system::~phys_physics_system()
{
    int sys_body_cnt = m_system_rigid_bodies.getSize();
    for (int r = 0; r < sys_body_cnt; ++r)
    {
        m_system_rigid_bodies[r]->object_remove_ref();
    }

    int sys_volume_cnt = m_border_volumes.getSize();
    for (int p = 0; p < sys_volume_cnt; ++p)
    {
        m_border_volumes[p]->object_remove_ref();
    }

    int nc = m_component_constraints.getSize();
    for (int c = 0; c < nc; ++c)
    {
        m_component_constraints[c]->object_remove_ref();
    }

    int sys_action_cnt = m_actions.getSize();
    for (int a = 0; a < sys_action_cnt; ++a)
    {
        if (m_actions[a])
        {
            m_actions[a]->object_remove_ref();
        }
    }
}

void phys_physics_system::copy(const phys_physics_system& to_copy)
{
    m_system_rigid_bodies = to_copy.m_system_rigid_bodies;
    m_border_volumes = to_copy.m_border_volumes;
    m_component_constraints = to_copy.m_component_constraints;
    m_actions = to_copy.m_actions;
    m_name = to_copy.m_name;
    m_user_data = to_copy.m_user_data;
    m_active = to_copy.m_active;
}

phys_physics_system* phys_physics_system::clone() const
{
    phys_physics_system* cloned_system = new phys_physics_system();
    cloned_system->m_name = m_name;
    cloned_system->m_user_data = m_user_data;
    cloned_system->m_active = m_active;

    cloned_system->m_system_rigid_bodies.set_array_size( m_system_rigid_bodies.getSize() );
    cloned_system->m_border_volumes.set_array_size( m_border_volumes.getSize() );
    cloned_system->m_component_constraints.set_array_size( m_component_constraints.getSize() );
    cloned_system->m_actions.set_array_size( m_actions.getSize() );

    int i;
    for (i = 0; i < m_system_rigid_bodies.getSize(); ++i )
    {
        cloned_system->m_system_rigid_bodies[i] = m_system_rigid_bodies[i]->clone();
    }
    for (i = 0; i < m_border_volumes.getSize(); ++i )
    {
        cloned_system->m_border_volumes[i] = m_border_volumes[i]->clone();
    }

    for (i = 0; i < m_component_constraints.getSize(); ++i )
    {
        phys_constraint* c = m_component_constraints[i];

        int new_body_a_idx = m_system_rigid_bodies.array_index_of( (phys_rigid_body*)( c->get_entity_a() ) );
        int new_body_b_idx = m_system_rigid_bodies.array_index_of( (phys_rigid_body*)( c->get_entity_b() ) );

        phys_rigid_body* cloned_body_a = cloned_system->m_system_rigid_bodies[new_body_a_idx];
        phys_rigid_body* cloned_body_b = cloned_system->m_system_rigid_bodies[new_body_b_idx];

        cloned_system->m_component_constraints[i] = c->clone( cloned_body_a, cloned_body_b );
    }

    for (i = 0; i < m_actions.getSize(); ++i )
    {
        phys_action* a = m_actions[i];
        if (!a)
        {
            cloned_system->m_actions[i] = 0;
            continue;
        }

        phys_array<phys_entity*> action_entities;
        a->get_entities(action_entities);

        phys_array<phys_entity*> new_rigid_bodies;
        int action_entity_count = action_entities.getSize();
        new_rigid_bodies.set_array_size(action_entity_count);
        int j = 0;
        for (j = 0; j < action_entity_count; ++j)
        {
            phys_rigid_body* rb = (phys_rigid_body*)action_entities[j];
            int new_body_idx = m_system_rigid_bodies.array_index_of( rb );
            new_rigid_bodies[j] = cloned_system->m_system_rigid_bodies[ new_body_idx ];
        }

        phys_array<phys_volume*> action_volumes;
        a->get_volumes(action_volumes);

        phys_array<phys_volume*> new_volumes;
        int action_volume_count = action_volumes.getSize();
        new_volumes.set_array_size(action_volume_count);
        for (j = 0; j < action_volume_count; ++j)
        {
            phys_volume* ph = (phys_volume*)action_volumes[j];
            int new_volume_idx = m_border_volumes.array_index_of( ph );
            new_volumes[j] = cloned_system->m_border_volumes[ new_volume_idx ];
        }

        cloned_system->m_actions[i] = a->clone( new_rigid_bodies, new_volumes );
    }

    return cloned_system;
}

void phys_physics_system::add_system_rigid_body( phys_rigid_body* rb )
{
    if (rb)
    {
        rb->add_object_reference();
        m_system_rigid_bodies.array_push_back(rb);
    }
}

void phys_physics_system::add_volume(  phys_volume* p )
{
    if (p)
    {
        p->add_object_reference();
        m_border_volumes.array_push_back(p);
    }
}

void phys_physics_system::add_world_constraint( phys_constraint* c )
{
    if (c)
    {
        c->add_object_reference();
        m_component_constraints.array_push_back(c);
    }
}

void phys_physics_system::add_world_action( phys_action* a )
{
    if (a)
    {
        a->add_object_reference();
        m_actions.array_push_back(a);
    }
}

void phys_physics_system::remove_rigid_body( int i )
{
    m_system_rigid_bodies[i]->object_remove_ref();
    m_system_rigid_bodies.remove_at(i);
}

void phys_physics_system::remove_volume( int i )
{
    m_border_volumes[i]->object_remove_ref();
    m_border_volumes.remove_at(i);
}

void phys_physics_system::remove_constraint( int i )
{
    m_component_constraints[i]->object_remove_ref();
    m_component_constraints.remove_at(i);
}

void phys_physics_system::remove_action( int i )
{
    m_actions[i]->object_remove_ref();
    m_actions.remove_at(i);
}


void fixup_loaded_physics_system(void *object)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (object) phys_physics_system(flag);
}
void destroy_loaded_physics_system(void *object)
{
    static_cast<phys_physics_system *>(object)->~phys_physics_system();
}
const void *get_physics_system_vtable()
{
    union { __declspec(align(16)) void *ptr; char buffer[sizeof(phys_physics_system)]; } object;
    phys_fixup_tag flag;
    new (object.buffer) phys_physics_system(flag);
    return object.ptr;
}

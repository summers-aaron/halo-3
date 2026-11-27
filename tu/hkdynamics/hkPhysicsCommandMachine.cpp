// hkdynamics/hkPhysicsCommandMachine.obj: from the records 0x664078 0x6640c0
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x664078 ?add_constraint_to_locked_island@@YAXPAVphys_world@@PAVphys_constraint@@H@Z
// @address 0x6640c0 ?phys_command_machine_process@@YAXPAVphys_world@@PAUphys_physics_command@@H@Z

#include "phys/phys_physics_command_machine.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_constraint.h"
#include "phys/phys_world_op_util.h"

void add_constraint_to_locked_island(phys_world *world, phys_constraint *constraint,
    int constraint_callback_request)
{
    phys_world_op_util::add_constraint_to_locked_island(world, constraint);
    constraint->m_constraint_internal->m_callback_request |= constraint_callback_request;
}

void phys_command_machine_process(phys_world *world, phys_physics_command *bvh_commands, int size)
{
    phys_physics_command *end = phys_add_byte_offset(bvh_commands, size);
    while (bvh_commands < end)
    {
        switch (bvh_commands->m_type)
        {
        case phys_physics_command::CMD_ADD_CONSTRAINT_TO_LOCKED_ISLAND:
            {
                phys_add_constraint_island_command *command = static_cast<phys_add_constraint_island_command *>(bvh_commands);
                add_constraint_to_locked_island(world, command->m_object_a, command->m_object_b);
                bvh_commands = phys_add_byte_offset(bvh_commands, (sizeof(phys_add_constraint_island_command) + 15) & ~15);
                break;
            }
        case phys_physics_command::CMD_REMOVE_CONSTRAINT_FROM_LOCKED_ISLAND:
            {
                phys_remove_constraint_locked_cmd *command =
                    static_cast<phys_remove_constraint_locked_cmd *>(bvh_commands);
                phys_world_op_util::remove_constraint_locked_island(world, command->m_object);
                bvh_commands = phys_add_byte_offset(bvh_commands, (sizeof(phys_remove_constraint_locked_cmd) + 15) & ~15);
                break;
            }
        default:
            return;
        }
    }
}


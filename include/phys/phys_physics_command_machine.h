#ifndef INCLUDE_PHYS_PHYSICS_COMMAND_MACHINE_H
#define INCLUDE_PHYS_PHYSICS_COMMAND_MACHINE_H

class phys_constraint;
class phys_world;

struct phys_physics_command {
    enum TYPE {
        CMD_ADD_CONSTRAINT_TO_LOCKED_ISLAND,
        CMD_REMOVE_CONSTRAINT_FROM_LOCKED_ISLAND,
        CMD_END
    };
    __forceinline phys_physics_command(TYPE type) : m_type((unsigned char)type) {}
    unsigned char m_type;
};

template <phys_physics_command::TYPE CMD_KIND, typename CMD_OBJECT_TYPE>
struct phys_single_object_command : public phys_physics_command {
    __forceinline phys_single_object_command(CMD_OBJECT_TYPE object)
        : phys_physics_command(CMD_KIND), m_object(object) {}
    CMD_OBJECT_TYPE m_object;
};

template <phys_physics_command::TYPE CMD_KIND, typename CMD_OBJECT_TYPE_A, typename CMD_OBJECT_TYPE_B>
struct phys_dual_object_command : public phys_physics_command {
    __forceinline phys_dual_object_command(CMD_OBJECT_TYPE_A command_object0, CMD_OBJECT_TYPE_B second_object)
        : phys_physics_command(CMD_KIND), m_object_a(command_object0), m_object_b(second_object) {}
    CMD_OBJECT_TYPE_A m_object_a;
    CMD_OBJECT_TYPE_B m_object_b;
};

typedef phys_dual_object_command<phys_physics_command::CMD_ADD_CONSTRAINT_TO_LOCKED_ISLAND,
    phys_constraint *, int> phys_add_constraint_island_command;
typedef phys_single_object_command<phys_physics_command::CMD_REMOVE_CONSTRAINT_FROM_LOCKED_ISLAND,
    phys_constraint *> phys_remove_constraint_locked_cmd;

typedef char phys_command_size[(sizeof(phys_physics_command) == 1) ? 1 : -1];
typedef char phys_add_constraint_cmd_size[(sizeof(void *) != 4 || ((sizeof(phys_add_constraint_island_command) == 12) )) ? 1 : -1];
typedef char phys_remove_constraint_command_size[(sizeof(void *) != 4 || ((sizeof(phys_remove_constraint_locked_cmd) == 8) )) ? 1 : -1];

void phys_command_machine_process(phys_world *world, phys_physics_command *bvh_commands, int size);

#endif

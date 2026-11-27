#ifndef AI_CL_ENGINE_H
#define AI_CL_ENGINE_H

#include <stddef.h>

#include "math/real_math_types.h"
#include "ai/ai_point3d.h"

// Command-script atoms (ai\cl_engine.cpp): a command script runs up to three atoms at once, one per channel;
// each channel has the command it was given (cs_command) and its running state (s_atom_control). Every atom's
// start/update/end callback takes both.

// Atom types, indexing g_atom_definitions. Names from March 2007 (g_atom_names, the _ai_atom_vehicle assert); the
// shipped table has two atoms March lacks, at 6 (started by approach_object_start) and 8 (updated by actor_is_using_equipment).
enum e_ai_atom
{
    _ai_atom_pause,
    _ai_atom_go_to,
    _ai_atom_go_by,
    _ai_atom_go_to_and_face,
    _ai_atom_go_to_and_posture,
    _ai_atom_move_direction,
    _ai_atom_grenade = 7,
    _ai_atom_vehicle = 9,
    _ai_atom_jump,
    _ai_atom_targeted_jump,
    _ai_atom_recording,
    _ai_atom_action,
    _ai_atom_vocalize,
    _ai_atom_die,
    _ai_atom_behavior,
    _ai_atom_fly_to,
    _ai_atom_fly_by,
    _ai_atom_fly_to_and_face,
    _ai_atom_formation,
    _ai_atom_approach,
    _ai_atom_deploy_turret,
    _ai_atom_custom_animation,
    _ai_atom_play_sound,
    _ai_atom_melee,
    _ai_atom_smash,
    _ai_atom_swarm_to,
    _ai_atom_swarm_from,

    _ai_atom_count
};

// One atom command. March 2007 debug cs_initialize_command: 44 bytes, atom_type stored as a byte at +0 (the
// asserts name command->atom_type), bit 0 of +1 set and bit 1 set to the active argument, +2 set to NONE.
struct cs_command
{
    signed char atom_type;
    char flags;
    short index;                                  // the atom's integer argument (direction, vocalization, formation, grenade mode)
    union {
        float parameter;
        long formation_index;
    };
    union {
        float angle;
        bool push;                                // approach
    };
    char spare_after_angle[0x24 - 0x0c];
    union {
        long point_ref;
        long object_index;                        // the target atoms
    };
    long seat_ref;
};

// The running state of one atom. cs_command_start stores the command's atom type at +0 and the start result at
// +4; cs_command_update counts +2 down every tick and stores the update result at +4; cs_command_end clears +4.
// From +8 on each atom keeps its own data.
struct s_atom_control
{
    short atom_type;
    short timer;
    short status;
    short flags;
    union
    {
        struct
        {
            short index;
            short spare_after_index;
            real_vector3d direction;
        };
        c_ai_point3d point;
        long object_index;                        // the target atoms
    };
    real_point3d origin;
};

typedef char cs_command_size_must_be_0x2c[(sizeof(cs_command) == 0x2c) ? 1 : -1];
typedef char cs_command_offset_point_ref[((offsetof(cs_command, point_ref)) == 0x24) ? 1 : -1];
typedef char atom_control_size_must_be_0x24[(sizeof(s_atom_control) == 0x24) ? 1 : -1];
typedef char atom_control_offset_origin[((offsetof(s_atom_control, origin)) == 0x18) ? 1 : -1];


// Functions defined in tu/ailibxbox/cl_engine.cpp that other units call.

class c_behavior_state;

short cs_command_end(long actor_index, long script_index, cs_command *command, s_atom_control *control, long argument);
bool valid_point_ref(long point_reference);
bool valid_point_set(long point_set_reference);

// A point reference packs the point set index in the high 16 bits and the point index in the low 16 bits.
static __forceinline unsigned short point_reference_get_point_set_index(long point_reference)
{
    return (unsigned short)(point_reference >> 16);
}

static __forceinline unsigned short point_reference_get_point_index(long point_reference)
{
    return (unsigned short)point_reference;
}

long cs_attach_command_script(long actor_index, short script_index, long thread_index);
long cs_stack_command_script(long actor_index, short script_index, long thread_index);
long cs_queue_command_script(long actor_index, short script_index, long thread_index);
long cs_reserve(long actor_index, long thread_index, bool mandatory, short priority);
void cs_handle_deleted_object(long actor_index, long thread_index);
void cs_actor_update(long actor_index);
void cs_actor_execute_command_script(long actor_index, long script_index, short *result, c_behavior_state *behavior_state);
bool cs_atom_executing(long actor_handle, long command_script_handle, short atom_mask);
bool cs_blocked(long thread_index);
long cs_index_from_thread(long actor_index, long thread_index, bool create);
bool cs_cast(long thread_index, long ai_index, bool mandatory, short priority, long *role_names, short role_count);
void cs_apply_persistent_state(long actor_index);
extern "C" void cs_remove_command_script(long owner_index, long member_index);
extern "C" bool point_try_and_get_frame(long point_reference, real_point3d *position, real_vector3d *forward, real_vector3d *up);

// Data defined in tu/ailibxbox/cl_engine.cpp that other units use.

extern short *global_new_behavior;
extern c_behavior_state *global_new_state_data;

typedef short (*cs_callback)(long, long, cs_command *, s_atom_control *, long);
short cs_helper(long actor_index, short atom, cs_callback callback, long script_index);

extern "C" cs_command *cs_initialize_command(long owner_index, short type, bool active);
void __tls_set_g_command_script_allocator(void *object);

#endif

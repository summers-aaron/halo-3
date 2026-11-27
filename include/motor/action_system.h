#ifndef MOTOR_ACTION_SYSTEM_H
#define MOTOR_ACTION_SYSTEM_H

#include "units/motor_enums.h"

// Functions defined in tu/objectslibxbox/action_system.cpp that other units call.

struct action_request;
union real_vector2d;
union real_vector3d;

bool action_update(long object_index);
bool action_submit(long index, const action_request *request);
void unit_align_facing(long object_index, const real_vector3d *data);
void unit_align_facing2d(long unit_index, const real_vector2d *facing);
extern "C" bool action_submit_event(long index, long type);
extern "C" void unit_handle_message_from_weapon(long unit_index, long weapon_index, long message, long parameter);
extern "C" bool action_submit_initial_animation(long object_index);
bool action_executing(long object_index, e_action action);

// One row per e_action: the action's name, its priority against other actions and its flags.
struct s_action_usage
{
    const char *name;
    short priority;
    short flags;
};

extern s_action_usage g_action_usage_definitions[k_action_count];

// The callbacks of one action (March: const s_action_definition *action_get_definition(e_action), asserting
// action). The table of these is indexed
// by e_action; its entries' first slots are the *_submit functions (unit_action_ping_submit,
// action_custom_animation_submit, action_get_infected_submit, ...). action_system.cpp calls submit and update;
// motor_system.cpp calls animation_event, finished and interrupted.
struct s_action_definition
{
    bool (*submit)(long object_index, const action_request *request);
    bool (*update)(long object_index, e_action action);
    void (*animation_event)(long object_index, e_action action, long event);
    void (*finished)(long object_index, e_action action);
    void (*interrupted)(long object_index, e_action action);
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char action_definition_size[(sizeof(void *) != 4 || (sizeof(s_action_definition) == 0x14)) ? 1 : -1];
#endif

// The table is a file static of action_system.cpp in March (March 0x83ce4918 bears no symbol of its own),
// so it takes a descriptive name.
extern "C" s_action_definition *g_action_definitions[];

// Returns the callbacks for an action.
static __forceinline const s_action_definition *action_get_definition(e_action action)
{
    return g_action_definitions[action];
}

#endif // MOTOR_ACTION_SYSTEM_H

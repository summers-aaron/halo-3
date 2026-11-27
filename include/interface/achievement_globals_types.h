#ifndef INTERFACE_ACHIEVEMENT_GLOBALS_TYPES_H
#define INTERFACE_ACHIEVEMENT_GLOBALS_TYPES_H

#include "interface/controller_achievements_types.h"

class c_achievement_write_overlapped_task;
class c_achievements_enumerate_overlapped_task;

struct s_achievement_globals
{
    c_achievements_enumerate_overlapped_task *enumerate_task;
    c_achievement_write_overlapped_task *write_task;
    unsigned char enumeration_pending[4];
    s_controller_achievements controllers[4];
    float last_update;
};

extern "C" s_achievement_globals g_achievement_globals;

#endif // INTERFACE_ACHIEVEMENT_GLOBALS_TYPES_H

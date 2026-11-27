#ifndef AI_BEHAVIOR_MISC_H
#define AI_BEHAVIOR_MISC_H

// Functions defined in tu/ailibxbox/behavior_misc.cpp that other units call.

class c_ai_point3d;

bool behavior_plan_grenade_throw(long actor_index, const c_ai_point3d *target, long target_unit, short flags);
bool behavior_try_to_throw_grenade(long actor_index);

#endif // AI_BEHAVIOR_MISC_H

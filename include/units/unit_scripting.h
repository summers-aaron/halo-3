#ifndef UNITS_UNIT_SCRIPTING_H
#define UNITS_UNIT_SCRIPTING_H

#include "motor/action_system.h"

// Functions defined in tu/objectslibxbox/unit_scripting.cpp that other units call.

void unit_scripting_set_maximum_vitality(long object_index, float maximum_body, float maximum_shield);
void unit_scripting_list_set_maximum_vitality(long list_index, float maximum_body, float maximum_shield);
extern "C" void unit_scripting_list_set_current_vitality(long list_index, float current_body, float current_shield);
void unit_scripting_list_set_object_flag_8000(long list_index);
void unit_scripting_list_set_object_flag_40000(long list_index, bool set);
void unit_scripting_set_component_flag_8(long object_index, bool set);
bool unit_scripting_start_user_animation_list(long list_index, long animation, long flags, bool relative);
bool unit_scripting_vehicle_test_seat(long vehicle_index, long seat_label, long unit_index);
bool unit_scripting_vehicle_test_seat_list(long unit_index, long seat_label, long list_index);
void unit_scripting_enter_vehicle(long unit_index, long vehicle_index, long seat_label);
void unit_scripting_exit_vehicle2(long unit_index, short exit_type);
long unit_scripting_object_get_turret(long object_index, short turret_index);
short vehicle_scripting_load_magic(long vehicle_index, long seats, long list_index);
void unit_scripting_magic_melee_attack();
bool unit_start_user_animation(long unit_index, long animation, long flags, bool relative);
bool unit_start_user_animation_relative(long unit_index, long animation, long flags, bool relative, long relative_object_index);
void unit_scripting_set_current_vitality(long unit_index, float body_vitality, float shield_vitality);

// March unit_scripting.obj: whether the unit is running a custom (scripted) animation action.
static __forceinline bool unit_is_playing_custom_animation(long unit_index)
{
    bool playing = false;
    if (unit_index != -1)
        playing = action_executing(unit_index, _action_custom_animation);
    return playing;
}

#endif // UNITS_UNIT_SCRIPTING_H

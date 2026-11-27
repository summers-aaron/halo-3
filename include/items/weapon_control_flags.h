#ifndef ITEMS_WEAPON_CONTROL_FLAGS_H
#define ITEMS_WEAPON_CONTROL_FLAGS_H

enum e_weapon_control_flags
{
    _weapon_control_integrated_light = 0, // weapons toggles integrated_light_power while set; units maps control 22/27 onto it
    _weapon_control_trigger_down_0 = 1,  // trigger_down[0]; barrel 0 fires while set
    _weapon_control_trigger_down_1 = 2,  // trigger_down[1]; barrel 1 fires while set
    _weapon_control_trigger_held_0 = 3,  // trigger_held[0]
    _weapon_control_trigger_held_1 = 4,  // trigger_held[1]
    _weapon_control_busy = 5,            // the weapon is busy (reload owner); units sets it from the reload-owner check
    _weapon_control_autofire = 6,        // forces fire while set; units sets it from the player trigger ticks
    _weapon_control_use_secondary = 7,   // the trigger uses the secondary barrel; units sets it while zoomed
    _weapon_control_trigger_down_2 = 8,  // trigger_down[2]; units maps control 31 onto it
    k_weapon_control_flags_count = 8
};

#endif

#ifndef OBJECTS_OBJECT_DAMAGE_FLAGS_H
#define OBJECTS_OBJECT_DAMAGE_FLAGS_H

enum e_object_damage_flags
{
    _object_passed_shield_damage_threshold_bit = 1,
    _object_damage_body_depleted_bit = 2,
    _damage_shield_depleted = 3,
    _damage_shield_destroyed = 4,
    _object_shield_over_charging_bit = 5,
    _object_die_act_of_god_bit = 6,
    _object_die_act_of_god_silent_bit = 7,
    _object_body_charging_bit = 9,
    _object_shield_charging_bit = 10,
    _object_shield_charging_networked_bit = 11,
    _object_die_act_of_god_no_statistics_bit = 12,
    _object_shield_damage_scripted_bit = 13,
    _object_hanging_on_to_life_bit = 14,
    _object_die_from_no_riders_bit = 15,
    _object_damage_vitality_pinned_bit = 16
};

#endif // OBJECTS_OBJECT_DAMAGE_FLAGS_H

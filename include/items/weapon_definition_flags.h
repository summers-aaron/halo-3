#ifndef ITEMS_WEAPON_DEFINITION_FLAGS_H
#define ITEMS_WEAPON_DEFINITION_FLAGS_H

enum e_weapon_definition_flags
{
    _weapon_definition_must_be_readied = 3,
    _weapon_definition_does_not_count_towards_inventory_limit = 4,
    _weapon_definition_prevents_grenade_throwing = 6,
    _weapon_definition_detonates_when_dropped = 10,
    _weapon_definition_cannot_fire_at_maximum_age = 11,
    _weapon_definition_support_weapon = 13,
    _weapon_definition_cannot_fire_while_boosting = 19,
    _weapon_definition_can_be_dual_wielded = 22,
    _weapon_definition_can_only_be_dual_wielded = 23,
    _weapon_definition_melee_only = 24,
    _weapon_definition_cannot_fire_if_parent_dead = 25,
    _weapon_definition_weapon_ages = 26,
    _weapon_definition_dual_wield_error_simple = 27
};

#endif // ITEMS_WEAPON_DEFINITION_FLAGS_H

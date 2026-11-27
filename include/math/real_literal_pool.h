#ifndef MATH_REAL_LITERAL_POOL_H
#define MATH_REAL_LITERAL_POOL_H

// Shared pool anchors; storage remains with the existing owners.
extern "C" const float literal_4_0f;
// Neighboring-value pool view read by c_player_trait_weapons::get_damage_modifier_percentage.
extern "C" const float literal_1000_0f[];

// Shared 0.66f scalar.
extern "C" const float literal_0_66f;

// Address-only pool anchor; the behavior dive table retains its existing view.
extern "C" void literal_neg_0_5f();

#endif

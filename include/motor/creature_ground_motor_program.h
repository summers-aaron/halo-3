#ifndef MOTOR_CREATURE_GROUND_MOTOR_PROGRAM_H
#define MOTOR_CREATURE_GROUND_MOTOR_PROGRAM_H

// Functions defined in tu/objectslibxbox/creature_ground_motor_program.cpp.

bool creature_ground_in_airborne_state(long object_index);

// Pooled delay constant; the ground program also uses this address as its pool anchor.
extern "C" const float k_creature_ground_airborne_delay[];

#endif // MOTOR_CREATURE_GROUND_MOTOR_PROGRAM_H

#ifndef PHYSICS_PHYSICS_VARIABLES_H
#define PHYSICS_PHYSICS_VARIABLES_H

// Functions defined in tu/blamlibxbox/physics_variables.cpp that other units call.

struct physics_variable_position;

bool physics_variable_position_update_seek(float *value, const physics_variable_position *position, bool unused, float target, float rate, float delta_time);

struct physics_variable_speed;

extern "C" int physics_variable_speed_update_seek(float *v, const physics_variable_speed *s, float target, float unused, float dt);

#endif // PHYSICS_PHYSICS_VARIABLES_H

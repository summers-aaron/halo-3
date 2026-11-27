#ifndef PHYSICS_PHYSICS_VARIABLE_TYPES_H
#define PHYSICS_PHYSICS_VARIABLE_TYPES_H

struct physics_variable_speed
{
    float positive_limit;
    float negative_limit;
    float away_rate;
    float toward_rate;
};

struct physics_variable_position
{
    float upper_bound;
    float lower_bound;
};

typedef char physics_variable_speed_size[(sizeof(physics_variable_speed) == 16) ? 1 : -1];
typedef char physics_variable_position_size[(sizeof(physics_variable_position) == 8) ? 1 : -1];

#endif // PHYSICS_PHYSICS_VARIABLE_TYPES_H

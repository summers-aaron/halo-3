#ifndef NETWORKING_REPLICATION_REPLICATION_PREDICTION_H
#define NETWORKING_REPLICATION_REPLICATION_PREDICTION_H

#include <stddef.h>
#include "math/real_math_types.h"

// These fields are read and written individually.
// This leaf lets prediction users share the records without importing motion.
struct s_player_prediction
{
    long entity, identifier;
    unsigned char flags;
    real_point3d origin;
    real_vector3d forward, up, velocity, angular;
};

struct s_player_predicted_aim_assist
{
    float primary_autoaim_level;
    float secondary_autoaim_level;
    long target;
    long model_target;
    bool lead_vector_valid;
    bool only_lead_vector_valid;
    real_vector3d lead_vector;
};

typedef char player_prediction_size[(sizeof(s_player_prediction) == 0x48) ? 1 : -1];
typedef char player_prediction_origin_offset[(offsetof(s_player_prediction, origin) == 0xc) ? 1 : -1];
typedef char player_predicted_aim_assist_size[(sizeof(s_player_predicted_aim_assist) == 0x20) ? 1 : -1];
typedef char player_predicted_aim_assist_vector_offset[(offsetof(s_player_predicted_aim_assist, lead_vector) == 0x14) ? 1 : -1];

#endif // NETWORKING_REPLICATION_REPLICATION_PREDICTION_H

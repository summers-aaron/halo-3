#ifndef NETWORKING_REPLICATION_REPLICATION_MOTION_H
#define NETWORKING_REPLICATION_REPLICATION_MOTION_H

#include <stddef.h>
#include "networking/replication/replication_prediction.h"

struct s_player_motion
{
    long type;
    unsigned char identifier;
    real_euler_angles2d facing;
    real_vector2d throttle;
    unsigned short flags;
    short weapon_set;
    signed char primary, secondary;
    short zoom;
    bool fire[4], aim_exists;
    s_player_predicted_aim_assist aim_assist;
    bool exceeded;
};

typedef char player_motion_size[(sizeof(s_player_motion) == 0x4c) ? 1 : -1];
typedef char player_motion_aim_assist_offset[(offsetof(s_player_motion, aim_assist) == 0x28) ? 1 : -1];
typedef char player_motion_exceeded_offset[(offsetof(s_player_motion, exceeded) == 0x48) ? 1 : -1];

#endif // NETWORKING_REPLICATION_REPLICATION_MOTION_H

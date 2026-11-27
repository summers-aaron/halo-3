#ifndef AI_ACTOR_POSTURE_H
#define AI_ACTOR_POSTURE_H

// Legacy full-index C bridge used by motor_tasks_common.cpp.
// actor_moving.cpp declares an unsigned-short slot; preserve this caller ABI.
extern "C" long actor_get_posture(long actor_index);

#endif

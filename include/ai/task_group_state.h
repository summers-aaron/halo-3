#ifndef AI_TASK_GROUP_STATE_H
#define AI_TASK_GROUP_STATE_H

struct s_task_group;

typedef bool (*group_state_override_proc)(float *a, float *b);
typedef short (*group_state_next_state_proc)(s_task_group *group, float *a, float *b);
struct s_group_state_definition { short state; group_state_override_proc override; group_state_next_state_proc next_state; };

extern "C" { extern s_group_state_definition k_task_group_state_definitions[]; }

#endif // AI_TASK_GROUP_STATE_H

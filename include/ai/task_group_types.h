#ifndef AI_TASK_GROUP_TYPES_H
#define AI_TASK_GROUP_TYPES_H

struct s_task_group
{
    short state;
    short flags;
    long time;
};

typedef char task_group_size[(sizeof(s_task_group) == 8) ? 1 : -1];

#endif // AI_TASK_GROUP_TYPES_H

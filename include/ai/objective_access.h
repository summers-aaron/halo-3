#ifndef AI_OBJECTIVE_ACCESS_H
#define AI_OBJECTIVE_ACCESS_H

#include "ai/objective_definitions.h"
#include "ai/objective_storage.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario.h"
#include "tag_files/tag_groups.h"

static __forceinline s_task_record *objective_get_task_record(short objective_index, short task_index)
{
    s_objective_record *record = &((s_objective_record *)g_objectives_data->data)[(unsigned short)objective_index];
    short index = (short)(record->first_task + task_index);
    return &g_tasks_records[index];
}

inline s_task *objective_get_task(s_objective *objective, short task_index)
{
    return (s_task *)tag_block_get_element_with_size(&objective->tasks, task_index, sizeof(s_task));
}

inline s_task *objective_get_task(short objective_index, short task_index)
{
    return objective_get_task(
        (s_objective *)tag_block_get_element_with_size(&global_scenario->objectives, objective_index, sizeof(s_objective)),
        task_index);
}

#endif // AI_OBJECTIVE_ACCESS_H

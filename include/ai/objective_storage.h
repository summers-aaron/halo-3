#ifndef AI_OBJECTIVE_STORAGE_H
#define AI_OBJECTIVE_STORAGE_H

#include "ai/objective_runtime.h"

extern "C" {
extern __declspec(thread) s_data_array *g_objectives_data;
extern __declspec(thread) s_task_record *g_tasks_records;
}

#endif

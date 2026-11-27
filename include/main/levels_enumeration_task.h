#ifndef MAIN_LEVELS_ENUMERATION_TASK_H
#define MAIN_LEVELS_ENUMERATION_TASK_H

struct s_find_file_data;

struct s_levels_enumeration_task
{
    long state;
    s_find_file_data *find_file_data;
};

#endif // MAIN_LEVELS_ENUMERATION_TASK_H

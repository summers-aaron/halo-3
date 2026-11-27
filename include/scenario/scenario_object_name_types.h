#ifndef SCENARIO_SCENARIO_OBJECT_NAME_TYPES_H
#define SCENARIO_SCENARIO_OBJECT_NAME_TYPES_H

struct s_scenario_object_name
{
    char name[32];
    short object_type;
    short placement_index;
};

typedef char s_scenario_object_name_size_assert[(sizeof(s_scenario_object_name) == 0x24) ? 1 : -1];

#endif // SCENARIO_SCENARIO_OBJECT_NAME_TYPES_H

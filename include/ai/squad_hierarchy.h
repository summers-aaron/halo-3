#ifndef AI_SQUAD_HIERARCHY_H
#define AI_SQUAD_HIERARCHY_H

extern "C" bool squad_ancestor_of_squad_group(unsigned int squad_index, int group_index);
extern "C++" void squads_build_hierarchy(void);

#endif // AI_SQUAD_HIERARCHY_H

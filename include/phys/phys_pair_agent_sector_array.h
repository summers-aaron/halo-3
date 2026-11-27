#ifndef INCLUDE_PHYS_PAIR_AGENT_SECTOR_ARRAY_H
#define INCLUDE_PHYS_PAIR_AGENT_SECTOR_ARRAY_H
#include "phys/phys_base.h"
struct phys_pair_agent_sector;
template <>
void phys_array<phys_pair_agent_sector *>::insert_at(int index, phys_pair_agent_sector *const *entries, int count);
#endif

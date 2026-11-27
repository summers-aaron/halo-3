// interfacelibxbox/overhead_map.obj: from the records 0x47c790
// @flags /TP /O2
// @address 0x47c790 ?overhead_map_initialize_for_new_map@@YAXXZ
#include <string.h>
#include "interface/overhead_map.h"

struct s_overhead_map_globals
{
    char map_state[144];
};

s_overhead_map_globals overhead_map_globals;

// @address 0x5bec98 ?overhead_map_initialize@@YAXXZ
void overhead_map_initialize(void)
{
}

// @address 0x5bec98 ?overhead_map_dispose@@YAXXZ
void overhead_map_dispose(void)
{
}

void overhead_map_initialize_for_new_map(void)
{
    memset(&overhead_map_globals, 0, sizeof(overhead_map_globals));
}

// @address 0x5bec98 ?overhead_map_dispose_from_old_map@@YAXXZ
void overhead_map_dispose_from_old_map(void)
{
}

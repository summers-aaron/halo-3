#ifndef NETWORKING_LOGIC_NETWORK_RECRUITING_SEARCH_INTERNALS_H
#define NETWORKING_LOGIC_NETWORK_RECRUITING_SEARCH_INTERNALS_H

#include "memory/static_arrays.h"
#include "networking/logic/network_recruiting_search.h"
#include "networking/logic/logic_recruiting_seeker_types.h"
#include "networking/logic/life_cycle/life_cycle_matchmaking_quality.h"
#include "networking/logic/network_search_types.h"

struct s_network_recruiting_search_globals
{
    bool initialized;
    __declspec(align(8)) c_recruiting_seeker seeker;
    c_matchmaking_quality matchmaking_quality;
    char quality_storage_tail[4];
    bool enabled;
    bool dirty;
    int count;
    s_available_session *entries;
};

typedef char recruiting_quality_offset[(offsetof(s_network_recruiting_search_globals, matchmaking_quality) == 0x2e40) ? 1 : -1];
typedef char recruiting_enabled_offset[(sizeof(void *) != 4 || ((offsetof(s_network_recruiting_search_globals, enabled) == 0xc6b8))) ? 1 : -1];
typedef char recruiting_count_offset[(sizeof(void *) != 4 || ((offsetof(s_network_recruiting_search_globals, count) == 0xc6bc))) ? 1 : -1];
typedef char recruiting_globals_size[(sizeof(void *) != 4 || ((sizeof(s_network_recruiting_search_globals) == 0xc6c8))) ? 1 : -1];



#endif // NETWORKING_LOGIC_NETWORK_RECRUITING_SEARCH_INTERNALS_H

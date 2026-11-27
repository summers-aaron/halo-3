// networkinglibxbox/network_recruiting_search.obj: from the records 0x90738
// @flags /TP /O2 /Oc
// @address 0x90738 ?network_recruiting_search_update@@YAXXZ
// @address 0x8c447c g_network_time_globals
#include "cseries/crt_xdk.h"
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "networking/logic/network_recruiting_search.h"
#include "networking/logic/network_recruiting_search_internals.h"
#include "networking/network_time_types.h"

void network_recruiting_search_update(void)
{
    if (g_recruiting_search_globals.enabled) {
        if (!(g_recruiting_search_globals.seeker.flags.get() & 32)) {
            g_recruiting_search_globals.seeker.update_session_search();
        }
        for (int i = 0; i < g_recruiting_search_globals.count; i++) {
            s_available_session *entry = &g_recruiting_search_globals.entries[i];
            if (entry->active) {
                int entry_time = entry->last_reply_time;
                int now;
                if (g_network_time_globals.enabled) {
                    now = g_network_time_globals.time;
                } else {
                    now = GetTickCount();
                }
                if (now - entry_time > 10000) {
                    memset(entry, 0, sizeof(*entry));
                    g_recruiting_search_globals.dirty = 1;
                }
            }
        }
    }
}

s_network_recruiting_search_globals g_recruiting_search_globals;

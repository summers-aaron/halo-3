// interfacelibxbox/user_interface_network_search.obj: from the records 0x4a0b18 0x4a0ba0
// @flags /TP /O2
#include "interface/user_interface_network_search.h"
#include "networking/logic/network_search.h"

#include "networking/logic/network_search_globals.h"
// @address 0x80a2e4 g_network_search_globals
// @address 0x8c4948 g_user_interface_network_search_started
static __forceinline bool count_valid(short count)
{
    return count >= 0 && 1;
}
static __forceinline bool status_valid(const s_network_squad_status_data *status)
{
    short count = status->details.player_count;
    return count_valid(count) && count <= 16;
}

// ?user_interface_available_squads_iterate@@YAJJ@Z (0x4a0b18)
long user_interface_available_squads_iterate(long index)
{
    long result = -1;
    if (g_user_interface_network_search_started) {
        long i = index == -1 ? 0 : index + 1;
        long count = g_network_search_globals.count;
        for (; i < count; i = i + 1) {
            if (network_search_session(i) != 0) {
                result = i;
                break;
            }
        }
    }
    return result;
}

const s_network_squad_status_data *user_interface_available_squad_get_status(long index)
{
    s_network_squad_status_data *result = 0;
    if (g_user_interface_network_search_started) {
        s_available_session *session = (s_available_session *)network_search_session(index);
        if (session) {
            s_network_squad_status_data *status = &session->status;
            if (status_valid(status))
                result = status;
        }
    }
    return (const s_network_squad_status_data *)result;
}

#ifndef NETWORKING_ONLINE_ONLINE_SESSION_SEARCH_XENON_H
#define NETWORKING_ONLINE_ONLINE_SESSION_SEARCH_XENON_H

#include "networking/online/online_session_search_types.h"

#include <stddef.h>


class c_session_search_overlapped_task;
class c_session_search_by_id_overlapped_task;
#pragma pack(push, 4)
struct s_online_session_search_globals
{
    bool initialized;
    c_session_search_overlapped_task *search_task;
    c_session_search_by_id_overlapped_task *search_by_id_task;
};
#pragma pack(pop)
typedef char online_session_search_globals_size[(sizeof(void *) != 4 || ((sizeof(s_online_session_search_globals) == 12) )) ? 1 : -1];
typedef char online_session_search_task_offset[(offsetof(s_online_session_search_globals, search_task) == 4) ? 1 : -1];

e_online_session_search_status online_session_search_by_id_get_status();

// Functions defined in tu/networkinglibxbox/online_session_search_xenon.cpp.

struct s_online_session_search_parameters;

void online_session_search_initialize();
bool online_session_search_initiate(const s_online_session_search_parameters *parameters);

struct s_online_search_query_internal;

extern "C" bool online_session_search_build_internal_search_parameters(
    // match: 0x3d2a68: restricted parameter pointers prevent alias reloads.
    const s_online_session_search_parameters *__restrict parameters, s_online_search_query_internal *__restrict internal_parameters);

// Data defined in tu/networkinglibxbox/online_session_search_xenon.cpp that other units use.
extern s_online_session_search_globals online_session_search_globals;
extern unsigned long k_online_property_id_map[];
extern unsigned long k_online_context_value_map[];
extern unsigned long k_online_context_id_map[];
extern unsigned long k_online_session_search_query_id_map[];
e_online_session_search_status online_session_search_get_status();

#endif // NETWORKING_ONLINE_ONLINE_SESSION_SEARCH_XENON_H

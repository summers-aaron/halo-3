#ifndef NETWORKING_ONLINE_ONLINE_SESSION_RESULTS_TYPES_H
#define NETWORKING_ONLINE_ONLINE_SESSION_RESULTS_TYPES_H

#include <xtl.h>
#include "networking/transport/transport_security_types.h"

struct s_online_search_result
{
    wchar_t name[32];
    union
    {
        XSESSION_INFO info;
        s_transport_session_description description;
    };
    unsigned long public_open, private_open, public_filled, private_filled;
    long property_count;
    XUSER_PROPERTY properties[3];
    long context_count;
    XUSER_CONTEXT contexts[2];
};
typedef char online_session_description_size[(sizeof(XSESSION_INFO) == sizeof(s_transport_session_description)) ? 1 : -1];

struct s_online_search_results { unsigned long count; s_online_search_result entries[50]; };

#endif

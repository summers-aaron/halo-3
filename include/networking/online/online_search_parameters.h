#ifndef NETWORKING_ONLINE_ONLINE_SEARCH_PARAMETERS_H
#define NETWORKING_ONLINE_ONLINE_SEARCH_PARAMETERS_H

#include "networking/online/online_context_types.h"
#include "cseries/cseries_wrapped.h"

#include <stddef.h>

enum e_online_data_type;

struct s_online_property
{
    long id;
    long pad4;
    c_enum<e_online_data_type, unsigned char, 0, 255> type;
    union { long value; float real_value; double real; unsigned __int64 integer64; };
};

#pragma pack(push, 4)
struct s_online_session_search_parameters
{
    long query_type;
    long controller_index;
    long property_count;
    long padc;
    s_online_property properties[20];
    long context_count;
    s_online_context contexts[6];
};
#pragma pack(pop)

// Property payload and alignment are the XDK XUSER_PROPERTY/XUSER_DATA ABI.
#if defined(_M_PPCBE) || defined(_XBOX)
typedef char online_search_context_size[(sizeof(s_online_context) == 8) ? 1 : -1];
typedef char online_search_property_size[(sizeof(void *) != 4 || (sizeof(s_online_property) == 24)) ? 1 : -1];
typedef char online_search_property_type_offset[(offsetof(s_online_property, type) == 8) ? 1 : -1];
typedef char online_search_property_value_offset[(offsetof(s_online_property, value) == 16) ? 1 : -1];
typedef char online_search_parameters_size[(sizeof(s_online_session_search_parameters) == 548) ? 1 : -1];
typedef char online_search_parameters_properties_offset[(offsetof(s_online_session_search_parameters, properties) == 16) ? 1 : -1];
typedef char online_search_parameters_contexts_offset[(sizeof(void *) != 4 || (offsetof(s_online_session_search_parameters, contexts) == 500)) ? 1 : -1];
#endif

#endif

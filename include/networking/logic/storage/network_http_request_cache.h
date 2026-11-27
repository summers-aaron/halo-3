#ifndef NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_REQUEST_CACHE_H
#define NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_REQUEST_CACHE_H

#include "memory/static_arrays.h"

// Functions defined in tu/networkinglibxbox/network_http_request_cache.cpp that other units call.

class c_network_http_request_description;
struct s_network_http_request_hash;

void network_http_request_cache_add_file_to_cache(const c_network_http_request_description *description, const void *data,
    long size, s_network_http_request_hash *hash);

struct s_network_request_cache_entry
{
    c_static_string<256> name;
    unsigned char digest[20];
};

// network_update clears the whole cache once the first entry is older than the configured interval.
struct s_network_request_cache_globals
{
    s_network_request_cache_entry entries[32];
    unsigned long first_entry_time;
};

typedef char network_request_cache_entry_size[(sizeof(s_network_request_cache_entry) == 276) ? 1 : -1];
typedef char network_request_cache_globals_size[(sizeof(s_network_request_cache_globals) == 8836) ? 1 : -1];

// Data defined in tu/networkinglibxbox/network_http_request_cache.cpp that other units use.
extern s_network_request_cache_globals g_network_request_cache_globals;

#endif // NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_REQUEST_CACHE_H

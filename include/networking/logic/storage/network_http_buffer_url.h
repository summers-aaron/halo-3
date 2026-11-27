#ifndef NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_BUFFER_URL_H
#define NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_BUFFER_URL_H

class c_http_buffer_downloader;

// The URL a buffer downloader fetches: the same layout as c_url_string (the text, service type, cache mode, lifetime
// and queue type), held by c_http_buffer_downloader::m_url.
struct s_network_http_buffer_url
{
    char name[256];
    long service_type;
    long cache_mode;
    long lifetime_seconds;
    long queue_type;
};

// Defined in tu/networkinglibxbox/network_http_buffer_downloader.cpp: resets the downloader when the URL changes.
#include "networking/logic/storage/network_http_buffer_url_api.h"

#endif // NETWORKING_LOGIC_STORAGE_NETWORK_HTTP_BUFFER_URL_H

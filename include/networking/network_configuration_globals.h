#ifndef NETWORKING_NETWORK_CONFIGURATION_GLOBALS_H
#define NETWORKING_NETWORK_CONFIGURATION_GLOBALS_H

#include "networking/network_configuration.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include <stddef.h>

// The network configuration file download: a 7909-byte stored buffer and the last parsed result.
struct s_network_configuration_downloader
{
    c_http_stored_buffer_downloader<7909> m_downloader;
    long m_last_retry_count;
    void *m_data;
    unsigned long m_data_size;
};

struct s_network_configuration_state
{
    s_network_configuration_downloader download;
    bool download_failed;
    bool downloaded_data_valid;
    long last_processed_retry_count;
};

typedef char network_configuration_downloader_size[(sizeof(void *) != 4 || ((sizeof(s_network_configuration_downloader) == 0x2584) )) ? 1 : -1];
typedef char network_configuration_state_size[(sizeof(void *) != 4 || ((sizeof(s_network_configuration_state) == 0x258c) )) ? 1 : -1];
typedef char network_configuration_state_failed_offset[(sizeof(void *) != 4
    || ((offsetof(s_network_configuration_state, download_failed) == 0x2584) )) ? 1 : -1];
typedef char network_configuration_state_retry_offset[(sizeof(void *) != 4
    || ((offsetof(s_network_configuration_state, last_processed_retry_count) == 0x2588) )) ? 1 : -1];

#endif // NETWORKING_NETWORK_CONFIGURATION_GLOBALS_H

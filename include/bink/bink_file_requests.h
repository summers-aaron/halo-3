#ifndef BINK_BINK_FILE_REQUESTS_H
#define BINK_BINK_FILE_REQUESTS_H

#include "tag_files/file_reference.h"
#include "memory/static_arrays.h"

// Staging view of one netdebug upload slot, layout-identical to
// s_netdebug_upload_task; bink_playback_queue_file_request fills one
// locally and copies it into g_netdebug_globals.requests.
struct s_bink_file_request
{
    bool active;
    c_static_string<256> stream_name;
    void (*progress_callback)(long, long);
    void (*completion_callback)(bool, void *);
    void *callback_data;
    s_file_reference file;
    unsigned long file_crc;
};

#endif

#ifndef NETWORKING_TOOLS_NETWORK_WEBSTATS_GLOBALS_H
#define NETWORKING_TOOLS_NETWORK_WEBSTATS_GLOBALS_H

// Shared allocation at g_webstats_globals. Keep the unresolved storage explicit.
#include "networking/tools/http_client.h"
#include "tag_files/file_reference.h"

struct s_webstats_upload_buffer
{
    long size;
    long version;
    unsigned long uncompressed_size;
    char compressed_data[0x9ff4];
};

// The file/zip upload path fields keep their March 2007 debug build names:
// submit_file stores the filename/content type here and the zip worker asserts
// on zip_file, file_in_zip_open, uncompressed_source_file_open and
// compression_bytes_remaining_to_read. March offsets match the shipped image
// +0x410 from upload_queue_count through the tail. The two 3-byte runs are
// never read or written in either build.
#pragma pack(push, 1)
struct s_webstats_globals {
    bool upload_queued;
    __declspec(align(4)) c_http_post_stream http_stream;
    __declspec(align(8)) c_http_client http_client;
    s_webstats_upload_buffer upload_buffer;
    long upload_queue_count;
    s_file_reference uncompressed_source_file;
    bool uncompressed_source_file_open;
    s_file_reference upload_file;
    char content_type[32];
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    void (*failure_callback)(long);
    void *zip_file;
    bool file_in_zip_open;
    unsigned char : 8;
    unsigned char : 8;
    unsigned char : 8;
    long compression_bytes_remaining_to_read;
    long upload_state;
    long service_type;
    long server_token;
    long failure_count;
    long step_timeout_seconds;
    long step_deadline_tick;
};
#pragma pack(pop)
typedef char webstats_stream_offset[(offsetof(s_webstats_globals, http_stream) == 4) ? 1 : -1];
typedef char webstats_client_offset[(sizeof(void *) != 4 || ((offsetof(s_webstats_globals, http_client) == 0xdd8))) ? 1 : -1];
typedef char webstats_source_file_offset[(sizeof(void *) != 4 || ((offsetof(s_webstats_globals, uncompressed_source_file) == 0xbe3c))) ? 1 : -1];
typedef char webstats_source_open_offset[(sizeof(void *) != 4 || ((offsetof(s_webstats_globals, uncompressed_source_file_open) == 0xbf4c))) ? 1 : -1];
typedef char webstats_file_offset[(sizeof(void *) != 4 || ((offsetof(s_webstats_globals, upload_file) == 0xbf4d))) ? 1 : -1];
typedef char webstats_content_type_offset[(sizeof(void *) != 4 || ((offsetof(s_webstats_globals, content_type) == 0xc05d))) ? 1 : -1];
typedef char webstats_callback_offset[(sizeof(void *) != 4 || ((offsetof(s_webstats_globals, failure_callback) == 0xc080))) ? 1 : -1];
typedef char webstats_zip_file_offset[(sizeof(void *) != 4 || ((offsetof(s_webstats_globals, zip_file) == 0xc084))) ? 1 : -1];
typedef char webstats_zip_open_offset[(sizeof(void *) != 4 || ((offsetof(s_webstats_globals, file_in_zip_open) == 0xc088))) ? 1 : -1];
typedef char webstats_compression_left_offset[
    (sizeof(void *) != 4 || ((offsetof(s_webstats_globals, compression_bytes_remaining_to_read) == 0xc08c))) ? 1 : -1];
typedef char webstats_upload_state_offset[(sizeof(void *) != 4 || ((offsetof(s_webstats_globals, upload_state) == 0xc090))) ? 1 : -1];


#endif

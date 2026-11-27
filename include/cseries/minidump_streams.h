#ifndef CSERIES_MINIDUMP_STREAMS_H
#define CSERIES_MINIDUMP_STREAMS_H

#include <xtl.h>
#include <stddef.h>

// Windows minidump file header; the flags occupy bytes 24..31.
struct _MINIDUMP_HEADER
{
    union
    {
        unsigned long words[8];
        struct
        {
            unsigned long signature;
            unsigned long version;
            unsigned long stream_count;
            unsigned long directory_rva;
            unsigned long checksum;
            unsigned long timestamp;
            ULARGE_INTEGER flags;
        } fields;
    };
};
typedef char main_minidump_file_header_size[(sizeof(_MINIDUMP_HEADER) == 32) ? 1 : -1];
typedef char main_minidump_file_header_flags_offset[(offsetof(_MINIDUMP_HEADER, fields.flags) == 24) ? 1 : -1];

struct s_minidump_writer;

extern "C" {
bool minidump_writer_write_system_info(s_minidump_writer *writer);
bool minidump_writer_write_thread_list(s_minidump_writer *writer);
bool minidump_writer_write_module_list(s_minidump_writer *writer);
bool minidump_writer_write_module_names(s_minidump_writer *writer);
bool minidump_writer_write_exception_stream(s_minidump_writer *writer);
bool minidump_writer_write_version_block(s_minidump_writer *writer);
int minidump_writer_write_memory_list(s_minidump_writer *writer);
}

#endif // CSERIES_MINIDUMP_STREAMS_H

#ifndef CSERIES_MINIDUMP_WRITER_H
#define CSERIES_MINIDUMP_WRITER_H

// Functions defined in tu/blamlibxbox/minidump_writer.cpp, and the helpers it calls in other units.

#include "cseries/minidump_interfaces.h"
#include "cseries/xenon_minidump_endian.h"

struct s_minidump_writer;

#pragma pack(push, 4)
struct _MINIDUMP_MODULE
{
    unsigned __int64 base_of_image;
    unsigned long size_of_image;
    unsigned long checksum;
    unsigned long time_date_stamp;
    unsigned long module_name_rva;
    unsigned long signature;
    unsigned long struct_version;
    unsigned long file_version_ms;
    unsigned long file_version_ls;
    unsigned long product_version_ms;
    unsigned long product_version_ls;
    unsigned long file_flags_mask;
    unsigned long file_flags;
    unsigned long file_os;
    unsigned long file_type;
    unsigned long file_subtype;
    unsigned long file_date_hi;
    unsigned long file_date_lo;
    s_minidump_location_descriptor cv_record;
    s_minidump_location_descriptor misc_record;
    unsigned __int64 reserved0;
    unsigned __int64 reserved1;
};
#pragma pack(pop)

struct _MINIDUMP_MISC_INFO
{
    unsigned long size_of_info;
    unsigned long flags1;
    unsigned long process_id;
    unsigned long process_create_time;
    unsigned long process_user_time;
    unsigned long process_kernel_time;
};

extern "C" {
bool minidump_test_memory_readable(const void *memory, unsigned long size);
bool minidump_writer_write_thread_contexts(s_minidump_writer *writer, const s_thread_information *threads, long thread_count);
bool minidump_writer_write_thread_entries(s_minidump_writer *writer, s_thread_information *threads, long thread_count);
bool minidump_writer_write_module_names(s_minidump_writer *writer);
bool main_write_uint32_value(s_minidump_writer *writer, long value); // main.cpp
bool minidump_writer_write_string(s_minidump_writer *writer, const char *name);
void minidump_writer_initialize_exception_stream(s_minidump_writer *writer, MINIDUMP_EXCEPTION_STREAM *stream);
void minidump_writer_add_directory_entry(s_minidump_writer *writer, long rva, long data_size, long stream_type);
void xenon_minidump_endian_swap_module(_MINIDUMP_MODULE *module); // xenon_minidump_endian.cpp
}

// The compiler intrinsic that reads a general register (the XDK headers do not declare it).
#if defined(_MSC_VER) && !defined(__clang__)
extern "C" unsigned __int64 __gregister_get(unsigned int reg);
#else
// Port fallback: the PC has no PowerPC register file, so report the current frame address.
inline unsigned __int64 __gregister_get(unsigned int /*reg*/)
{
    return (unsigned __int64)(size_t)__builtin_frame_address(0);
}
#endif

#endif // CSERIES_MINIDUMP_WRITER_H

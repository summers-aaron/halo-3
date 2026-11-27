#ifndef CSERIES_MINIDUMP_INTERFACES_H
#define CSERIES_MINIDUMP_INTERFACES_H

#include "cseries/pointer_utilities.h"

struct _EXCEPTION_POINTERS;
struct s_thread_information;
struct _MINIDUMP_MODULE;
struct _MINIDUMP_MISC_INFO;
struct _MINIDUMP_SYSTEM_INFO;
struct MINIDUMP_EXCEPTION_STREAM;
struct _MINIDUMP_HEADER;

// March c_minidump_file_output_insecure vtable at 0x82174f64, in slot order.
class c_minidump_file_output_interface
{
public:
    virtual void open(const char *name) = 0;
    virtual bool close() = 0;
    virtual bool is_valid() = 0;
    virtual bool write(const void *data, long size) = 0;
    __forceinline bool write(long offset, long size)
    {
        return write(pointer_from_address((unsigned int)offset), size);
    }
    virtual bool seek(long position) = 0;
    virtual long tell() = 0;
};

// March c_retail_minidump_info_provider vtable at 0x8216de28, in slot order.
class c_minidump_provider_interface
{
public:
    virtual bool minidump_begin(_EXCEPTION_POINTERS *exception) = 0;
    virtual void minidump_end() = 0;
    virtual void suspend_non_calling_threads() = 0;
    virtual void resume_non_calling_threads() = 0;
    virtual void begin_thread_dump() = 0;
    virtual void end_thread_dump() = 0;
    virtual void get_thread_id_array(unsigned long *ids, unsigned long maximum, unsigned long *count) = 0;
    virtual void get_thread_information(unsigned long id, s_thread_information *info) = 0;
    virtual void get_minidump_header(_MINIDUMP_HEADER *header, unsigned long stream_count) = 0;
    virtual void get_loaded_module_count(unsigned long *count) = 0;
    virtual void get_loaded_module(unsigned long index, _MINIDUMP_MODULE *module) = 0;
    virtual void get_loaded_module_name(unsigned long index, char *name, unsigned long size) = 0;
    virtual void get_misc_information(_MINIDUMP_MISC_INFO *info) = 0;
    virtual void get_system_information(_MINIDUMP_SYSTEM_INFO *info) = 0;
    virtual void get_system_version_string(char *name, unsigned long size) = 0;
    virtual void update_exception_information(MINIDUMP_EXCEPTION_STREAM *stream) = 0;
    virtual bool start_committed_region_query() = 0;
    virtual void end_committed_region_query() = 0;
    virtual void get_committed_region_count(unsigned long *count) = 0;
    virtual bool get_next_committed_region_info(unsigned long *base, unsigned long *size) = 0;
    virtual bool should_resolve_memory() = 0;
    virtual bool resolve_memory(const void *source, unsigned long size, void *destination, unsigned long *resolved_size) = 0;
    virtual bool start_module_section_memory_query(unsigned long module) = 0;
    virtual void end_module_section_memory_query() = 0;
    virtual void get_loaded_module_section_count(unsigned long module, unsigned long *count) = 0;
    virtual bool get_next_module_section_memory_info(unsigned long *base, unsigned long *size) = 0;
    virtual ~c_minidump_provider_interface() {}
};

#endif // CSERIES_MINIDUMP_INTERFACES_H

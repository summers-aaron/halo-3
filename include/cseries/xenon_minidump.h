#ifndef CSERIES_XENON_MINIDUMP_H
#define CSERIES_XENON_MINIDUMP_H

// Functions defined in tu/blamlibxbox/xenon_minidump.cpp.

struct s_xenon_minidump_memory_region
{
    unsigned long base_address;
    unsigned long size;
};

typedef char xenon_minidump_memory_region_size[(sizeof(long) != 4 || sizeof(s_xenon_minidump_memory_region) == 8) ? 1 : -1];

struct xenon_minidump_geometry_buffer;

extern "C" bool xenon_minidump_get_game_state_region(s_xenon_minidump_memory_region *region);
extern "C" bool xenon_minidump_get_geometry_data(xenon_minidump_geometry_buffer *out);

// A loaded module's description as the system reports it: load address, size, link timestamp, checksum, flags (bit 0:
// a DLL) and file name.
struct s_xenon_minidump_module_info
{
    unsigned long image_base;
    unsigned long image_size;
    unsigned long timestamp;
    unsigned long checksum;
    unsigned long flags;
    char filename[260];
};

// A thread's kernel description as the system reports it: suspend count, priority, TLS base, entry point, stack
// bounds, creation time, stack slack, current processor and last error.
struct s_xenon_minidump_thread_information
{
    long suspend_count;
    long priority;
    long tls_base;
    long start_address;
    long stack_base;
    long stack_limit;
    unsigned __int64 creation_time;
    unsigned long stack_slack_space;
    unsigned char current_processor;
    unsigned long last_error;
};
typedef char xenon_minidump_thread_information_size[(sizeof(long) != 4 || sizeof(s_xenon_minidump_thread_information) == 0x30) ? 1 : -1];

struct s_minidump_writer_context;
extern "C" {
// The kernel clears *info before looking up the module by handle.
int xenon_minidump_get_module_info(int module, s_xenon_minidump_module_info *info);
void *xenon_minidump_get_thread_handle(int thread_id);
void xenon_minidump_get_thread_information(void *thread, s_xenon_minidump_thread_information *information);
void xenon_minidump_get_thread_context(void *thread, s_minidump_writer_context *context);
void xenon_minidump_enumerate_thread_handles(int, void *);
__int64 xenon_minidump_get_loaded_module_count(void);
int tag_resource_cache_next_readable_memory_range(int, void *);
int tag_resource_cache_module_get_section(int module, unsigned int section_index, void *section);
int tag_resource_cache_module_section_count(int);
}

#endif // CSERIES_XENON_MINIDUMP_H

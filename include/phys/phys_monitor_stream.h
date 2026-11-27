#ifndef INCLUDE_PHYS_MONITOR_STREAM_H
#define INCLUDE_PHYS_MONITOR_STREAM_H

#include <stddef.h>

#include "phys/phys_base_types.h"
#include "phys/phys_thread_memory.h"

class phys_monitor_stream {
public:
    phys_monitor_stream() {}
    static phys_thread_local_data<phys_monitor_stream *> m_instance;
    static void init();
    static __forceinline phys_monitor_stream &get_singleton_instance() { return *m_instance.get_chain_data(); }
    void reset();
    void resize(int new_size);
    void quit_system();
    void set_static_buffer(char *buffer, int buffer_size);
    __forceinline char *get_start() { return m_start.val(); }
    __forceinline char *get_track_end() { return m_end.val(); }
    __forceinline char *get_sector_capacity() { return m_small_capacity.val(); }
    __forceinline char *get_capacity_minus16() { return m_capacity_minus_16.val(); }
    __forceinline void set_stream_end(char *end) { m_end = end; }
    __forceinline phys_bool has_free_memory() { return get_track_end() < get_capacity_minus16(); }
    __forceinline phys_bool is_buffer_on_heap() { return m_heap_buffer; }

    phys_padded<char *> m_start;
    phys_padded<char *> m_end;
    phys_padded<char *> m_small_capacity;
    phys_padded<char *> m_capacity_minus_16;
    phys_bool m_heap_buffer;
};

typedef char havok_size_hkMonitorStream[(sizeof(void *) != 4 || ((sizeof(phys_monitor_stream) == 0x14) )) ? 1 : -1];
typedef char havok_offset_hkMonitorStream_heap[(sizeof(void *) != 4 || ((offsetof(phys_monitor_stream, m_heap_buffer) == 0x10) )) ? 1 : -1];

// Functions defined in tu/hkbase/hkMonitorStream.cpp.

extern "C" void phys_monitor_stream_delete(void *data);

#endif // HAVOK_HK_MONITOR_STREAM_H

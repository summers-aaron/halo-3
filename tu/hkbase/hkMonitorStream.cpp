// hkbase/hkMonitorStream.obj: from the records 0x1ba870 0x1ba8b0 0x60d498 0x60d4a8 0x60d500 0x60d570 0x60d618
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x60d498 ?reset@phys_monitor_stream@@QAAXXZ
// @address 0x60d500 ?quit_system@phys_monitor_stream@@QAAXXZ
// @address 0x60d570 ?resize@phys_monitor_stream@@QAAXH@Z
#include "phys/phys_monitor_stream.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_base_types.h"
#include "physics/havok_monitor_storage.h"

#include "phys/phys_thread_memory_allocator.h"

phys_thread_local_data<phys_monitor_stream *> phys_monitor_stream::m_instance;
void phys_monitor_stream::reset()
{
    m_end = m_start;
}
void phys_monitor_stream::quit_system()
{
    if (get_start() != 0 && is_buffer_on_heap())
        phys_free_memory(get_start());
    phys_monitor_stream_delete(m_instance.get_chain_data());
    m_instance.lock_set_data(0);
}
// @address 0x60d618 ?set_static_buffer@phys_monitor_stream@@QAAXPADH@Z
void phys_monitor_stream::set_static_buffer(char *buffer, int solver_buffer_size)
{
    if (is_buffer_on_heap() && m_small_capacity.val() - m_start.val() != 0)
        quit_system();
    m_heap_buffer = false;
    m_start = buffer;
    m_end = buffer;
    m_small_capacity = m_start + solver_buffer_size;
    m_capacity_minus_16 = m_small_capacity - 32;
}
void phys_monitor_stream::resize(int new_size)
{
    if (new_size == get_sector_capacity() - get_start())
        return;
    if (new_size > 0) {
        if (get_start() != 0 && is_buffer_on_heap())
            phys_free_memory(get_start());
        m_heap_buffer = true;
        m_start = phys_alloc<char>(new_size, PHYS_MEM_MONITOR);
        m_end = m_start;
        m_small_capacity = (char *)m_start + new_size;
        m_capacity_minus_16 = (char *)m_small_capacity - 32;
    } else {
        quit_system();
    }
}

// @address 0x74b910 g_havok_monitor_globals

// @address 0x1ba870 phys_monitor_stream_new
extern "C" void *phys_monitor_stream_new(void)
{
    return &g_havok_monitor_globals.monitor_streams[g_havok_monitor_globals.monitor_stream_count++];
}

// @address 0x1ba8b0 phys_monitor_stream_delete
extern "C" void phys_monitor_stream_delete(void *data)
{
    g_havok_monitor_globals.monitor_stream_count--;
}

// @address 0x60d4a8 ?init@phys_monitor_stream@@SAXXZ
void phys_monitor_stream::init()
{
    phys_monitor_stream *instance = (phys_monitor_stream *)phys_monitor_stream_new();
    m_instance.lock_set_data(instance);
    phys_monitor_stream *stream = m_instance.get_chain_data();
    stream->m_heap_buffer = false;
    stream->m_start = 0;
    stream->m_small_capacity = 0;
    stream->m_end = 0;
    stream->m_capacity_minus_16 = 0;
}


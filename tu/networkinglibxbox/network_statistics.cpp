// networkinglibxbox/network_statistics.obj: from the records 0x43c8b0 0x43c960 0x43c9d0 0x43cbf0 0x43cc78
// @flags /TP /O2 /Oc
// @address 0x43c8b0 ?initialize@c_network_time_statistics@@QAAXJ@Z
// @address 0x43c960 ?add_event@c_network_time_statistics@@QAAXJ@Z
// @address 0x43cc78 ?add_event@c_network_window_statistics@@QAAXJ@Z
#define NOD3D
#include "multithreading/synchronization_types.h"
#include "cseries/cseries_types.h"
#include <string.h>
#include <ppcintrinsics.h>
#include "multithreading/synchronization.h"
#include "networking/network_statistics.h"
#include "networking/network_time_types.h"
#include "networking/network_statistics.h"



inline unsigned long NS_NETWORK_TIME_GET()
{
    if (g_network_time_globals.enabled) return g_network_time_globals.time;
    return GetTickCount();
}

void c_network_time_statistics::initialize(long period_duration_msec)
{
    m_period_duration_msec = period_duration_msec;
    m_interval_duration_msec = m_period_duration_msec / 20;
    m_period_reciprocal = 1000.0f / (float)m_period_duration_msec;
    reset();
}

void c_network_time_statistics::add_event(long value)
{
    check_interval_rollover();
    m_total_events = m_total_events + 1;
    m_total_values = (long long)value + m_total_values;
    m_current.events = m_current.events + 1;
    m_current.total_values = value + m_current.total_values;
}

// @address 0x43c9d0 ?check_interval_rollover@c_network_time_statistics@@AAAXXZ
void c_network_time_statistics::check_interval_rollover()
{
    unsigned long last = m_interval_start;
    unsigned long now = NS_NETWORK_TIME_GET();
    if (last == 0 || (long)(now - last) >= m_interval_duration_msec)
    {
        ::internal_critical_section_enter(55);
        last = m_interval_start;
        long elapsed = NS_NETWORK_TIME_GET() - last;
        if (m_interval_start == 0)
        {
            m_interval_start = NS_NETWORK_TIME_GET();
        }
        else if (elapsed >= m_interval_duration_msec * 20)
        {
            memset(m_intervals, 0, sizeof(m_intervals));
            m_stored_total.events = 0;
            m_stored_total.total_values = 0;
            m_current.events = 0;
            m_current.total_values = 0;
            m_interval_start = NS_NETWORK_TIME_GET() - elapsed % m_interval_duration_msec;
        }
        else
        {
            while (elapsed >= m_interval_duration_msec)
            {
                long old_events = m_intervals[m_next_interval_index].events;
                long old_values = m_intervals[m_next_interval_index].total_values;
                m_stored_total.events -= old_events;
                m_stored_total.total_values -= old_values;
                m_stored_total.events += m_current.events;
                m_stored_total.total_values += m_current.total_values;
                m_intervals[m_next_interval_index] = m_current;
                m_next_interval_index = (m_next_interval_index + 1) % 20;
                m_interval_start += m_interval_duration_msec;
                m_current.events = 0;
                m_current.total_values = 0;
                elapsed -= m_interval_duration_msec;
            }
        }
        _InterlockedDecrement(&g_synch_globals.critical_sections[55].lock_count.value);
        RtlLeaveCriticalSection(&g_synch_globals.critical_sections[55].critical_section);
    }
}

void c_network_window_statistics::add_event(long value)
{
    unsigned long timestamp = NS_NETWORK_TIME_GET();
    m_window_aperture_msec = timestamp - m_window_entries[m_window_next_entry].timestamp;
    m_window_total_values -= m_window_entries[m_window_next_entry].value;
    m_window_entries[m_window_next_entry].timestamp = timestamp;
    m_window_entries[m_window_next_entry].value = value;
    m_window_total_values += value;
    m_window_next_entry = (m_window_next_entry + 1) % m_window_size;
}
// @address 0x43cbf0 ?reset@c_network_window_statistics@@QAAXXZ
void c_network_window_statistics::reset()
{
    unsigned long time_now = NS_NETWORK_TIME_GET();
    for (long entry_index = 0; entry_index < m_window_size; ++entry_index)
    {
        m_window_entries[entry_index].timestamp = time_now;
        m_window_entries[entry_index].value = 0;
    }
    m_window_next_entry = 0;
    m_window_total_values = 0;
    m_window_aperture_msec = 0;
    return;
}

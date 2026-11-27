#ifndef NETWORKING_NETWORK_STATISTICS_H
#define NETWORKING_NETWORK_STATISTICS_H

#include <stddef.h>
#include <string.h>

// Network time statistics are collected over
// twenty event/value intervals. The constructor initializes only the periods.
struct s_network_interval_statistics
{
    long events;
    long total_values;
};

class c_network_time_statistics
{
public:
    c_network_time_statistics()
    {
        m_period_duration_msec = 0;
        m_interval_duration_msec = 0;
        m_period_reciprocal = 0.0f;
    }
    ~c_network_time_statistics() {}
    void initialize(long period_duration_msec);
    void add_event(long value);
    void reset()
    {
        m_total_events = 0;
        m_total_values = 0;
        m_interval_start = 0;
        memset(&m_current, 0, sizeof(m_current));
        m_next_interval_index = 0;
        memset(m_intervals, 0, sizeof(m_intervals));
        memset(&m_stored_total, 0, sizeof(m_stored_total));
    }
    unsigned __int64 total_events() const { return m_total_events; }
    unsigned __int64 total_values() const { return m_total_values; }
    float average_events_per_second();
    float average_values_per_second();

    unsigned __int64 m_total_events;
    unsigned __int64 m_total_values;
    unsigned long m_interval_start;
    s_network_interval_statistics m_current;
    long m_period_duration_msec;
    long m_interval_duration_msec;
    float m_period_reciprocal;
    long m_next_interval_index;
    s_network_interval_statistics m_intervals[20];
    s_network_interval_statistics m_stored_total;
private:
    void check_interval_rollover();
};

typedef char network_time_statistics_size[(sizeof(c_network_time_statistics) == 0xd8) ? 1 : -1];
typedef char network_time_statistics_period_offset[(offsetof(c_network_time_statistics, m_period_duration_msec) == 0x1c) ? 1 : -1];
typedef char network_time_statistics_intervals_offset[(offsetof(c_network_time_statistics, m_intervals) == 0x2c) ? 1 : -1];
typedef char network_time_statistics_stored_total_offset[(offsetof(c_network_time_statistics, m_stored_total) == 0xcc) ? 1 : -1];

struct s_network_window_entry
{
    unsigned long timestamp;
    long value;
};

class c_network_window_statistics
{
public:
    c_network_window_statistics() : m_window_size(0) {}
    ~c_network_window_statistics() {}
    void initialize(long window_size)
    {
        m_window_size = window_size;
        reset();
    }
    void reset();
    void add_event(long value);
    long window_size() const { return m_window_size; }
    long window_aperture_msec() const { return m_window_aperture_msec; }
    __forceinline float average_values_in_window() { return (float)m_window_total_values / (float)m_window_size; }

    long m_window_size;
    long m_window_next_entry;
    s_network_window_entry m_window_entries[32];
    long m_window_total_values;
    long m_window_aperture_msec;
};

typedef char network_window_statistics_size[(sizeof(c_network_window_statistics) == 0x110) ? 1 : -1];
typedef char network_window_statistics_entries_offset[(offsetof(c_network_window_statistics, m_window_entries) == 8) ? 1 : -1];
typedef char network_window_statistics_values_offset[(offsetof(c_network_window_statistics, m_window_total_values) == 0x108) ? 1 : -1];

#endif

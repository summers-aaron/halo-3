#ifndef MEMORY_SLIDING_WINDOW_H
#define MEMORY_SLIDING_WINDOW_H

#include "memory/ring_buffer.h"

// The network
// connection, channel and message queue use the same 28-byte window.
class c_sliding_window
{
public:
    c_sliding_window(long size) : m_window(size) { m_initialized = false; m_size = size; }
    ~c_sliding_window() {}

    bool initialized() const { return m_initialized; }
    long current_size() const
    {
        long size = m_leading_sequence_number - m_trailing_sequence_number;
        return size;
    }
    bool full() const { return current_size() >= m_size; }
    bool empty() const { return current_size() == 0; }
    long newest_sequence_number() const { return m_leading_sequence_number; }
    long oldest_sequence_number() const { return m_trailing_sequence_number + 1; }
    long trailing_edge() const { return m_trailing_sequence_number; }
    long get_storage_index(long sequence_number) const
    {
        long storage_index = -1;
        if (sequence_number > m_trailing_sequence_number && sequence_number <= m_leading_sequence_number)
        {
            long index_in_window = sequence_number - (m_trailing_sequence_number + 1);
            storage_index = (m_window.first_element() + index_in_window) % m_size;
        }
        return storage_index;
    }
    void reset(bool initialized, long sequence_number)
    {
        m_leading_sequence_number = sequence_number;
        m_trailing_sequence_number = sequence_number;
        m_window.clear();
        m_initialized = initialized;
    }
    void close(long new_trailing_edge)
    {
        long amount_to_close = new_trailing_edge - m_trailing_sequence_number;
        if (amount_to_close > 0)
        {
            m_window.remove_elements(amount_to_close);
            m_trailing_sequence_number = new_trailing_edge;
        }
    }
    void open(long new_leading_edge)
    {
        if (new_leading_edge > m_leading_sequence_number)
        {
            m_window.add_elements(new_leading_edge - m_leading_sequence_number);
            m_leading_sequence_number = new_leading_edge;
        }
    }

    bool m_initialized;
    long m_size;
    long m_leading_sequence_number;
    long m_trailing_sequence_number;
    c_ring_buffer m_window;
};

typedef char sliding_window_size[(sizeof(c_sliding_window) == 28) ? 1 : -1];
typedef char sliding_window_leading_offset[(offsetof(c_sliding_window, m_leading_sequence_number) == 8) ? 1 : -1];
typedef char sliding_window_trailing_offset[(offsetof(c_sliding_window, m_trailing_sequence_number) == 12) ? 1 : -1];
typedef char sliding_window_ring_offset[(offsetof(c_sliding_window, m_window) == 16) ? 1 : -1];

#endif // MEMORY_SLIDING_WINDOW_H

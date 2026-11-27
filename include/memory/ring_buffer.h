#ifndef MEMORY_RING_BUFFER_H
#define MEMORY_RING_BUFFER_H

#include <stddef.h>

// March ring_buffer.obj supplies the methods; network_memory's constructor
// Layout is capacity, first-element index, then live element count.
class c_ring_buffer
{
public:
    c_ring_buffer(long size) { m_size = size; clear(); }
    ~c_ring_buffer() {}

    long size() const { return m_size; }
    long count() const { return m_element_count; }
    bool empty() const { return count() == 0; }
    bool full() const { return count() == m_size; }
    long first_element() const { return empty() ? -1 : m_first_element; }
    long last_element() const
    {
        return empty() ? -1 : (m_first_element + count() - 1) % count();
    }
    long next_element(long element_index) const
    {
        long next_element_index = -1;
        if (element_index != -1)
        {
            long index = (element_index - m_first_element + m_size) % m_size;
            if (index >= 0 && index < m_element_count - 1)
                next_element_index = (element_index + 1) % m_size;
        }
        return next_element_index;
    }
    long previous_element(long element_index) const
    {
        long previous_element_index = -1;
        if (element_index != -1)
        {
            long index = (element_index - m_first_element + m_size) % m_size;
            if (index > 0 && index < m_element_count)
                previous_element_index = (element_index + m_size - 1) % m_size;
        }
        return previous_element_index;
    }
    void clear() { m_first_element = 0; m_element_count = 0; }
    long add_element()
    {
        long result = (m_first_element + m_element_count) % m_size;
        ++m_element_count;
        return result;
    }
    long remove_element()
    {
        long result = -1;
        if (!empty())
        {
            result = m_first_element;
            m_first_element = (m_first_element + 1) % m_size;
            --m_element_count;
        }
        return result;
    }
    long add_elements(long amount)
    {
        long result = (m_first_element + m_element_count) % m_size;
        m_element_count += amount;
        return result;
    }
    long remove_elements(long amount)
    {
        long result = -1;
        if (m_element_count >= amount)
        {
            result = m_first_element;
            m_first_element = (m_first_element + amount) % m_size;
            m_element_count -= amount;
        }
        return result;
    }

    long m_size;
    long m_first_element;
    long m_element_count;
};

typedef char ring_buffer_size[(sizeof(c_ring_buffer) == 12) ? 1 : -1];
typedef char ring_buffer_first_offset[(offsetof(c_ring_buffer, m_first_element) == 4) ? 1 : -1];
typedef char ring_buffer_count_offset[(offsetof(c_ring_buffer, m_element_count) == 8) ? 1 : -1];

#endif // MEMORY_RING_BUFFER_H

#ifndef SIMULATION_SIMULATION_QUEUE_H
#define SIMULATION_SIMULATION_QUEUE_H

#include "simulation/simulation_queue_types.h"

class c_bitstream;

// Methods and layout from simulation\simulation_queue.cpp in the March build;
// allocation and serialization bodies remain in their defining translation units.
class c_simulation_queue
{
public:
    void initialize();
    void transfer_elements(c_simulation_queue *queue);
    void encode(c_bitstream *stream) const;
    bool decode(c_bitstream *stream);
    bool compare(c_simulation_queue *queue) const;
    void clear();
    void allocate(long size, s_simulation_queue_element **element_out);
    void deallocate(s_simulation_queue_element *element);

    void dispose()
    {
        if (initialized)
        {
            clear();
            initialized = false;
        }
    }

    long queued_count() const
    {
        long result = 0;
        if (initialized)
            result = count;
        return result;
    }

    long queued_size_in_bytes() const
    {
        long result = 0;
        if (initialized)
            result = bytes;
        return result;
    }

    long allocated_count() const
    {
        long result = 0;
        if (initialized)
            result = allocated_elements;
        return result;
    }

    long allocated_size_in_bytes() const
    {
        long result = 0;
        if (initialized)
            result = allocated_bytes;
        return result;
    }

    long allocated_encoded_size_in_bytes() const
    {
        long header_bytes = (allocated_count() * 14 + 37 - 1) / 8;
        long data_bytes = allocated_size_in_bytes() - allocated_count() * sizeof(s_simulation_queue_element);
        long result = header_bytes + data_bytes;
        return result;
    }

    long allocated_new_encoded_size_bytes(long size) const
    {
        long new_size = size + 2;
        long result = new_size + allocated_encoded_size_in_bytes();
        return result;
    }

    s_simulation_queue_element *get_first_element() const
    {
        s_simulation_queue_element *result = 0;
        if (initialized && queued_count() > 0)
            result = first;
        return result;
    }

    s_simulation_queue_element *get_next_element(s_simulation_queue_element *element) const
    {
        s_simulation_queue_element *result = 0;
        if (initialized && element)
            result = element->next;
        return result;
    }

    void enqueue(s_simulation_queue_element *element)
    {
        if (initialized)
        {
            if (!last)
                first = element;
            else
                last->next = element;
            last = element;
            count++;
            bytes += element->data_size + sizeof(s_simulation_queue_element);
        }
    }

    void deque(s_simulation_queue_element **element_out)
    {
        *element_out = 0;
        if (initialized)
        {
            if (first)
            {
                *element_out = first;
                first = first->next;
                count--;
                bytes -= (*element_out)->data_size + sizeof(s_simulation_queue_element);
                (*element_out)->next = 0;
            }
            if (!first)
                last = 0;
        }
    }

    bool initialized;
    long allocated_elements;
    long allocated_bytes;
    long count;
    long bytes;
    s_simulation_queue_element *first;
    s_simulation_queue_element *last;
};

typedef char simulation_queue_size[(sizeof(void *) != 4 || ((sizeof(c_simulation_queue) == 0x1c) )) ? 1 : -1];
typedef char simulation_queue_head_offset[(sizeof(void *) != 4 || ((offsetof(c_simulation_queue, first) == 0x14) )) ? 1 : -1];

#endif

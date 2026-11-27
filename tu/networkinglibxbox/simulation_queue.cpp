// networkinglibxbox/simulation_queue.obj: from the records 0xa58d0 0x3d6a00 0x3d6b50
// @flags /TP /O2 /Oc
// @address 0x3d6b50 ?allocate@c_simulation_queue@@QAAXJPAPAUs_simulation_queue_element@@@Z
// @address 0x3d6a00 ?compare@c_simulation_queue@@QBA_NPAV1@@Z
#include <string.h>

#include "simulation/simulation_queue_types.h"
#include "simulation/simulation_queue.h"
#include "networking/network_memory_types.h"
namespace {
static __forceinline c_rockall_heap *queue_network_heap()
{
    return network_shared_memory_globals.heap;
}
static __forceinline void *queue_network_heap_allocate_block(const char *file, long line, long type, long size)
{
    void *allocation = queue_network_heap()->allocate(size);
    return allocation;
}
static __forceinline void queue_network_heap_free_block(const char *file, long line, long type, void *allocation)
{
    long allocation_size = queue_network_heap()->get_allocation_size(allocation);
    queue_network_heap()->deallocate(allocation);
}
static __forceinline char *queue_copy_memory_description(char *destination, const char *source, unsigned int size)
{
    char *result = strncpy(destination, source, size);
    destination[size - 1] = 0;
    return result;
}
}

__forceinline void c_simulation_queue::deallocate(s_simulation_queue_element *element)
{
    if (initialized)
    {
        allocated_bytes -= element->data_size + sizeof(s_simulation_queue_element);
        allocated_elements--;
        queue_network_heap_free_block(0, 405, 17, element);
    }
}

// @address 0xa58d0 ?clear@c_simulation_queue@@QAAXXZ
void c_simulation_queue::clear()
{
    if (initialized)
    {
        while (first)
        {
            s_simulation_queue_element *element_to_dequeue = 0;
            deque(&element_to_dequeue);
            deallocate(element_to_dequeue);
        }
    }
}

bool c_simulation_queue::compare(c_simulation_queue *queue) const
{
    bool equal = true;
    if (count != queue->queued_count())
        equal = false;
    else if (bytes != queue->queued_size_in_bytes())
        equal = false;
    else {
        s_simulation_queue_element *element = get_first_element();
        s_simulation_queue_element *other_element = queue->get_first_element();
        while (equal && element && other_element) {
            const s_simulation_queue_element *header = element;
            const s_simulation_queue_element *other_header = other_element;
            if (header->type != other_header->type)
                equal = false;
            else if (header->data_size != other_header->data_size)
                equal = false;
            else if (memcmp(element->data, other_element->data, header->data_size))
                equal = false;
            element = get_next_element(element);
            other_element = queue->get_next_element(other_element);
        }
        if (element || other_element)
            equal = false;
    }
    return equal;
}

// @address 0x3d6b50 ?allocate@c_simulation_queue@@QAAXJPAPAUs_simulation_queue_element@@@Z
void c_simulation_queue::allocate(long size, s_simulation_queue_element **element_out_argument)
{
    // match: 0x3d6b50: pointer __restrict qualifier prevents alias reload
    s_simulation_queue_element ** __restrict element_out = element_out_argument;
    *element_out = 0;
    if (initialized)
    {
        s_simulation_queue_element *element = 0;
        long allocation_size = size + sizeof(s_simulation_queue_element);
        if (allocated_count() + 1 < 4096 &&
            allocated_size_in_bytes() + allocation_size < 0x20000 &&
            size < 1024 &&
            allocated_new_encoded_size_bytes(size) < 0xe800)
        {
            element = (s_simulation_queue_element *)queue_network_heap_allocate_block(0, 355, 17, allocation_size);
            if (!element)
            {
                char description[1024];
                queue_copy_memory_description(description, "UNKNOWN", sizeof(description));
            }
            else
            {
                memset(element, 0, allocation_size);
                element->type = 0;
                element->data_size = size;
                element->next = 0;
                *element_out_argument = element;
                element->data = reinterpret_cast<unsigned char *>(element + 1);
                allocated_elements++;
                allocated_bytes += allocation_size;
            }
        }
    }
}

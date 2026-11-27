#ifndef CSERIES_SYSTEM_MEMORY_INFORMATION_TYPES_H
#define CSERIES_SYSTEM_MEMORY_INFORMATION_TYPES_H

struct s_system_memory_information
{
    long available_physical_memory;
    long total_physical_memory;
    long tracked_memory;
    long tracked_allocation_count;
};

#endif // CSERIES_SYSTEM_MEMORY_INFORMATION_TYPES_H

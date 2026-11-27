#ifndef MEMORY_DATA_ARRAY_ALLOCATION_SENTINEL_H
#define MEMORY_DATA_ARRAY_ALLOCATION_SENTINEL_H

// Address token passed for data arrays whose storage is supplied by their caller.
extern "C" { extern char g_data_array_allocation_sentinel[]; }

#endif // MEMORY_DATA_ARRAY_ALLOCATION_SENTINEL_H

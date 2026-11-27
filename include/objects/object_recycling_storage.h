#ifndef OBJECTS_OBJECT_RECYCLING_STORAGE_H
#define OBJECTS_OBJECT_RECYCLING_STORAGE_H

extern "C" __declspec(thread) void *g_recycling_volumes;

// The volume lists g_recycling_volumes points at: the free/active heads and sixteen volumes.
struct s_recycling_volume { long previous, next, volume_index, maximum_count, due; };
struct s_recycling_volumes { long active, free; s_recycling_volume volumes[16]; };
typedef char recycling_volumes_size[(sizeof(s_recycling_volumes) == 328) ? 1 : -1];

#endif

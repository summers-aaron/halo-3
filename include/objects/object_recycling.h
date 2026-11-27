#ifndef OBJECTS_OBJECT_RECYCLING_H
#define OBJECTS_OBJECT_RECYCLING_H

#include "objects/recycling_methods_types.h"
#include "cache/restricted_memory_regions.h"
#include "math/real_math_types.h"

class c_recycling_pressure_flags
{
public:
    __forceinline c_recycling_pressure_flags() { clear(); }
    __forceinline void clear() { value = 0; }
    __forceinline void set(long bit, bool enabled) { if (enabled) value |= 1 << bit; else value &= ~(1 << bit); }
    __forceinline bool test(long bit) const { return ((unsigned long)value >> bit) & 1; }
    long value;
};
struct group { short identifier; short count; long first; real_point3d center; };

struct object_recycling_group_entry { unsigned long score; long index; real_rectangle3d bounds; };

struct s_recycling_workspace { bool initialized; long count; object_recycling_group_entry groups[128]; };

struct object_recycling_method {
    const char *name;
    long pressure;
    long workspace_index;
    void (*setup)(long, void *, long);
    long (*recycle)(long, void *, long, bool *, char *, long);
    long flags;
};


extern "C" long g_recycling_critical_memory_threshold;
extern "C" long g_recycling_tolerance_memory_threshold;
extern "C" long g_recycling_target_memory_threshold;
extern "C" const float k_recycling_group_split_threshold;
extern "C" object_recycling_method g_object_recycling_methods[10];
extern float k_recycling_min_group_distance_squared;

// Functions defined in tu/objectslibxbox/object_recycling.cpp.

extern "C" c_recycling_pressure_flags recycling_get_target_flags(c_recycling_pressure_flags tolerance);
void object_recycling_group_remove_object(long index);
void object_recycling_reset_volume_list();
extern "C" void object_recycling_system_add_volume(long value, long context, long length);


extern "C" long find_worst_object_in_group(long flags,group *recycling_group,long threshold);
void __tls_set_g_recycling_groups_allocator(void *address);
extern "C" long recycle_worst_object_available(long flags, s_recycling_workspace *work, long size, bool *more, char *reason, long reason_size);
void objects_recycling_group_process(s_recycling_workspace *workspace, long entry_index);
void objects_recycling_update_membership(s_recycling_workspace *workspace);
void prepare_recycling_group_list(s_recycling_workspace *workspace);
void objects_recycling_update(e_recycling_methods method);

void __tls_set_g_depth_of_field_game_globals_allocator(void *address);
typedef t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_recycling_groups_allocator> c_recycling_groups_allocator;
typedef t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_depth_of_field_game_globals_allocator> c_recycling_volumes_allocator;
// Allocators defined in tu/objectslibxbox/object_recycling.cpp (March object_recycling.obj
// defines both under these C++ spellings; objects.cpp views them through these objects).
extern c_recycling_groups_allocator g_recycling_groups_allocator;
extern c_recycling_volumes_allocator g_recycling_volumes_allocator;
extern "C" void *depth_of_field_new_game_globals(c_recycling_volumes_allocator *allocation,
    const char *name, const char *type, unsigned int size, long alignment_bits);

#endif // OBJECTS_OBJECT_RECYCLING_H

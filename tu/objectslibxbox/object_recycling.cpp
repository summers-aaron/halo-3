// objectslibxbox/object_recycling.obj: from the records 0x112298 0x112990 0x112c48 0x362cf0 0x362d08 0x362de0 0x363190 0x363638 0x363660 0x3637d8 0x3638b8 0x363a60 0x363b90 0x363ce8 0x363d40 0x363e90 0x364010 0x364070 0x364118 0x364208 0x364680 0x364720 0x364740 0x3647e0 0x709200 0x709218 0x709228 0x709238 0x70c3c8 0x70c3e0
// @address 0x11df6f0 recycling_groups
// @flags /TP /O2 /Oc
#include "objects/recycling_groups.h"
#include "memory/data_types.h"
#include "memory/datum_index_inlines.h"
#include "objects/object_cluster_globals.h"
#include "memory/tls_globals.h"
#include "game/game_globals_storage.h"
#include "math/matrix_math_types.h"
#include "math/real_math.h"
#include "game/game_time_types.h"
#include "objects/object_datum.h"
#include "objects/object_header.h"
#include "objects/object_lifecycle.h"
#include "objects/object_placement.h"
#include "objects/objects.h"
#include "objects/object_recycling.h"
#include "objects/object_recycling_storage.h"
#include "ai/ai.h"
#include "scenario/scenario_trigger_volume.h"
#include "scenario/scenario_trigger_volumes.h"
#include "scenario/scenario.h"
#include "scenario/scenario_tag.h"
#include "cache/restricted_memory_regions.h"
#include "profiler/profiler_stopwatch.h"
#include "memory/data.h"
#include "math/matrix_math.h"
#include "objects/recycling_methods_types.h"
#include "render/depth_of_field.h"
#include "objects/object_runtime_globals_types.h"
#include "objects/object_runtime_tls.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_registration.h"
#include "cache/restricted_memory_regions_inline.h"
#include "multithreading/thread_tls.h"
#include <float.h>
#include <stdlib.h>

__declspec(thread) s_data_array *recycling_groups;

// @address 0x112298 ?object_recycling_group_remove_object@@YAXJ@Z







union s_recycling_workspace_storage { s_recycling_workspace workspace; char buffer[0x1400]; };
static __forceinline s_recycling_volumes *recycling_volumes_get() { return static_cast<s_recycling_volumes *>(g_recycling_volumes); }

static __forceinline void recycling_switch_group(long index,long previous,long source,long destination) {
 object_datum *recycling_object=object_get(index);
 if(source!=-1) {
  struct group *recycling_group=datum_get<struct group>(tls_recycling_groups(), source);
  if(previous==-1) recycling_group->first=recycling_object->next_recycling_group_member;
  else {
   object_datum *previous_object=object_get(previous);
   previous_object->next_recycling_group_member=recycling_object->next_recycling_group_member;
  }
  recycling_group->count--;
  recycling_object->parent_recycling_group=-1;
  recycling_object->next_recycling_group_member=-1;
 }
 if(destination!=-1) {
  struct group *recycling_group=datum_get<struct group>(tls_recycling_groups(), destination);
  recycling_object->next_recycling_group_member=recycling_group->first;
  recycling_group->first=index;
  recycling_group->count++;
  recycling_object->parent_recycling_group=destination;
 }
}

void object_recycling_group_remove_object(long index) {
 object_datum *recycling_object=object_get(index);
 long group_index=recycling_object->parent_recycling_group;
 if(group_index!=-1) {
  struct group *recycling_group=datum_get<struct group>(tls_recycling_groups(), group_index);
  long current=recycling_group->first;
  long previous=-1;
  while(current!=-1) {
   object_datum *member=object_get(current);
   long next=member->next_recycling_group_member;
   if(current==index) {
    recycling_switch_group(current,previous,group_index,-1);
    next=-1;
   } else previous=current;
   current=next;
  }
 }
}


// @address 0x362d08 ?move_recycling_object_between_groups@@YAXJJJJ@Z
void move_recycling_object_between_groups(long index,long previous,long source,long destination)
{
 object_datum *recycling_object=object_get(index);
 if(source!=-1) {
  struct group *recycling_group=datum_get<struct group>(tls_recycling_groups(), source);
  if(previous==-1) recycling_group->first=recycling_object->next_recycling_group_member;
  else {
   object_datum *previous_object=object_get(previous);
   previous_object->next_recycling_group_member=recycling_object->next_recycling_group_member;
  }
  recycling_group->count--;
  recycling_object->parent_recycling_group=-1;
  recycling_object->next_recycling_group_member=-1;
 }
 if(destination!=-1) {
  struct group *recycling_group=datum_get<struct group>(tls_recycling_groups(), destination);
  long first=recycling_group->first;
  short count=recycling_group->count;
  recycling_group->first=index;
  recycling_group->count=count+1;
  recycling_object->parent_recycling_group=destination;
  recycling_object->next_recycling_group_member=first;
 }
}

// @address 0x363638 ?sort_recycling_entry_descending@@YAHPBX0@Z
int sort_recycling_entry_descending(const void *entry_a,const void *entry_b)
{
 long index_a=*(const long *)entry_a;
 long index_b=*(const long *)entry_b;
 if(index_a==index_b) return 0;
 if(index_a>index_b) return -1;
 return 1;
}


// @address 0x363660 closest_player_distance_squared
static __forceinline real_point3d *object_recycling_vector_between(const real_point3d *from, const real_point3d *to, real_point3d *v)
{
    v->x = to->x - from->x;
    v->y = to->y - from->y;
    v->z = to->z - from->z;
    return v;
}
static __forceinline float object_recycling_magnitude_squared(const real_point3d *v) { return v->x*v->x+v->z*v->z+v->y*v->y; }
static __forceinline float object_recycling_distance_squared(const real_point3d *from, const real_point3d *to)
{
    real_point3d v;
    object_recycling_vector_between(from, to, &v);
    return object_recycling_magnitude_squared(&v);
}
extern "C" float closest_player_distance_squared(const real_point3d *test_point,const real_point3d *player_positions,long count) {
 float closest=32767.0f;
 for(long i=0;i<count;i++) {
  float distance=object_recycling_distance_squared(&player_positions[i],test_point);
  closest=closest>distance?distance:closest;
 }
 return closest;
}




// @address 0x3637d8 object_recycling_get_player_positions
extern "C" long object_recycling_get_player_positions(real_point3d *player_positions, long maximum_count)
{
    long count = 0;
    object_marker marker;
    for (long index = ::data_next_index(tls_player_data(), -1); index != -1;
        index = inl_data_next_index(tls_player_data(), index))
    {
        if (count >= 16)
            break;
        player_datum *player = datum_get<player_datum>(tls_player_data(), index);
        if (player->unit_index != -1) {
            internal_object_get_markers_by_string_id(player->unit_index, 157, &marker, 1, false);
            player_positions[count++] = marker.matrix.position;
        }
    }
    return count;
}

__forceinline long objects_get_free_object_header_count() { return tls_object_header_data()->maximum_count-tls_object_header_data()->first_unallocated; }
__forceinline long objects_get_active_garbage_count() { return tls_object_runtime_globals()->recyclable_garbage_object_count; }
// @address 0x363a60 recycling_get_tolerance_flags
extern "C" c_recycling_pressure_flags recycling_get_tolerance_flags() {
 c_recycling_pressure_flags result;
 result.clear();
 long memory=objects_get_contiguous_free_memory_size();
 long headers=objects_get_free_object_header_count();
 long garbage_count=objects_get_active_garbage_count();
 result.set(0,memory<g_recycling_tolerance_memory_threshold ? true:false);
 result.set(1,headers<102 ? true:false);
 result.set(2,garbage_count>120 ? true:false);
 result.set(3,memory<g_recycling_critical_memory_threshold || headers<51);
 return result;
}

// @address 0x363b90 recycling_get_target_flags
c_recycling_pressure_flags recycling_get_target_flags(c_recycling_pressure_flags tolerance) {
 c_recycling_pressure_flags result;result.clear();
 long memory=objects_get_contiguous_free_memory_size();
 long free_headers=tls_object_header_data()->maximum_count-tls_object_header_data()->first_unallocated;
 long active_garbage=tls_object_runtime_globals()->recyclable_garbage_object_count;
 result.set(0,tolerance.test(0) && memory<g_recycling_target_memory_threshold);
 result.set(1,tolerance.test(1) && free_headers<204);
 result.set(2,tolerance.test(2) && active_garbage>115);
 result.set(3,memory<g_recycling_critical_memory_threshold || free_headers<51);
 return result;
}

// @address 0x363d40 find_worst_object_in_group
long find_worst_object_in_group(long flags,struct group *recycling_group,long threshold)
{
 long worst=-1;
 long worst_score=0x7fffffff;
 if(recycling_group->count) {
  long index=recycling_group->first;
  while(index!=-1) {
   object_datum *object=object_get(index);
   s_object_header *header=object_header_get(index);
   long score=object->garbage_time;
   if(score<worst_score) {
    if(!(object->recycling_flags & 8)) {
     object->m_recycling_flags.set(static_cast<e_object_recycling_flags>(3), true);
     object->m_recycling_flags.set(static_cast<e_object_recycling_flags>(4), score >= threshold);
     object->m_recycling_flags.set(static_cast<e_object_recycling_flags>(5), ::object_visible_to_any_player(index));
    }
    if((!(flags&4) || (header->flags&1)) && ((flags&1) || !(object->recycling_flags&16)) && ((flags&2) || !(object->recycling_flags&32))) {
     worst_score=score;
     worst=index;
    }
   }
   index=object->next_recycling_group_member;
  }
 }
 return worst;
}


// Reset the active and free volume lists.
// @address 0x364010 ?object_recycling_reset_volume_list@@YAXXZ
void object_recycling_reset_volume_list()
{
    s_recycling_volumes *recycling = recycling_volumes_get();
    recycling->free = 0;
    recycling->active = -1;
    for (long volume_index = 0; volume_index < 16; volume_index++)
    {
        s_recycling_volume *volume = &recycling->volumes[volume_index];
        recycling->volumes[volume_index].previous = volume_index - 1;
        recycling->volumes[volume_index].next = volume_index + 1;
        volume->volume_index = -1;
    }
    recycling->volumes[0].previous = -1;
    recycling->volumes[15].next = -1;
}

// @address 0x364070 ?object_recycling_switch_lists@@YAXJPAJ0@Z
void object_recycling_switch_lists(long volume_index, long *top_of_source_list, long *top_of_dest_list)
{
    s_recycling_volume *volume = &recycling_volumes_get()->volumes[volume_index];
    if (volume->previous != -1) {
        recycling_volumes_get()->volumes[volume->previous].next = volume->next;
    } else {
        *top_of_source_list = -1;
    }
    if (volume->next != -1) {
        recycling_volumes_get()->volumes[volume->next].previous = volume->previous;
    }
    volume->previous = -1;
    volume->next = *top_of_dest_list;
    if (volume->next != -1) {
        recycling_volumes_get()->volumes[volume->next].previous = volume_index;
    }
    *top_of_dest_list = volume_index;
}

// @address 0x364118 object_recycling_system_add_volume
extern "C" void object_recycling_system_add_volume(long value, long context, long length)
{
    if (value != -1) {
        s_recycling_volumes *queue = recycling_volumes_get();
        long *free_list = &queue->free;
        if (queue->free == -1) {
            long best = queue->active;
            long best_due = queue->volumes[best].due;
            long index = queue->volumes[best].next;
            while (index != -1) {
                long due = queue->volumes[index].due;
                if (due < best_due) {
                    best_due = due;
                    best = index;
                }
                index = queue->volumes[index].next;
            }
            object_recycling_switch_lists(best, &queue->active, free_list);
        }
        long slot = *free_list;
        object_recycling_switch_lists(slot, free_list, &queue->active);
        game_time_globals_definition *time = tls_game_time_globals();
        long *due_slot = &queue->volumes[slot].due;
        queue->volumes[slot].volume_index = value;
        *due_slot = time->ticks_per_second * length + time->time;
        queue->volumes[slot].maximum_count = context;
    }
}

// @address 0x1ec830 ?data_set_new_base_address@@YAXPAPAUs_data_array@@PAU1@@Z

// @address 0x362cf0 ?__tls_set_g_recycling_groups_allocator@@YAXPAX@Z
void __tls_set_g_recycling_groups_allocator(void *address)
{
    data_set_new_base_address((s_data_array **)&tls_recycling_groups(), (s_data_array *)address);
}

namespace {
static __forceinline long restricted_region_add_recycling_groups_member(long region, const char *name, const char *type,
    unsigned int size, long alignment, field_proc_t setter)
{
    long member_index = restricted_memory_register_member(&g_restricted_regions[region], name, type, size, alignment, setter, 0, 0);
    void *address = restricted_region_get_member_address(region, member_index);
    data_set_new_base_address((s_data_array **)&tls_recycling_groups(), (s_data_array *)address);
    return member_index;
}
}
// @address 0x364680 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_recycling_groups_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template <>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_recycling_groups_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = restricted_region_add_recycling_groups_member(1, name, "", size, 0, __tls_set_g_recycling_groups_allocator);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}

// @address 0x364720 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_recycling_groups_allocator@@YAXPAX@Z@@UAAXPAX@Z
template <>
void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_recycling_groups_allocator>::deallocate(void *memory)
{
    tls_recycling_groups() = 0;
    restricted_region_free_member(1, m_member_index);
}

// @address 0x364740 depth_of_field_new_game_globals
void *depth_of_field_new_game_globals(c_recycling_volumes_allocator *allocation,
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    int member_index = restricted_memory_register_member(&g_restricted_regions[1], name, type, size,
        0, __tls_set_g_depth_of_field_game_globals_allocator, 0, 0);
    unsigned int offset = restricted_region_get_member_offset(1, member_index);
    void *address = restricted_region_get_base_address(1) + offset;
    int owner = get_current_thread_index();
    allocation->m_thread_id = owner;
    allocation->m_member_index = member_index;
    depth_of_field_game_globals_set((s_depth_of_field *)address);
    return restricted_region_get_base_address(1) + restricted_region_get_member_offset(1, member_index);
}

// @address 0x3647e0 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_depth_of_field_game_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template <>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_depth_of_field_game_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return depth_of_field_new_game_globals(this, name, "", size, 0);
}

// @address 0x112c48 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_depth_of_field_game_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template <>
void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_depth_of_field_game_globals_allocator>::deallocate(void *memory)
{
    long value = m_member_index;
    depth_of_field_game_globals_set(0);
    restricted_region_free_member(1, value);
}

static __forceinline void object_recycling_update_group_list(s_recycling_workspace *work) {
 for (long i = 0; i < work->count; ++i) {
  if (work->groups[i].index != -1) {
   struct group *group_entry = datum_get<struct group>(tls_recycling_groups(), work->groups[i].index);
   work->groups[i].score &= 0xffff;
   work->groups[i].score |= group_entry->count << 16;
  } else work->groups[i].score = 0;
 }
 if (work->count > 1) { if (work->count != 0) qsort(work->groups, work->count, sizeof(object_recycling_group_entry), sort_recycling_entry_descending); }
}
// @address 0x363e90 recycle_worst_object_available
long recycle_worst_object_available(long flags, s_recycling_workspace *work, long size, bool *more, char *reason, long reason_size) {
 *more = false;
 long worst = -1;
 long released = 0;
 if (work->count > 0) {
  object_recycling_update_group_list(work);
  long time = tls_game_time_globals()->time;
  for (long i = 0; worst == -1 && i < work->count; ++i) {
   if (work->groups[i].index != -1) {
    long index = work->groups[i].index;
    struct group *recycling_group = datum_get<struct group>(tls_recycling_groups(), index);
    worst = find_worst_object_in_group(flags, recycling_group, time);
   }
  }
  if (worst != -1) {
   object_activation_regions_update_for_object(worst, false);
   object_set_garbage(worst, false, -1);
   objects_purge_deleted_object(worst);
   ++released;
   *more = true;
  }
 }
 return released;
}


static __forceinline s_data_array *object_recycling_groups() { return (s_data_array *)tls_recycling_groups(); }
static __forceinline void object_recycling_rectangle_reset(real_rectangle3d *rectangle)
{
    rectangle->x0 = 3.402823466e+38f;
    rectangle->x1 = -3.402823466e+38f;
    rectangle->y0 = 3.402823466e+38f;
    rectangle->y1 = -3.402823466e+38f;
    rectangle->z0 = 3.402823466e+38f;
    rectangle->z1 = -3.402823466e+38f;
}
static __forceinline void object_recycling_group_add_object(long object_index, object_datum *object, struct group *group, long group_index)
{
    long first = group->first;
    short count = group->count;
    group->first = object_index;
    group->count = count + 1;
    object->parent_recycling_group = group_index;
    object->next_recycling_group_member = first;
}
static __forceinline void object_recycling_group_update_center(struct group *group, const real_rectangle3d *bounds)
{
    group->center.x = (bounds->x1 + bounds->x0) * 0.5f;
    group->center.y = (bounds->y1 + bounds->y0) * 0.5f;
    group->center.z = (bounds->z1 + bounds->z0) * 0.5f;
}
static __forceinline void object_recycling_group_update_center(long group_index, const real_rectangle3d *bounds)
{
    object_recycling_group_update_center(datum_get<struct group>(tls_recycling_groups(), group_index), bounds);
}


// @address 0x363ce8 ?setup_recycling_workspace@@YAJJPAEJPA_NPADJ@Z
long setup_recycling_workspace(long flags, unsigned char *workspace, long size, bool *more, char *reason, long reason_size)
{
    objects_recycling_update_membership((s_recycling_workspace *)workspace);
    prepare_recycling_group_list((s_recycling_workspace *)workspace);
    if (more)
        *more = false;
    return 0;
}

static __forceinline void object_recycling_process_center(struct group *group, const real_rectangle3d *bounds, float half)
{
    group->center.x = (bounds->x1 + bounds->x0) * half;
    group->center.y = (bounds->y1 + bounds->y0) * half;
    group->center.z = (bounds->z1 + bounds->z0) * half;
}
static __forceinline void object_recycling_process_center(long index, const real_rectangle3d *bounds, float half)
{
    object_recycling_process_center(datum_get<struct group>(tls_recycling_groups(), index), bounds, half);
}
// @rdata 0x82021694
static const float object_recycling_empty_bounds_lower = FLT_MAX;
// @rdata 0x82022024
static const float object_recycling_empty_bounds_upper = -FLT_MAX;
// match: 0x362de0: named file statics keep 0.5f the float-pool base (plain FLT_MAX/-FLT_MAX literals move it to 0x82022024)
static __forceinline void object_recycling_process_reset(real_rectangle3d *bounds)
{
    const float maximum = object_recycling_empty_bounds_lower;
    const float minimum = object_recycling_empty_bounds_upper;
    bounds->x0 = maximum; bounds->x1 = minimum;
    bounds->y0 = maximum; bounds->y1 = minimum;
    bounds->z0 = maximum; bounds->z1 = minimum;
}

static __forceinline long object_recycling_process_axis(const real_point3d *size)
{
    long axis = 2;
    if (size->x >= size->y && size->x >= size->z)
        axis = 0;
    else if (size->y >= size->x && size->y >= size->z)
        axis = 1;
    return axis;
}

// @address 0x362de0 ?objects_recycling_group_process@@YAXPAUs_recycling_workspace@@J@Z
void objects_recycling_group_process(s_recycling_workspace *workspace, long entry_index)
{
    object_recycling_group_entry *entry = workspace->groups + entry_index;

    bool split;
    do {
        split = false;
        long group_index = entry->index;
        struct group *group = datum_get<struct group>(tls_recycling_groups(), group_index);
        long first = group->first;
        if (group->count > 0) {
            real_point3d size;
            size.x = entry->bounds.x1 - entry->bounds.x0;
            size.y = entry->bounds.y1 - entry->bounds.y0;
            size.z = entry->bounds.z1 - entry->bounds.z0;
            object_recycling_process_center(group_index, &entry->bounds, 0.5f);
            long axis = object_recycling_process_axis(&size);
            if (group->count > 1 && size.n[axis] > k_recycling_group_split_threshold && workspace->count < 112) {
                long new_index = datum_new(object_recycling_groups());
                if (new_index != -1) {
                    struct group *new_group = datum_get<struct group>(tls_recycling_groups(), new_index);
                    object_recycling_group_entry *new_entry = &workspace->groups[workspace->count++];
                    new_entry->index = new_index;
                    new_group->first = -1;
                    new_group->count = 0;
                    new_entry->bounds = entry->bounds;
                    entry->bounds.n[axis].upper = group->center.n[axis];
                    new_entry->bounds.n[axis].lower = group->center.n[axis];
                    long index = first;
                    long previous = -1;
                    for (; index != -1;) {
                        object_datum *object = object_get(index);
                        long next = object->next_recycling_group_member;
                        if (object->bounding_center.n[axis] > group->center.n[axis])
                            move_recycling_object_between_groups(index, previous, group_index, new_index);
                        else
                            previous = index;
                        index = next;
                    }
                    if (new_group->count == 0) {
                        ::datum_delete(object_recycling_groups(), new_index);
                        --workspace->count;
                    }
                    if (group->count == 0) {
                        ::datum_delete(object_recycling_groups(), group_index);
                        entry->index = new_index;
                        entry->bounds = new_entry->bounds;
                        --workspace->count;
                    }
                    split = true;
                }
            }
        }
    } while (split);
    struct group *group = datum_get<struct group>(tls_recycling_groups(), entry->index);
    if (group->count != 0) {
        const float half = 0.5f;
        object_recycling_process_reset(&entry->bounds);
        for (long index = group->first; index != -1;) {
            object_datum *object = object_get(index);
            ::real_rectangle3d_enclose_point(&entry->bounds, &object->bounding_center);
            index = object->next_recycling_group_member;
        }
        object_recycling_process_center(group, &entry->bounds, half);
    }
}
static __forceinline long object_recycling_next_garbage(long index)
{
    long next = -1;
    if (index != -1)
        next = object_get(index)->next_garbage_object_index;
    return next;
}
static __forceinline void object_recycling_membership_reset_bounds(real_rectangle3d *bounds)
{
    // match: 0x363190: plain FLT_MAX is the pool base; the named static moves the base to 0x82022024
    const float maximum = FLT_MAX;
    const float minimum = object_recycling_empty_bounds_upper;
    bounds->x0 = maximum;
    bounds->x1 = minimum;
    bounds->y0 = maximum;
    bounds->y1 = minimum;
    bounds->z0 = maximum;
    bounds->z1 = minimum;
}
static __forceinline bool object_recycling_is_distributed(long index, const game_globals_storage *game)
{
    bool distributed = false;
    bool distributed_game = false;
    if (game->options.simulation == 4)
        distributed_game = true;
    if (distributed_game)
        distributed = object_get(index)->simulation_entity_index != -1;
    return distributed;
}
// @address 0x363190 ?objects_recycling_update_membership@@YAXPAUs_recycling_workspace@@@Z
void objects_recycling_update_membership(s_recycling_workspace *workspace)
{
    if (!workspace->initialized) {
        workspace->initialized = true;
        c_data_iterator<group> iterator;
        iterator.begin(object_recycling_groups());
        while (iterator.next())
            ::datum_delete(object_recycling_groups(), iterator.get_index());
        workspace->count = 0;
        long initial_index = datum_new(object_recycling_groups());
        if (initial_index != -1) {
            struct group *initial_group = datum_get<struct group>(tls_recycling_groups(), initial_index);
            real_rectangle3d *bounds = &workspace->groups[0].bounds;
            initial_group->first = -1;
            initial_group->count = 0;
            workspace->count = 1;
            workspace->groups[0].index = initial_index;
            object_recycling_membership_reset_bounds(bounds);
            s_object_globals *garbage = tls_object_runtime_globals();
            long index = garbage->first_garbage_object_index;
            if (index != -1) {
                const game_globals_storage *game = tls_game_globals();
                do {
                    object_datum *object = object_get(index);
                    object->m_recycling_flags.set(static_cast<e_object_recycling_flags>(3), false);
                    object->parent_recycling_group = -1;
                    object->next_recycling_group_member = -1;
                    if (!object_recycling_is_distributed(index, game) && object->parent_object_index == -1) {
                        object_recycling_group_add_object(index, object, initial_group, initial_index);
                        ::real_rectangle3d_enclose_point(bounds, &object->bounding_center);
                    }
                    index = object_recycling_next_garbage(index);
                } while (index != -1);
            }
            if (initial_group->count != 0) {
                workspace->count = 1;
                workspace->groups[0].index = initial_index;
                for (long entry = 0; entry < workspace->count; ++entry)
                    objects_recycling_group_process(workspace, entry);
            } else {
                ::datum_delete(object_recycling_groups(), initial_index);
            }
            iterator.begin(object_recycling_groups());
            while (iterator.next()) {
                struct group *source = iterator.get_datum();
                c_data_iterator<group> other = iterator;
                long source_index = iterator.get_index();
                while (other.next()) {
                    struct group *destination = other.get_datum();
                    long destination_index = other.get_index();
                    const real_point3d *source_center = &source->center;
                    const real_point3d *destination_center = &destination->center;
                    if (object_recycling_distance_squared(source_center, destination_center) < k_recycling_min_group_distance_squared) {
                        long previous = -1;
                        for (long index = source->first; index != -1; ) {
                            object_datum *object = object_get(index);
                            long next = object->next_recycling_group_member;
                            float source_distance = object_recycling_distance_squared(source_center, &object->bounding_center);
                            float destination_distance = object_recycling_distance_squared(destination_center, &object->bounding_center);
                            if (destination_distance < source_distance)
                                move_recycling_object_between_groups(index, previous, source_index, destination_index);
                            else
                                previous = index;
                            index = next;
                        }
                        previous = -1;
                        for (long index = destination->first; index != -1; ) {
                            object_datum *object = object_get(index);
                            long next = object->next_recycling_group_member;
                            float source_distance = object_recycling_distance_squared(source_center, &object->bounding_center);
                            float destination_distance = object_recycling_distance_squared(destination_center, &object->bounding_center);
                            if (source_distance < destination_distance)
                                move_recycling_object_between_groups(index, previous, destination_index, source_index);
                            else
                                previous = index;
                            index = next;
                        }
                    }
                    if (destination->count == 0)
                        ::datum_delete(object_recycling_groups(), destination_index);
                }
                if (source->count == 0)
                    ::datum_delete(object_recycling_groups(), source_index);
            }
        }
    }
}

// @address 0x3638b8 ?prepare_recycling_group_list@@YAXPAUs_recycling_workspace@@@Z
void prepare_recycling_group_list(s_recycling_workspace *workspace)
{
    workspace->count = 0;
    c_data_iterator<group> iterator;
    iterator.begin(object_recycling_groups());
    real_point3d player_positions[16];
    long player_count = object_recycling_get_player_positions(player_positions, 16);
    while (iterator.next()) {
        if (workspace->count >= 128)
            break;
        struct group *group = iterator.get_datum();
        if (group->count > 0) {
            long distance = (long)(double)closest_player_distance_squared(&group->center, player_positions, player_count);
            float score = (group->count > 0) ? ((float)distance * 100.0f) : ((float)distance * 100.0f);
            score = (score > 0.0f ? score : 0.0f) < 32767.0f ? (score > 0.0f ? score : 0.0f) : 32767.0f;
            workspace->groups[workspace->count].index = iterator.get_index();
            workspace->groups[workspace->count].score = (group->count << 16) + (long)score;
            ++workspace->count;
        } else {
            ::datum_delete(object_recycling_groups(), iterator.get_index());
        }
    }
    if (workspace->count > 1) {
        if (workspace->count != 0)
            qsort(workspace->groups, workspace->count, sizeof(object_recycling_group_entry), sort_recycling_entry_descending);
    }
}
static __forceinline void object_recycling_scale_point(const real_point3d *point, float scale, real_point3d *out)
{
    out->x = point->x * scale;
    out->y = point->y * scale;
    out->z = point->z * scale;
}
static __forceinline bool object_recycling_volume_center(const scenario_trigger_volume *volume, real_point3d *center)
{
    bool valid = false;
    real_matrix4x3 matrix;
    if (trigger_volume_get_matrix(volume, &matrix)) {
        real_point3d midpoint;
        object_recycling_scale_point(&volume->extents, 0.5f, &midpoint);
        matrix4x3_transform_point(&matrix, &midpoint, center);
        valid = true;
    }
    return valid;
}
static __forceinline bool object_recycling_bounds_intersect(const real_rectangle3d *a, const real_rectangle3d *b)
{
    bool intersects = true;
    float x0 = a->x0 > b->x0 ? a->x0 : b->x0;
    float x1 = a->x1 > b->x1 ? b->x1 : a->x1;
    float y0 = a->y0 > b->y0 ? a->y0 : b->y0;
    float y1 = a->y1 > b->y1 ? b->y1 : a->y1;
    float z0 = a->z0 > b->z0 ? a->z0 : b->z0;
    float z1 = a->z1 > b->z1 ? b->z1 : a->z1;
    if (x0 > x1 || y0 > y1 || z0 > z1) intersects = false;
    return intersects;
}
static __forceinline void object_recycling_resume_sphere_bounds(const real_point3d *center, const float &radius, real_rectangle3d *bounds)
{
    bounds->x0 = center->x - radius;
    bounds->x1 = center->x + radius;
    bounds->y0 = center->y - radius;
    bounds->y1 = center->y + radius;
    bounds->z0 = center->z - radius;
    bounds->z1 = center->z + radius;
}
// @address 0x364208 ?update_active_recycling_volumes@@YAXPAUs_recycling_workspace@@@Z
void update_active_recycling_volumes(s_recycling_workspace *workspace)
{
    if (recycling_volumes_get()->active != -1) {
        objects_recycling_update_membership(workspace);
        prepare_recycling_group_list(workspace);
        if (workspace->count > 0) {
            long time = tls_game_time_globals()->time;
            for (long index = recycling_volumes_get()->active; index != -1; ) {
                s_recycling_volume *volume = &recycling_volumes_get()->volumes[index];
                long next = volume->next;
                long maximum = volume->maximum_count;
                const scenario_trigger_volume *definition = &((const scenario_trigger_volume *)global_scenario->trigger_volumes.address)[volume->volume_index];
                long remaining = volume->due - time;
                real_point3d center;
                if (object_recycling_volume_center(definition, &center)) {
                    float radius = definition->radius;
                    if (workspace->count < 128) {
                        long group_index = datum_new(object_recycling_groups());
                        if (group_index != -1) {
                            long count = workspace->count;
                            struct group *group = datum_get<struct group>(tls_recycling_groups(), group_index);
                            object_recycling_group_entry *entry = &workspace->groups[count];
                            workspace->count = count + 1;
                            entry->index = group_index;
                            group->count = 0;
                            group->first = -1;
                            group->center = center;
                            object_recycling_resume_sphere_bounds(&center, radius, &entry->bounds);
                            for (long i = 0; i < count; ++i) {
                                object_recycling_group_entry *source = &workspace->groups[i];
                                if (object_recycling_bounds_intersect(&entry->bounds, &source->bounds)) {
                                    long previous = -1;
                                    for (long object_index = datum_get<struct group>(tls_recycling_groups(), source->index)->first; object_index != -1; ) {
                                        object_datum *object = object_get(object_index);
                                        long next_object = object->next_recycling_group_member;
                                        if (::scenario_trigger_volume_test_point(volume->volume_index, &object->bounding_center))
                                            move_recycling_object_between_groups(object_index, previous, object->parent_recycling_group, group_index);
                                        else previous = object_index;
                                        object_index = next_object;
                                    }
                                }
                            }
                            if (group->count > maximum) {
                                long release_count = group->count - maximum;
                                long flags[] = { 0, 1, 3 };
                                long method = 0;
                                if (remaining > 0) {
                                    if (release_count > 0) {
                                        if (remaining < release_count) release_count /= remaining;
                                        else if (remaining > release_count) release_count = time % (remaining / release_count) == 0;
                                        else release_count = 1;
                                    }
                                }
                                while (release_count > 0 && method < 3) {
                                    long object_index = find_worst_object_in_group(flags[method], (struct group *)group, time);
                                    if (object_index != -1) {
                                        object_activation_regions_update_for_object(object_index, 0);
                                        object_set_garbage(object_index, false, -1);
                                        objects_purge_deleted_object(object_index);
                                        --release_count;
                                    } else ++method;
                                }
                            }
                        }
                    }
                }
                if (remaining <= 0)
                    object_recycling_switch_lists(index, &recycling_volumes_get()->active, &recycling_volumes_get()->free);
                index = next;
            }
        }
    }
}
// Method name slots, sized to their shipped 16-byte (setup: 8-byte) pool slots.
// @address 0x10e4 k_recycling_method_name_setup
const char k_recycling_method_name_setup[8] = "setup";
// @address 0x43798 k_recycling_method_name_remove_active0
const char k_recycling_method_name_remove_active0[16] = "remove_active0";
// @address 0x43788 k_recycling_method_name_remove_active1
const char k_recycling_method_name_remove_active1[16] = "remove_active1";
// @address 0x43778 k_recycling_method_name_remove_active2
const char k_recycling_method_name_remove_active2[16] = "remove_active2";
// @address 0x43768 k_recycling_method_name_remove_worst0
const char k_recycling_method_name_remove_worst0[16] = "remove worst0"; // space matches the shipped image
// @address 0x43758 k_recycling_method_name_remove_worst1
const char k_recycling_method_name_remove_worst1[16] = "remove_worst1";
// @address 0x43748 k_recycling_method_name_remove_swarms
const char k_recycling_method_name_remove_swarms[16] = "remove_swarms";
// @address 0x43738 k_recycling_method_name_remove_squads
const char k_recycling_method_name_remove_squads[16] = "remove_squads";
// @address 0x43728 k_recycling_method_name_remove_worst2
const char k_recycling_method_name_remove_worst2[16] = "remove_worst2";
// @address 0x43718 k_recycling_method_name_remove_worst3
const char k_recycling_method_name_remove_worst3[16] = "remove_worst3";
// @address 0x73cb38 g_object_recycling_methods
object_recycling_method g_object_recycling_methods[10] = {
    { k_recycling_method_name_setup, -1, 0, 0,
        (long (*)(long, void *, long, bool *, char *, long))setup_recycling_workspace, 0 },
    { k_recycling_method_name_remove_active0, 4, 0, 0,
        (long (*)(long, void *, long, bool *, char *, long))recycle_worst_object_available, 4 },
    { k_recycling_method_name_remove_active1, 4, 0, 0,
        (long (*)(long, void *, long, bool *, char *, long))recycle_worst_object_available, 5 },
    { k_recycling_method_name_remove_active2, 4, 0, 0,
        (long (*)(long, void *, long, bool *, char *, long))recycle_worst_object_available, 7 },
    { k_recycling_method_name_remove_worst0, 3, 0, 0,
        (long (*)(long, void *, long, bool *, char *, long))recycle_worst_object_available, 0 },
    { k_recycling_method_name_remove_worst1, 3, 0, 0,
        (long (*)(long, void *, long, bool *, char *, long))recycle_worst_object_available, 1 },
    { k_recycling_method_name_remove_swarms, 8, 1, 0,
        (long (*)(long, void *, long, bool *, char *, long))ai_release_inactive_swarms, 0 },
    { k_recycling_method_name_remove_squads, 8, 1,
        (void (*)(long, void *, long))ai_find_inactive_squads,
        (long (*)(long, void *, long, bool *, char *, long))ai_release_inactive_squads, 0 },
    { k_recycling_method_name_remove_worst2, 8, 0, 0,
        (long (*)(long, void *, long, bool *, char *, long))recycle_worst_object_available, 2 },
    { k_recycling_method_name_remove_worst3, 8, 0, 0,
        (long (*)(long, void *, long, bool *, char *, long))recycle_worst_object_available, 3 },
};
// @address 0x112990 ?objects_recycling_update@@YAXW4e_recycling_methods@@@Z
void objects_recycling_update(e_recycling_methods method)
{
    s_recycling_workspace_storage storage[2];
    s_recycling_workspace *workspace = &storage[0].workspace;
    workspace->initialized = false;
    workspace->count = 0;
    char reason[512];
    bool more;
    if (method != _recycling_normal) {
        objects_recycling_update_membership(workspace);
        prepare_recycling_group_list(workspace);
        long flags;
        switch (method) {
        case _recycling_aggressive: flags = 1; break;
        case _recycling_all: flags = 3; break;
        default: flags = 0; break;
        }
        long released = 0;
        more = true;
        do {
            released += recycle_worst_object_available(flags, workspace, sizeof(storage[0]), &more, reason, 512);
            more &= released != 0;
        } while (more);
    } else {
        c_recycling_pressure_flags tolerance = recycling_get_tolerance_flags();
        if (tolerance.value & 15UL) {
            long released = 0;
            if (tolerance.test(0)) objects_compact_memory_pool();
            c_recycling_pressure_flags pressure = recycling_get_target_flags(tolerance);
            if (pressure.value & 15UL) {
            object_recycling_method *entry = g_object_recycling_methods;
            unsigned long offset = 0;
            do {
                if (offset >= sizeof(g_object_recycling_methods)) break;
                if (pressure.value & entry->pressure) {
                    c_stop_watch stopwatch;
                    stopwatch.start();
                    void *buffer = 0;
                    if (entry->workspace_index >= 0 && entry->workspace_index < 2)
                        buffer = &storage[entry->workspace_index];
                    if (entry->setup) entry->setup(entry->flags, buffer, sizeof(storage[0]));
                    bool compact = false;
                    more = (pressure.value & entry->pressure) != 0;
                    while (more) {
                        long count = entry->recycle(entry->flags, buffer, sizeof(storage[0]), &more, reason, 512);
                        released += count;
                        more &= count != 0;
                        if (count != 0) {
                            compact = true;
                            if (released % 16 == 0 && pressure.test(0)) {
                                objects_compact_memory_pool();
                                compact = false;
                            }
                        }
                        pressure = recycling_get_target_flags(tolerance);
                        more &= (pressure.value & entry->pressure) != 0;
                    }
                    if (compact && pressure.test(0)) objects_compact_memory_pool();
                    pressure = recycling_get_target_flags(tolerance);
                }
                offset += sizeof(object_recycling_method);
                ++entry;
            } while (pressure.value & 15UL);
            }
        }
    }
    update_active_recycling_volumes(workspace);
}

float k_recycling_min_group_distance = 24.0f;
// @address 0x709200 ??__Ek_recycling_min_group_distance_squared@@YAXXZ
float k_recycling_min_group_distance_squared = k_recycling_min_group_distance * k_recycling_min_group_distance;
c_recycling_groups_allocator g_recycling_groups_allocator;
c_recycling_volumes_allocator g_recycling_volumes_allocator;

// @address 0x79dc28 g_recycling_critical_memory_threshold
long g_recycling_critical_memory_threshold = 39321;
// @address 0x79dc2c g_recycling_tolerance_memory_threshold
long g_recycling_tolerance_memory_threshold = 78643;
// @address 0x79dc30 g_recycling_target_memory_threshold
long g_recycling_target_memory_threshold = 157286;
// @address 0x2b100 k_recycling_group_split_threshold
const float k_recycling_group_split_threshold = 12.0f;
// @address 0x3ad90 k_recycling_object_color
// @rdata 0x8203ad90..0x8203ada0
static const real_argb_color k_recycling_object_color = { 1.0f, 0.81f, 0.13f, 0.56f };
// @rdata-end
// @address 0x709238 ??__Eg_recycling_object_colors@@YAXXZ
const real_argb_color *g_recycling_object_colors[6] = {
    global_real_argb_pink,
    global_real_argb_magenta,
    global_real_argb_green,
    global_real_argb_blue,
    &k_recycling_object_color,
    global_real_argb_orange,
};


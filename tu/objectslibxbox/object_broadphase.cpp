// objectslibxbox/object_broadphase.obj: from the records 0x9e280 0x9ec98 0xe3c40 0xe3f98 0xe40e8 0x1ba6f0 0x317488 0x320b80 0x3328b8 0x332940 0x332d70 0x332d90 0x332ec0 0x332f98 0x332fb8 0x3330c0 0x3331b0 0x333248 0x3332d8 0x9ee90
#include "cache/restricted_memory.h"
#include "math/real_math_functions.h"
#include "math/real_math_bounds_storage.h"
#include "objects/object_runtime_globals_types.h"
#include "objects/object_header.h"
#include "multithreading/thread_tls.h"
#include "math/real_math_types.h"
#include <stddef.h>
#include "math/real_math.h"
#include "cseries/crt_xdk.h"
#include "cseries/cseries.h"
#include "objects/object_iterator.h"
#include "objects/object_datum.h"
#include <math.h>
#include <vectorintrinsics.h>
#include "cache/restricted_memory_regions.h"
#include "objects/objects.h"
#include "objects/object_broadphase.h"
#include "objects/object_broadphase_types.h"
#include "phys/phys_math.h"
#include "phys/phys_typed_broadphase_handle_pair.h"
#include "phys/phys_memory.h"
#include "phys/phys_monitor_stream.h"
#include "cache/restricted_memory_layout.h"
#include "physics/collision_types.h"
#include "physics/memory_allocator_types.h"
#include "cache/cache_files.h"
#include "physics/collisions.h"
#include "scenario/scenario.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "structures/structure_bsp_tag.h"
#include "objects/object_broadphase_allocator.h"
#include "physics/havok_math_types.h"
#include "phys/phys_aabb_types.h"
#include "phys/phys_broadphase_cast_collector.h"
#include "objects/object_runtime_tls.h"
#include "objects/object_broadphase_globals.h"
#include "phys/phys_base.h"
#include "phys/phys_broadphase_handle.h"
#include "phys/phys_axis_sweep.h"
#include "physics/havok_memory.h"
#include "physics/havok_memory_globals.h"
#include "cache/restricted_memory_region_members.h"
#include "memory/datum_index.h"
#include "tag_files/tag_groups.h"
// @address 0x7c5d34 k_null_rectangle3d_storage
// @flags /TP /O2

struct s_collision_test_flags;
union real_point3d;
union real_vector3d;
struct collision_result;
union real_point3d;
struct collision_result;

struct s_object_globals;

class phys_thread_memory;
class phys_axis_sweep;
struct s_object_broadphase;

typedef char object_broadphase_handle_data_size[(sizeof(s_object_broadphase_handle_data) == 20) ? 1 : -1];
typedef char object_broadphase_handle_data_cull_offset[(offsetof(s_object_broadphase_handle_data, cull_flags) == 2) ? 1 : -1];

typedef char object_broadphase_sweep_nodes_offset[(offsetof(phys_axis_sweep, m_nodes) == 0x70) ? 1 : -1];

class c_object_broadphase_handle : public phys_broadphase_handle      // 28 bytes
{
public:
    __forceinline c_object_broadphase_handle() : phys_broadphase_handle(phys_fixup_tag()) {}
    long get_object_index() const { return m_object_index; }
    __forceinline void clear()
    {
        memset(&m_data, 0, sizeof(m_data));
        m_handle_id = 0;
        m_object_index = NONE;
    }
    s_object_broadphase_handle_data m_data;
    long m_object_index;
};

struct s_object_broadphase
{
#ifdef HALO3_64BIT
    __declspec(align(16)) char sweep_storage[(sizeof(phys_axis_sweep) + 15) & ~15];
    phys_axis_sweep::phys_sweep_node nodes[2048];
    phys_axis_sweep::phys_sweep_endpoint end_points[3][4096];
#else
    union {
        char broadphase_buffer[82096];
        struct {
            char sweep_storage[0xb0];
            phys_axis_sweep::phys_sweep_node nodes[2048];
            phys_axis_sweep::phys_sweep_endpoint end_points[3][4096];
        };
    };
#endif
    c_static_array<c_object_broadphase_handle, 2048> handle_array;
    c_object_broadphase_handle *handle_base;
    phys_axis_sweep::phys_sweep_marker markers[16];
    unsigned short overlapping_objects[16][2048];
    real_rectangle3d world_bounds;
    bool initialized;
    bool objects_in_broadphase;
    __forceinline phys_axis_sweep *get_sweep() { return (phys_axis_sweep *)sweep_storage; }
};

// The sweep object is copied raw with the game state, so its vtable pointer is saved after construction and restored on load.
struct s_object_broadphase_vtable_globals
{
    void *sweep_vtable_pointer;
};

// @address 0x7f1a0c g_object_broadphase_vtable_globals
s_object_broadphase_vtable_globals g_object_broadphase_vtable_globals;

// @address 0xd28 ??_7c_object_broadphase_ray_cast_callback@@6B@
// @address 0x332860 ??_Gc_object_broadphase_ray_cast_callback@@UAAPAXI@Z
// @address 0xa430e0 g_havok_memory_globals
// @address 0x74b910 g_havok_monitor_globals
class c_object_broadphase_ray_cast_callback : public phys_broadphase_cast_collector
{
public:
    __forceinline c_object_broadphase_ray_cast_callback(unsigned __int64 flags, unsigned long collision_model_flags, const real_point3d *point,
        const real_vector3d *vector, long first_ignore_object_index, long second_ignore_object_index, long ignore_index,
        collision_result *collision, float min_t, float max_t)
    {
        m_hit = false;
        // match: 0x9ec98: copy the 64-bit filter as one aggregate so it is stored with one std.
        memcpy(&m_flags, &flags, sizeof(m_flags));
        m_collision_model_flags = collision_model_flags;
        m_point = point;
        m_vector = vector;
        m_first_ignore_object_index = first_ignore_object_index;
        m_second_ignore_object_index = second_ignore_object_index;
        m_ignore_index = ignore_index;
        m_collision = collision;
        m_start = min_t;
        m_scale = 1.0f / ((max_t - min_t > 0.0001f) ? max_t - min_t : 0.0001f);
        m_fraction = 1.0f;
    }
    bool m_hit;
protected:
    virtual float add_broadphase_handle(const phys_broadphase_handle *broad_phase_handle, int cast_index);
    s_collision_test_flags m_flags;
    unsigned long m_collision_model_flags;
    const real_point3d *m_point;
    const real_vector3d *m_vector;
    long m_first_ignore_object_index;
    long m_second_ignore_object_index;
    long m_ignore_index;
    collision_result *m_collision;
    float m_start;
    float m_scale;
    float m_fraction;
};

namespace
{
    struct s_object_broadphase_publisher
    {
        static __forceinline void publish(void *address) { __tls_set_g_object_broadphase(static_cast<s_object_broadphase *>(address)); }
    };
    static __forceinline s_object_broadphase *object_broadphase_get()
    {
        return g_object_broadphase;
    }
    static __forceinline void havok_memory_allocator_lock()
    {
        g_havok_memory_allocator_locked = true;
    }
    static __forceinline void havok_memory_allocator_unlock()
    {
        g_havok_memory_allocator_locked = false;
    }
}

// @address 0x9e280 ?clip_vector_to_rectangle@@YA_NPBTreal_rectangle3d@@PBTreal_point3d@@PBTreal_vector3d@@MPAM3@Z
bool clip_vector_to_rectangle(const real_rectangle3d *rectangle, const real_point3d *point, const real_vector3d *vector, float distance,
    float *minimum, float *maximum)
{
    bool result = true;
    real_point3d endpoint;
    if (point_in_rectangle3d(point, rectangle) && point_in_rectangle3d(point_from_line3d(point, vector, distance, &endpoint), rectangle))
    {
        *minimum = 0.0f;
        *maximum = distance;
    }
    else
    {
        float minimum_distance = 0.0f;
        float maximum_distance = distance;
        for (long axis = 0; result && axis < 3; ++axis)
        {
            float position = point->n[axis];
            float direction = vector->n[axis];
            float lower = rectangle->n[axis].lower;
            float upper = rectangle->n[axis].upper;
            if (fabsf(direction - 0.0f) >= 0.0001f)
            {
                float inverse_direction = 1.0f / direction;
                float lower_distance = inverse_direction * (lower - position);
                float upper_distance = inverse_direction * (upper - position);
                if (direction > 0.0f)
                {
                    if (minimum_distance < lower_distance)
                        minimum_distance = lower_distance;
                    if (maximum_distance > upper_distance)
                        maximum_distance = upper_distance;
                }
                else
                {
                    if (minimum_distance < upper_distance)
                        minimum_distance = upper_distance;
                    if (maximum_distance > lower_distance)
                        maximum_distance = lower_distance;
                }
                if (minimum_distance > maximum_distance)
                    result = false;
            }
            else if (position < lower || position > upper)
            {
                result = false;
            }
        }
        *minimum = minimum_distance;
        *maximum = maximum_distance;
    }
    return result;
}

static __forceinline object_datum *object_broadphase_aabb_datum(long object_index)
{
    s_object_header *object_header = object_header_get(object_index);
    return (object_datum *)object_header->datum;
}

// @address 0xe3c40 ?object_broadphase_set_handle_data@@YAXJPBUs_object_broadphase_handle_data@@@Z
void object_broadphase_set_handle_data(long object_index, const s_object_broadphase_handle_data *data)
{
    s_object_broadphase *broadphase = object_broadphase_get();
    if (broadphase->objects_in_broadphase)
    {
        broadphase->handle_array.m_storage[datum_index_to_absolute_index(object_index)].m_data = *data;
    }
}

// @address 0xe3f98 ?object_broadphase_update_object@@YAXJ@Z
void object_broadphase_update_object(long object_index)
{
    if (object_broadphase_get()->objects_in_broadphase)
    {
        object_datum *object = object_broadphase_aabb_datum(object_index);
        havok_memory_allocator_lock();
        if ((unsigned char)((object->object_flags >> 8) & 1) && (unsigned char)((object->object_flags >> 9) & 1))
        {
            phys_aabb aabb;
            object_calculate_broadphase_aabb(object_index, &aabb);
            phys_broadphase_handle *handle = &object_broadphase_get()->handle_array.m_storage[datum_index_to_absolute_index(object_index)];
            phys_inline_array<phys_broadphase_handle_pair, 2048> added_pairs(0);
            phys_inline_array<phys_broadphase_handle_pair, 2048> removed_pairs(0);
            object_broadphase_get()->get_sweep()->update_aabbs(&handle, &aabb, 1, added_pairs, removed_pairs);
        }
        havok_memory_allocator_unlock();
    }
}

// @address 0xe40e8 ?object_broadphase_dispose_from_old_structure_bsp@@YAXK@Z
void object_broadphase_dispose_from_old_structure_bsp(unsigned long)
{
    s_object_broadphase *broadphase = g_object_broadphase;
    memset(broadphase, 0, sizeof(s_object_broadphase));
    broadphase->objects_in_broadphase = false;
}

// @address 0x1ba6f0 ?get_singleton_instance@phys_thread_memory@@SAAAV1@XZ

void __tls_set_g_object_broadphase_allocator(void *allocator)
{
    object_runtime_globals_set((s_object_globals *)allocator);
}

// @address 0x320b80 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$1?object_broadphase_post_copy_fixup@@YAXPAX@Z$1?__tls_set_g_object_broadphase_allocator@@YAX0@Z@@UAAXPAX@Z

template<>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), &object_broadphase_post_copy_fixup,
    &__tls_set_g_object_broadphase_allocator>::deallocate(
    void *allocation)
{
    long member_index = m_member_index;
    __tls_set_g_object_broadphase_allocator(0);
    restricted_region_free_member(3, member_index);
}

// @address 0x332d70 ?__tls_set_g_object_broadphase@@YAXPAUs_object_broadphase@@@Z
void __tls_set_g_object_broadphase(s_object_broadphase *broadphase)
{
    g_object_broadphase = broadphase;
    if (g_object_broadphase != 0)
        object_broadphase_post_copy_fixup(object_broadphase_get());
}

// @address 0x332d90 ?object_broadphase_post_copy_fixup@@YAXPAX@Z
void object_broadphase_post_copy_fixup(void *allocation)
{
    s_object_broadphase *broadphase = static_cast<s_object_broadphase *>(allocation);
    if (broadphase->initialized)
    {
        phys_axis_sweep *sweep = broadphase->get_sweep();
        sweep->m_markers = broadphase->markers;
        for (int marker_index = 0; marker_index < sweep->m_sweep_marker_count; ++marker_index)
            sweep->m_markers[marker_index].m_overlapping_objects.m_data = broadphase->overlapping_objects[marker_index];
        sweep->m_nodes.m_data = broadphase->nodes;
        for (int dimension = 0; dimension < 3; ++dimension)
            sweep->m_axis[dimension].m_sweep_endpoints.m_data = broadphase->end_points[dimension];
        for (int node_index = 0; node_index < sweep->m_nodes.m_size; ++node_index)
        {
            phys_array<phys_axis_sweep::phys_sweep_node> &nodes = sweep->m_nodes;
            phys_axis_sweep::phys_sweep_node &node = nodes[node_index];
            if (node_index != 0 && node.m_handle != 0 && !node.is_marker())
            {
                size_t handle_offset = reinterpret_cast<char *>(node.m_handle) - reinterpret_cast<char *>(broadphase->handle_base);
                node.m_handle = &broadphase->handle_array.m_storage[handle_offset / sizeof(c_object_broadphase_handle)];
            }
        }
        broadphase->handle_base = broadphase->handle_array.m_storage;
    }
}

// @address 0x5bec98 ?object_broaphase_save_to_game_state@@YAXJ@Z
void object_broaphase_save_to_game_state(long)
{
}

// @address 0x332f98 ?object_broaphase_load_from_game_state@@YAXJ@Z
void object_broaphase_load_from_game_state(long)
{
    *(void **)object_broadphase_get()->get_sweep() = g_object_broadphase_vtable_globals.sweep_vtable_pointer;
}

// @address 0x332fb8 ?object_broadphase_add_object@@YAXJPBUs_object_broadphase_handle_data@@@Z
void object_broadphase_add_object(long object_index, const s_object_broadphase_handle_data *data)
{
    if (object_broadphase_get()->objects_in_broadphase)
    {
        havok_memory_allocator_lock();
        {
            c_object_broadphase_handle *handle = &object_broadphase_get()->handle_array.m_storage[datum_index_to_absolute_index(object_index)];
            phys_aabb aabb;
            object_calculate_broadphase_aabb(object_index, &aabb);
            phys_inline_array<phys_broadphase_handle_pair, 2048> added_pairs(0);
            handle->m_object_index = object_index;
            handle->m_data = *data;
            object_broadphase_get()->get_sweep()->add_broadphase_object(handle, aabb, added_pairs);
        }
        havok_memory_allocator_unlock();
    }
}

// @address 0x3330c0 ?object_broadphase_remove_object@@YAXJ@Z
void object_broadphase_remove_object(long object_index)
{
    if (object_broadphase_get()->objects_in_broadphase)
    {
        havok_memory_allocator_lock();
        {
            c_object_broadphase_handle *handle = &object_broadphase_get()->handle_array.m_storage[datum_index_to_absolute_index(object_index)];
            phys_inline_array<phys_broadphase_handle_pair, 2048> removed_pairs(0);
            object_broadphase_get()->get_sweep()->remove_broadphase_object(handle, removed_pairs);
            handle->clear();
        }
        havok_memory_allocator_unlock();
    }
}

static __forceinline void object_broadphase_prepare_thread()
{
    if (!TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot))
    {
        TlsSetValue(phys_thread_memory::s_threadMemoryInstance.m_slot, g_havok_monitor_globals.thread_memory);
        TlsSetValue(phys_monitor_stream::m_instance.m_slot, g_havok_monitor_globals.monitor_stream);
    }
}
// match: 0x9ec98: components are stored z, x, y, then w.
static __forceinline void object_broadphase_point_to_havok(phys_vec4 *v, const real_point3d *p)
{
    phys_simd_vec_union u;
    u.r[2] = p->z;
    u.r[0] = p->x;
    u.r[1] = p->y;
    u.r[3] = 0.0f;
    v->m_simd_quad = u.q;
}

// @address 0x9ec98 ?object_broadphase_ray_cast@@YA_N_KKPBTreal_point3d@@PBTreal_vector3d@@JJJPAUcollision_result@@@Z
bool object_broadphase_ray_cast(unsigned __int64 filter, unsigned long collision_model_flags, union real_point3d const *point,
    union real_vector3d const *vector, long first_ignore_object_index, long second_ignore_object_index, long ignore_index,
    struct collision_result *result)
{
    bool success = false;
    havok_memory_allocator_lock();
    object_broadphase_prepare_thread();
    if (g_havok_memory_globals.active_allocator)
    {
        float min_t, max_t;
        s_object_broadphase *broadphase = object_broadphase_get();
        if (clip_vector_to_rectangle(&broadphase->world_bounds, point, vector, result->t, &min_t, &max_t))
        {
            c_object_broadphase_ray_cast_callback collector(filter, collision_model_flags, point,
                vector, first_ignore_object_index, second_ignore_object_index, ignore_index, result, min_t, max_t);
            phys_vec4 to;
            phys_broadphase::phys_ray_cast_input input;
            real_point3d from_point, to_point;
            point_from_line3d(point, vector, min_t, &from_point);
            object_broadphase_point_to_havok(&input.m_ray_from, &from_point);
            point_from_line3d(point, vector, max_t, &to_point);
            object_broadphase_point_to_havok(&to, &to_point);
            input.m_to_base = &to;
            input.m_aabb_cache_info = 0;
            broadphase->get_sweep()->trace_ray(input, &collector, sizeof(collector));
            success = collector.m_hit;
        }
    }
    havok_memory_allocator_unlock();
    return success;
}

// @address 0x9ee90 ?add_broadphase_handle@c_object_broadphase_ray_cast_callback@@MAAMPBVphys_broadphase_handle@@H@Z
float c_object_broadphase_ray_cast_callback::add_broadphase_handle(const phys_broadphase_handle *broad_phase_handle, int cast_index)
{
    const c_object_broadphase_handle *handle = static_cast<const c_object_broadphase_handle *>(broad_phase_handle);
    const s_object_broadphase_handle_data *data = &handle->m_data;
    long object_types = (m_flags.object_flags >> 1) & 0x3fff;
    if ((object_types & (1 << data->object_type)) != 0 &&
        (collision_compute_cull_flags(m_flags) & data->cull_flags) == 0 &&
        fast_vector_intersects_sphere(m_point, m_vector, &data->center, data->radius) &&
        ::object_test_vector_internal(handle->get_object_index(), false, m_flags, m_collision_model_flags, m_point, m_vector,
            m_first_ignore_object_index, m_second_ignore_object_index, m_ignore_index, m_collision))
    {
        float fraction;
        if (TEST_BIT((unsigned short)(m_flags.collision_flags >> 16), 0))
            fraction = 0.0f;
        else
            fraction = (m_collision->t - m_start) * m_scale;
        m_hit = true;
        m_fraction = fraction;
    }
    return m_fraction;
}

// @address 0x3331b0 ?object_calculate_broadphase_aabb@@YAXJPAVphys_aabb@@@Z
void object_calculate_broadphase_aabb(long object_index, phys_aabb *aabb)
{
    object_datum *object = object_broadphase_aabb_datum(object_index);
    const real_point3d *center = &object->bounding_sphere_center;
    float radius = object->bounding_sphere_radius;
    aabb->m_min_limit.set(center->x - radius, center->y - radius, center->z - radius, 0.0f);
    aabb->m_max_limit.set(center->x + radius, center->y + radius, center->z + radius, 0.0f);
}


// @address 0x333248 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$1?object_broadphase_post_copy_fixup@@YAXPAX@Z$1?__tls_set_g_object_broadphase_allocator@@YAX0@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), &object_broadphase_post_copy_fixup,
    &__tls_set_g_object_broadphase_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    m_member_index = restricted_region_add_member<s_object_broadphase_publisher>(3, name, type, size, alignment_bits,
        (field_proc_t)__tls_set_g_object_broadphase, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// @address 0x3332d8 ?allocate@?$t_restricted_allocation_manager@$02$0A@$1?object_broadphase_post_copy_fixup@@YAXPAX@Z$1?__tls_set_g_object_broadphase_allocator@@YAX0@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), &object_broadphase_post_copy_fixup,
    &__tls_set_g_object_broadphase_allocator>::allocate(
    unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x332ec0 object_broadphase_add_all_objects
extern "C" void object_broadphase_add_all_objects(void)
{
    c_object_iterator<object_datum> iterator;
    s_object_cluster_payload payload;
    iterator.begin(0, 0);
    object_broadphase_get()->objects_in_broadphase = true;
    while (iterator.next())
    {
        object_datum *header = iterator.get_datum();
        unsigned long flags = header->flags;
        unsigned char active = (flags >> 9) & 1;
        if (active)
        {
            unsigned char visible = (flags >> 8) & 1;
            if (visible)
            {
                long object_index = iterator.get_index();
                cluster_build_object_payload(object_index, &payload);
                object_broadphase_add_object(object_index, (const s_object_broadphase_handle_data *)&payload);
            }
        }
    }
}

// @address 0x3328b8 ?object_broadphase_initialize@@YAXXZ
void object_broadphase_initialize()
{
    s_object_broadphase *broadphase = static_cast<s_object_broadphase *>(g_object_broadphase_allocator.reserve_memory(
        "object_broadphase", "global", sizeof(s_object_broadphase), 4));
    g_object_broadphase = broadphase;
    memset(broadphase, 0, sizeof(s_object_broadphase));
    memset(&g_object_broadphase_vtable_globals, 0, sizeof(g_object_broadphase_vtable_globals));
    broadphase->initialized = false;
}

// @address 0x5bec98 ?object_broadphase_dispose@@YAXXZ
void object_broadphase_dispose()
{
}

// @address 0x5bec98 ?object_broadphase_initialize_for_new_map@@YAXXZ
void object_broadphase_initialize_for_new_map()
{
}

// @address 0x5bec98 ?object_broadphase_dispose_from_old_map@@YAXXZ
void object_broadphase_dispose_from_old_map()
{
}

static __forceinline void object_broadphase_initialize_handle(c_object_broadphase_handle *handle)
{
    memset(&handle->m_data, 0, sizeof(handle->m_data));
    handle->m_object_index = NONE;
    handle->m_handle_id = 0;
}

static __forceinline c_object_broadphase_handle &object_broadphase_handle_get(c_object_broadphase_handle *handles, long index)
{
    return handles[index];
}

// @address 0x332940 ?object_broadphase_initialize_for_new_structure_bsp@@YAXK@Z
void object_broadphase_initialize_for_new_structure_bsp(unsigned long activating_structure_bsp_mask)
{
    s_object_broadphase *broadphase = object_broadphase_get();
    memset(broadphase, 0, sizeof(*broadphase));
    real_rectangle3d *bounds = &object_broadphase_get()->world_bounds;
    unsigned long structure_bsp_mask = g_active_structure_bsp_mask | activating_structure_bsp_mask;
    *bounds = k_null_rectangle3d_storage;
    for (long bsp_index = 0; bsp_index < 16; ++bsp_index)
    {
        if (structure_bsp_mask & (1UL << bsp_index))
        {
            bounds = real_rectangle3d_enclose_rectangle(bounds,
                &TAG_GET('sbsp', structure_bsp, ((const scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[bsp_index]
                    .structure_bsp.index)
                    ->world_bounds);
        }
    }
    phys_vec4 minimum, padding, maximum;
    minimum.set(bounds->x0, object_broadphase_get()->world_bounds.y0, object_broadphase_get()->world_bounds.z0, 0.0f);
    padding.set(64.0f, 64.0f, 64.0f, 0.0f);
    maximum.set(object_broadphase_get()->world_bounds.x1, object_broadphase_get()->world_bounds.y1, object_broadphase_get()->world_bounds.z1, 0.0f);
    minimum.sub4(padding);
    maximum.add4(padding);
    object_broadphase_get()->initialized = true;
    new (object_broadphase_get()->get_sweep()) phys_axis_sweep(minimum, maximum, 16);
    g_object_broadphase_vtable_globals.sweep_vtable_pointer = *(void **)object_broadphase_get()->get_sweep();
    c_object_broadphase_handle *handles = object_broadphase_get()->handle_array.m_storage;
    for (long handle_index = 0; handle_index < object_broadphase_get()->handle_array.get_count(); ++handle_index)
        object_broadphase_initialize_handle(&object_broadphase_get()->handle_array[handle_index]);
    object_broadphase_get()->handle_base = handles;
    unsigned long nodes_old_count = object_broadphase_get()->get_sweep()->m_nodes.m_size;
    memcpy(object_broadphase_get()->nodes, object_broadphase_get()->get_sweep()->m_nodes.m_data, nodes_old_count * sizeof(phys_axis_sweep::phys_sweep_node));
    object_broadphase_get()->get_sweep()->m_nodes.clear_and_deallocate();
    object_broadphase_get()->get_sweep()->m_nodes.m_capacity_and_flags = 0x80000800;
    object_broadphase_get()->get_sweep()->m_nodes.m_data = object_broadphase_get()->nodes;
    object_broadphase_get()->get_sweep()->m_nodes.m_size = nodes_old_count;
    for (long dimension = 0; dimension < 3; ++dimension)
    {
        unsigned long end_points_old_count = object_broadphase_get()->get_sweep()->m_axis[dimension].m_sweep_endpoints.m_size;
        memcpy(object_broadphase_get()->end_points[dimension], object_broadphase_get()->get_sweep()->m_axis[dimension].m_sweep_endpoints.m_data,
            end_points_old_count * sizeof(phys_axis_sweep::phys_sweep_endpoint));
        object_broadphase_get()->get_sweep()->m_axis[dimension].m_sweep_endpoints.clear_and_deallocate();
        object_broadphase_get()->get_sweep()->m_axis[dimension].m_sweep_endpoints.m_capacity_and_flags = 0x80001000;
        object_broadphase_get()->get_sweep()->m_axis[dimension].m_sweep_endpoints.m_data = object_broadphase_get()->end_points[dimension];
        object_broadphase_get()->get_sweep()->m_axis[dimension].m_sweep_endpoints.m_size = end_points_old_count;
    }
    memcpy(object_broadphase_get()->markers, object_broadphase_get()->get_sweep()->m_markers,
        object_broadphase_get()->get_sweep()->m_sweep_marker_count * sizeof(phys_axis_sweep::phys_sweep_marker));
    phys_axis_sweep::phys_sweep_marker *markers = object_broadphase_get()->get_sweep()->m_markers;
    phys_thread_memory::get_singleton_instance().deallocate(markers);
    object_broadphase_get()->get_sweep()->m_markers = object_broadphase_get()->markers;
    for (long marker_index = 0; marker_index < object_broadphase_get()->get_sweep()->m_sweep_marker_count; ++marker_index)
    {
        object_broadphase_get()->get_sweep()->m_markers[marker_index].m_overlapping_objects.clear_and_deallocate();
        object_broadphase_get()->get_sweep()->m_markers[marker_index].m_overlapping_objects.m_capacity_and_flags = 0x80000800;
        object_broadphase_get()->get_sweep()->m_markers[marker_index].m_overlapping_objects.m_data = object_broadphase_get()->overlapping_objects[marker_index];
    }
    object_broadphase_add_all_objects();
}

t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), &object_broadphase_post_copy_fixup, &__tls_set_g_object_broadphase_allocator>
    g_object_broadphase_allocator;

// @address 0x11df6c0 g_object_broadphase
__declspec(thread) s_object_broadphase *g_object_broadphase;

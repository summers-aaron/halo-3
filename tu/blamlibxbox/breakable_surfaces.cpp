// blamlibxbox/breakable_surfaces.obj: from the records 0x9e468 0x9e520 0xf0240 0x115bf8 0x229ae0 0x229b08 0x229ba8 0x229bf0 0x229c58 0x229c70 0x229d98 0x229e70 0x22a230 0x22a6b0 0x22aa38 0x22ae28 0x22afb0 0x22b078 0x22b590 0x22b628 0x22b760 0x22b810 0x22b888 0x22baa0 0x22bbd0 0x22bcb8 0x22bde8 0x22bf40 0x22c3c8 0x22c618 0x22cc80 0x22cd88 0x22cf40 0x22d0e0 0x22d180 0x22d198 0x22d278
#include "structures/structure_bsp_tag.h"
#include "rasterizer/rasterizer_state_globals.h"
#include "render_methods/render_method_submit_globals.h"
#include "physics/breakable_surface_event_data.h"
#include <stddef.h>
#include "rasterizer/rasterizer_render_targets.h"
#include "rasterizer/rasterizer_main_globals.h"
#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
#include "math/real_math_bounds_storage.h"
#include "objects/damage_data.h"
#include "memory/tls_data_arrays.h"
#include "math/real_math_types.h"
#include "tag_files/tag_index.h"
#include "math/real_math.h"
#include "math/matrix_math_types.h"
#include "memory/static_arrays.h"
#include "memory/data_types.h"
#include "tag_files/tag_types.h"
#include "game/game_time_types.h"
#include <string.h>
#include <math.h>
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "tag_files/tag_groups.h"
#include "cseries/cseries.h"
#include "game/game_globals.h"
#include "game/materials_definitions.h"
#include "cache/restricted_memory_regions.h"
#include "sound/game_sound.h"
#include "objects/damage.h"
#include "physics/breakable_surfaces.h"
#include "physics/breakable_surface_types.h"
#include "physics/havok.h"
#include "scenario/scenario_soft_ceilings.h"
#include "phys/phys_base.h"
#include "physics/havok_utilities.h"
#include "scenario/scenario.h"
#include "physics/soft_ceiling_plane_sphere_types.h"
#include "physics/rasterizer_vertex_transparent_types.h"
#include "physics/soft_ceiling_triangle_types.h"
#include "physics/soft_ceiling_group_types.h"
#include "units/bipeds.h"
#include "math/matrix_math.h"
#include "memory/data.h"
#include "scenario/scenario_tag.h"
#include "cache/restricted_memory.h"
#include "bitmaps/bitmap_data.h"
#include "objects/object_datum.h"
#include "structures/structure_runtime_resources.h"
#include "cache/restricted_memory_regions_inline.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "physics/collision_bsp_cache_types.h"
#include "structures/structure_instanced_geometry_definition.h"
#include "sound/game_sound_types.h"
#include "structures/structure_instanced_geometry.h"
#include "physics/breakable_surface_globals.h"
#include "physics/collisions_breakable_surface_types.h"
#include "physics/collision_materials.h"
#include "rasterizer/rasterizer_implicit_geometry.h"
#include "math/random_math.h"
#include "physics/breakable_surface_event_types.h"
#include "physics/breakable_surface_allocators.h"
#include "scenario/cluster_reference_helpers.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_registration.h"
#include "rasterizer/simple_hash_table.h"
#include "math/real_math_functions.h"
#include "physics/breakable_surface_key.h"
#include "phys/phys_bvh_vm.h"
#include "phys/phys_bvh_sphere_query.h"
#include "cseries/crt_xdk.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "rasterizer/rasterizer.h"
#include "bitmaps/bitmap_group.h"
#include "bitmaps/bitmap_group_definitions.h"
#include "physics/breakable_surface_definition.h"
#include "rasterizer/rasterizer_stencil.h"
#include "objects/damage_effect_definitions.h"
#include "physics/soft_ceiling_design_types.h"
#include "effects/breakable_surface_effects.h"
#include "memory/datum_index.h"
#include "math/random_math_range.h"
#include "math/integer_math.h"

// @flags /TP /O2 /Oc
// @address 0x11df74c g_breakable_surface_event_data
extern "C" {
__declspec(thread) s_data_array *g_breakable_surface_event_data;
}
// @address 0x7c5d34 k_null_rectangle3d_storage
// @address 0x9e520 ?breakable_surface_get_surface_set_count@@YAJJJ@Z
// @address 0x229ae0 ?__tls_set_g_breakable_surface_globals_allocator@@YAXPAX@Z
// @address 0x229d98 breakable_surfaces_dispose_from_old_structure_bsp


// @address 0x11df6ac g_breakable_surface_globals
extern "C" {
__declspec(thread) void *g_breakable_surface_globals;
}

// @address 0x7c6178 g_rasterizer_sampler_filter_modes
// @address 0x195e88 rasterizer_device_set_sampler_filter_modes


struct s_breakable_surfaces_dispose_iterator
{
    s_breakable_surfaces_broken_event *event;
    s_data_iterator iterator;
};


template <long count> struct c_breakable_surfaces_static_flags;

template <long count> struct c_breakable_surfaces_static_flags {
    unsigned long bits[(count + 31) / 32];
    __forceinline void set(long index, bool value)
    {
        unsigned long mask = 1UL << (index & 31);
        long word_index = index >> 5;
        if (value)
            bits[word_index] |= mask;
        else
            bits[word_index] &= ~mask;
    }
    __forceinline bool test(long index) const { unsigned long mask = 1UL << (index & 31); return (mask & bits[index >> 5]) != 0; }
};

struct s_breakable_surface_hash {
    short buckets[1024];
    c_additive_hash<1024, 4096, 1, 0>::data_t data[4096];
    short count;
    short mask;
};

struct s_breakable_surface_sound_location {
    s_sound_location sound;
    s_breakable_surfaces_event_material material;
};

namespace {
struct s_breakable_surfaces_traversal_frame {
    short surface_index;
    short adjacent_surface_index;
    unsigned short edge_index;
    bool root;
    bool side;
    collision_edge *edge;
    collision_surface *surface;
    collision_surface *adjacent_surface;
    __forceinline void initialize(long index, bool is_root) { surface_index = (short)index; root = is_root; }
};
struct s_breakable_surfaces_traversal_stack {
    long count;
    s_breakable_surfaces_traversal_frame frames[1024];
    __forceinline s_breakable_surfaces_traversal_frame &top() { return frames[count - 1]; }
    __forceinline s_breakable_surfaces_traversal_frame &push() { return frames[count++]; }
    __forceinline void pop() { --count; }
    __forceinline void push_surface(long surface_index, bool root) {
        long index = count++;
        frames[index].initialize(surface_index, root);
    }
};
}

long breakable_surface_get_surface_set_count(long slot_index, long variant_index)
{
    int result = 0;
    structure_bsp *definition =
        (structure_bsp *)
            global_tag_instances[datum_index_to_absolute_index(((scenario_structure_bsp_reference
                *)global_scenario->structure_bsps.address)[slot_index].structure_bsp.index)]
                .definition;
    if (variant_index == -1)
        return definition->breakable_surfaces.count;
    if (variant_index >= 0 && variant_index < definition->instanced_geometry_instances.count) {
        structure_instanced_geometry_instance *variant = &((structure_instanced_geometry_instance
            *)definition->instanced_geometry_instances.address)[variant_index];
        result = ((structure_instanced_geometry_definition
            *)g_collision_bsp_cache.users[get_current_thread_index()].entries[slot_index].data)[variant->definition_index].breakable_surfaces.count;
    }
    return result;
}

void __tls_set_g_breakable_surface_globals_allocator(void *value)
{
    g_breakable_surface_globals = value;
}


// @address 0x9e468 ?breakable_surface_get_first_set@@YAPAUs_breakable_surface_set@@JJ@Z
struct s_breakable_surface_set * breakable_surface_get_first_set(long slot_index, long variant_index)
{
    s_breakable_surface_set *result = 0;
    collisions_breakable_surface_globals *globals = (collisions_breakable_surface_globals *)g_breakable_surface_globals;
    if ((globals->slot_mask & (1 << slot_index)) != 0 && breakable_surface_get_surface_set_count(slot_index, variant_index) > 0) {
        short entry_index = variant_index == -1
            ? globals->default_entry_indices[slot_index]
            : globals->entry_indices[slot_index][variant_index];
        if (entry_index != -1)
            result = &globals->entries[entry_index];
    }
    return result;
}


// @address 0x229b08 ?breakable_surfaces_initialize@@YAXXZ
void breakable_surfaces_initialize(void)
{
    g_breakable_surface_globals = (char *)g_breakable_surface_globals_allocator.reserve_memory("breakable surface breakable_surface_globals", 0,
        0x3ce18, 0);
    g_breakable_surface_event_data = data_new("breakable surface set broken events", 0x40, 0x464, 0, (c_allocation_base
        *)&g_breakable_surface_events_allocator);
    collisions_breakable_surface_globals *globals = (collisions_breakable_surface_globals *)g_breakable_surface_globals;
    globals->active = 1;
    globals->reset_pending = 0;
}

// @address 0x229ba8 breakable_surfaces_dispose
void breakable_surfaces_dispose(void)
{
    data_dispose(g_breakable_surface_event_data);
    g_breakable_surface_event_data = 0;
}


// @address 0x229bf0 breakable_surfaces_initialize_for_new_map
void breakable_surfaces_initialize_for_new_map(void)
{
    s_data_array *data = g_breakable_surface_event_data;
    data->valid = 1;
    data_delete_all(data);
    breakable_surfaces_reset();
    data_delete_all(g_breakable_surface_event_data);
    collisions_breakable_surface_globals *globals = (collisions_breakable_surface_globals *)g_breakable_surface_globals;
    globals->next_set_index = 0;
    globals->slot_mask = 0;
}

// @address 0x229c58 breakable_surfaces_dispose_from_old_map
void breakable_surfaces_dispose_from_old_map(void)
{
    g_breakable_surface_event_data->valid = 0;
}

// 0x229c70
static __forceinline long breakable_surfaces_bsp_surface_set_count(long slot_index)
{
    structure_bsp *definition =
        (structure_bsp *)tag_get('sbsp', ((scenario_structure_bsp_reference
            *)global_scenario->structure_bsps.address)[slot_index].structure_bsp.index);
    return definition->breakable_surfaces.count;
}

// @address 0x229c70 ?breakable_surfaces_initialize_for_new_structure_bsp@@YAXK@Z
void breakable_surfaces_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask)
{
    for (long slot_index = 0; slot_index < 16; slot_index++) {
        if ((structure_bsp_mask & (1 << slot_index)) && !((1 << slot_index) & ((collisions_breakable_surface_globals
            *)g_breakable_surface_globals)->slot_mask)) {
            long count = breakable_surfaces_bsp_surface_set_count(slot_index);
            if (count == 0) {
                ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->default_entry_indices[slot_index] = -1;
            } else {
                ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->default_entry_indices[slot_index] =
                    (short)((collisions_breakable_surface_globals *)g_breakable_surface_globals)->next_set_index;
                ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->next_set_index += count;
            }
            for (long variant_index = 0; variant_index < 4096; variant_index++) {
                long count = breakable_surface_get_surface_set_count(slot_index, variant_index);
                if (count == 0) {
                    ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->entry_indices[slot_index][variant_index] = -1;
                } else {
                    ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->entry_indices[slot_index][variant_index] =
                        (short)((collisions_breakable_surface_globals *)g_breakable_surface_globals)->next_set_index;
                    ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->next_set_index += count;
                }
            }
            ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->slot_mask |= 1 << slot_index;
        }
    }
}


namespace {


static __forceinline void breakable_surfaces_set_visited(unsigned long *flags, long index, bool value)
{
    unsigned long mask = 1UL << (index & 31);
    long word_index = index >> 5;
    if (value)
        flags[word_index] |= mask;
    else
        flags[word_index] &= ~mask;
}
static __forceinline bool breakable_surfaces_test_visited(const unsigned long *flags, long index)
{
    unsigned long mask = 1UL << (index & 31);
    return (mask & flags[index >> 5]) != 0;
}
}

static __forceinline c_breakable_surfaces_static_flags<256> *breakable_surfaces_get_surface_flags(long slot_index, long variant_index, long set_index)
{
    structure_bsp *definition = (structure_bsp *)global_tag_instances[
        datum_index_to_absolute_index(((scenario_structure_bsp_reference
            *)global_scenario->structure_bsps.address)[slot_index].structure_bsp.index)].definition;
    if (variant_index == -1)
        return &((c_breakable_surfaces_static_flags<256> *)definition->breakable_surfaces.address)[set_index];
    structure_instanced_geometry_definition *instances = (structure_instanced_geometry_definition
        *)g_collision_bsp_cache.users[get_current_thread_index()].entries[slot_index].data;
    return &((c_breakable_surfaces_static_flags<256> *)instances[((structure_instanced_geometry_instance
        *)definition->instanced_geometry_instances.address)[variant_index].definition_index].breakable_surfaces.address)[set_index];
}

// @address 0x22a230 breakable_surfaces_collect_connected_surfaces
extern "C" void breakable_surfaces_collect_connected_surfaces(long slot_index, long variant_index,
    long set_index, long surface_index, collision_bsp *bsp, long *surface_count, long *surface_indices)
{
    c_breakable_surfaces_static_flags<256> *set_flags = breakable_surfaces_get_surface_flags(slot_index, variant_index, set_index);
    s_breakable_surfaces_traversal_stack stack;
    c_breakable_surfaces_static_flags<32768> visited;
    long collision_surface_count = bsp->surfaces.count;
    *surface_count = 0;
    memset(&visited, 0, ((collision_surface_count + 31) >> 5) * sizeof(unsigned long));
    stack.count = 0;
    stack.push().surface_index = (short)surface_index;
    stack.top().root = true;
    for (;;) {
        stack.top().surface = &((collision_surface *)bsp->surfaces.address)[stack.top().surface_index];
        stack.top().edge_index = stack.top().surface->first_edge;
        bool extant = (set_flags->bits[stack.top().surface->breakable_surface_bit >> 5] & (1UL << (stack.top().surface->breakable_surface_bit &
        31))) != 0;
        visited.bits[stack.top().surface_index >> 5] |= 1UL << (stack.top().surface_index & 31);
        if (extant) {
            if (*surface_count < 1024) {
                long index = (*surface_count)++;
                surface_indices[index] = stack.top().surface_index;
            }
        }
        for (;;) {
            stack.top().edge = &((collision_edge *)bsp->edges.address)[stack.top().edge_index];
            stack.top().side = stack.top().edge->surfaces[1] == stack.top().surface_index;
            stack.top().adjacent_surface_index = stack.top().edge->surfaces[!stack.top().side];
            if (!breakable_surfaces_test_visited(visited.bits, stack.top().adjacent_surface_index)) {
                stack.top().adjacent_surface = &((collision_surface *)bsp->surfaces.address)[stack.top().adjacent_surface_index];
                if (stack.top().adjacent_surface->breakable_surface == set_index) {
                    stack.push_surface(stack.top().adjacent_surface_index, false);
                    break;
                }
            }
            stack.top().edge_index = stack.top().edge->edges[stack.top().side];
            while (stack.top().edge_index == stack.top().surface->first_edge) {
                if (stack.top().root)
                return;
                stack.pop();
                stack.top().edge_index = stack.top().edge->edges[stack.top().side];
            }
        }
    }
}


// @address 0x22a6b0 breakable_surfaces_mark_connected_surfaces
extern "C" void breakable_surfaces_mark_connected_surfaces(s_breakable_surface_set *set, long set_index, long surface_index,
    collision_bsp *bsp, unsigned long *visited)
{
    s_breakable_surfaces_traversal_stack stack;
    stack.count = 0;
    stack.push().surface_index = (short)surface_index;
    stack.top().root = true;
    for (;;) {
        stack.top().surface = &((collision_surface *)bsp->surfaces.address)[stack.top().surface_index];
        stack.top().edge_index = stack.top().surface->first_edge;
        breakable_surfaces_set_visited(visited, stack.top().surface_index, true);
        for (;;) {
            stack.top().edge = &((collision_edge *)bsp->edges.address)[stack.top().edge_index];
            stack.top().side = stack.top().edge->surfaces[1] == stack.top().surface_index;
            stack.top().adjacent_surface_index = stack.top().edge->surfaces[!stack.top().side];
            if (!breakable_surfaces_test_visited(visited, stack.top().adjacent_surface_index)) {
                stack.top().adjacent_surface = &((collision_surface *)bsp->surfaces.address)[stack.top().adjacent_surface_index];
                if (stack.top().adjacent_surface->breakable_surface == set_index &&
                set->surface_flags.test(stack.top().adjacent_surface->breakable_surface_bit)) {
                    stack.push_surface(stack.top().adjacent_surface_index, false);
                    break;
                }
            }
            stack.top().edge_index = stack.top().edge->edges[stack.top().side];
            while (stack.top().edge_index == stack.top().surface->first_edge) {
                if (stack.top().root)
                return;
                stack.pop();
                stack.top().edge_index = stack.top().edge->edges[stack.top().side];
            }
        }
    }
}

// 0x22b590
void breakable_surfaces_reset(void)
{
    ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->entries.set_all(s_breakable_surface_set());
    ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->oldest_slot = -1;
    ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->next_slot = 0;
}

// 0x22b628
// @address 0x22b628 breakable_surface_is_valid_set
unsigned char breakable_surface_is_valid_set(long slot_index, long variant_index, long surface_a, long surface_b, long set_index)
{
    const collision_bsp *table = 0;
    bool result = false;
    if (variant_index == -1) {
        table = g_collision_bsp_cache.thread_users[get_current_thread_index()].collision_bsps[slot_index];
    } else {
        structure_bsp *definition =
            (structure_bsp *)
                global_tag_instances[datum_index_to_absolute_index(((scenario_structure_bsp_reference
                    *)global_scenario->structure_bsps.address)[slot_index].structure_bsp.index)]
                    .definition;
        if (variant_index >= 0 && variant_index < definition->instanced_geometry_instances.count)
            table = &((structure_instanced_geometry_definition *)g_collision_bsp_cache.users[get_current_thread_index()]
                    .entries[slot_index]
                    .data)[((structure_instanced_geometry_instance
                        *)definition->instanced_geometry_instances.address)[variant_index].definition_index]
                         .bsp;
    }
    if (table != 0 && (set_index >= 0 && set_index < table->surfaces.count)) {
        collision_surface *set = &((collision_surface *)table->surfaces.address)[set_index];
        if ((set->flags & 8) && set->breakable_surface == surface_a && set->breakable_surface_bit == surface_b)
            result = true;
    }
    return result;
}

static __forceinline s_breakable_surface_set *breakable_surface_get_set(long slot_index, long variant_index, long set_index)
{
    s_breakable_surface_set *first = ::breakable_surface_get_first_set(slot_index, variant_index);
    return first ? &first[set_index] : 0;
}

// 0x22b760
bool breakable_surface_extant(long slot_index, long variant_index, long set_index, long surface_index)
{
    s_breakable_surface_set *set = breakable_surface_get_set(slot_index, variant_index, set_index);
    return set && global_structure_bsp_is_active(slot_index) && set->surface_flags.test(surface_index);
}

// 0x22b810
// @address 0x22b810 breakable_surface_mark_broken
void breakable_surface_mark_broken(long slot_index, long variant_index, long set_index, long surface_index, long unused)
{
    collisions_breakable_surface_globals *globals = (collisions_breakable_surface_globals *)g_breakable_surface_globals;
    if (globals->active) {
        s_breakable_surface_set *set = breakable_surface_get_set(slot_index, variant_index, set_index);
        if (set) {
            unsigned long *flags = set->surface_flags.get_writeable_bits_direct();
            flags[surface_index >> 5] &= ~(1UL << (surface_index & 31));
            globals->dirty = 1;
        }
    }
}


static __forceinline s_breakable_surfaces_broken_event *breakable_surfaces_event_get(long index)
{
    return &((s_breakable_surfaces_broken_event *)g_breakable_surface_event_data->data)[datum_index_to_absolute_index(index)];
}
// 0x229d98
void breakable_surfaces_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask)
{
    if (((collisions_breakable_surface_globals *)g_breakable_surface_globals)->reset_pending) {
        breakable_surfaces_initialize_for_new_map();
        ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->reset_pending = 0;
    }
    s_breakable_surfaces_dispose_iterator iterator;
    iterator.iterator.absolute_index = -1;
    iterator.iterator.index = -1;
    iterator.iterator.data = g_breakable_surface_event_data;
    iterator.event = (s_breakable_surfaces_broken_event *)data_iterator_next(&iterator.iterator);
    while (iterator.event != 0) {
        if (((1 << iterator.event->bsp_index) & structure_bsp_mask) != 0) {
            s_breakable_surface_set *entry = ::breakable_surface_get_first_set(iterator.event->bsp_index, iterator.event->variant_index);
            if (entry)
                entry = &entry[iterator.event->set_index];
            ::datum_delete(g_breakable_surface_event_data, iterator.iterator.index);
            entry->next_index = -1;
        }
        iterator.event = (s_breakable_surfaces_broken_event *)data_iterator_next(&iterator.iterator);
    }
}
// @address 0x22b888 breakable_surfaces_new_broken_event
extern "C" void breakable_surfaces_new_broken_event(const real_point3d *position, const real_vector3d *direction,
    const s_breakable_surfaces_event_material *material, long bsp_index, long variant_index,
    long set_index, long surface_index, long cause)
{
    s_breakable_surface_set *set = breakable_surface_get_set(bsp_index, variant_index, set_index);
    if (set) {
        long previous = set->next_index;
        if (previous == -1 || breakable_surfaces_event_get(previous)->time != game_time_globals->time) {
            if (g_breakable_surface_event_data->actual_count < 64) {
                structure_bsp *definition = (structure_bsp *)global_tag_instances[
                    datum_index_to_absolute_index(((scenario_structure_bsp_reference
                        *)global_scenario->structure_bsps.address)[bsp_index].structure_bsp.index)].definition;
                long event_index = datum_new(g_breakable_surface_event_data);
                set->next_index = event_index;
                s_breakable_surfaces_broken_event *event = breakable_surfaces_event_get(event_index);
                unsigned long *flags = event->surface_bits;
                unsigned long mask = 1 << (surface_index & 31);
                event->cause = cause;
                event->material = *material;
                event->previous_event = previous;
                event->bsp_index = (char)bsp_index;
                event->variant_index = (short)variant_index;
                event->set_index = set_index;
                event->time = game_time_globals->time;
                flags[surface_index >> 5] |= mask;
                if (variant_index != -1) {
                    const real_matrix4x3 *matrix = &((structure_instanced_geometry_instance
                        *)definition->instanced_geometry_instances.address)[variant_index].transform;
                    matrix4x3_inverse_transform_point(matrix, position, &event->position);
                    matrix4x3_inverse_transform_vector(matrix, direction, &event->direction);
                } else {
                    event->position = *position;
                    event->direction = *direction;
                }
                normalize3d(&event->direction);
            }
        }
        if (set->next_index != -1) {
            unsigned long *flags = breakable_surfaces_event_get(set->next_index)->surface_bits;
            flags[surface_index >> 5] |= 1 << (surface_index & 31);
        }
    }
}

// @address 0x22baa0 breakable_surface_break_from_record
extern "C" void breakable_surface_break_from_record(long bsp_index, long instance_index, long surface_index, const real_point3d *position,
    const s_breakable_surfaces_event_material *material)
{
    const collision_bsp *table;
    if (instance_index == -1) {
        table = g_collision_bsp_cache.thread_users[get_current_thread_index()].collision_bsps[bsp_index];
    } else {
        structure_bsp *definition =
            (structure_bsp *)
                global_tag_instances[datum_index_to_absolute_index(((scenario_structure_bsp_reference
                    *)global_scenario->structure_bsps.address)[bsp_index].structure_bsp.index)]
                    .definition;
        table = &((structure_instanced_geometry_definition *)g_collision_bsp_cache.users[get_current_thread_index()]
                .entries[bsp_index]
                .data)[((structure_instanced_geometry_instance *)definition->instanced_geometry_instances.address)[instance_index].definition_index]
                     .bsp;
    }
    collision_surface *record = &((collision_surface *)table->surfaces.address)[surface_index];
    if (((collisions_breakable_surface_globals *)g_breakable_surface_globals)->active) {
        breakable_surfaces_new_broken_event(position, 0, material, bsp_index, instance_index, record->breakable_surface,
            record->breakable_surface_bit, surface_index);
    }
}

// 0x22bbd0
// @address 0x22bbd0 breakable_surfaces_is_shortest_edge
unsigned char breakable_surfaces_is_shortest_edge(collision_bsp *bsp, long surface_index, long edge_index)
{
    long nearest_edge = -1;
    float nearest_distance_squared = 3.4028234663852886e+38f;
    long first_edge = ((collision_surface *)bsp->surfaces.address)[surface_index].first_edge;
    long current_edge = first_edge;
    do {
        collision_edge *edge = &((collision_edge *)bsp->edges.address)[current_edge];
        real_point3d *start = &((collision_vertex *)bsp->vertices.address)[edge->vertices[0]].point;
        real_point3d *end = &((collision_vertex *)bsp->vertices.address)[edge->vertices[1]].point;
        float dx = end->x - start->x;
        float dy = end->y - start->y;
        float dz = end->z - start->z;
        float distance_squared = dx * dx;
        distance_squared += dz * dz;
        distance_squared += dy * dy;
        bool side = edge->surfaces[1] == surface_index;
        if (!(distance_squared >= nearest_distance_squared)) {
            nearest_edge = current_edge;
            nearest_distance_squared = distance_squared;
        }
        current_edge = edge->edges[side];
    } while (current_edge != first_edge);
    return edge_index == nearest_edge;
}

// 0x22bcb8
// @address 0x22bcb8 breakable_surfaces_new_slot
long breakable_surfaces_new_slot(void)
{
    long slot_index = ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->next_slot;
    if (((collisions_breakable_surface_globals *)g_breakable_surface_globals)->oldest_slot == -1) {
        ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->next_slot = 1;
        ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->oldest_slot = 0;
        return slot_index;
    }
    if (slot_index != ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->oldest_slot) {
        ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->next_slot = (slot_index + 1) % 127;
        return slot_index;
    }
    collisions_breakable_surface_slot *slot = &((collisions_breakable_surface_globals *)g_breakable_surface_globals)->slots[slot_index];
    s_breakable_surface_set *entry = ::breakable_surface_get_first_set(slot->slot_index, slot->variant_index);
    if (entry)
        entry += slot->set_index;
    for (long index = 0; index < 16; index++) {
        if (entry->material_indices[index] == slot_index)
            entry->material_indices[index] = -1;
    }
    ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->next_slot = (((collisions_breakable_surface_globals
        *)g_breakable_surface_globals)->next_slot + 1) % 127;
    ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->oldest_slot = ((collisions_breakable_surface_globals
        *)g_breakable_surface_globals)->next_slot;
    return slot_index;
}


// @address 0x22cc80 breakable_surface_get_bsp
real_matrix4x3 *breakable_surface_get_bsp(long object_index, long node_index, const void **node_out)
{
    structure_bsp *definition =
        (structure_bsp *)
            global_tag_instances[datum_index_to_absolute_index(((scenario_structure_bsp_reference
                *)global_scenario->structure_bsps.address)[object_index].structure_bsp.index)]
                .definition;
    if (node_index != -1) {
        structure_instanced_geometry_instance *node = &((structure_instanced_geometry_instance
            *)definition->instanced_geometry_instances.address)[node_index];
        structure_instanced_geometry_definition *block = &((structure_instanced_geometry_definition
            *)g_collision_bsp_cache.users[get_current_thread_index()].entries[object_index].data)[node->definition_index];
        if (node_out)
            *node_out = node;
        return (real_matrix4x3 *)&block->bsp;
    }
    if (node_out)
        *node_out = global_identity4x3;
    s_structure_bsp_resource *resource = &definition->resource;
    s_structure_bsp_resources *reference;
    if (resource->use_resource_items)
        reference = resource->collision_resource_interface.get_resources();
    else
        reference = (s_structure_bsp_resources *)resource->raw_resources.address;
    return (real_matrix4x3 *)reference->collision_bsp;
}

static __forceinline void breakable_surfaces_get_plane(const collision_bsp *bsp, long plane_index, real_plane3d *plane);
static __forceinline float breakable_surfaces_plane_distance(const real_plane3d *plane, const real_point3d *point)
{
    return point->x * plane->n.i + (point->y * plane->n.j + point->z * plane->n.k) - plane->d;
}
// @address 0x22cd88 breakable_surface_break_surface
extern "C" void breakable_surface_break_surface(long bsp_index, long variant_index, long set_index, long surface_index, long record_index,
    const real_point3d *position, const s_breakable_surfaces_event_material *material, const real_vector3d *direction)
{
    if (breakable_surface_extant(bsp_index, variant_index, set_index, surface_index)) {
        s_breakable_surface_set *entry = ::breakable_surface_get_first_set(bsp_index, variant_index);
        if (entry) entry += set_index;
        real_vector3d normal;
        if (!direction) {
            const void *node;
            const collision_bsp *bsp = (const collision_bsp *)breakable_surface_get_bsp(bsp_index, variant_index, &node);
            real_plane3d plane;
            breakable_surfaces_get_plane(bsp, ((collision_surface *)bsp->surfaces.address)[record_index].plane, &plane);
            float distance = breakable_surfaces_plane_distance(&plane, position);
            if (distance == 0.0f) direction = &global_identity4x3->up;
            else {
                long side = distance < 0.0f ? -1 : 1;
                scale_vector3d(&plane.n, (float)-side, &normal);
                direction = &normal;
            }
        }
        breakable_surface_mark_broken(bsp_index, variant_index, set_index, surface_index, 0);
        breakable_surfaces_new_broken_event(position, direction, material, bsp_index, variant_index, set_index, surface_index, record_index);
        entry->active = true;
    }
}
// 0x22cf40
static __forceinline float breakable_surfaces_distance(const real_point3d *a, const real_point3d *b)
{
    float dx = b->x - a->x;
    float dy = b->y - a->y;
    float dz = b->z - a->z;
    return sqrtf(dx * dx + dy * dy + dz * dz);
}

// @address 0x22cf40 breakable_surfaces_compute_surface_area
float breakable_surfaces_compute_surface_area(collision_bsp *bsp, long surface_index, float scale)
{
    const real_point3d *points[8];
    long point_count = 0;
    float area = 0.0f;
    long first_edge = ((collision_surface *)bsp->surfaces.address)[surface_index].first_edge;
    long current_edge = first_edge;
    do {
        collision_edge *edge = &((collision_edge *)bsp->edges.address)[current_edge];
        bool side = edge->surfaces[1] == surface_index;
        points[point_count++] = &((collision_vertex *)bsp->vertices.address)[edge->vertices[side]].point;
        current_edge = edge->edges[side];
    } while (current_edge != first_edge);
    for (long index = 0; index < point_count - 2; index++) {
        float c = breakable_surfaces_distance(points[0], points[index + 1]) * scale;
        float a = breakable_surfaces_distance(points[index + 1], points[index + 2]) * scale;
        float b = breakable_surfaces_distance(points[0], points[index + 2]) * scale;
        float half_perimeter = (b + a + c) * 0.5f;
        area += sqrtf((half_perimeter - b) * (half_perimeter - a) * (half_perimeter - c) * half_perimeter);
    }
    return area;
}

extern "C" {
}


// 0x22d0e0
template <>
void *t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void
    *)>(0),__tls_set_g_breakable_surface_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment)
{
    m_member_index = restricted_region_register_member(3, name, type, size, 0, __tls_set_g_breakable_surface_globals_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

extern "C" {
}

// 0x22d180
template <>
void *t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void
    *)>(0),__tls_set_g_breakable_surface_globals_allocator>::allocate(
    unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x22d198
template <>
void t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void
    *)>(0),__tls_set_g_breakable_surface_globals_allocator>::deallocate(void *memory)
{
    long value = m_member_index;
    g_breakable_surface_globals = 0;
    restricted_region_free_member(3, value);
}


// 28 bytes, same layout as collisions_breakable_surface_slot


// @address 0x22d278 breakable_surface_hash_insert
bool breakable_surface_hash_insert(s_breakable_surface_hash *hash, long first_hash, long second_hash, unsigned short key, long value)
{
    bool result = false;
    if (hash->count < 4096) {
        short bucket = (short)(hash->mask & first_hash);
        short head = hash->buckets[bucket];
        long index = hash->count;
        hash->count++;
        hash->buckets[bucket] = (short)index;
        hash->data[index].key = key;
        hash->data[index].value = value;
        hash->data[index].next = head;
        result = true;
    }
    return result;
}

// @address 0x22bde8 breakable_surfaces_new_crack
void breakable_surfaces_new_crack(const real_point3d *position, long slot_index, long variant_index, long record_index)
{
    const void *node;
    real_matrix4x3 *matrix = breakable_surface_get_bsp(slot_index, variant_index, &node);
    collision_surface *record = &((collision_surface *)((collision_bsp *)matrix)->surfaces.address)[record_index];
    long free_material_index = 0;
    s_breakable_surface_set *set = breakable_surface_get_set(slot_index, variant_index, record->breakable_surface);
    if (set != 0) {
        free_material_index = -1;
        for (long material_index = 0; material_index < 16; material_index++) {
            if ((unsigned char)set->material_indices[material_index] == 255)
                free_material_index = material_index;
        }
        if (free_material_index != -1) {
            collisions_breakable_surface_globals *globals = (collisions_breakable_surface_globals *)g_breakable_surface_globals;
            collisions_breakable_surface_slot *slot = &globals->slots[globals->next_slot];
            long new_slot_index = breakable_surfaces_new_slot();
            set->material_indices[free_material_index] = (char)new_slot_index;
            slot->time = game_time_globals->time;
            slot->surface_b = record->breakable_surface_bit;
            short set_index_value = record->breakable_surface;
            slot->slot_index = slot_index;
            slot->variant_index = variant_index;
            slot->record_index = record_index;
            slot->set_index = set_index_value;
            slot->position = *position;
        }
    }
}


static __forceinline c_additive_hash<1024, 4096, 1, 0>::data_t *breakable_surfaces_hash_find(s_breakable_surface_hash *hash, long first_hash,
    unsigned short key)
{
    c_additive_hash<1024, 4096, 1, 0>::data_t *found = 0;
    short index = hash->buckets[hash->mask & first_hash];
    while (index != -1) {
        if (hash->data[index].key == key) {
            found = &hash->data[index];
            index = -1;
        } else {
            index = hash->data[index].next;
        }
    }
    return found;
}

static __forceinline void breakable_surfaces_hash_insert(s_breakable_surface_hash *hash, long first_hash, unsigned short key, long value)
{
    if (hash->count < 4096) {
        short bucket = (short)(hash->mask & first_hash);
        short head = hash->buckets[bucket];
        long index = hash->count;
        hash->count++;
        hash->buckets[bucket] = (short)index;
        hash->data[index].key = key;
        hash->data[index].value = value;
        hash->data[index].next = head;
    }
}

// @address 0x229e70 breakable_surfaces_collect_broken_surfaces
void breakable_surfaces_collect_broken_surfaces(s_breakable_surfaces_list *list, real_rectangle3d *bounds, long surface_index,
    long set_index, const unsigned long *flags, float *area, float scale, collision_bsp *bsp)
{
    short queue[4096];
    s_breakable_surface_hash hash;
    long queue_count;
    long queue_head = 0;
    long material_index = ((collision_surface *)bsp->surfaces.address)[surface_index].material;
    hash.mask = 1023;
    memset(hash.buckets, -1, sizeof(hash.buckets));
    hash.count = 0;
    *area = breakable_surfaces_compute_surface_area(bsp, surface_index, scale);
    *bounds = k_null_rectangle3d_storage;
    long list_count = list->count++;
    queue[0] = (short)surface_index;
    list->indices[list_count] = (short)surface_index;
    queue_count = 1;
    breakable_surface_hash_insert(&hash, surface_index, 0, (unsigned short)surface_index, surface_index);
    do {
        long current = queue[queue_head++];
        collision_surface *surface = &((collision_surface *)bsp->surfaces.address)[current];
        long edge_index = surface->first_edge;
        do {
            collision_edge *edge = &((collision_edge *)bsp->edges.address)[edge_index];
            long side = edge->surfaces[1] == current;
            const real_point3d *point = &((collision_vertex *)bsp->vertices.address)[edge->vertices[side]].point;
            long adjacent = edge->surfaces[!side];
            short key = (short)adjacent;
            if (!breakable_surfaces_hash_find(&hash, key, (unsigned short)key)) {
                if (adjacent != -1) {
                    collision_surface *adjacent_surface = &((collision_surface *)bsp->surfaces.address)[adjacent];
                    if (adjacent_surface->breakable_surface == set_index && adjacent_surface->material == material_index) {
                        if ((flags[adjacent_surface->breakable_surface_bit >> 5] & (1UL << (adjacent_surface->breakable_surface_bit & 31))) != 0) {
                            if (list->count != 512) {
                                long count = list->count++;
                                list->indices[count] = key;
                                *area += breakable_surfaces_compute_surface_area(bsp, adjacent, scale);
                            }
                        }
                        if (queue_count >= 4096)
                            break;
                        breakable_surfaces_hash_insert(&hash, adjacent, (unsigned short)key, adjacent);
                        queue[queue_count++] = key;
                    }
                }
            }
            if (bounds->x0 > point->x)
                bounds->x0 = point->x;
            if (bounds->x1 < point->x)
                bounds->x1 = point->x;
            if (bounds->y0 > point->y)
                bounds->y0 = point->y;
            if (bounds->y1 < point->y)
                bounds->y1 = point->y;
            if (bounds->z0 > point->z)
                bounds->z0 = point->z;
            if (bounds->z1 < point->z)
                bounds->z1 = point->z;
            edge_index = edge->edges[side];
        } while (edge_index != surface->first_edge);
    } while (queue_head < queue_count);
}

extern "C" {
}

static __forceinline s_breakable_surfaces_broken_event *breakable_surfaces_event_try_and_get(long index)
{
    s_data_array *data = g_breakable_surface_event_data;
    if (index != -1) {
        long absolute_index = datum_index_to_absolute_index(index);
        if (absolute_index < data->first_unallocated) {
            s_breakable_surfaces_broken_event *event = (s_breakable_surfaces_broken_event *)(data->data + data->size * absolute_index);
            long identifier = event->identifier;
            if (identifier != 0 && identifier == (index >> 16))
                return event;
        }
    }
    return 0;
}

static __forceinline unsigned short breakable_surfaces_random_word()
{
    unsigned long *seed = get_local_random_seed_address();
    random_seed_next(seed);
    return (unsigned short)(*seed >> 16);
}

static __forceinline collision_bsp *breakable_surfaces_get_bsp(long slot_index, long variant_index)
{
    if (variant_index == -1)
        return (collision_bsp *)g_collision_bsp_cache.thread_users[get_current_thread_index()].collision_bsps[slot_index];
    structure_bsp *definition =
        (structure_bsp *)
            global_tag_instances[datum_index_to_absolute_index(((scenario_structure_bsp_reference
                *)global_scenario->structure_bsps.address)[slot_index].structure_bsp.index)]
                .definition;
    return (collision_bsp *)&((structure_instanced_geometry_definition *)g_collision_bsp_cache.users[get_current_thread_index()]
            .entries[slot_index]
            .data)[((structure_instanced_geometry_instance *)definition->instanced_geometry_instances.address)[variant_index].definition_index]
        .bsp;
}

static __forceinline void breakable_surfaces_get_plane(const collision_bsp *bsp, long plane_index, real_plane3d *plane)
{
    const real_plane3d *source = &((real_plane3d *)bsp->planes.address)[plane_index & 0x7fff];
    if (plane_index & 0x8000) {
        plane->n.i = -source->n.i;
        plane->n.j = -source->n.j;
        plane->n.k = -source->n.k;
        plane->d = -source->d;
    } else {
        *plane = *source;
    }
}

// @address 0x22b078 breakable_surfaces_get_particle_emission
void breakable_surfaces_get_particle_emission(long event_index, real_point3d *point, real_vector3d *direction)
{
    s_breakable_surfaces_broken_event *event = breakable_surfaces_event_try_and_get(event_index);
    if (event) {
        s_breakable_surface_set *set = breakable_surface_get_first_set(event->bsp_index, event->variant_index);
        if (set)
            set = &set[event->set_index];
        if (set) {
            long surface_index = event->surface_list.indices[breakable_surfaces_random_word() % event->surface_list.count];
            collision_bsp *bsp;
            if (event->variant_index == -1) {
                bsp = (collision_bsp *)g_collision_bsp_cache.thread_users[get_current_thread_index()].collision_bsps[event->bsp_index];
            } else {
                structure_bsp *definition =
                    (structure_bsp *)
                        global_tag_instances[datum_index_to_absolute_index(((scenario_structure_bsp_reference
                            *)global_scenario->structure_bsps.address)[event->bsp_index].structure_bsp.index)]
                            .definition;
                bsp = (collision_bsp *)&((structure_instanced_geometry_definition *)g_collision_bsp_cache.users[get_current_thread_index()]
                        .entries[event->bsp_index]
                        .data)[((structure_instanced_geometry_instance
                            *)definition->instanced_geometry_instances.address)[event->variant_index].definition_index]
                          .bsp;
            }
            collision_surface *surface = &((collision_surface *)bsp->surfaces.address)[surface_index];
            long first_edge = surface->first_edge;
            long edge_index = first_edge;
            real_rectangle3d bounds = k_null_rectangle3d_storage;
            real_plane3d plane;
            breakable_surfaces_get_plane(bsp, surface->plane, &plane);
            do {
                collision_edge *edge = &((collision_edge *)bsp->edges.address)[edge_index];
                long side = edge->surfaces[1] == surface_index;
                real_rectangle3d_enclose_point(&bounds, &((collision_vertex
                    *)bsp->vertices.address)[edge->vertices[side]].point);
                edge_index = edge->edges[side];
            } while (edge_index != first_edge);
            random_seed_next(get_local_random_seed_address());
            float random_fraction = (float)(unsigned short)(*get_local_random_seed_address() >> 16) * 1.5259022e-05f;
            point->x = interpolate_linear(bounds.x0, bounds.x1, random_fraction);
            random_seed_next(get_local_random_seed_address());
            random_fraction = (float)(unsigned short)(*get_local_random_seed_address() >> 16) * 1.5259022e-05f;
            point->y = interpolate_linear(bounds.y0, bounds.y1, random_fraction);
            random_seed_next(get_local_random_seed_address());
            random_fraction = (float)(unsigned short)(*get_local_random_seed_address() >> 16) * 1.5259022e-05f;
            point->z = interpolate_linear(bounds.z0, bounds.z1, random_fraction);
            float magnitude = plane.n.j * plane.n.j + (plane.n.k * plane.n.k + plane.n.i * plane.n.i);
            float t;
            if (magnitude != 0.0f) {
                const float &z = +point->z;
                t = (point->y * plane.n.j + (point->x * plane.n.i + z * plane.n.k) - plane.d) * (-1.0f / magnitude);
            } else {
                t = 0.0f;
            }
            point_from_line3d(point, &plane.n, t, point);
            if (event->direction.i * event->direction.i + event->direction.j * event->direction.j + event->direction.k * event->direction.k != 0.0f) {
                *direction = event->direction;
            } else {
                direction->i = point->x - event->position.x;
                direction->j = point->y - event->position.y;
                direction->k = point->z - event->position.z;
                fast_normalize3d(direction);
            }
            random_seed_next(get_local_random_seed_address());
            random_fraction = (float)(unsigned short)(*get_local_random_seed_address() >> 16) * 1.5259022e-05f;
            direction->i = random_fraction * 0.2f - 0.1f + direction->i;
            random_seed_next(get_local_random_seed_address());
            random_fraction = (float)(unsigned short)(*get_local_random_seed_address() >> 16) * 1.5259022e-05f;
            direction->j = random_fraction * 0.2f - 0.1f + direction->j;
            random_seed_next(get_local_random_seed_address());
            random_fraction = (float)(unsigned short)(*get_local_random_seed_address() >> 16) * 1.5259022e-05f;
            direction->k = random_fraction * 0.2f - 0.1f + direction->k;
        }
    }
}


// @address 0x22aa38 breakable_surfaces_break_disconnected
void breakable_surfaces_break_disconnected(long bsp_index, long variant_index, long set_index, long surface_index, collision_bsp *bsp,
    const unsigned long *flags, const real_point3d *position, const s_breakable_surfaces_event_material *material)
{
    s_breakable_surfaces_traversal_stack stack;
    c_breakable_surfaces_static_flags<32768> visited;
    memset(&visited, 0, ((bsp->surfaces.count + 31) >> 5) * sizeof(unsigned long));
    stack.count = 0;
    stack.push().surface_index = (short)surface_index;
    stack.top().root = true;
    for (;;) {
        stack.top().surface = &((collision_surface *)bsp->surfaces.address)[stack.top().surface_index];
        stack.top().edge_index = stack.top().surface->first_edge;
        bool extant = (flags[stack.top().surface_index >> 5] & (1UL << (stack.top().surface_index & 31))) != 0;
        visited.bits[stack.top().surface_index >> 5] = (1UL << (stack.top().surface_index & 31)) | visited.bits[stack.top().surface_index >> 5];
        if (!extant)
        breakable_surface_break_surface(
        bsp_index, variant_index, set_index, stack.top().surface->breakable_surface_bit, stack.top().surface_index, position, material, 0);
        for (;;) {
            stack.top().edge = &((collision_edge *)bsp->edges.address)[stack.top().edge_index];
            stack.top().side = stack.top().edge->surfaces[1] == stack.top().surface_index;
            stack.top().adjacent_surface_index = stack.top().edge->surfaces[!stack.top().side];
            if (!(visited.bits[stack.top().adjacent_surface_index >> 5] & (1UL << (stack.top().adjacent_surface_index & 31)))) {
                visited.bits[stack.top().adjacent_surface_index >> 5] |= 1UL << (stack.top().adjacent_surface_index & 31);
                stack.top().adjacent_surface = &((collision_surface *)bsp->surfaces.address)[stack.top().adjacent_surface_index];
                if (stack.top().adjacent_surface->breakable_surface == set_index) {
                    stack.push_surface(stack.top().adjacent_surface_index, false);
                    break;
                }
            }
            stack.top().edge_index = stack.top().edge->edges[stack.top().side];
            while (stack.top().edge_index == stack.top().surface->first_edge) {
                if (stack.top().root)
                return;
                stack.pop();
                stack.top().edge_index = stack.top().edge->edges[stack.top().side];
            }
        }
    }
}

// @address 0x22ae28 breakable_surfaces_propagate_break
void breakable_surfaces_propagate_break(long bsp_index, long variant_index, long record_index, const real_point3d *position,
    const s_breakable_surfaces_event_material *material)
{
    long surface_count;
    c_breakable_surfaces_static_flags<32768> visited;
    long surface_indices[1024];
    collision_bsp *bsp = (collision_bsp *)breakable_surface_get_bsp(bsp_index, variant_index, 0);
    collision_surface *surface = &((collision_surface *)bsp->surfaces.address)[record_index];
    s_breakable_surface_set *set = ::breakable_surface_get_first_set(bsp_index, variant_index);
    if (set)
        set = &set[surface->breakable_surface];
    memset(&visited, 0, ((bsp->surfaces.count + 31) >> 5) * sizeof(unsigned long));
    breakable_surfaces_collect_connected_surfaces(bsp_index, variant_index, surface->breakable_surface, record_index, bsp, &surface_count,
        surface_indices);
    for (long index = 0; index < surface_count; index++) {
        long surface_index = surface_indices[index];
        if (!breakable_surfaces_test_visited(visited.bits, surface_index)) {
            if (set->surface_flags.test(((collision_surface *)bsp->surfaces.address)[surface_index].breakable_surface_bit))
                breakable_surfaces_mark_connected_surfaces(set, surface->breakable_surface, surface_index, bsp, visited.bits);
        }
    }
    breakable_surfaces_break_disconnected(bsp_index, variant_index, surface->breakable_surface, record_index, bsp, visited.bits, position, material);
}

// @address 0x22afb0 breakable_surfaces_handle_pending_break
void breakable_surfaces_handle_pending_break(void)
{
    collisions_breakable_surface_globals *globals = (collisions_breakable_surface_globals *)g_breakable_surface_globals;
    long count = globals->next_set_index;
    long index = 0;
    if (index < count) {
        s_breakable_surface_set *entry = &globals->entries[0];
        do {
            if (entry->active && entry->next_index != -1) {
                s_breakable_surfaces_broken_event *event = breakable_surfaces_event_get(entry->next_index);
                breakable_surfaces_propagate_break(event->bsp_index, event->variant_index, event->cause, &event->damage_position, &event->material);
                entry->active = false;
                return;
            }
            entry++;
            index++;
        } while (index < count);
    }
}


static __forceinline long breakable_surfaces_first_active_structure_bsp()
{
    return lowest_bit_set(g_active_structure_bsp_mask);
}
static __forceinline long breakable_surfaces_next_active_structure_bsp(long structure_bsp_index)
{
    return lowest_bit_set(g_active_structure_bsp_mask & ~(unsigned long)((1i64 << (structure_bsp_index + 1)) - 1));
}

// @address 0x22c3c8 breakable_surface_damage_area_of_effect
extern "C" void breakable_surface_damage_area_of_effect(const s_breakable_surfaces_event_material *material, const real_point3d *position, float
    radius)
{
    if (((collisions_breakable_surface_globals *)g_breakable_surface_globals)->active && material->type != 0xff) {
        if (radius > global_lean_pivot_offset)
            radius = global_lean_pivot_offset;
        for (long structure_bsp_index = breakable_surfaces_first_active_structure_bsp(); structure_bsp_index != -1;
             structure_bsp_index = breakable_surfaces_next_active_structure_bsp(structure_bsp_index)) {
            structure_bsp *definition = (structure_bsp *)tag_get('sbsp',
                ((scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[structure_bsp_index].structure_bsp.index);
            s_structure_physics *breakable_surfaces = &definition->structure_physics;
            if (breakable_surfaces->breakable_surfaces_mopp_code_block.count > 0) {
                const phys_bvh_bytecode *code = (const phys_bvh_bytecode *)breakable_surfaces->breakable_surfaces_mopp_code_block.address;
                phys_vec4 center;
                center.set(position->x, position->y, position->z, 0.0f);
                phys_inline_array<phys_bvh_primitive_info, 2048> hits;
                float query_radius = radius > 0.1f ? radius : 0.1f;
                phys_sphere sphere(center, query_radius);
                phys_bvh_sphere_vm_query_sphere(code, sphere, &hits);
                if (hits.getSize() > 0) {
                    for (long hit_index = 0; hit_index < hits.getSize(); hit_index++) {
                        s_breakable_surfaces_mopp_record *record = &((s_breakable_surfaces_mopp_record
                            *)breakable_surfaces->breakable_surface_key_table.address)[hits[hit_index].ID];
                        breakable_surface_break_surface(structure_bsp_index, record->variant_index, record->set_index,
                            record->surface_index, record->record_index, position, material, 0);
                    }
                }
            }
        }
    }
    if (((collisions_breakable_surface_globals *)g_breakable_surface_globals)->dirty) {
        havok_notify_static_geometry_modfied(-1, -1, -1, -1, -1);
        ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->dirty = 0;
    }
}


extern "C" {
// @address 0x81daac g_rasterizer_stencil_mode
c_rasterizer::e_stencil_mode g_rasterizer_stencil_mode = c_rasterizer::_stencil_mode_off;
}

static __forceinline void breakable_surfaces_render_identity(real_matrix4x3 *matrix)
{
    matrix->scale = 1.0f;
    matrix->forward.i = 1.0f; matrix->forward.j = 0.0f; matrix->forward.k = 0.0f;
    matrix->left.i = 0.0f; matrix->left.j = 1.0f; matrix->left.k = 0.0f;
    matrix->up.i = 0.0f; matrix->up.j = 0.0f; matrix->up.k = 1.0f;
    matrix->position.x = 0.0f; matrix->position.y = 0.0f; matrix->position.z = 0.0f;
}




// @rdata 0x82032900
static const float breakable_surfaces_crack_delay = 20.0f;
// @rdata 0x820249c8
static const float breakable_surfaces_crack_two_pi = 6.2831855f;

static __forceinline real_vector3d *breakable_surfaces_render_vector_from_points(const real_point3d *a, const real_point3d *b, real_vector3d *result)
{
    result->i = b->x - a->x;
    result->j = b->y - a->y;
    result->k = b->z - a->z;
    return result;
}

static __forceinline long breakable_surfaces_render_random_range(unsigned long *seed, long lower, long upper)
{
    random_seed_next(seed);
    long value = (long)(*seed >> 16);
    return lower + value % (upper - lower);
}

static __forceinline float breakable_surfaces_render_real_random_range(unsigned long *seed, float lower, float upper)
{
    random_seed_next(seed);
    unsigned long bits = *seed >> 16;
    float unit = (float)(unsigned __int64)bits * 1.5259022e-05f;
    return unit * (upper - lower) + lower;
}

// @address 0x22c618 ?breakable_surface_render_cracks@@YAXJJJ@Z
void breakable_surface_render_cracks(long bsp_index, long variant_index, long set_index)
{
    s_breakable_surface_set *first = ::breakable_surface_get_first_set(bsp_index, variant_index);
    if (!first)
        return;
    s_breakable_surface_set *set = &first[set_index];
    if (!set)
        return;
    const cache_file_tag_instance *tag_instances = global_tag_instances;
    const structure_bsp *definition =
        (const struct structure_bsp *)
            tag_instances[datum_index_to_absolute_index(((scenario_structure_bsp_reference
                *)global_scenario->structure_bsps.address)[bsp_index].structure_bsp.index)]
                .definition;
    const collision_bsp *bsp = (const collision_bsp *)breakable_surface_get_bsp(bsp_index, variant_index, 0);
    const s_global_material_definition *material = 0;
    const s_breakable_surface_definition *crack = 0;
    const bitmap_sequence *sequence = 0;
    if (g_render_method_submit_state_dirty) {
        c_rasterizer::g_current_alpha_blend_mode = static_cast<c_rasterizer::e_alpha_blend_mode>(13);
        c_rasterizer::g_current_separate_alpha_blend_mode = (c_rasterizer::e_separate_alpha_blend_mode)5;
        c_rasterizer::g_current_z_buffer_mode = static_cast<c_rasterizer::e_z_buffer_mode>(8);
        g_rasterizer_stencil_mode = static_cast<c_rasterizer::e_stencil_mode>(17);
        XMemSet(c_rasterizer::x_last_sampler_textures, 0, sizeof(c_rasterizer::x_last_sampler_textures));
        tag_instances = global_tag_instances;
    }
    for (long index = 0; index < 16; index++) {
        long slot_index = (signed char)set->material_indices[index];
        if (slot_index == -1)
            continue;
        unsigned long random = (unsigned long)slot_index * 0x10100;
        const collisions_breakable_surface_slot *slot = (const collisions_breakable_surface_slot *)&((collisions_breakable_surface_globals
            *)g_breakable_surface_globals)->slots[slot_index];
        const collision_surface *surface = &((collision_surface *)bsp->surfaces.address)[slot->record_index];
        float alpha = 1.0f - ((float)(game_time_globals->time - slot->time) * game_time_globals->tick_length - breakable_surfaces_crack_delay) *
            0.06666667f;
        alpha = PIN(alpha, 0.0f, 1.0f);
        real_plane3d plane;
        breakable_surfaces_get_plane(bsp, surface->plane, &plane);
        if (!material) {
            short material_index =
                TAG_BLOCK_GET_ELEMENT(&definition->collision_materials, surface->material, s_collision_material)->global_material.m_index;
            const s_global_material_definition *game_material = 0;
            if (material_index != -1 && material_index >= 0 && material_index < global_game_globals->material_count)
                game_material = &global_game_globals->materials[material_index];
            crack = (const s_breakable_surface_definition
                *)tag_instances[datum_index_to_absolute_index(game_material->breakable_surface.index)].definition;
            if (crack->crack_bitmap.index == -1)
                return;
            const bitmap_group *bitmap = (const bitmap_group *)tag_instances[datum_index_to_absolute_index(crack->hole_bitmap.index)].definition;
            if (bitmap->sequences.count == 0)
                return;
            sequence = (const bitmap_sequence *)bitmap->sequences.address;
            if (sequence->sprites.count == 0)
                return;
            material = game_material;
        }
        {
            rasterizer_vertex_transparent vertices[4];
            long sprite_index = breakable_surfaces_render_random_range(&random, 0, sequence->sprites.count);
            const bitmap_sprite *sprite = &((const bitmap_sprite *)sequence->sprites.address)[sprite_index];
            unsigned long color = ((long)(alpha * 255.0f) << 24) | 0xffffff;
            vertices[0].position.x = -0.2f; vertices[0].position.y = 0.0f; vertices[0].position.z = 0.2f;
            vertices[0].u = sprite->bounds.x1; vertices[0].v = sprite->bounds.x0; vertices[0].color = color;
            vertices[1].position.x = 0.2f; vertices[1].position.y = 0.0f; vertices[1].position.z = 0.2f;
            vertices[1].u = sprite->bounds.x1; vertices[1].v = sprite->bounds.x1; vertices[1].color = color;
            vertices[2].position.x = -0.2f; vertices[2].position.y = 0.0f; vertices[2].position.z = -0.2f;
            vertices[2].u = sprite->bounds.x0; vertices[2].v = sprite->bounds.x0; vertices[2].color = color;
            vertices[3].position.x = 0.2f; vertices[3].position.y = 0.0f; vertices[3].position.z = -0.2f;
            vertices[3].u = sprite->bounds.x0; vertices[3].v = sprite->bounds.x1; vertices[3].color = color;
            c_rasterizer_texture_ref texture;
            const bitmap_data *bitmap_entry;
            bitmap_group_get_xenon_hardware_format(crack->hole_bitmap.index, sprite->bitmap_index, &texture, &bitmap_entry, 0);
            c_rasterizer::set_sampler_texture(0, texture);
            c_rasterizer::set_sampler_address_mode(
                0, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp, c_rasterizer::_sampler_address_clamp);
            if (g_rasterizer_sampler_filter_modes[0] != 2) {
                g_rasterizer_sampler_filter_modes[0] = 2;
                rasterizer_device_set_sampler_filter_modes(c_rasterizer::get_device(), 0, 2, 2);
            }
            real_matrix4x3 node_matrix;
            if (variant_index == -1)
                breakable_surfaces_render_identity(&node_matrix);
            else {
                const structure_bsp *transform_definition =
                    (const structure_bsp *)
                        global_tag_instances[datum_index_to_absolute_index(((scenario_structure_bsp_reference
                            *)global_scenario->structure_bsps.address)[bsp_index].structure_bsp.index)]
                            .definition;
                memcpy(&node_matrix, &((const structure_instanced_geometry_instance
                    *)transform_definition->instanced_geometry_instances.address)[variant_index].transform, sizeof(node_matrix));
            }
            real_point3d position;
            matrix4x3_transform_point(&node_matrix, &((collision_vertex *)bsp->vertices.address)[((collision_edge
                *)bsp->edges.address)[surface->first_edge].vertices[0]].point, &position);
            matrix4x3_transform_vector(&node_matrix, &plane.n, &plane.n);
            real_vector3d forward;
            breakable_surfaces_render_vector_from_points(&slot->position, &position, &forward);
            float angle = breakable_surfaces_render_real_random_range(&random, 0.0f, breakable_surfaces_crack_two_pi);
            float cosine = (float)cos(angle);
            float sine = (float)sin(angle);
            rotate_vector_about_axis(&forward, &plane.n, sine, cosine);
            real_matrix4x3 matrix;
            matrix4x3_from_point_and_nonaxial_forward_and_left(&matrix, &slot->position, &forward, &plane.n);
            matrix.scale = breakable_surfaces_render_real_random_range(&random, 0.15f, 0.4f);
            matrix4x3_transform_point(&matrix, &vertices[0].position, &vertices[0].position);
            matrix4x3_transform_point(&matrix, &vertices[1].position, &vertices[1].position);
            matrix4x3_transform_point(&matrix, &vertices[2].position, &vertices[2].position);
            matrix4x3_transform_point(&matrix, &vertices[3].position, &vertices[3].position);
            c_rasterizer::draw_textured_transparent_quad(vertices, c_rasterizer::_alpha_blend_alpha_blend);
        }
        tag_instances = global_tag_instances;
    }
}


static __forceinline real_vector3d *breakable_surfaces_vector_from_points(
    const real_point3d *a, const real_point3d *b, real_vector3d *v)
{
    v->i = b->x - a->x;
    v->j = b->y - a->y;
    v->k = b->z - a->z;
    return v;
}
static __forceinline float breakable_surfaces_magnitude_squared(const real_vector3d *v)
{
    return v->i * v->i + v->j * v->j + v->k * v->k;
}
static __forceinline float breakable_surfaces_distance_squared(const real_point3d *a, const real_point3d *b)
{
    real_vector3d v;
    return breakable_surfaces_magnitude_squared(breakable_surfaces_vector_from_points(a, b, &v));
}
// @rdata 0x82021694 breakable_damage_max_distance
static const float breakable_damage_max_distance = 3.402823466e38f;
// @rdata 0x82022c0c breakable_damage_count_scale
static const float breakable_damage_count_scale = 2.5f;
// @address 0x22bf40 ?breakable_surface_damage@@YAXJJJJPBUs_damage_data@@J@Z
void breakable_surface_damage(long bsp_index, long variant_index, long set_index, long surface_index,
    const s_damage_data *damage_data, long record_index)
{
    collision_bsp *bsp;
    collision_surface *record;
    do {
        if (((collisions_breakable_surface_globals *)g_breakable_surface_globals)->active == 0)
            break;
        if (breakable_surface_extant(bsp_index, variant_index, set_index, surface_index) == 0)
            break;
        const void *node_out;
        bsp = (collision_bsp *)breakable_surface_get_bsp(bsp_index, variant_index, &node_out);
        const structure_bsp *structure_bsp =
            (const struct structure_bsp *)
                global_tag_instances[datum_index_to_absolute_index(((scenario_structure_bsp_reference
                    *)global_scenario->structure_bsps.address)[bsp_index].structure_bsp.index)]
                    .definition;
        record = &((collision_surface *)bsp->surfaces.address)[record_index];
        short material_index =
            TAG_BLOCK_GET_ELEMENT(&structure_bsp->collision_materials, record->material, s_collision_material)->global_material.m_index;
        const s_global_material_definition *entry = 0;
        if (material_index != -1 && material_index >= 0 && material_index < global_game_globals->material_count)
            entry = &global_game_globals->materials[material_index];
        if (entry->breakable_surface.index == -1)
            break;
        const cache_file_tag_instance *tag_instances = global_tag_instances;
        const s_breakable_surface_definition *material_def =
            (const s_breakable_surface_definition *)tag_instances[datum_index_to_absolute_index(entry->breakable_surface.index)].definition;
        float best_count = material_def->maximum_vitality;
        if (best_count <= 0.0f)
            break;
        float force = 0.0f;
        long damage_index = damage_data->effect;
        unsigned char damage_bit = (unsigned char)((damage_data->flags >> 8) & 1);
        if (damage_index != -1) {
            const s_damage_effect_definition *damage_def = (const s_damage_effect_definition
                *)tag_instances[datum_index_to_absolute_index(damage_index)].definition;
            float damage = _real_random_range(tls_random_seed(), damage_def->damage.damage_upper_bound.lower,
                damage_def->damage.damage_upper_bound.upper);
            float base = damage_def->damage.damage_lower_bound;
            damage = (damage - base) * damage_data->amount + base;
            damage *= damage_armor_table_lookup(damage_def->damage.general_damage, damage_def->damage.specific_damage, entry->armor_type_a,
                entry->armor_type_b);
            float limit = material_def->maximum_vitality * breakable_damage_count_scale;
            if (limit > 0.001f)
                force = damage / limit;
        }
        if (damage_bit != 0) {
            if (material_def->maximum_vitality <= 10.0f) goto apply_damage;
        } else {
            unsigned long seed2 = *tls_random_seed();
            random_seed_next(&seed2);
            unsigned __int64 random2 = seed2 >> 16;
            *tls_random_seed() = seed2;
            float r = (float)(double)(__int64)random2 * 1.5259022e-05f;
            if (r <= force) goto apply_damage;
        }
        if (damage_bit == 0)
            breakable_surfaces_new_crack(&damage_data->position, bsp_index, variant_index, record_index);
        break;
    apply_damage:
        float best = breakable_damage_max_distance;
        long best_edge = -1;
        breakable_surface_break_surface(bsp_index, variant_index, set_index, surface_index, record_index,
            &damage_data->position, (const s_breakable_surfaces_event_material *)&damage_data->location, &damage_data->direction);
        record = &((collision_surface *)bsp->surfaces.address)[record_index];
        long first_edge = record->first_edge;
        long edge_index = first_edge;
        do {
            collision_edge *edge = &((collision_edge *)bsp->edges.address)[edge_index];
            short edge_surface = edge->surfaces[1];
            real_point3d *point_a = &((collision_vertex *)bsp->vertices.address)[edge->vertices[0]].point;
            real_point3d *point_b = &((collision_vertex *)bsp->vertices.address)[edge->vertices[1]].point;
            bool side = record_index - edge_surface == 0;
            float dist = breakable_surfaces_distance_squared(point_a, point_b);
            if (!(dist >= best)) {
                best_edge = edge_index;
                best = dist;
            }
            edge_index = edge->edges[side];
        } while (edge_index != first_edge);
        edge_index = first_edge;
        do {
            collision_edge *edge = &((collision_edge *)bsp->edges.address)[edge_index];
            bool side = record_index - edge->surfaces[1] == 0;
            if (edge_index != best_edge) {
                short nearby_index = edge->surfaces[!side];
                collision_surface *nearby = &((collision_surface *)bsp->surfaces.address)[nearby_index];
                if ((nearby->flags & 8) != 0) {
                    long nearby_surface = nearby->breakable_surface_bit;
                    const long &record_surface = record->breakable_surface_bit;
                    if (nearby_surface != record_surface) {
                        long nearby_set = nearby->breakable_surface;
                        if (nearby_set == record->breakable_surface && breakable_surfaces_is_shortest_edge(bsp, nearby_index, edge_index) == 0)
                            breakable_surface_break_surface(bsp_index, variant_index, nearby_set, nearby_surface, nearby_index,
                                &damage_data->position, (const s_breakable_surfaces_event_material *)&damage_data->location, 0);
                    }
                }
            }
            edge_index = edge->edges[side];
        } while (edge_index != record->first_edge);
    } while (false);
    if (((collisions_breakable_surface_globals *)g_breakable_surface_globals)->dirty != 0) {
        havok_notify_static_geometry_modfied(-1, -1, -1, -1, -1);
        ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->dirty = 0;
    }
}
class phys_bvh_bytecode;

struct s_scenario_soft_ceiling;

static __forceinline long breakable_surfaces_structure_design_index(long bsp_index)
{
    const scenario *scenario_definition = global_scenario;
    BLAM_ASSUME(scenario_definition != 0);
    return ((const scenario_structure_bsp_reference *)scenario_definition->structure_bsps.address)[bsp_index].structure_design.index;
}

// @address 0xf0240 breakable_surfaces_is_in_soft_ceiling
extern "C" bool breakable_surfaces_is_in_soft_ceiling(long object_index)
{
    bool result = false;
    const object_datum *object = (object_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
    if ((unsigned char)((object->physics_flags >> 7) & 1) != 0 && object->parent_object_index == -1) {
        float radius = object->bounding_radius;
        real_point3d center = object->bounding_center;
        if (!(radius > 0.3f))
            radius = 0.3f;
        phys_vec4 query_center;
        query_center.set(center.x, center.y, center.z, 0.0f);
        long bsp_index = breakable_surfaces_first_active_structure_bsp();
        do {
            if (bsp_index == -1)
                break;
            long design_index = breakable_surfaces_structure_design_index(bsp_index);
            if (design_index != -1) {
                const s_soft_ceiling_design *design =
                    TAG_GET('sddt', const s_soft_ceiling_design, design_index);
                if (design && design->mopp_size != 0) {
                    const phys_bvh_bytecode *code = design->code;
                    phys_sphere sphere(query_center, radius);
                    phys_inline_array<phys_bvh_primitive_info, 8192> hits;
                    phys_bvh_sphere_vm_query_sphere(code, sphere, &hits);
                    for (long hit_index = 0; hit_index < hits.getSize(); hit_index++) {
                        unsigned long key = hits[hit_index].ID;
                        unsigned long group_index = key >> 16;
                        const s_soft_ceiling_group *group = &design->groups[group_index];
                        if (group->type == 1) {
                            unsigned long triangle_index = key & 0xffff;
                            const s_soft_ceiling_triangle *triangle = &group->triangles[triangle_index];
                            float plane_distance;
                            if (scenario_soft_ceilings_is_point_in_triangle(&design->plane_count, triangle,
                                    &center, radius, &plane_distance) &&
                                scenario_soft_ceiling_from_name(group->name)) {
                                real_vector3d normal;
                                float distance;
                                if (havok_calculate_sphere_vs_triangle_collision(&center, radius, &triangle->vertices[0],
                                        &triangle->vertices[1], &triangle->vertices[2], &normal, &distance) &&
                                    distance - radius < 0.0f) {
                                    result = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            bsp_index = breakable_surfaces_next_active_structure_bsp(bsp_index);
        } while (!result);
    }
    return result;
}


// @address 0x72f77a ?breakable_surface_effect_enabled@@3_NA
bool breakable_surface_effect_enabled = true;

static inline const real_quaternion *breakable_surfaces_identity_quaternion() { return global_identity_quaternion; }
// @address 0x115bf8 breakable_surfaces_update
extern "C" void breakable_surfaces_update(void)
{
    long lifetime = (long)((float)game_time_globals->ticks_per_second * 0.15f + 0.5f);
    real_rectangle3d bounds;
    float area;
    s_breakable_surface_sound_location location;
    breakable_surfaces_handle_pending_break();
    if (((collisions_breakable_surface_globals *)g_breakable_surface_globals)->dirty) {
        havok_notify_static_geometry_modfied(-1, -1, -1, -1, -1);
        ((collisions_breakable_surface_globals *)g_breakable_surface_globals)->dirty = 0;
    }
    s_breakable_surfaces_dispose_iterator iterator;
    iterator.iterator.absolute_index = -1;
    iterator.iterator.index = -1;
    iterator.iterator.data = g_breakable_surface_event_data;
    iterator.event = (s_breakable_surfaces_broken_event *)data_iterator_next(&iterator.iterator);
    while (iterator.event != 0) {
        if (!iterator.event->effects_created) {
            // match: 0x115bf8: the char->short->long widening keeps the event-data and
            // global_game_globals bases in r21/r20; a plain bool test moves them to r22/r21
            const unsigned char &flag = breakable_surface_effect_enabled;
            const unsigned short &value = flag;
            const unsigned long &enabled = value;
            if (enabled != 0 && value != 0) {
                structure_bsp *definition = (structure_bsp *)global_tag_instances[
                    datum_index_to_absolute_index(((scenario_structure_bsp_reference
                        *)global_scenario->structure_bsps.address)[iterator.event->bsp_index].structure_bsp.index)].definition;
                collision_bsp *bsp;
                real_matrix4x3 *matrix;
                float scale;
                if (iterator.event->variant_index == -1) {
                    bsp = (collision_bsp *)g_collision_bsp_cache.thread_users[get_current_thread_index()].collision_bsps[iterator.event->bsp_index];
                    matrix = 0;
                    scale = 1.0f;
                } else {
                    structure_instanced_geometry_instance *variant = &((structure_instanced_geometry_instance
                        *)definition->instanced_geometry_instances.address)[iterator.event->variant_index];
                    bsp = (collision_bsp *)&((structure_instanced_geometry_definition *)g_collision_bsp_cache.users[get_current_thread_index()]
                            .entries[iterator.event->bsp_index]
                            .data)[variant->definition_index]
                              .bsp;
                    matrix = &variant->transform;
                    scale = variant->transform.scale;
                }
                collision_surface *surface = &((collision_surface *)bsp->surfaces.address)[iterator.event->cause];
                s_global_material_definition *material = 0;
                short material_index = TAG_BLOCK_GET_ELEMENT(&definition->collision_materials, surface->material,
                    s_collision_material)->global_material.m_index;
                if (material_index != -1 && material_index >= 0 && material_index < global_game_globals->material_count)
                    material = &global_game_globals->materials[material_index];
                breakable_surfaces_collect_broken_surfaces(&iterator.event->surface_list, &bounds, iterator.event->cause,
                    iterator.event->set_index, iterator.event->surface_bits, &area, scale, bsp);
                real_point3d *position = &iterator.event->damage_position;
                position->x = (bounds.x0 + bounds.x1) * 0.5f;
                position->y = (bounds.y0 + bounds.y1) * 0.5f;
                position->z = (bounds.z0 + bounds.z1) * 0.5f;
                if (matrix != 0)
                    matrix4x3_transform_point(matrix, position, position);
                if (material->breakable_surface.index != -1) {
                    s_breakable_surface_definition *effect = (s_breakable_surface_definition
                        *)global_tag_instances[datum_index_to_absolute_index(material->breakable_surface.index)].definition;
                    if (effect->effect.index != -1) {
                        float volume = area * 0.33333334f;
                        volume = PIN(volume, 0.05f, 1.0f);
                        effect_new_from_breakable_surface(effect->effect.index, iterator.iterator.index, 1.0f, volume);
                    }
                    if (effect->sound.index != -1) {
                        location.sound.position = *position;
                        location.sound.rotation = *breakable_surfaces_identity_quaternion();
                        location.sound.velocity = *global_zero_vector3d;
                        location.material = iterator.event->material;
                        unattached_impulse_sound_new(effect->sound.index, &location.sound, 1.0f);
                    }
                }
            }
            iterator.event->effects_created = 1;
        } else if (game_time_globals->time - iterator.event->time > lifetime) {
            s_breakable_surface_set *set = breakable_surface_get_set(iterator.event->bsp_index, iterator.event->variant_index,
                iterator.event->set_index);
            if (set != 0) {
                if (set->next_index == iterator.iterator.index) {
                    set->next_index = iterator.event->previous_event;
                } else if (set->next_index != -1) {
                    long previous = set->next_index;
                    do {
                        s_breakable_surfaces_broken_event *previous_event = breakable_surfaces_event_get(previous);
                        previous = previous_event->previous_event;
                        if (previous == iterator.iterator.index) {
                            previous_event->previous_event = iterator.event->previous_event;
                            break;
                        }
                    } while (previous != -1);
                }
            }
            datum_delete(g_breakable_surface_event_data, iterator.iterator.index);
        }
        iterator.event = (s_breakable_surfaces_broken_event *)data_iterator_next(&iterator.iterator);
    }
}

t_breakable_surface_globals_allocator g_breakable_surface_globals_allocator;
t_breakable_surface_events_allocator g_breakable_surface_events_allocator;

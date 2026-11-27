// blamlibxbox/structure_seams.obj: from the records 0x9c078 0x198b88 0x1b65b0 0x1b65c0 0x1b66c8 0x1b6a28 0x1b6c40 0x1b6d90 0x1b6e18 0x1b6eb8 0x1b6ed0 0x24ba40
// @flags /TP /O2
#include "objects/object_reference_types.h"
#include "structures/structure_runtime_resources.h"
#include "tag_files/tag_groups.h"
#include "multithreading/thread_tls.h"
#include "math/real_math_types.h"
#include "memory/static_arrays.h"
#include "cache/restricted_memory_regions.h"
// @address 0x11df774 g_structure_seam_globals
#include "structures/structure_seams.h"
#include "structures/structure_seam_types.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
// @address 0x1b6a28 ?structure_seams_disconnect@@YAXJJ@Z
// @address 0x1b6d90 ?structure_seams_calculate_active_seams_mask@@YA?AV?$c_static_flags@$0EA@@@XZ

#include "hs/hs_runtime_data.h"
#include "structures/structure_bsp_tag.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "physics/structure_bsp_seam_link_types.h"
#include "structures/structure_seam_cluster_stack.h"
#include "scenario/scenario_tag.h"
#include "tag_files/tag_types.h"
#include "physics/collision_bsp_types.h"
#include <string.h>
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_registration.h"
#include "cache/restricted_memory.h"
#include "structures/structure_seam_allocator.h"
#include "memory/datum_index.h"


extern "C" {
__declspec(thread) s_structure_seam_globals *g_structure_seam_globals;
}

void __tls_set_g_structure_seam_globals_allocator(void *allocator)
{
    g_hs_runtime_globals = static_cast<hs_runtime_globals *>(allocator);
}






// @address 0x9c078 structure_seams_connected_cluster_references_get
bool structure_seams_connected_cluster_references_get(long scenario_tag, s_cluster_reference reference, bool include_self, s_cluster_stack *out)
{
    scenario *scenario_data = (scenario *)tag_get(0, scenario_tag);
    structure_bsp *bsp = (structure_bsp *)tag_get(0,
        ((scenario_structure_bsp_reference *)scenario_data->structure_bsps.address)[reference.bsp_index].structure_bsp.index);
    structure_cluster *cluster = &((structure_cluster *)bsp->clusters.address)[reference.cluster_index];
    if (include_self)
        out->push_back(reference);
    long seam_index_index = 0;
    if (cluster->seam_indices.count > 0) {
        s_structure_seam_globals *globals = g_structure_seam_globals;
        do {
        long seam_index = ((signed char *)cluster->seam_indices.address)[seam_index_index];
        s_seam_mapping &mapping = globals->mappings[seam_index];
        bool above = mapping.structure_indices[1] == reference.bsp_index;
        if (mapping.structure_indices[above] == reference.bsp_index && mapping.structure_indices[!above] != -1) {
            bool unmatched = false;
            for (long i = 0; i < mapping.cluster_count; ++i) {
                const s_seam_cluster_mapping &cm = mapping.clusters[i];
                if (cm.cluster_indices[above] == reference.cluster_index) {
                    long other_cluster = cm.cluster_indices[!above];
                    if (other_cluster != -1) {
                        if (!out->full()) {
                            long other_bsp = mapping.structure_indices[!above];
                            bool unique = true;
                            for (long j = 0; j < out->m_count; ++j) {
                                const s_cluster_reference &existing = out->m_elements[j];
                                if (existing.bsp_index == other_bsp && existing.cluster_index == other_cluster) {
                                    unique = false;
                                    break;
                                }
                            }
                            if (unique) {
                                out->push();
                                s_cluster_reference *top = out->get_top();
                                top->bsp_index = (signed char)other_bsp;
                                top->cluster_index = (unsigned char)other_cluster;
                            }
                        }
                    } else {
                        unmatched = true;
                    }
                }
            }
            if (unmatched) {
                for (long i = 0; i < mapping.cluster_count; ++i) {
                    if (mapping.clusters[i].cluster_indices[above] == -1) {
                        if (!out->full()) {
                            out->push();
                            long other_cluster = mapping.clusters[i].cluster_indices[!above];
                            long other_bsp = mapping.structure_indices[!above];
                            s_cluster_reference *top = out->get_top();
                            top->bsp_index = (signed char)other_bsp;
                            top->cluster_index = (unsigned char)other_cluster;
                        }
                    }
                }
            }
        }
        } while (++seam_index_index < cluster->seam_indices.count);
    }
    return out->m_count != 0;
}


// @address 0x1b65b0 structure_seams_set_globals
void structure_seams_set_globals(void *value)
{
    g_structure_seam_globals = (s_structure_seam_globals *)value;
}


// @address 0x1b65c0 structure_seams_compute_bsp_flags
void structure_seams_compute_bsp_flags(long scenario_tag, c_static_flags<512> *flags, long bsp_index)
{
    unsigned long *active_seam_flags = g_structure_seam_globals->active_seam_flags.m_storage;
    scenario *scenario_data = (scenario *)tag_get(0, scenario_tag);
    structure_bsp *bsp =
        (structure_bsp *)tag_get(0, ((scenario_structure_bsp_reference *)scenario_data->structure_bsps.address)[bsp_index].structure_bsp.index);
    flags->clear();
    for (long index = 0; index < bsp->surface_seam_links.count; ++index) {
        short seam_index = ((s_structure_bsp_seam_link *)bsp->surface_seam_links.address)[index].seam_index;
        if (seam_index != -1 && (active_seam_flags[seam_index >> 5] & (1 << (seam_index & 31))) != 0)
            flags->set(index, false);
        else
            flags->set(index, true);
    }
}



static __forceinline structure_bsp *structure_seams_connect_bsp_get(scenario *scenario_data, long index)
{
    return (structure_bsp *)tag_get(0, ((scenario_structure_bsp_reference *)scenario_data->structure_bsps.address)[index].structure_bsp.index);
}

void structure_seams_disconnect(long scenario_index, long structure_bsp_index)
{
    structure_bsp *structure = structure_seams_connect_bsp_get(
        (scenario *)tag_get(0, global_scenario_index), structure_bsp_index);
    for (long seam_index = 0; seam_index < structure->seam_identifiers.count(); ++seam_index) {
        s_seam_mapping &seam_mapping = g_structure_seam_globals->mappings[seam_index];
        if (seam_mapping.structure_indices[0] == structure_bsp_index ||
            seam_mapping.structure_indices[1] == structure_bsp_index) {
            bool side = seam_mapping.structure_indices[1] == structure_bsp_index;
            seam_mapping.structure_indices[side] = -1;
            for (long cluster_mapping_index = 0; cluster_mapping_index < seam_mapping.cluster_count; ++cluster_mapping_index) {
                seam_mapping.clusters[cluster_mapping_index].cluster_indices[side] = -1;
                seam_mapping.clusters[cluster_mapping_index].cluster_mapping_indices[side] = -1;
            }
            for (long cluster_mapping_index = 0; cluster_mapping_index < seam_mapping.cluster_count; ++cluster_mapping_index) {
                if ((unsigned char)seam_mapping.clusters[cluster_mapping_index].cluster_indices[0] == 255 &&
                    (unsigned char)seam_mapping.clusters[cluster_mapping_index].cluster_indices[1] == 255) {
                    for (long move_index = cluster_mapping_index; move_index < seam_mapping.cluster_count - 1; ++move_index)
                        memcpy(&seam_mapping.clusters[move_index], &seam_mapping.clusters[move_index + 1], sizeof(s_seam_cluster_mapping));
                    --seam_mapping.cluster_count;
                    --cluster_mapping_index;
                }
            }
        }
    }
    g_structure_seam_globals->active_seam_flags = structure_seams_calculate_active_seams_mask();
    g_structure_seam_globals->connected_structure_bsp_mask &= ~(1 << structure_bsp_index);
}

c_static_flags<64> structure_seams_calculate_active_seams_mask()
{
    c_static_flags<64> active_seams;
    active_seams.clear();
    for (long seam_index = 0; seam_index < 64; ++seam_index) {
        s_seam_mapping &mapping = g_structure_seam_globals->mappings[seam_index];
        active_seams.set(seam_index, (unsigned char)mapping.structure_indices[0] != 255 && (unsigned char)mapping.structure_indices[1] != 255);
    }
    return active_seams;
}


// Registers a member of field layout 1 (see restricted_memory_register_member) and publishes its
// per-thread address. One of a family of identical allocators that differ
// only in the layout index, the update callback and the thread-local pointer.








// @address 0x1b6e18 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_structure_seam_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0), &__tls_set_g_structure_seam_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = restricted_memory_register_member(&g_restricted_regions[3], name, type, size, 0, structure_seams_set_globals, 0, 0);
  unsigned int offset = g_restricted_regions[3].get_member_offset(member_index);
  void *address = restricted_region_get_base_address(3) + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  g_structure_seam_globals = (s_structure_seam_globals *)address;
  return restricted_region_get_base_address(3) + g_restricted_regions[3].get_member_offset(member_index);
}



// @address 0x1b6eb8 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_structure_seam_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_structure_seam_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}


// @address 0x1b6ed0 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_structure_seam_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0), &__tls_set_g_structure_seam_globals_allocator>::deallocate(void *address)
{
    int value = m_member_index;
    g_structure_seam_globals = 0;
    restricted_region_free_member(3, value);
}

static __forceinline structure_bsp *structure_seams_global_bsp_get(scenario_structure_bsp_reference *references, long index)
{
    return (structure_bsp *)tag_get(0, references[index].structure_bsp.index);
}
static __forceinline s_structure_bsp_resources *structure_seams_resources_get(s_structure_bsp_resource *resource)
{
    s_structure_bsp_resources *result;
    if (resource->use_resource_items)
        result = resource->collision_resource_interface.get_resources();
    else
        result = (s_structure_bsp_resources *)resource->raw_resources.address;
    return result;
}
static __forceinline long structure_seams_other_bsp(long first_bsp, long second_bsp, long bsp_index)
{
    long result = first_bsp;
    if (result == bsp_index)
        result = second_bsp;
    return result;
}
// @address 0x1b6c40 structure_seams_try_and_get_connected_edge
// match: 0x1b6c40: pointer __restrict qualifier prevents alias reload
extern "C" bool structure_seams_try_and_get_connected_edge(long bsp_index, long edge_index, long * __restrict other_bsp_index, long *other_edge_index)
{
    scenario_structure_bsp_reference *references = (scenario_structure_bsp_reference *)((scenario *)global_scenario)->structure_bsps.address;
    structure_bsp *bsp = structure_seams_global_bsp_get(references, bsp_index);
    s_structure_bsp_resources *resources = structure_seams_resources_get(&bsp->resource);
    bool result = false;
    collision_edge *edge = &((collision_edge *)resources->collision_bsp->edges.address)[edge_index];
    if (edge->surfaces[0] != -1 && edge->surfaces[1] != -1) {
        s_structure_seam_edge_mapping *edge_mapping = &bsp->edge_to_seam_edge[edge_index];
        short seam_index = edge_mapping->seam_index;
        if (seam_index != -1) {
            s_seam_mapping *mapping = &g_structure_seam_globals->mappings[seam_index];
            long first_bsp = mapping->structure_indices[0];
            long second_bsp = mapping->structure_indices[1];
            if (first_bsp != -1 && second_bsp != -1) {
                long connected_bsp = structure_seams_other_bsp(first_bsp, second_bsp, bsp_index);
                long seam_edge_index = edge_mapping->seam_edge_index;
                *other_bsp_index = connected_bsp;
                structure_bsp *other_bsp = structure_seams_global_bsp_get(references, connected_bsp);
                *other_edge_index = other_bsp->seam_identifiers[seam_index].edge_indices[seam_edge_index];
                result = *other_edge_index != -1;
            }
        }
    }
    return result;
}

static __forceinline bool structure_seams_identifiers_are_equal(const char *first, const char *second)
{
    unsigned long remaining = 16;
    const char *cursor = first;
    do {
        if (*cursor != 0)
            break;
        --remaining;
        ++cursor;
    } while (remaining != 0);
    return remaining != 0 && memcmp(first, second, sizeof(((s_structure_seam *)0)->identifier)) == 0;
}
static __forceinline real_vector3d *structure_seams_vector_from_points(const real_point3d *a, const real_point3d *b, real_vector3d *result)
{
    result->i = b->x - a->x;
    result->j = b->y - a->y;
    result->k = b->z - a->z;
    return result;
}
static __forceinline float structure_seams_magnitude_squared(const real_vector3d *v)
{
    return v->k * v->k + (v->i * v->i + v->j * v->j);
}
static __forceinline float structure_seams_distance_squared(const real_point3d *a, const real_point3d *b)
{
    real_vector3d delta;
    structure_seams_vector_from_points(a, b, &delta);
    return structure_seams_magnitude_squared(&delta);
}
// @address 0x1b66c8 ?structure_seams_connect@@YAXJJ@Z
void structure_seams_connect(long scenario_index, long structure_bsp_index)
{
    s_structure_seam_globals *globals = g_structure_seam_globals;
    long bsp_index = structure_bsp_index;
    if ((unsigned long)((1 << bsp_index) & g_structure_seam_globals->connected_structure_bsp_mask) == 0) {
        const cache_file_tag_instance *scenario_entry =
            &global_tag_instances[datum_index_to_absolute_index(scenario_index)];
        structure_bsp *structure = structure_seams_connect_bsp_get(
            (scenario *)scenario_entry->definition, structure_bsp_index);
        for (long seam_index = 0; seam_index < structure->seam_identifiers.count(); ++seam_index) {
            s_structure_seam *seam = &structure->seam_identifiers[seam_index];
            s_seam_mapping &mapping = g_structure_seam_globals->mappings[seam_index];
            for (long cluster_index = 0; cluster_index < seam->clusters.count(); ++cluster_index) {
                bool matched = false;
                s_structure_seam_cluster_mapping *cluster = &seam->clusters[cluster_index];
                if (mapping.structure_indices[0] != structure_bsp_index && mapping.structure_indices[1] != structure_bsp_index) {
                    if (mapping.structure_indices[0] == -1)
                        mapping.structure_indices[0] = (signed char)structure_bsp_index;
                    else if (mapping.structure_indices[1] == -1)
                        mapping.structure_indices[1] = (signed char)structure_bsp_index;
                    else
                        break;
                }
                bool above = mapping.structure_indices[1] == structure_bsp_index;
                long other_bsp_index = mapping.structure_indices[!above];
                if (other_bsp_index != -1) {
                    structure_bsp *other = structure_seams_connect_bsp_get(
                        (scenario *)scenario_entry->definition, other_bsp_index);
                    if (seam_index < 0 || seam_index >= other->seam_identifiers.count() ||
                        !structure_seams_identifiers_are_equal(other->seam_identifiers[seam_index].identifier,
                            structure->seam_identifiers[seam_index].identifier)) {
                        mapping.structure_indices[above] = -1;
                        break;
                    }
                }
                if (mapping.structure_indices[!above] != -1) {
                        // match: 0x1b66c8: pointer __restrict qualifier prevents alias reload
                        s_structure_seam_cluster_mapping *__restrict matching_cluster = cluster;
                        long runtime_index = 0;
                        if (mapping.cluster_count > 0) {
                        signed char *cluster_indices = &mapping.clusters[0].cluster_indices[above];
                        do {
                            if ((unsigned char)*cluster_indices == 255 &&
                                !(structure_seams_distance_squared(&mapping.clusters[runtime_index].position, &matching_cluster->position) >= 0.040000003f)) {
                                long mapped_cluster_index = matching_cluster->cluster_index;
                                matched = true;
                                cluster_indices[2] = (signed char)cluster_index;
                                *cluster_indices = (signed char)mapped_cluster_index;
                            }
                            ++runtime_index;
                            cluster_indices += sizeof(s_seam_cluster_mapping);
                        } while (runtime_index < mapping.cluster_count);
                        }
                }
                if (!matched && mapping.cluster_count != 32) {
                    s_seam_cluster_mapping &runtime = mapping.clusters[mapping.cluster_count];
                    runtime.position = cluster->position;
                    mapping.clusters[mapping.cluster_count].cluster_indices[above] = (signed char)cluster->cluster_index;
                    mapping.clusters[mapping.cluster_count].cluster_mapping_indices[above] = (signed char)cluster_index;
                    ++mapping.cluster_count;
                }
            }
        }
        g_structure_seam_globals->active_seam_flags = structure_seams_calculate_active_seams_mask();
        g_structure_seam_globals->connected_structure_bsp_mask |= (1 << bsp_index);
    }
}

// @address 0x198b88 ?scenario_initialize_for_new_structure_bsp@@YAXK@Z
void scenario_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask)
{
    unsigned long active_mask = g_active_structure_bsp_mask | structure_bsp_mask;
    long bsp_index = 0;
    do {
        long bit = 1 << bsp_index;
        if ((bit & structure_bsp_mask) != 0 &&
            (unsigned long)(g_structure_seam_globals->connected_structure_bsp_mask & bit) == 0)
            structure_seams_connect((global_scenario_index), bsp_index);
        ++bsp_index;
    } while (bsp_index < 16);
    bsp_index = 0;
    do {
        if (((1 << bsp_index) & active_mask) != 0)
            structure_seams_compute_bsp_flags((global_scenario_index), &g_structure_seam_globals->bsp_seam_flags[bsp_index], bsp_index);
        ++bsp_index;
    } while (bsp_index < 16);
}


c_structure_seam_globals_allocator g_structure_seam_globals_allocator;

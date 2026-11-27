// blamlibxbox/visibility_collection_structure.obj: from the records 0x156478 0x156ed0 0x157248 0x157498 0x158c30 0x2aff48
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "visibility/visibility_collection_structure_queries.h"
#include "render/render_lightmap_shadows_globals.h"
#include "cseries/crt_xdk.h"
#include <stddef.h>
#include "cseries/pointer_utilities.h"
#include "multithreading/thread_tls.h"
#include "visibility/visibility_projection_types.h"
// @address 0x157a88 ?get_part_bitvector_space@c_visibility_collection@@SAPAKJ@Z
// @address 0x745d38 ?m_marker_count@c_visible_items@@0JA
// @address 0x761558 ?m_items@c_visible_items@@2Us_visible_items@@A
// @address 0x7cf608 ?g_collision_bsp_cache@@3Us_collision_bsp_cache@@A
// @address 0x7cff08 ?global_scenario@@3PAUscenario@@A
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x7e88b8 ?m_markers@c_visible_items@@0PAUs_marker_indices@1@A
// @address 0x81dc68 ?g_render_structure_globals@@3Urender_structure_globals@@A
// @address 0x8368d0 g_player_render_tuning

#include "cseries/cseries.h"
#include "tag_files/tag_groups.h"
#include "objects/object_reference_types.h"
#include <string.h>
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include "structures/structures.h"
#include "phys/phys_sphere.h"
#include "render/render_structure.h"
#include "scenario/scenario.h"
#include "math/real_math_types.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "scenario/scenario_tag.h"

#include "phys/phys_thread_memory.h"
#include "structures/structure_bsp_tag.h"
// @address 0x60a380 ?free_chunk@phys_thread_memory@@QAAXPAXHW4phys_memory_class@@@Z

#include "phys/phys_collide.h"
#include "phys/phys_bvh_float_aabb_vm.h"
#include "memory/datum_index.h"
#include "structures/structure_globals.h"
#include "render/views/player_view_types.h"
#include "render/render_camera_projection.h"

struct s_per_mesh_mopp {
    s_tag_data code;
    s_tag_block part_indices;
};
// @address 0x677d10 phys_bvh_float_aabb_vm_query_planes_opt

// @address 0x678160 phys_bvh_float_aabb_vm_query_sphere_opt


#include "visibility/visibility_region_types.h"

#include "visibility/visible_items_types.h"
#include "visibility/visible_items.h"





#include "structures/structure_instanced_geometry.h"


#include "physics/collision_bsp_cache_types.h"
#include "geometry/render_geometry_mesh_types.h"
struct s_render_mesh_subpart {
    short mesh_index;
    long count;
    TAG_POINTER(unsigned short) indices;
    TAG_POINTER(void) definition;
};

struct s_scenario_lightmap_bsp_data;

#include "render/render_structure_globals_types.h"


#include "visibility/visibility_collection.h"

namespace {
// match: 0x156478: two-layer forwarding preserves the inlined call shape that
// downstream register allocation in collect_structure_elements_from_regions needs.
static __forceinline void *tag_block_element_at(const s_tag_block *block, long index, long element_size)
{
    const s_tag_block *target = block;
    long element = index;
    long size = element_size;
    return tag_block_get_element_with_size(target, element, size);
}
static __forceinline unsigned short *visibility_collection_structure_mesh_get_mopp_part_reorder_element(
    s_per_mesh_mopp *mopp, long part_offset, long row, short part)
{
    return (unsigned short *)tag_block_get_element_with_size(&mopp->part_indices, part + (row << 1) + part_offset, sizeof(unsigned short));
}
static __forceinline unsigned short *visibility_collection_structure_mesh_get_mopp_part_order_element(
    s_per_mesh_mopp *mopp, long part_offset, long row, short part)
{
    return (unsigned short *)tag_block_get_element_with_size(&mopp->part_indices, part + (row << 1), sizeof(unsigned short));
}

static __forceinline structure_instanced_geometry_definition *visibility_collection_structure_collision_definitions_get(long structure_bsp_index)
{
    return g_collision_bsp_cache.users[get_current_thread_index()].entries[structure_bsp_index].data;
}
static __forceinline bool visibility_collection_structure_mesh_has_part_mopp(const s_render_geometry *geometry, long subpart_count, long mesh_index)
{
    if (geometry->per_mesh_mopp.count > mesh_index) {
        s_per_mesh_mopp *mopp = (s_per_mesh_mopp *)tag_block_get_element_with_size(
            &geometry->per_mesh_mopp, mesh_index, sizeof(s_per_mesh_mopp));
        return mopp->part_indices.count > (subpart_count << 1);
    } else {
        return false;
    }
}
static __forceinline phys_bvh_bytecode *mopp_code_data_get(s_tag_data *data, long offset)
{
    phys_bvh_bytecode *code = (phys_bvh_bytecode *)offset_pointer(data->address, offset);
    code->m_data.m_data = (unsigned char *)offset_pointer(code, sizeof(*code));
    return code;
}
static __forceinline long mopp_code_tag_data_size(const phys_bvh_bytecode *code)
{
    return (code->m_data.m_size + 63) & ~15;
}
static __forceinline const visibility_cluster *visibility_collection_structure_visibility_region_get_cluster(const s_visibility_region *region, long index)
{
    return &region->clusters[index];
}
static __forceinline const visibility_volume *visibility_collection_structure_visibility_region_get_cluster_volume(
    const s_visibility_region *region, const visibility_cluster *cluster, long projection, long index)
{
    long volume_index = cluster->first_volume[projection] + index;
    __dcbt(0, &region->volumes[volume_index + 1]);
    return &region->volumes[volume_index];
}
}





static __forceinline s_structure_globals *visibility_collection_structure_get_globals()
{
    long thread_index = get_current_thread_index();
    s_structure_globals *globals = thread_structure_globals + thread_index;
    return globals - 1;
}
static __forceinline c_additive_hash<512, 4096, 3307, 3> *visibility_collection_structure_instance_marker_get()
{
    return &visibility_collection_structure_get_globals()->marker;
}
static __forceinline bool visibility_collection_structure_instance_unmarked(long bsp_index, long instance_index)
{
    s_structure_globals *globals = visibility_collection_structure_get_globals();
    if (globals->marker.element_exists(instance_index, bsp_index, ((bsp_index << 12) | instance_index)) == 0)
        return true;
    else
        return false;
}
static __forceinline bool visibility_collection_structure_instance_mark_with_data(long bsp_index, long instance_index, long data)
{
    s_structure_globals *globals = visibility_collection_structure_get_globals();
    bool result = globals->marker.add_element(instance_index, bsp_index, ((bsp_index << 12) | instance_index), data);
    return result;
}
static __forceinline void visibility_collection_structure_instance_marker_end()
{
    visibility_collection_structure_get_globals()->instanced_geometry_marker_initialized = false;
}

static __forceinline structure_bsp *visibility_collection_structure_region_bsp_get(long bsp_index)
{
    scenario_structure_bsp_reference *reference =
        (scenario_structure_bsp_reference *)tag_block_get_element_with_size(
            &global_scenario->structure_bsps, bsp_index, sizeof(scenario_structure_bsp_reference));
    return (structure_bsp *)global_tag_instances[datum_index_to_absolute_index(reference->structure_bsp.index)].definition;
}

static __forceinline void visibility_collection_structure_region_bitvector_add_index(short index, short region_index)
{
    c_visible_items::get_global_items().region_cluster_bitvectors[index][region_index >> 5] |= 1 << (region_index & 31);
}
static __forceinline long visibility_collection_structure_region_bitvector_new(short region_index)
{
    long index = c_visible_items::get_global_items().instance_visibility_bit_count++;
    if (index >= 0 && index < 512) {
        c_visible_items::get_global_items().region_cluster_bitvectors[index][0] =
        c_visible_items::get_global_items().region_cluster_bitvectors[index][1] =
        c_visible_items::get_global_items().region_cluster_bitvectors[index][2] =
        c_visible_items::get_global_items().region_cluster_bitvectors[index][3] = 0;
        visibility_collection_structure_region_bitvector_add_index((short)index, region_index);
    } else {
        index = -1;
    }
    return index;
}

static __forceinline const s_render_geometry *visibility_collection_structure_region_geometry_get(long bsp_index)
{
    return g_render_structure_globals.render_geometry[bsp_index];
}

// @address 0x156478 ?collect_structure_elements_from_regions@c_visibility_collection@@IAAXXZ
void c_visibility_collection::collect_structure_elements_from_regions()
{
    structure_instanced_geometry_marker_begin();
    if ((m_region->flags & 16) == 0) {
        short region_index = 0;
        if (m_region->cluster_count > 0) {
            s_visible_items &items = c_visible_items::get_global_items();
            bool fully_contained, lit, shadow_casting, unmarked;
            do {
                visibility_cluster *cluster = &m_region->clusters[region_index];
                long bsp_index = cluster->bsp_index;
                structure_bsp *bsp = visibility_collection_structure_region_bsp_get(bsp_index);
                structure_instanced_geometry_instance *instance_block = (structure_instanced_geometry_instance *)bsp->instanced_geometry_instances.address;
                structure_cluster *bsp_cluster = (structure_cluster *)tag_block_element_at(
                    &bsp->clusters, cluster->cluster_index, sizeof(structure_cluster));
                const s_render_geometry *geometry = visibility_collection_structure_region_geometry_get(bsp_index);
                if (bsp_cluster->mesh_index >= 0 && bsp_cluster->mesh_index < geometry->meshes.count) {
                    m_region->visible_cluster_bits[bsp_index][cluster->cluster_index >> 5] |= 1 << (cluster->cluster_index & 31);
                    m_region->visible_cluster_region[cluster->bsp_index][cluster->cluster_index] = (unsigned char)region_index;
                    long cluster_entry_index = items.clusters.add();
                    if (cluster_entry_index != -1) {
                            s_visible_clusters *visible_cluster = &items.clusters[cluster_entry_index];
                            visible_cluster->flags = 3;
                            visible_cluster->cluster_reference = cluster->cluster_reference;
                            visible_cluster->pad4 = 0;
                            visible_cluster->region_cluster = region_index;
                            visible_cluster->mesh_index = bsp_cluster->mesh_index;
                            visible_cluster->visible_parts = 0;
                        }
                    
                    s_mesh *mesh = (s_mesh *)tag_block_element_at(
                        &geometry->meshes, bsp_cluster->mesh_index, sizeof(s_mesh));
                    long subpart_index = 0;
                    if (mesh->instance_buckets.count > 0) {
                        do {
                            s_render_mesh_subpart *subpart =
                                (s_render_mesh_subpart *)tag_block_element_at(
                                    &mesh->instance_buckets, subpart_index, sizeof(s_render_mesh_subpart));
                            long list_index = items.instance_lists.add();
                    if (list_index != -1) {
                                    s_visible_instance_list *list = &items.instance_lists[list_index];
                                    list->cluster_reference = cluster->cluster_reference;
                                    list->unknown = 0;
                                    list->subpart_index = subpart_index;
                                    list->first_instance = items.instances.count;
                                    list->instance_count = 0;
                                    list->mesh_index = subpart->mesh_index;
                                    unsigned short instance_index = subpart->indices[0];
                                    {
                                        for (long i = 0; i < subpart->count; i++) {
                                            unsigned short current_index = instance_index;
                                            structure_instanced_geometry_instance *geometry_instance = &instance_block[current_index];
                                            if (i < subpart->count - 1) {
                                                instance_index = subpart->indices[i + 1];
                                                __dcbt((long)instance_index * sizeof(structure_instanced_geometry_instance), instance_block);
                                            }
                                            if (visibility_collection_structure_instance_unmarked(bsp_index, current_index)) {
                                                unsigned char visible =
                                                    sphere_is_visible(&geometry_instance->world_bounding_sphere_center,
                                                    geometry_instance->world_bounding_sphere_radius, region_index, false, &fully_contained, &lit,
                                                    &shadow_casting);
                                                if (visible || (m_region->flags & 64) != 0) {
                                                    float alpha = compute_instance_alpha(geometry_instance, bsp_index);
                                                    if (alpha > 0.0f) {
                                                        long instance_entry_index = items.instances.add();
                    if (instance_entry_index != -1) {
                                                                short bitvector_index = -1;
                                                                s_visible_instances *visible_instance = &items.instances[instance_entry_index];
                                                                if (!fully_contained) {
                                                                    bitvector_index = (short)visibility_collection_structure_region_bitvector_new(region_index);
                                                                    if (bitvector_index == -1) {
                                                                        fully_contained = true;
                                                                    }
                                                                }
                                                                visible_instance->flags = (lit ? 1 : 0) | (shadow_casting ? 2 : 0) | (fully_contained ? 4 : 0);
                                                                visible_instance->instance_index = current_index;
                                                                visible_instance->bsp_index = (short)bsp_index;
                                                                visible_instance->region_cluster_bitvector_start_index = bitvector_index;
                                                                visible_instance->visible_parts = 0;
                                                                visible_instance->alpha = (unsigned char)(alpha * 255.0f);
                                                                list->instance_count++;
                                                                visibility_collection_structure_instance_mark_with_data(
                                                                    bsp_index, current_index, instance_entry_index);
                                                            }
                                                        
                                                    }
                                                }
                                            }
                                            else {
                                                int found_index;
                                                if (structure_instanced_geometry_mark_get_user_data(bsp_index, current_index, &found_index)) {
                                                    s_visible_instances *marked_instance = &items.instances[found_index];
                                                    if (!(marked_instance->flags & 4))
                                                        visibility_collection_structure_region_bitvector_add_index(
                                                            marked_instance->region_cluster_bitvector_start_index, region_index);
                                                }
                                            }
                                        }
                                    }
                                    if (list->instance_count == 0) {
                                        items.instance_lists.count--;
                                    }
                                }
                            
                            subpart_index++;
                        } while (subpart_index < mesh->instance_buckets.count);
                    }
                }
                region_index++;
            } while (region_index < m_region->cluster_count);
        }
    }
    generate_structure_visible_subparts();
    visibility_collection_structure_instance_marker_end();
}

// @address 0x157248 ?generate_structure_visible_subparts@c_visibility_collection@@IAAXXZ
void c_visibility_collection::generate_structure_visible_subparts()
{
    s_visible_items &items = c_visible_items::get_global_items();
    long iterator;
    for (s_visible_clusters *cluster = items.clusters.list_iterator_new(&iterator, c_visible_items::get_cluster_starting_index()); cluster;
        cluster = items.clusters.list_iterator_next(&iterator))
        generate_cluster_visible_parts(cluster);
    for (s_visible_instance_list *list =
             items.instance_lists.list_iterator_new(&iterator, c_visible_items::get_instance_list_starting_index());
        list; list = items.instance_lists.list_iterator_next(&iterator))
    {
        long first = list->first_instance;
        long end = first + list->instance_count;
        for (long i = first; i < end; ++i)
        {
            s_visible_instances *instance = &items.instances[i];
            if (!(instance->flags & 4))
            {
                structure_bsp *bsp = visibility_collection_structure_region_bsp_get(instance->bsp_index);
                structure_instanced_geometry_instance *definition =
                    (structure_instanced_geometry_instance *)tag_block_get_element_with_size(
                    &bsp->instanced_geometry_instances, instance->instance_index, sizeof(structure_instanced_geometry_instance));
                instance->visible_parts = 0;
                if (definition->world_bounding_sphere_radius > 6.0f)
                    generate_instance_visible_parts(instance);
                else
                    instance->flags |= 4;
            }
        }
    }
}

// @address 0x157498 ?generate_cluster_visible_parts@c_visibility_collection@@IAAXPAUs_visible_clusters@@@Z
void c_visibility_collection::generate_cluster_visible_parts(s_visible_clusters *visible)
{
    phys_bvh_bytecode *mopp_code = 0;
    bool success = false;
    phys_inline_array<unsigned int, 4096> subpart_hits;
    phys_inline_array<unsigned int, 4096> subpart_partial_hits;
    const s_visibility_region *region = (const s_visibility_region *)m_region;
    phys_bvh_planes_query_input query;
    query.m_plane_count = 6;
    phys_vec4 planes[6];
    query.m_aabb_planes = planes;
    subpart_hits.clear();
    subpart_partial_hits.clear();
    structure_bsp *bsp = visibility_collection_structure_region_bsp_get(visible->cluster_reference.bsp_index);
    const visibility_cluster *region_cluster = visibility_collection_structure_visibility_region_get_cluster(region, visible->region_cluster);
    structure_cluster *cluster = (structure_cluster *)tag_block_get_element_with_size(
        &bsp->clusters, visible->cluster_reference.cluster_index, sizeof(structure_cluster));
    const s_render_geometry *geometry = g_render_structure_globals.render_geometry[visible->cluster_reference.bsp_index];
    s_mesh *mesh = (s_mesh *)tag_block_get_element_with_size(
        &geometry->meshes, cluster->mesh_index, sizeof(s_mesh));
#ifdef HALO3_64BIT
    phys_bvh_bytecode::bytecode_info mopp_info_storage;
    mopp_info_storage.m_offset.set_zero4();
    phys_bvh_bytecode mopp_code_storage(mopp_info_storage, 0, 0);
#endif
    if (visibility_collection_structure_mesh_has_part_mopp(geometry, mesh->subparts.count, cluster->mesh_index)) {
        s_per_mesh_mopp *mopp = (s_per_mesh_mopp *)tag_block_get_element_with_size(
            &geometry->per_mesh_mopp, cluster->mesh_index, sizeof(s_per_mesh_mopp));
#ifdef HALO3_64BIT
        const char *raw0 = (const char *)mopp->code.address;
        if (raw0) {
            long raw0_size = *(const int *)(raw0 + 0x24);
            long code_size = (raw0_size + 63) & ~15;
            const char *raw1 = raw0 + code_size;
            memcpy(&mopp_code_storage.m_bvh_info, raw1 + 0x10, sizeof(phys_bvh_bytecode::bytecode_info));
            long raw1_size = *(const int *)(raw1 + 0x24);
            mopp_code_storage.m_data.m_data = (unsigned char *)(raw1 + 0x30);
            mopp_code_storage.m_data.m_size = (int)raw1_size;
            mopp_code_storage.m_data.m_capacity_and_flags = (int)raw1_size | (int)0x80000000u;
            mopp_code_storage.m_build_type = (phys_bvh_bytecode::bvh_build_kind)*(const signed char *)(raw1 + 0x2c);
            mopp_code = &mopp_code_storage;
        }
#else
        mopp_code = mopp_code_data_get(&mopp->code, 0);
        long code_size = mopp_code_tag_data_size(mopp_code);
        mopp_code = mopp_code_data_get(&mopp->code, code_size);
#endif
    }
    if (mopp_code) {
        if (mesh->subparts.count > 0) {
            visible->visible_parts = get_part_bitvector_space(mesh->parts.count);
            if (visible->visible_parts) {
                if (m_region->collection_shape == 0) {
                    for (long projection_index = 0; projection_index < region->projection_count; projection_index++) {
                        for (long volume_index = 0; volume_index < region_cluster->volume_count[projection_index]; volume_index++) {
                            const visibility_volume *volume = visibility_collection_structure_visibility_region_get_cluster_volume(
                                region, region_cluster, projection_index, volume_index);
                            for (long plane_index = 0; plane_index < 6; plane_index++) {
                                real_plane3d plane = volume->planes[plane_index];
                                planes[plane_index].set(plane.i, plane.j, plane.k, -plane.d);
                            }
                            phys_bvh_float_aabb_vm_query_planes_opt(mopp_code, &query, &subpart_partial_hits, &subpart_hits);
                        }
                        success |= convert_part_keys_to_bitvector(geometry, cluster->mesh_index, visible->visible_parts,
                            mesh->parts.count, mesh->subparts.count, subpart_hits, subpart_partial_hits);
                    }
                } else if (m_region->collection_shape == 1) {
                    phys_vec4 position;
                    phys_sphere sphere;
                    position.set(m_region->sphere_center.x, m_region->sphere_center.y, m_region->sphere_center.z, 0.0f);
                    sphere.contact_set_position(position);
                    sphere.set_shape_radius(m_region->sphere_radius);
                    phys_bvh_float_aabb_vm_query_sphere_opt(mopp_code, &sphere, &subpart_partial_hits, &subpart_hits);
                    success |= convert_part_keys_to_bitvector(geometry, cluster->mesh_index, visible->visible_parts, mesh->parts.count,
                        mesh->subparts.count, subpart_hits, subpart_partial_hits);
                } else {
                    memset(visible->visible_parts, -1, ((mesh->parts.count + 31) >> 5) * sizeof(unsigned long));
                    success = true;
                }
            }
        }
    }
    visible->flags &= ~4;
    if (success)
        visible->flags |= 8;
    else
        visible->flags &= ~8;
}

// @address 0x158c30 ?generate_instance_visible_parts@c_visibility_collection@@IAAXPAUs_visible_instances@@@Z
void c_visibility_collection::generate_instance_visible_parts(s_visible_instances *visible)
{
    bool success = false;
    structure_bsp *bsp = visibility_collection_structure_region_bsp_get(visible->bsp_index);
    structure_instanced_geometry_instance *instance = (structure_instanced_geometry_instance *)tag_block_get_element_with_size(
        &bsp->instanced_geometry_instances, visible->instance_index, sizeof(structure_instanced_geometry_instance));
    structure_instanced_geometry_definition *definitions = visibility_collection_structure_collision_definitions_get(visible->bsp_index);
    const s_render_geometry *geometry = g_render_structure_globals.render_geometry[visible->bsp_index];
    s_mesh *mesh = (s_mesh *)tag_block_get_element_with_size(
        &geometry->meshes, (unsigned short)definitions[instance->definition_index].mesh_index, sizeof(s_mesh));
    if (mesh->parts.count > 0)
    {
        visible->visible_parts = get_part_bitvector_space(mesh->parts.count);
        if (visible->visible_parts)
        {
            memset(visible->visible_parts, -1, ((mesh->parts.count + 31) >> 5) * sizeof(unsigned long));
            success = true;
        }
    }
    if (success)
        visible->flags |= 8;
    else
        visible->flags &= ~8;
}

// @address 0x156ed0 ?compute_instance_alpha@c_visibility_collection@@KAMPAUstructure_instanced_geometry_instance@@J@Z
float c_visibility_collection::compute_instance_alpha(structure_instanced_geometry_instance *geometry,long bsp_index){
 c_player_view *view=c_player_view::get_current();
 long fade_start=instance_default_fade_start_pixels;
 long fade_end=instance_default_fade_end_pixels;
 float alpha=1.0f;
 scenario_structure_bsp_reference *structure = (scenario_structure_bsp_reference *)tag_block_get_element_with_size(
     &global_scenario->structure_bsps, bsp_index, sizeof(scenario_structure_bsp_reference));
 if(structure->fade_start>0 && structure->fade_start>structure->fade_end){fade_start=structure->fade_start;fade_end=structure->fade_end;}
 if(geometry->fade_start>0 && geometry->fade_start>=geometry->fade_end){fade_start=geometry->fade_start;fade_end=geometry->fade_end;}
 if(fade_start>0 && fade_start>=fade_end){
  float scale=PIN(g_player_render_tuning.instance_fade_scale,0.001f,1.0f);
  if(view){
   float fade_range=(float)(fade_start-fade_end);
   long diameter=(long)render_camera_compute_projected_diameter(view->get_render_projection(),
       &geometry->world_bounding_sphere_center,geometry->world_bounding_sphere_radius);
   diameter=(long)((float)diameter*scale);
   if(fade_start!=fade_end) alpha=PIN((float)(diameter-fade_end)/fade_range,0.0f,1.0f);
   else alpha=diameter>=fade_start?1.0f:0.0f;
  }
 }
 return alpha;
}

// @address 0x2aff48 ?convert_part_keys_to_bitvector@@YA_NPBUs_render_geometry@@JPAKJJAAV?$phys_array@I@@2@Z
bool convert_part_keys_to_bitvector(const s_render_geometry *render_geometry, long mesh_index, unsigned long *part_visibility_bitvector,
    long part_offset, long row, phys_array<unsigned int> &hits, phys_array<unsigned int> &part_indices)
{
    s_per_mesh_mopp *mopp = (s_per_mesh_mopp *)tag_block_get_element_with_size(
        &render_geometry->per_mesh_mopp, mesh_index, sizeof(s_per_mesh_mopp));
    bool result = false;
    if (hits.getSize() > 0 || part_indices.getSize() > 0)
        result = true;
    for (long key_count = 0; key_count < hits.getSize(); key_count++) {
        unsigned short start_index = *visibility_collection_structure_mesh_get_mopp_part_reorder_element(
            mopp, part_offset, row, (unsigned short)hits[key_count]);
        unsigned short end_index = *visibility_collection_structure_mesh_get_mopp_part_reorder_element(
            mopp, part_offset, row, (unsigned short)hits[key_count + 1]);
        for (long part_index = start_index; part_index <= end_index; part_index++) {
            unsigned short real_part_index = *visibility_collection_structure_mesh_get_mopp_part_order_element(mopp, part_offset, row, (short)part_index);
            part_visibility_bitvector[real_part_index >> 5] |= 1 << (real_part_index & 31);
        }
        key_count++;
    }
    for (long index = 0; index < part_indices.getSize(); index++) {
        long part_index = part_indices[index];
        part_visibility_bitvector[part_index >> 5] |= 1 << (part_index & 31);
    }
    return result;
}

#if !defined(_WIN64) && !defined(__LP64__)
typedef char visible_instances_subparts_offset[(offsetof(s_visible_instances, visible_parts) == 12) ? 1 : -1];
typedef char structure_globals_sequence_offset[(offsetof(s_structure_globals, cluster_marker_sequence) == 4) ? 1 : -1];
typedef char structure_globals_instance_sequence_offset[(offsetof(s_structure_globals, instanced_geometry_marker_sequence) == 0x400c) ? 1 : -1];
#endif

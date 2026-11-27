// blamlibxbox/visibility_collection.obj: from the records 0x1493c0 0x153d28 0x154818 0x154960 0x156b40 0x157a88 0x172e70 0x17d478 0x222db8 0x222e18 0x222e78
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "cseries/long_sort.h"
#include "math/real_math.h"
#include "visibility/visibility_projection_types.h"
#include "tag_files/tag_types.h"
#include "objects/object_reference_types.h"
#include <string.h>
#include <math.h>
#include "scenario/scenario.h"
#include "visibility/visibility_projections_and_volumes.h"
#include "objects/object_datum.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "visibility/visibility_portal_traversal.h"
#include "structures/structures.h"
#include "visibility/visibility_collection_types.h"
#include "visibility/visible_items.h"
#include "visibility/visible_items_types.h"
#include "visibility/visibility_collection.h"
#include "visibility/visibility_region_types.h"
#include "objects/object_definitions.h"
#include "visibility/visibility_globals.h"

// @address 0x9e7ea0 g_visibility_globals
#if !defined(_XBOX) || defined(__clang__)
s_visibility_globals g_visibility_globals;
#endif

// Portable builds define the local random seed here (zero, as in the shipped image).
#if !defined(_XBOX) || defined(__clang__)
extern "C" {
unsigned long g_local_random_seed = 0;
}
#endif

// @address 0x745d30 ?instance_default_fade_start_pixels@@3JA
long instance_default_fade_start_pixels = 36;
// @address 0x745d34 ?instance_default_fade_end_pixels@@3JA
long instance_default_fade_end_pixels = 30;

// Static globals owned by this unit: the marker count starts at -1 and each
// m_items list header starts with its capacity; the rest is zero-initialized.
// @address 0x761558 ?m_items@c_visible_items@@2Us_visible_items@@A
s_visible_items c_visible_items::m_items = { {768}, {896}, {1024}, {1152}, {348}, {50}, {6} };
// @address 0x745d38 ?m_marker_count@c_visible_items@@0JA
long c_visible_items::m_marker_count = -1;
// @address 0x7e88b8 ?m_markers@c_visible_items@@0PAUs_marker_indices@1@A
c_visible_items::s_marker_indices c_visible_items::m_markers[6];
extern "C" {
// @address 0x7ea8d4 g_scripted_player_effect_screen_fade_color
real_rgb_color g_scripted_player_effect_screen_fade_color;
}

__forceinline void c_visible_items::clear()
{
    memset(m_items.instance_visibility, 0, m_items.instance_visibility_count * sizeof(long));
    m_items.hierarchies.clear();
    m_items.objects.clear();
    m_items.instance_lists.clear();
    m_items.instances.clear();
    m_items.clusters.clear();
    m_items.objects_to_render.clear();
    m_items.skies.clear();
    m_items.instance_visibility_count = 0;
    m_items.instance_visibility_bit_count = 0;
    m_marker_count = -1;
}
void c_visible_items::clear_all()
{
    clear();
}
void c_visible_items::push_marker()
{
    ++m_marker_count;
    m_markers[m_marker_count].clusters = (unsigned short)m_items.clusters.get_count();
    m_markers[m_marker_count].instance_lists = (unsigned short)m_items.instance_lists.get_count();
    m_markers[m_marker_count].instances = (unsigned short)m_items.instances.get_count();
    m_markers[m_marker_count].objects_to_render = (unsigned short)m_items.objects_to_render.get_count();
    m_markers[m_marker_count].objects = (unsigned short)m_items.objects.get_count();
    m_markers[m_marker_count].root_objects_starting_index = (unsigned short)m_items.hierarchies.get_count();
    m_markers[m_marker_count].skies = (unsigned short)m_items.skies.get_count();
    m_markers[m_marker_count].instance_visibility_count = m_items.instance_visibility_count;
    m_markers[m_marker_count].instance_visibility_bit_count = m_items.instance_visibility_bit_count;
}
// @address 0x156b40 ?sphere_is_visible@c_visibility_collection@@IAA_NPBTreal_point3d@@MF_NPA_N22@Z
bool c_visibility_collection::sphere_is_visible(const real_point3d *center, float radius, short cluster_index, bool unused,
    // match: 0x156b40: pointer __restrict qualifier prevents alias reload
    bool *__restrict fully_contained, bool *__restrict lit, bool *__restrict shadow_casting)
{
    s_visibility_region_build *region = m_region;
    bool result = true;
    *shadow_casting = true;
    *lit = true;
    *fully_contained = false;
    if (region->collection_shape == _collection_shape_0)
    {
        result = visibility_region_test_sphere(region, cluster_index, center, radius, fully_contained);
    }
    else if (region->collection_shape == _collection_shape_1)
    {
        result = point_in_sphere(&region->sphere_center, center, region->sphere_radius + radius);
        if (result)
            *fully_contained = distance3d(center, &region->sphere_center) + radius <= region->sphere_radius;
    }
    return result;
}
// @address 0x154960 ?prepare_collection_for_build@c_visibility_collection@@IAA?AW4e_collection_shape@@JW4e_collection_type@@PBUvisibility_projection@@JUs_cluster_reference@@JPBTreal_point3d@@MJJ@Z
e_collection_shape c_visibility_collection::prepare_collection_for_build(long flags, e_collection_type collection_type,
    const visibility_projection *projections, long projection_count, s_cluster_reference cluster_reference, long unused,
    const real_point3d *point, float sphere_radius, long option0, long option1)
{
    this->m_region->sphere_cluster_reference = cluster_reference;
    this->m_region->collection_type = collection_type;
    this->m_region->option0 = option0;
    long marker_index = c_visible_items::get_marker_index();
    this->m_region->option1 = option1;
    this->m_region->flags = flags;
    this->m_region->original_projection_count = (short)projection_count;
    this->m_region->sphere_center = *point;
    this->m_region->sphere_radius = sphere_radius;
    this->m_region->marker_index = marker_index;
    if (projections != this->m_region->projections)
        memcpy(this->m_region->projections, projections, projection_count * sizeof(visibility_projection));
    this->m_region->projection_count = (short)projection_count;
    if ((unsigned char)cluster_reference.bsp_index == 255)
    {
        s_location location;
        scenario_location_from_point(&location, point);
        this->m_region->sphere_cluster_reference = location.cluster_reference;
    }
    if (this->m_region->collection_type == 0 && (this->m_region->flags & 64))
        this->m_region->collection_shape = 2;
    else if (projection_count == 0 || (sphere_radius < 3.0f && !(this->m_region->flags & 128)))
        this->m_region->collection_shape = 1;
    else
        this->m_region->collection_shape = 0;
    return (e_collection_shape)this->m_region->collection_shape;
}
// @address 0x157a88 ?get_part_bitvector_space@c_visibility_collection@@SAPAKJ@Z
unsigned long *c_visibility_collection::get_part_bitvector_space(long bit_count)
{
    unsigned long *result = 0;
    if (bit_count > 0)
    {
        long word_count = (bit_count + 31) >> 5;
        result = (unsigned long *)c_visible_items::m_items.instance_visibility;
        long used = c_visible_items::m_items.instance_visibility_count;
        if (used + word_count < 512)
        {
            c_visible_items::m_items.instance_visibility_count = (unsigned short)(used + word_count);
            result = (unsigned long *)&c_visible_items::m_items.instance_visibility[used];
        }
    }
    return result;
}
void c_visible_items::pop_marker()
{
    if (m_marker_count >= 0)
    {
        m_items.clusters.set_count(m_markers[m_marker_count].clusters);
        m_items.instance_lists.set_count(m_markers[m_marker_count].instance_lists);
        m_items.instances.set_count(m_markers[m_marker_count].instances);
        m_items.objects_to_render.set_count(m_markers[m_marker_count].objects_to_render);
        m_items.objects.set_count(m_markers[m_marker_count].objects);
        m_items.hierarchies.set_count(m_markers[m_marker_count].root_objects_starting_index);
        m_items.skies.set_count(m_markers[m_marker_count].skies);
        m_items.instance_visibility_count = m_markers[m_marker_count].instance_visibility_count;
        m_items.instance_visibility_bit_count = m_markers[m_marker_count].instance_visibility_bit_count;
        --m_marker_count;
    }
}
namespace {
static __forceinline void visibility_globals_initialize(s_visibility_globals *globals)
{
    globals->initialized = true;
    memset(globals, 0, offsetof(s_visibility_globals, initialized));
}
static __forceinline s_visibility_globals *visibility_globals_get(s_visibility_globals *globals)
{
    return globals;
}
static __forceinline void visibility_collection_set_input(s_visibility_globals *globals, s_visibility_input *input)
{
    globals->collection_input = input;
}
}
void visibility_collection_initialize()
{
    visibility_globals_initialize(&g_visibility_globals);
    s_visibility_globals *globals = visibility_globals_get(&g_visibility_globals);
    visibility_collection_set_input(visibility_globals_get(&g_visibility_globals), &globals->input);
}

// The March release map folds this empty game-system callback into the shared blr at 0x5bec98.
// @address 0x5bec98 ?visibility_collection_dispose@@YAXXZ
void visibility_collection_dispose()
{
}
// @address 0x222e18 ?compare_region_cluster_indices@@YA_NJJPBUs_visibility_region@@@Z
static bool compare_region_cluster_indices(long index1,long index2,const s_visibility_region *region)
{
    const visibility_cluster *cluster1=&region->clusters[index1];
    const visibility_cluster *cluster2=&region->clusters[index2];
    return cluster1->cluster_reference.bsp_index != cluster2->cluster_reference.bsp_index
        ? cluster1->cluster_reference.bsp_index > cluster2->cluster_reference.bsp_index
        : cluster1->cluster_reference.cluster_index > cluster2->cluster_reference.cluster_index;
}
namespace {
static __forceinline visibility_cluster *visibility_collection_region_get_cluster(s_visibility_region *region, long cluster_index)
{
    return &region->clusters[cluster_index];
}
static __forceinline void visibility_collection_qsort_4byte(void *elements, unsigned int count, bool (*compare)(long, long, const void *), const void *context)
{
    if (count >= 2)
        templated_sort<long>::qsort((long *)elements, count, compare, context);
}
}
// @address 0x17d478 ?postprocess_filter_isolate_shadowing_object@c_visibility_collection@@SA_NJ_N@Z
bool c_visibility_collection::postprocess_filter_isolate_shadowing_object(long object_index, bool isolate)
{
    bool result = false;
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, NONE);
    const s_object_definition *definition = TAG_GET('obje', const s_object_definition, object->definition_index);
    if (definition->model.index != -1)
    {
        long iterator;
        for (s_object_hierarchy *root =
                 c_visible_items::m_items.hierarchies.list_iterator_new(&iterator, c_visible_items::get_root_objects_starting_index());
            root; root = c_visible_items::m_items.hierarchies.list_iterator_next(&iterator))
        {
            if ((root->flags & 8) && root->object_count > 0)
            {
                s_object_visibility &visibility = c_visible_items::m_items.objects[root->first_object];
                if (visibility.render_setup.object_index == object_index)
                {
                    result = true;
                    root->flags |= 2;
                    if (isolate)
                        root->flags |= 1;
                }
                else
                {
                    root->flags &= ~2;
                    root->flags |= 1;
                }
            }
        }
        for (s_visible_clusters *cluster =
                 c_visible_items::m_items.clusters.list_iterator_new(&iterator, c_visible_items::get_cluster_starting_index());
            cluster; cluster = c_visible_items::m_items.clusters.list_iterator_next(&iterator))
        {
            cluster->flags &= ~2;
            cluster->flags |= 1;
        }
        for (s_visible_instances *instance =
                 c_visible_items::m_items.instances.list_iterator_new(&iterator, c_visible_items::get_instances_starting_index());
            instance; instance = c_visible_items::m_items.instances.list_iterator_next(&iterator))
        {
            instance->flags &= ~2;
            instance->flags |= 1;
        }
    }
    return result;
}
// @address 0x222e78 ?setup_sphere_region@c_visibility_collection@@IAAXXZ
void c_visibility_collection::setup_sphere_region()
{
    s_cluster_reference cluster_references[256];
    s_cluster_reference *refs = cluster_references;
    long count = structure_clusters_in_sphere(m_region->sphere_cluster_reference, &m_region->sphere_center,
        m_region->sphere_radius, 256, cluster_references, 0);
    get_sphere_region()->cluster_count = (short)count;
    for (long cluster_index = 0; cluster_index < get_sphere_region()->cluster_count; cluster_index++)
    {
        visibility_cluster *cluster = visibility_collection_region_get_cluster(get_sphere_region(), cluster_index);
        s_cluster_reference reference = refs[cluster_index];
        cluster->cluster_reference = reference;
        cluster->volume_count[0] = 0;
        get_sphere_region()->cluster_indices[cluster_index] = cluster_index;
    }
    visibility_collection_qsort_4byte(get_sphere_region()->cluster_indices, get_sphere_region()->cluster_count,
        (bool (*)(long, long, const void *))compare_region_cluster_indices, get_sphere_region());
}
// @address 0x154818 ?compute@c_visibility_collection@@QAAPAV1@W4e_collection_type@@JPBUvisibility_projection@@JPBTreal_point3d@@MJUs_cluster_reference@@JJ@Z
c_visibility_collection *c_visibility_collection::compute(e_collection_type collection_type, long flags,
    const visibility_projection *projections, long projection_count, const real_point3d *point, float separation, long unused,
    s_cluster_reference cluster, long option0, long option1)
{
    unsigned shape = prepare_collection_for_build(flags, collection_type, projections, projection_count, cluster, unused, point, separation, option0, option1);
    for (long i = 0; i < 16; i++)
    {
        long *clear = (long *)(i * 32 + (char *)m_region + offsetof(s_visibility_region_build, visible_cluster_bits));
        long clear_count = 8;
        do
        {
            *clear++ = 0;
        } while (--clear_count != 0);
        signed char *set = (signed char *)(i * 256 + (char *)m_region + offsetof(s_visibility_region_build, visible_cluster_region));
        long set_count = 256;
        do
        {
            *set++ = -1;
        } while (--set_count != 0);
    }
    switch (shape)
    {
    case 0:
        visibility_build_region_from_projections(projection_count, projections, m_region->sphere_cluster_reference, m_region->collection_type == 0,
            (s_visibility_region *)m_region, ((unsigned long)m_region->flags >> 8) & 1);
        break;
    case 2:
        visibility_build_region_from_pvs(projections, cluster, (s_visibility_region *)m_region);
        break;
    case 1:
        setup_sphere_region();
        break;
    }
    collect_structure_elements_from_regions();
    collect_objects_and_lights_from_regions();
    return this;
}

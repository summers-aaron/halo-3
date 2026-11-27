// blamlibxbox/visibility_collection_objects.obj: from the records 0x158d60 0x1591f0 0x159590 0x15c180 0x162750
// @flags /TP /O2 /Oc /noGL
#include "objects/light_globals.h"
#include "math/real_math.h"
#include "objects/object_cluster_globals.h"
#include "memory/data_types.h"
#include "memory/datum_index.h"
#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
#include "objects/object_header.h"
#include "memory/static_arrays.h"
#include <string.h>
#include "objects/lights.h"
#include "render/render_objects_static_lighting.h"
#include "render/render_objects_static_lighting_hierarchy_levels_types.h"
#include "visibility/visibility_collection_frame_types.h"
#include "visibility/visibility_collection_types.h"
#include "math/real_math_types.h"
#include "objects/light_geometry_types.h"
#include "structures/cluster_partition_types.h"
#include "visibility/visible_items_types.h"
#include "visibility/visible_items.h"
#include "visibility/visibility_collection.h"
#include "visibility/visibility_region_types.h"
#include "objects/light_datum.h"
#include "objects/object_marker_globals_types.h"
#include "render/render_visibility_collection_imports.h"
#include "objects/object_marker_access.h"
#include "objects/object_cluster_payload.h"
#include "visibility/visibility_collection_objects_imports.h"
#include "render/views/lightmap_shadows_view.h"

union visibility_position_words
{
    real_point3d point;
    long words[3];
};

// @address 0x7fc358 g_visibility_collection_frame
extern "C" {
s_visibility_collection_frame g_visibility_collection_frame = {};
}

// @address 0x162750 ?generate_objects_visible_subparts@c_visibility_collection@@KAXXZ


void c_visibility_collection::generate_objects_visible_subparts()
{
    long iterator;
    s_object_hierarchy *hierarchy = c_visible_items::get_global_items().hierarchies.list_iterator_new(
        &iterator, c_visible_items::get_root_objects_starting_index());
    for (; hierarchy; hierarchy = c_visible_items::get_global_items().hierarchies.list_iterator_next(&iterator))
    {
        long first_object = hierarchy->first_object;
        long object_end = first_object + hierarchy->object_count;
        for (long object_index = first_object; object_index < object_end; object_index++)
        {
            s_object_visibility &visibility = c_visible_items::get_global_items().objects[object_index];
            visibility.render_setup.render_data = 0;
            hierarchy->flags |= 8;
        }
    }
}

// @address 0x7f4310 g_object_marker_globals

static __forceinline long *visibility_collection_objects_light_mark_slot(long light_index)
{
    return &g_nondeterministic_light_data[light_index].visibility_mark;
}
static __forceinline long visibility_collection_objects_light_marker()
{
    return g_visibility_collection_frame.light_mark;
}
static __forceinline float visibility_collection_objects_magnitude_squared_k_first(const real_vector3d *vector)
{
    float i_squared = vector->i * vector->i;
    float k_squared = vector->k * vector->k;
    float j_squared = vector->j * vector->j;
    return i_squared + k_squared + j_squared;
}
static __forceinline float visibility_collection_objects_distance_squared_k_first(const real_point3d *first, const real_point3d *second)
{
    real_vector3d displacement;
    return visibility_collection_objects_magnitude_squared_k_first(vector_from_points3d(first, second, &displacement));
}
static __forceinline bool visibility_collection_objects_point_in_sphere_k_first(const real_point3d *point, const real_point3d *center, float radius)
{
    return visibility_collection_objects_distance_squared_k_first(point, center) <= radius * radius;
}
// @address 0x15c180 ?light_try_and_add_to_collection@c_visibility_collection@@IAAXJJ_N@Z
void c_visibility_collection::light_try_and_add_to_collection(long light_index, long cluster_index, bool skip_test)
{
    long index = datum_index_to_absolute_index(light_index);
    long * const mark = visibility_collection_objects_light_mark_slot(index);
    if (*mark == g_visibility_collection_frame.light_mark)
        return;
    const long marker = g_visibility_collection_frame.light_mark;
    s_visibility_region_build *input = this->m_region;
    if (!light_theoretically_produces_light(light_index, input->option0, input->option1))
    {
        if (*mark != marker) *mark = marker;
        return;
    }
    bool visible = true;
    light_datum *light = &((light_datum *)light_data->data)[index];
    long collection_shape = input->collection_shape;
    real_point3d center;
    float radius;
    bool inside = false;
    radius = light->bounding_sphere_radius;
    center = light->bounding_sphere_center;
    if (collection_shape == 0)
        visible = visibility_region_test_sphere((s_visibility_region *)input, cluster_index, &center, radius, &inside);
    else if (collection_shape == 1)
        visible = visibility_collection_objects_point_in_sphere_k_first(&input->sphere_center, &center, input->sphere_radius + radius);
    if (!visible && !(this->m_region->flags & (1 << 6)))
        return;
    simple_list<long, 50> &list = c_visible_items::get_global_items().objects_to_render;
    long entry = list.add();
    if (entry == -1)
        return;
    list[entry] = light_index;
    if (*mark != marker) *mark = marker;
}

static __forceinline s_object_marker_globals *visibility_collection_objects_get_object_marker_globals()
{
    return &g_object_marker_globals[get_current_thread_index() - 1];
}
static __forceinline bool visibility_collection_objects_object_unmarked(long object_index)
{
    s_object_marker_globals *markers = visibility_collection_objects_get_object_marker_globals();
    return markers->object_marks[datum_index_to_absolute_index(object_index)] != markers->mark;
}
static __forceinline bool visibility_collection_objects_object_mark_get_user_data(long object_index, long *user_data)
{
    s_object_marker_globals *markers = visibility_collection_objects_get_object_marker_globals();
    long absolute_index = datum_index_to_absolute_index(object_index);
    if (markers->object_marks[absolute_index] != markers->mark)
        return false;
    *user_data = markers->mark_data[absolute_index];
    return true;
}
__forceinline e_collection_type c_visibility_collection::get_collection_type()
{
    return (e_collection_type)m_region->collection_type;
}
__forceinline void c_visibility_collection::region_cluster_bitvector_add_index(short region_index, short cluster_index)
{
    c_visible_items::get_global_items().region_cluster_bitvectors[region_index][cluster_index >> 5] |= FLAG(cluster_index & 31);
}
static __forceinline bool visibility_collection_objects_is_object_shadow_visible(long object_index)
{
    const unsigned long &shadow_visible = c_lightmap_shadows_view::object_shadow_visible(object_index, 0, 0);
    return shadow_visible != 0;
}
// @address 0x1591f0 ?object_try_and_add_to_collection@c_visibility_collection@@IAAXJJPBUs_object_cluster_payload@@@Z
void c_visibility_collection::object_try_and_add_to_collection(long object_index, long cluster_index, const s_object_cluster_payload *payload)
{
    if (visibility_collection_objects_object_unmarked(object_index))
    {
        bool lit = true;
        bool shadow = true;
        bool visible = true;
        bool fully_contained = true;
        render_objects_static_lighting_hierarchy_levels alphas;
        alphas.values[1] = 255;
        alphas.values[2] = 255;
        alphas.values[0] = 255;
        const unsigned long &payload_flags = payload->flags;
        bool cached_alpha = !TEST_BIT(payload_flags, 1);
        bool result = object_is_visible(object_index, cluster_index, &lit, &shadow, &visible, &fully_contained, payload);
        __dcbt(0, object_get(object_index));
        if (!TEST_BIT(payload->flags, 1) && get_collection_type() == _collection_type_0 && !result && shadow && visible)
        {
            render_object_calculate_alphas(object_index, &payload->bounding_sphere_center, payload->bounding_sphere_radius,
                payload->object_type, TEST_BIT(payload->flags, 2), cached_alpha, &alphas);
            const unsigned long &shadow_alpha = alphas.values[2];
            result = shadow_alpha != 0 && visibility_collection_objects_is_object_shadow_visible(object_index);
            lit = false;
        }
        if (result)
        {
            if (get_collection_type() == _collection_type_0 && lit)
                render_object_calculate_alphas(object_index, &payload->bounding_sphere_center, payload->bounding_sphere_radius,
                    payload->object_type, TEST_BIT(payload->flags, 2), cached_alpha, &alphas);
            if (alphas.values[0] != 0)
            {
                bool add = get_collection_type() == _collection_type_0 ||
                    ((object_datum *)object_get_and_verify_type(object_index, -1))->object_identifier.get_source() != 4;
                if (add)
                    render_objects_add_visible_hierarchy(object_index, &payload->bounding_sphere_center, payload->bounding_sphere_radius,
                        m_region->option1, lit, shadow, fully_contained, -1, alphas, get_collection_type() == _collection_type_0, true,
                        m_region->option0);
            }
        }
    }
    else
    {
        long user_data;
        visibility_collection_objects_object_mark_get_user_data(object_index, &user_data);
        s_object_hierarchy &hierarchy = c_visible_items::get_global_items().hierarchies[user_data];
        if (!TEST_BIT(hierarchy.flags, 2) && hierarchy.region_cluster_bitvector_index != 0xffff)
            region_cluster_bitvector_add_index(hierarchy.region_cluster_bitvector_index, (short)cluster_index);
        if (get_collection_type() == _collection_type_0 && !TEST_BIT(hierarchy.flags, 0))
        {
            bool lit = true;
            bool shadow = true;
            bool visible = true;
            bool fully_contained = true;
            if (object_is_visible(object_index, cluster_index, &lit, &shadow, &visible, &fully_contained, payload))
                hierarchy.flags = (lit ? 1 : 0) | (shadow ? 2 : 0) | (fully_contained ? 4 : 0);
        }
    }
}

// @address 0x159590 ?object_is_visible@c_visibility_collection@@IAA_NJJPA_N000PBUs_object_cluster_payload@@@Z
// match: 0x159590: the caller retains lit in r24 and shadow in r21 across visibility tests when both outputs are restricted.
bool c_visibility_collection::object_is_visible(long object_index, long cluster_index, bool *__restrict lit, bool *__restrict shadow,
    bool *visible, bool *fully_contained, const s_object_cluster_payload *payload)
{
    bool inside_out;
    visibility_position_words center;
    float radius;
    bool result;
    {
        visibility_position_words visible_center;
        radius = payload->bounding_sphere_radius;
        memcpy(center.words, payload->center_words, sizeof(center.words));
        long collection_type = m_region->collection_type;
        visible_center.words[0] = center.words[0];
        visible_center.words[1] = center.words[1];
        visible_center.words[2] = center.words[2];
        if (collection_type != 0)
        {
            object_datum *object = (object_datum *)object_get_and_verify_type(object_index, -1);
            radius = object->dynamic_light_sphere_radius;
            center.point = object->dynamic_light_sphere_center;
            visible_center.point = center.point;
        }
        // match: 0x159590: passing cluster_index directly assigns shadow to r25 at the sphere_is_visible call.
        const long &cluster = (long)_rotl(cluster_index, 0);
        const unsigned long &sphere_result = sphere_is_visible(&visible_center.point, radius, (short)cluster, false,
            fully_contained, lit, shadow);
        result = (unsigned char)sphere_result || (m_region->flags & 64);
    }
    const s_visibility_region_build *region = m_region;
    const s_visibility_region *const &shadow_region = region;
    bool shadow_flag = ((unsigned int)~payload->flags >> 1) & 1;
    // match: 0x159590: rotating the field directly moves the first center word from r27 to r28.
    const unsigned long &region_type = (unsigned long)region->collection_type;
    // match: 0x159590: bypassing this binding makes sphere_is_visible receive shadow from r25.
    const long &col_type_1 = _rotl(region_type, 32);
    // match: 0x159590: bypassing this binding moves the shadow flag store from 0(r28) to 0(r25).
    const long &col_type_2 = _rotl(col_type_1, 32);
    // match: 0x159590: testing the preceding long directly moves the shadow center stores to r28/r27/r26.
    const e_collection_type &collection_type = (e_collection_type)_rotl(col_type_2, 0);
    *shadow = shadow_flag;
    if (collection_type == _collection_type_0 && shadow_flag)
    {
        bool shadow_visible;
        if (result)
            shadow_visible = true;
        else
        {
            real_point3d shadow_center = center.point;
            shadow_visible = visibility_region_test_sphere(shadow_region, cluster_index, &shadow_center, radius * 3.0f, &inside_out);
        }
        *visible = shadow_visible;
    }
    return result;
}

static __forceinline const long *visibility_collection_objects_cluster_partition_get_structure_heads(const cluster_partition *partition, long bsp_index)
{
    return &partition->cluster_heads[bsp_index * 256];
}
static __forceinline const long *visibility_collection_objects_cluster_partition_get_cluster_head(
    const cluster_partition *partition, s_cluster_reference cluster)
{
    const long *heads = visibility_collection_objects_cluster_partition_get_structure_heads(partition, cluster.bsp_index);
    return &heads[cluster.cluster_index];
}
static __forceinline long visibility_collection_objects_cluster_partition_get_next_datum(const cluster_partition *partition, long *iterator)
{
    const void *payload;
    s_data_array *array = partition->data;
    long datum_index;
    if (*iterator != -1)
    {
        const s_cluster_partition_element *element = (const s_cluster_partition_element *)datum_get(array, *iterator);
        long next = element->next;
        payload = element + 1;
        if (next != -1)
            __dcbt(0, datum_get(array, next));
        datum_index = element->datum_index;
        *iterator = next;
    }
    else
    {
        datum_index = -1;
    }
    return datum_index;
}
static __forceinline long visibility_collection_objects_cluster_partition_get_first_datum(
    const cluster_partition *partition, long *iterator, s_cluster_reference cluster)
{
    const void *payload;
    *iterator = *visibility_collection_objects_cluster_partition_get_cluster_head(partition, cluster);
    s_data_array *array = partition->data;
    long datum_index;
    if (*iterator != -1)
    {
        const s_cluster_partition_element *element = (const s_cluster_partition_element *)datum_get(array, *iterator);
        long next = element->next;
        payload = element + 1;
        if (next != -1)
            __dcbt(0, datum_get(array, next));
        datum_index = element->datum_index;
        *iterator = next;
    }
    else
    {
        datum_index = -1;
    }
    return datum_index;
}
static __forceinline long cluster_get_first_collideable_object_and_payload(
    long *iterator, s_cluster_reference cluster, const s_object_cluster_payload **payload)
{
    return cluster_partition_get_first_datum_and_payload(
        &tls_collideable_object_cluster_partition(), iterator, cluster, sizeof(s_object_cluster_payload), (const void **)payload);
}
static __forceinline long cluster_get_next_collideable_object_and_payload(long *iterator, const s_object_cluster_payload **payload)
{
    const cluster_partition *partition = &tls_collideable_object_cluster_partition();
    const void *next_payload;
    s_data_array *array = partition->data;
    long datum_index;
    if (*iterator != -1)
    {
        const s_cluster_partition_element *element = (const s_cluster_partition_element *)datum_get(array, *iterator);
        long next = element->next;
        next_payload = element + 1;
        *payload = (const s_object_cluster_payload *)next_payload;
        if (next != -1)
            __dcbt(0, datum_get(array, next));
        datum_index = element->datum_index;
        *iterator = next;
    }
    else
    {
        datum_index = -1;
    }
    return datum_index;
}
static __forceinline long cluster_get_first_noncollideable_object_and_payload(
    long *iterator, s_cluster_reference cluster, const s_object_cluster_payload **payload)
{
    const cluster_partition *partition = &tls_noncollideable_object_cluster_partition();
    const void *next_payload;
    *iterator = *visibility_collection_objects_cluster_partition_get_cluster_head(partition, cluster);
    s_data_array *array = partition->data;
    long datum_index;
    if (*iterator != -1)
    {
        const s_cluster_partition_element *element = (const s_cluster_partition_element *)datum_get(array, *iterator);
        long next = element->next;
        next_payload = element + 1;
        *payload = (const s_object_cluster_payload *)next_payload;
        if (next != -1)
            __dcbt(0, datum_get(array, next));
        datum_index = element->datum_index;
        *iterator = next;
    }
    else
    {
        datum_index = -1;
    }
    return datum_index;
}
static __forceinline long cluster_get_next_noncollideable_object_and_payload(long *iterator, const s_object_cluster_payload **payload)
{
    const cluster_partition *partition = &tls_noncollideable_object_cluster_partition();
    const void *next_payload;
    s_data_array *array = partition->data;
    long datum_index;
    if (*iterator != -1)
    {
        const s_cluster_partition_element *element = (const s_cluster_partition_element *)datum_get(array, *iterator);
        long next = element->next;
        next_payload = element + 1;
        *payload = (const s_object_cluster_payload *)next_payload;
        if (next != -1)
            __dcbt(0, datum_get(array, next));
        datum_index = element->datum_index;
        *iterator = next;
    }
    else
    {
        datum_index = -1;
    }
    return datum_index;
}
static __forceinline long cluster_get_first_light(long *iterator, s_cluster_reference cluster)
{
    return visibility_collection_objects_cluster_partition_get_first_datum(&light_cluster_partition, iterator, cluster);
}
static __forceinline long cluster_get_next_light(long *iterator)
{
    return visibility_collection_objects_cluster_partition_get_next_datum(&light_cluster_partition, iterator);
}
static __forceinline void visibility_collection_objects_prefetch_memory(const void *base, unsigned long offset)
{
    __dcbt(offset, base);
}
static __forceinline void visibility_collection_objects_object_marker_begin()
{
    s_object_marker_globals *markers = visibility_collection_objects_get_object_marker_globals();
    markers->active = true;
    markers->mark++;
}
static __forceinline void visibility_collection_objects_object_marker_end()
{
    s_object_marker_globals *markers = visibility_collection_objects_get_object_marker_globals();
    markers->active = false;
}
static __forceinline bool visibility_collection_objects_object_mark(long object_index)
{
    s_object_marker_globals *markers = visibility_collection_objects_get_object_marker_globals();
    long absolute_index = datum_index_to_absolute_index(object_index);
    bool unmarked = markers->object_marks[absolute_index] != markers->mark;
    markers->object_marks[absolute_index] = markers->mark;
    return unmarked;
}
static __forceinline s_visibility_collection_frame *visibility_collection_objects_get_light_marker_globals()
{
    return &g_visibility_collection_frame;
}
static __forceinline void visibility_collection_objects_light_marker_begin()
{
    s_visibility_collection_frame *markers = visibility_collection_objects_get_light_marker_globals();
    markers->light_mark++;
    markers->active = true;
}
static __forceinline void visibility_collection_objects_light_marker_end()
{
    s_visibility_collection_frame *markers = visibility_collection_objects_get_light_marker_globals();
    markers->active = false;
}
static __forceinline const visibility_cluster *visibility_collection_objects_visibility_region_get_cluster(
    const s_visibility_region_build *region, long cluster_index)
{
    return &region->clusters[cluster_index];
}
static __forceinline simple_list<long, 6> &visibility_collection_objects_get_sky_objects()
{
    return c_visible_items::get_global_items().skies;
}
static const bool render_objects_enabled = true;
static const bool debug_objects = false;
// @address 0x158d60 ?collect_objects_and_lights_from_regions@c_visibility_collection@@IAAXXZ
void c_visibility_collection::collect_objects_and_lights_from_regions()
{
    visibility_collection_objects_object_marker_begin();
    visibility_collection_objects_light_marker_begin();
    for (long cluster_index = 0; cluster_index < m_region->cluster_count; cluster_index++)
    {
        const visibility_cluster *cluster = visibility_collection_objects_visibility_region_get_cluster(m_region, cluster_index);
        long iterator;
        const s_object_cluster_payload *payload;
        bool render_objects = render_objects_enabled || debug_objects;
        if (render_objects && !TEST_BIT(m_region->flags, 5))
        {
            for (long object_index = cluster_get_first_collideable_object_and_payload(&iterator, cluster->cluster_reference, &payload);
                object_index != -1;
                object_index = cluster_get_next_collideable_object_and_payload(&iterator, &payload))
            {
                if (!TEST_BIT(payload->flags, 0))
                {
                    visibility_collection_objects_prefetch_memory(
                        tls_object_header_data()->data, sizeof(s_object_header) * datum_index_to_absolute_index(object_index));
                    object_try_and_add_to_collection(object_index, cluster_index, payload);
                }
            }
            for (long object_index = cluster_get_first_noncollideable_object_and_payload(&iterator, cluster->cluster_reference, &payload);
                object_index != -1;
                object_index = cluster_get_next_noncollideable_object_and_payload(&iterator, &payload))
            {
                if (!TEST_BIT(payload->flags, 0))
                {
                    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, -1);
                    bool sky_object = object->object_identifier.get_source() == 4;
                    if (!sky_object)
                    {
                        object_try_and_add_to_collection(object_index, cluster_index, payload);
                    }
                    else if (visibility_collection_objects_object_unmarked(object_index) && get_collection_type() == _collection_type_0)
                    {
                        long sky_index = visibility_collection_objects_get_sky_objects().add();
                        if (sky_index != -1)
                        {
                            visibility_collection_objects_get_sky_objects()[sky_index] = object_index;
                            visibility_collection_objects_object_mark(object_index);
                        }
                    }
                }
            }
        }
        bool normal_collection = get_collection_type() == _collection_type_0;
        if (normal_collection && !TEST_BIT(m_region->flags, 3))
        {
            for (long light_index = cluster_get_first_light(&iterator, cluster->cluster_reference);
                light_index != -1;
                light_index = cluster_get_next_light(&iterator))
            {
                light_try_and_add_to_collection(light_index, cluster_index, false);
            }
        }
    }
    generate_objects_visible_subparts();
    visibility_collection_objects_object_marker_end();
    visibility_collection_objects_light_marker_end();
}


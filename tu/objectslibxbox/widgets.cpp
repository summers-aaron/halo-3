// objectslibxbox/widgets.obj: from the records 0xfc488 0x335830 0x335928 0x335a70 0x335bc0 0x335cd0 0x335dc0 0x335e48 0x335f20 0x336098 0x336128 0x336208 0x3362a8 0x55aea8
#include "geometry/geometry_sampling.h"
#include "ai/ai_tls_data.h"
// @flags /TP /O2 /J /Oc
#include "memory/data_types.h"
#include "memory/tls_data_arrays.h"
#include "math/matrix_math_types.h"
#include "math/real_math.h"
#include "tag_files/tag_types.h"
// @address 0x336098 ?set_real_quaternion@c_short_quaternion@@QAAXPAUreal_quaternion@@@Z
// @address 0x335928 ?widget_type_new_on_object@@YAXJFJ@Z
// @address 0x55aea8 ?__tls_set_g_widget_data_allocator@@YAXPAX@Z
#include <vectorintrinsics.h>
#include "cache/restricted_memory_regions.h"
#include "gpu_particle/particle_block.h"
#include "objects/widgets/widgets.h"
#include "objects/widgets/widget_types.h"
#include "memory/data.h"
#include "effects/transparent_sort_layer_types.h"
#include "cache/cache_files.h"
#include "render/render_objects_static_lighting.h"
#include "math/unit_vector_quantization.h"
#include "math/word_quantized_position.h"
#include "objects/object_header.h"
#include "math/matrix_math.h"
#include "cache/restricted_memory_layout.h"
#include "objects/widgets/widget_allocator.h"
#include "tag_files/tag_groups.h"

#include "math/short_quaternion.h"

#include "objects/object_definitions.h"
#include "cache/restricted_memory_registration.h"
#include "multithreading/thread_tls.h"

// @address 0x11df760 g_widget_data
__declspec(thread) s_data_array *g_widget_data;

static __forceinline s_widget_type *widgets_type_get(short type)
{
    return &widget_type_table()[type];
}
// @address 0x335830 ?widgets_dispose_from_old_structure_bsp@@YAXK@Z
void widgets_dispose_from_old_structure_bsp(unsigned long old_structure_bsp_mask)
{
    struct s_widgets_dispose_iterator
    {
        s_widget_datum *datum;
        s_data_iterator iterator;
    };
    struct s_widgets_dispose_query
    {
        long structure_index;
        s_location location;
    };
    s_widget_datum *datum;
    s_widgets_dispose_iterator iterator;
    iterator.iterator.absolute_index = -1;
    iterator.iterator.index = -1;
    iterator.iterator.data = g_widget_data;
    datum = (s_widget_datum *)data_iterator_next(&iterator.iterator);
    iterator.datum = datum;
    while (datum != 0)
    {
        if ((widget_type_table()[datum->type].flags & 4) != 0)
        {
            s_widget_type *type_definition = widgets_type_get(datum->type);
            if (type_definition->get_sorting_position != 0)
            {
                real_point3d position;
                s_widgets_dispose_query query;
                type_definition->get_sorting_position(datum->type_data, &position, &query.location, &query.structure_index);
                if (query.structure_index == -1 && (old_structure_bsp_mask & (1 << (signed char)query.location.cluster_reference.bsp_index)) != 0)
                {
                    type_definition->delete_widget(datum->type_data);
                    ::datum_delete(g_widget_data, iterator.iterator.index);
                }
            }
        }
        datum = (s_widget_datum *)data_iterator_next(&iterator.iterator);
        iterator.datum = datum;
    }
}

void c_short_quaternion::set_real_quaternion(real_quaternion *quaternion)
{
    quaternion_normalize(quaternion);
    i = (short)(quaternion->v.i * 32767.0f);
    j = (short)(quaternion->v.j * 32767.0f);
    k = (short)(quaternion->v.k * 32767.0f);
    w = (short)(quaternion->w * 32767.0f);
}

void widget_type_new_on_object(long object_index, short type, long definition_index)
{
    object_datum *object = object_get(object_index);
    if (type != -1 && definition_index != -1)
    {
        s_widget_type *type_definition = widgets_type_get(type);
        long widget_index = ::datum_new(g_widget_data);
        if (widget_index != -1)
        {
            s_widget_datum *widget = datum_get<s_widget_datum>(g_widget_data, widget_index);
            widget->type = type;
            if (type_definition->new_on_map)
            {
                real_matrix4x3 matrix;
                matrix4x3_identity(&matrix);
                widget->type_data = type_definition->new_on_map(definition_index, object_index, &matrix);
                if (widget->type_data != -1)
                {
                    widget->next_widget = object->first_widget_index;
                    object->first_widget_index = widget_index;
                }
                else
                {
                    ::datum_delete(g_widget_data, widget_index);
                }
            }
            else
            {
                widget->next_widget = object->first_widget_index;
                object->first_widget_index = widget_index;
                widget->type_data = -1;
            }
        }
    }
}

// @address 0x335a70 ?widget_delete_type_datum@@YAXJJ_N@Z
void widget_delete_type_datum(long object_index, long datum_index, bool delete_datum)
{
    if (datum_index != -1)
    {
        object_datum *object = object_get(object_index);
        long widget_index = object->first_widget_index;
        if (widget_index != -1)
        {
            s_widget_datum *widget = datum_get<s_widget_datum>(g_widget_data, widget_index);
            if (widget->type_data == datum_index)
            {
                s_widget_type *type_definition = widgets_type_get(widget->type);
                long next_widget_index = widget->next_widget;
                widget->next_widget = -1;
                object->first_widget_index = next_widget_index;
                if (delete_datum)
                {
                    if (widget->type_data != -1)
                        type_definition->delete_widget(widget->type_data);
                    ::datum_delete(g_widget_data, widget_index);
                }
            }
            else
            {
                while (widget_index != -1)
                {
                    s_widget_datum *current = datum_get<s_widget_datum>(g_widget_data, widget_index);
                    long next_widget_index = current->next_widget;
                    if (next_widget_index != -1)
                    {
                        s_widget_datum *next = datum_get<s_widget_datum>(g_widget_data, next_widget_index);
                        if (next->type_data == datum_index)
                        {
                            current->next_widget = next->next_widget;
                            next->next_widget = -1;
                            if (delete_datum)
                            {
                                s_widget_type *next_type_definition = widgets_type_get(next->type);
                                if (next->type_data != -1)
                                    next_type_definition->delete_widget(next->type_data);
                                ::datum_delete(g_widget_data, next_widget_index);
                            }
                            widget_index = -1;
                        }
                        else
                            widget_index = next_widget_index;
                    }
                    else
                        widget_index = -1;
                }
            }
        }
    }
}

// @address 0x335bc0 ?widget_submit_transparent@@YAXJ@Z
void widget_submit_transparent(long widget_index)
{
    struct s_widgets_submit_query
    {
        long structure_index;
        s_location location;
    };
    s_widget_datum *datum = datum_get<s_widget_datum>(g_widget_data, widget_index);
    s_widget_type *type_definition = widgets_type_get(datum->type);
    bool transparent = (widget_type_table()[datum->type].flags & 2) != 0 ||
        (type_definition->get_is_transparent != 0 && type_definition->get_is_transparent(datum->type_data) != 0);
    if (transparent && type_definition->render_transparent != 0 && type_definition->get_sorting_position != 0)
    {
        real_point3d position;
        s_widgets_submit_query query;
        type_definition->get_sorting_position(datum->type_data, &position, &query.location, &query.structure_index);
        render_objects_add_transparent_element(
            &position, 0, 0.0f, _transparent_sort_layer_normal, type_definition->render_transparent, 0, datum->type_data, 0.0f);
    }
}

// @address 0x335cd0 ?widgets_render_opaque@@YAXJJ@Z
void widgets_render_opaque(long object_index, long entry_point)
{
    long datum_index = object_get(object_index)->first_widget_index;
    while (datum_index != -1)
    {
        s_widget_datum *datum = datum_get<s_widget_datum>(g_widget_data, datum_index);
        s_widget_type *type_definition = widgets_type_get(datum->type);
        bool skip = (widget_type_table()[datum->type].flags & 2) != 0 ||
            (type_definition->get_is_transparent != 0 && type_definition->get_is_transparent(datum->type_data) != 0);
        if (!skip && type_definition->render_opaque != 0)
            type_definition->render_opaque(datum->type_data, entry_point);
        datum_index = datum->next_widget;
    }
}

// @address 0x335dc0 ?get_real_vector3d@c_dword_unit_vector@@QBAXPATreal_vector3d@@@Z
void c_dword_unit_vector::get_real_vector3d(real_vector3d *vector) const
{
    unsigned long packed = m_compressed;
    __vector4 shift_r = __vspltisw(-16);
    __vector4 unpacked = __vspltw(__lvlx(&packed, 0), 0);
    shift_r = __vsrw(__lvx(&k_dhenn3_unpack_shift, 0), shift_r);
    unpacked = __vslw(unpacked, __lvx(&k_dhenn3_unpack_shift, 0));
    unpacked = __vsraw(unpacked, shift_r);
    unpacked = __vcfsx(unpacked, 0);
    unpacked = __vmulfp(unpacked, __lvx(&k_dhenn3_unpack_scale, 0));
    vector->i = unpacked.x;
    vector->j = unpacked.y;
    vector->k = unpacked.z;
    normalize3d(vector);
}

void __tls_set_g_widget_data_allocator(void *allocator)
{
    data_set_new_base_address(&g_flocks_data, (s_data_array *)allocator);
}

// @address 0x335e48 ?get_real_point3d@c_word_quantized_position@@QBAXPBTreal_rectangle3d@@PATreal_point3d@@@Z
void c_word_quantized_position::get_real_point3d(const real_rectangle3d *bounds, real_point3d *point) const
{
    float x_extent = bounds->x1 - bounds->x0;
    float y_extent = bounds->y1 - bounds->y0;
    float z_extent = bounds->z1 - bounds->z0;

    ::set_real_point3d(point, bounds->x0, bounds->y0, bounds->z0);
    if (x_extent > 0.0f)
        point->x = bounds->x0 + x_extent * ((float)m_position[0] / 65535.0f);
    if (y_extent > 0.0f)
        point->y = bounds->y0 + y_extent * ((float)m_position[1] / 65535.0f);
    if (z_extent > 0.0f)
        point->z = bounds->z0 + z_extent * ((float)m_position[2] / 65535.0f);
}
#define WIDGETS_MAX(a, b) ((a) > (b) ? (a) : (b))
#define WIDGETS_MIN(a, b) ((a) < (b) ? (a) : (b))
#define WIDGETS_PIN(value, lower, upper) WIDGETS_MIN(WIDGETS_MAX(value, lower), upper)

// @address 0x335f20 ?set_real_point3d@c_word_quantized_position@@QAAXPBTreal_rectangle3d@@PBTreal_point3d@@@Z
void c_word_quantized_position::set_real_point3d(const real_rectangle3d *bounds, const real_point3d *point)
{
    float x_extent = bounds->x1 - bounds->x0;
    float y_extent = bounds->y1 - bounds->y0;
    float z_extent = bounds->z1 - bounds->z0;
    m_position[2] = 0;
    m_position[1] = 0;
    m_position[0] = 0;
    if (x_extent > 0.0f)
        m_position[0] = (unsigned short)(WIDGETS_PIN((point->x - bounds->x0) / x_extent, 0.0f, 1.0f) * 65535.0f);
    if (y_extent > 0.0f)
        m_position[1] = (unsigned short)(WIDGETS_PIN((point->y - bounds->y0) / y_extent, 0.0f, 1.0f) * 65535.0f);
    if (z_extent > 0.0f)
        m_position[2] = (unsigned short)(WIDGETS_PIN((point->z - bounds->z0) / z_extent, 0.0f, 1.0f) * 65535.0f);
}

__forceinline void c_short_quaternion::get_real_quaternion(real_quaternion *quaternion) const
{
    quaternion->v.i = (float)i / 32767.0f;
    quaternion->v.j = (float)j / 32767.0f;
    quaternion->v.k = (float)k / 32767.0f;
    quaternion->w = (float)w / 32767.0f;
    quaternion_normalize(quaternion);
}

// @address 0x336128 ?get_real_matrix4x3@c_short_quaternion@@QBAXPAUreal_matrix4x3@@@Z
void c_short_quaternion::get_real_matrix4x3(real_matrix4x3 *matrix) const
{
    real_quaternion quaternion;
    get_real_quaternion(&quaternion);
    matrix4x3_rotation_from_quaternion(matrix, &quaternion);
}

// the restricted-memory allocator family (region 3, layout 3): the template as camera_scripting.cpp declares it

template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_widget_data_allocator>::allocate(
    unsigned int size, const char *name)
{
    long member_index = restricted_memory_register_member(&g_restricted_regions[3], name, "", size, 0,
        __tls_set_g_particle_emitter_gpu_data_array_allocator, 0, 0);
    data_set_new_base_address(&g_widget_data, (s_data_array *)restricted_region_get_member_address(3, member_index));
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, member_index);
}

template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_widget_data_allocator>::deallocate(void *memory)
{
    g_widget_data = 0;
    restricted_region_free_member(3, m_member_index);
}

// the object's widgets, one widget_type_new_on_object per element of its definition's list

static __forceinline short widgets_type_index_from_id(unsigned long id)
{
    short index = -1;
    for (short j = 0; j < 4; j++)
    {
        if (widgets_type_get(j)->group_tag == id)
        {
            index = j;
            break;
        }
    }
    return index;
}

// @address 0xfc488 ?widgets_new@@YAXJ@Z
void widgets_new(long object_index)
{
    object_datum *object = object_get(object_index);
    s_object_definition *definition = TAG_GET('obje', s_object_definition, object->definition);
    object->first_widget_index = -1;
    for (short i = 0; i < definition->widgets.count; i++)
    {
        s_tag_reference *element = TAG_BLOCK_GET_ELEMENT(&definition->widgets, i, s_tag_reference);
        short type = widgets_type_index_from_id(element->group_tag);
        widget_type_new_on_object(object_index, type, element->index);
    }
}

typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_widget_data_allocator> t_widget_data_allocator;
// @address 0x79db70 ?g_widget_data_allocator@@3V?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_widget_data_allocator@@YAXPAX@Z@@A
t_widget_data_allocator g_widget_data_allocator;

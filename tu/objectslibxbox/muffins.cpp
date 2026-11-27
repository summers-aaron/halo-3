// objectslibxbox/muffins.obj: from the records 0x126ea0 0x242058 0x36beb8 0x36bed0 0x36bfb8 0x36c028 0x36c0d0 0x36c0e8 0x36c130 0x36c148 0x36c160 0x36c1b0 0x36c420 0x36c5e0 0x36c708 0x36cfe8 0x36d388 0x36d428
// @flags /TP /O2
#include <xtl.h>
#include "objects/object_header.h"
#include "multithreading/thread_tls.h"
#include "math/random_math.h"
#include "memory/data_types.h"
#include "tag_files/tag_index.h"
#include <string.h>
#include "game/game_time_types.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "math/matrix_math.h"
#include "tag_files/tag_groups.h"
#include "cache/restricted_memory_regions.h"
#include "objects/objects.h"
#include "models/model_skinning.h"
#include "objects/widgets/muffins.h"
#include "objects/widgets/widgets.h"
#include "objects/widgets/widget_types.h"
#include "render/render_mesh.h"
#include "render/render_objects_static_lighting.h"
#include "physics/havok_component.h"
#include "render/render_objects.h"
#include "memory/data.h"
#include "scenario/scenario.h"
#include "models/model_skinning_matrix_types.h"
#include "objects/widgets/muffin_definitions.h"
#include "models/render_model_definitions.h"
#include "render/render.h"
#include "rasterizer/rasterizer.h"
#include "effects/effects.h"
#include "objects/widgets/muffin_tls.h"
#include "objects/object_render_state_tls.h"
#include "geometry/compression_info_types.h"
#include "objects/widgets/muffins_render.h"
#include "render/render_visibility_cached_state_types.h"
#include "render_methods/render_method_submit.h"
#include "cache/restricted_memory_registration.h"
#include "effects/effect_creation.h"
#include "math/random_math_range.h"
#include "game/game_time_inline.h"
#include "objects/object_type_access.h"
#include "objects/object_markers_inline.h"
#include "objects/object_node_matrices_inline.h"
#include "memory/data_absolute_index.h"
#include "cache/render_model_region_types.h"
// @address 0x242058 ?__tls_set_g_muffin_data_allocator@@YAXPAX@Z

typedef s_visibility_cached_object_render_state muffins_render_state;

// match: 0x36c160: reads the slot directly; the inlined tls_muffin_data() accessor moves the register choice
// @address 0x11df800 g_muffin_data
__declspec(thread) s_data_array *g_muffin_data;
void muffins_dispose()
{
    if (g_muffin_data) {
        data_dispose(g_muffin_data);
        g_muffin_data = 0;
    }
}

void __tls_set_g_muffin_data_allocator(void *object)
{
    data_set_new_base_address(&tls_muffin_map_data(), (s_data_array *)object);
}

static __forceinline long object_get_first_widget(long object_index)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, NONE);
    return object->first_widget_index;
}
static __forceinline s_widget_datum *widget_get(long widget_index)
{
    return datum_get<s_widget_datum>(tls_widget_data(), widget_index);
}
static __forceinline void widget_get_type_data(long widget_index, long *type, long *type_data)
{
    s_widget_datum *widget = widget_get(widget_index);
    *type = widget->type;
    *type_data = widget->type_data;
}
static __forceinline long widget_get_next_widget(long widget_index)
{
    s_widget_datum *widget = widget_get(widget_index);
    return widget->next_widget;
}
namespace {
static __forceinline void widget_transfer_objects(long widget_index, long old_object_index, long new_object_index)
{
    s_widget_datum *widget = widget_get(widget_index);
    bool delete_datum = false;
    widget_delete_type_datum(old_object_index, widget->type_data, delete_datum);
    object_datum *object = (object_datum *)object_get_and_verify_type(new_object_index, _object_mask_all);
    long next_widget = object->first_widget_index;
    object->first_widget_index = widget_index;
    widget->next_widget = next_widget;
    s_widget_type *definition = &widget_type_table()[widget->type];
    if (definition->transfer)
        definition->transfer(widget_index, old_object_index, new_object_index);
}
}

// @address 0x36beb8 ?muffins_update_base_address@@YAXPAX@Z
void muffins_update_base_address(void *object)
{
    data_set_new_base_address(&tls_muffin_data(), (s_data_array *)object);
}

// @address 0x36bed0 ?muffins_transfer_to_new_object@@YAXJJ@Z
void muffins_transfer_to_new_object(long old_object_index, long new_object_index)
{
    long widget_index = object_get_first_widget(old_object_index);
    long type;
    while (widget_index != -1)
    {
        long type_data = -1;
        widget_get_type_data(widget_index, &type, &type_data);
        long next_widget_index = widget_get_next_widget(widget_index);
        if (type == 3)
            widget_transfer_objects(widget_index, old_object_index, new_object_index);
        widget_index = next_widget_index;
    }
}

bool object_has_muffins(long object_index)
{
    long widget_index = object_get_first_widget(object_index);
    bool result = false;
    long type;
    while (widget_index != -1) {
        long type_data = -1;
        widget_get_type_data(widget_index, &type, &type_data);
        if (type == 3) {
            result = true;
            break;
        } else {
            widget_index = widget_get_next_widget(widget_index);
        }
    }
    return result;
}

// @address 0x36c0e8 ?muffins_initialize@@YAXXZ
void muffins_initialize(void)
{
    tls_muffin_data() = data_new("muffin", 10, 6288, 0, (class c_allocation_base *)&g_muffin_data_allocator);
}

// @address 0x36c0d0 ?muffins_delete@@YAXJ@Z
void muffins_delete(long muffin_index)
{
    ::datum_delete(tls_muffin_data(), muffin_index);
}

// @address 0x36c130 muffins_initialize_data_for_new_map
void muffins_initialize_data_for_new_map()
{
    data_make_valid(tls_muffin_data());
}

// @address 0x36c148 muffins_dispose_data_from_old_map
void muffins_dispose_data_from_old_map()
{
    tls_muffin_data()->valid = false;
}

namespace {
}

__forceinline s_muffin_datum *s_muffin_datum::get(long muffin_index)
{
    return datum_get<s_muffin_datum>(tls_muffin_data(), muffin_index);
}

// @address 0x36c028 ?muffins_new@@YAJJJPAUreal_matrix4x3@@@Z
long muffins_new(long object_index, long widget_index, real_matrix4x3 *matrix)
{
    long muffin_index = -1;
    if (object_index != -1 && widget_index != -1)
    {
        muffin_index = datum_new(tls_muffin_data());
        if (muffin_index != -1)
        {
            s_muffin_datum *muffin = s_muffin_datum::get(muffin_index);
            muffin->definition_index = object_index;
            muffin->object_index = widget_index;
            muffin->time = 0.0f;
            if (!muffin_create(muffin_index, widget_index))
            {
                ::datum_delete(tls_muffin_data(), muffin_index);
                muffin_index = -1;
            }
        }
    }
    return muffin_index;
}

// @address 0x5bec98 ?muffins_render_debug@@YAXJJ@Z
void muffins_render_debug(long object_index, long muffin_index)
{
}

// @address 0x42d298 ?muffins_get_is_transparent@@YA_NJ@Z
bool muffins_get_is_transparent(long muffin_index)
{
    return false;
}

// @address 0x36c1b0 ?muffins_render_opaque@@YAXJJ@Z
void muffins_render_opaque(long muffin_index, long entry_point)
{
    s_muffin_datum *muffin = s_muffin_datum::get(muffin_index);
    const s_muffin_definition *definition = TAG_GET('mufn', s_muffin_definition, muffin->definition_index);
    long state_index = object_get_cached_render_state(muffin->object_index);
    if (state_index != -1 && definition->render_model.index != -1)
    {
        muffins_render_state *state = &((muffins_render_state *)tls_object_render_state_data()->data)[(unsigned short)state_index];
        render_model_definition *model = TAG_GET('mode', render_model_definition, definition->render_model.index);
        render_mesh_submit_compression((const s_compression_info *)model->render_geometry.compression_info.address);
        unsigned long active_mask = g_active_structure_bsp_mask;
        render_method_submit_set_extern_info(31 - _CountLeadingZeros(-active_mask & active_mask), &state->shader_info);
        if (entry_point >= 5)
            render_objects_set_lighting_constants(&state->shader_info.lighting, 0, entry_point);
        for (long piece_index = 0; piece_index < muffin->piece_count; ++piece_index)
        {
            s_muffin_piece *piece = &muffin->pieces[piece_index];
            if (piece->flags & 2)
            {
                if (c_render_globals::get_skinning_constants_owned())
                {
                    s_model_skinning_matrix *constants = (s_model_skinning_matrix *)D3DDevice_GpuBeginShaderConstantF4(
                        c_rasterizer::get_device(), FALSE, 16, 8);
                    const real_matrix4x3 *matrix = piece->render_matrices;
                    unsigned long count = 2;
                    do
                    {
                        real_matrix4x3_to_s_model_skinning_matrix(constants, matrix);
                        --count;
                        ++matrix;
                        ++constants;
                    } while (count);
                }
                else
                {
                    long start = 16;
                    const real_matrix4x3 *matrix = piece->render_matrices;
                    for (; start < 22; start += 3, ++matrix)
                    {
                        s_model_skinning_matrix constants;
                        real_matrix4x3_to_s_model_skinning_matrix(&constants, matrix);
                        unsigned long group = (unsigned long)start >> 2;
                        unsigned long shift = ((unsigned long)(start + 2) >> 2) - group;
                        unsigned __int64 mask = (unsigned __int64)((__int64)0x8000000000000000ui64 >> shift) >> group;
                        D3DDevice_SetVertexShaderConstantFN(c_rasterizer::get_device(), start, (const float *)&constants, 3, mask);
                    }
                }
                muffins_render_mesh *mesh = &((muffins_render_mesh *)model->render_geometry.meshes.address)[piece->mesh_index];
                for (long part_index = 0; part_index < mesh->parts.count; ++part_index)
                {
                    muffins_render_part *part = &((muffins_render_part *)mesh->parts.address)[part_index];
                    muffins_render_material *materials = (muffins_render_material *)model->materials.address;
                    bool opaque;
                    if (part->part_type != 5 && part->render_method_index != -1 && materials[part->render_method_index].render_method.index != -1)
                        opaque = true;
                    else
                        opaque = false;
                    if (opaque)
                    {
                        s_render_geometry *geometry = &model->render_geometry;
                        __int64 key = ((__int64)(short)part_index << 16) + piece->mesh_index;
                        key = (key << 32) + (intptr_t)geometry;
                        render_method_submit_draw_mesh_part(key, geometry, mesh, part, entry_point, 0, materials);
                    }
                }
            }
        }
    }
}

// @address 0x36c5e0 ?muffin_check_for_delete@@YAXJ@Z
void muffin_check_for_delete(long muffin_index)
{
    s_muffin_datum *muffin = s_muffin_datum::get(muffin_index);
    long object_index = muffin->object_index;
    bool delete_muffin = false;
    if (object_index != -1)
    {
        object_datum *object = (object_datum *)object_get_and_verify_type(object_index, _object_mask_all);
        if (object_get_type(object_index) == 0 && (bool)((object->damage_flags >> 2) & 1))
        {
            float time = game_ticks_to_seconds((float)(tls_game_time_globals()->time - object->last_motion_time));
            if (time > 10.0f)
                delete_muffin = true;
        }
    }
    if (!delete_muffin)
    {
        delete_muffin = true;
        for (long piece_index = 0; piece_index < muffin->piece_count; piece_index++)
        {
            s_muffin_piece *piece = &muffin->pieces[piece_index];
            if (!(piece->flags & 4))
            {
                delete_muffin = false;
                break;
            }
        }
    }
    if (delete_muffin)
        widget_delete_type_datum(object_index, muffin_index, true);
}

// @address 0x126ea0 ?muffins_update@@YAXM@Z
void muffins_update(float dt)
{
    long muffin_index = data_next_index(tls_muffin_data(), -1);
    while (muffin_index != -1) {
        s_muffin_datum *muffin = s_muffin_datum::get(muffin_index);
        float *time = &muffin->time;
        *time += dt;
        if (*time >= 1.0f / 30.0f) {
            havok_muffin_update(muffin_index, *time);
            *time = 0.0f;
            muffin_check_for_delete(muffin_index);
        }
        s_data_array *muffin_data = tls_muffin_data();
        muffin_index = datum_absolute_index_to_index(muffin_data, data_next_absolute_index(muffin_data, (muffin_index + 1) & 0xffff));
    }
}

// @address 0x36d388 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?muffins_update_base_address@@YAXPAX@Z@@UAAPAXIPBD@Z
template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &muffins_update_base_address>::allocate(unsigned int size, const char *name)
{
    m_member_index = restricted_region_register_member(3, name, "", size, 0,
        &muffins_update_base_address, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// @address 0x36d428 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?muffins_update_base_address@@YAXPAX@Z@@UAAXPAX@Z
template <>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &muffins_update_base_address>::deallocate(void *memory)
{
    tls_muffin_data() = 0;
    restricted_region_free_member(3, m_member_index);
}

// @address 0x36cfe8 muffins_destroy_piece_with_effect
extern "C" void muffins_destroy_piece_with_effect(s_muffin_piece *piece, s_muffin_datum *muffin,
    const s_muffin_definition_piece *piece_definition)
{
    piece->flags |= 4;
    if ((piece->flags & 2) != 0)
    {
        piece->flags &= ~2;
        if (piece_definition->destroyed_effect.index != -1)
        {
            unsigned int object_index = muffin->object_index;
            char markers_buf[0x78];
            bool found = false;
            if (object_try_and_get_and_verify_type(object_index, 0xffffffffu) != 0)
            {
                short marker_count = internal_object_get_markers_by_string_id(object_index, piece->marker_name, (object_marker *)markers_buf, 1, false);
                if (marker_count != 0)
                    found = true;
            }
            if (found)
            {
                ::effect_new_attached_from_marker_name(piece_definition->destroyed_effect.index, 0, 0, muffin->object_index,
                    (unsigned short)piece->object_node_index, piece->marker_name, 1.0f, 1.0f, 0,
                    0, 0, 0);
                return;
            }
            if ((piece->flags & 0x10) != 0)
            {
                effect_new_unattached_from_point(piece_definition->destroyed_effect.index, &piece->render_matrices[0].position,
                    &piece->render_matrices[0].rotation.forward, &piece->render_matrices[0].rotation.forward, 1, 0, 0, 0);
            }
        }
    }
}

// @address 0x36c420 ?muffins_transfer@@YAXJJJ@Z
void muffins_transfer(long widget_index, long old_object_index, long new_object_index)
{
    long widget_type;
    long muffin_index;
    widget_get_type_data(widget_index, &widget_type, &muffin_index);
    s_muffin_datum *muffin = datum_get<s_muffin_datum>(tls_muffin_data(), muffin_index);
    s_muffin_definition *definition = TAG_GET('mufn', s_muffin_definition, muffin->definition_index);
    muffin->object_index = new_object_index;
    for (long piece_index = 0; piece_index < muffin->piece_count; piece_index++) {
        s_muffin_piece *piece = &muffin->pieces[piece_index];
        if ((piece->flags & 4) == 0) {
            s_muffin_definition_piece *piece_definition = &definition->pieces[piece->node_index];
            object_marker old_marker;
            object_marker new_marker;
            object_get_markers_by_string_id(old_object_index, piece->marker_name, &old_marker, 1);
            if (object_get_markers_by_string_id(new_object_index, piece->marker_name, &new_marker, 1) <= 0) {
                muffins_destroy_piece_with_effect(piece, muffin, piece_definition);
            } else {
                short old_node_index = piece->object_node_index;
                piece->object_node_index = (unsigned char)new_marker.node_index;
                long old_node_count;
                long new_node_count;
                real_matrix4x3 *old_node_matrices = object_get_node_matrices(old_object_index, &old_node_count);
                real_matrix4x3 *new_node_matrices = object_get_node_matrices(new_object_index, &new_node_count);
                if ((piece_definition->flags & 2) != 0) {
                    piece->matrix = new_marker.node_to_marker;
                } else {
                    real_matrix4x3 old_marker_matrix;
                    real_matrix4x3 new_node_inverse;
                    matrix4x3_compute_product(&old_node_matrices[old_node_index], &piece->matrix, &old_marker_matrix);
                    matrix4x3_inverse(&new_node_matrices[new_marker.node_index], &new_node_inverse);
                    matrix4x3_compute_product(&new_node_inverse, &old_marker_matrix, &piece->matrix);
                }
            }
        }
    }
}

// @address 0x36c708 ?muffin_create@@YA_NJJ@Z
bool muffin_create(long muffin_index, long widget_index)
{
    s_muffin_datum *muffin = s_muffin_datum::get(muffin_index);
    const s_muffin_definition *definition = TAG_GET('mufn', s_muffin_definition, muffin->definition_index);
    const render_model_definition *render_model = TAG_GET('mode', render_model_definition, definition->render_model.index);
    const render_model_region *region = TAG_BLOCK_GET_ELEMENT(&render_model->regions, 0, render_model_region);
    for (long entry_index = 0; entry_index < definition->entries.count; entry_index++) {
        const s_muffin_entry *entry = TAG_BLOCK_GET_ELEMENT(&definition->entries, entry_index, s_muffin_entry);
        if (muffin->piece_count >= NUMBEROF(muffin->pieces))
            continue;
        if (_random_range(get_local_random_seed_address(), 0, 0, 0, 0, 100) >= entry->probability)
            continue;
        unsigned long bit_count = entry->piece_mask;
        long selected_piece = 0;
        bit_count = ((bit_count >> 1) & 0x55555555) + (bit_count & 0x55555555);
        bit_count = ((bit_count >> 2) & 0x33333333) + (bit_count & 0x33333333);
        bit_count = ((bit_count >> 4) & 0x0f0f0f0f) + (bit_count & 0x0f0f0f0f);
        bit_count = ((bit_count >> 8) & 0xFF) + (bit_count & 0xFF);
        long random_piece = _random_range(get_local_random_seed_address(), 0, 0, 0, 0, (short)bit_count);
        unsigned long piece_mask = entry->piece_mask;
        if (piece_mask != 0) {
            long last_piece = definition->piece_count - 1;
            do {
                if (selected_piece >= last_piece)
                    break;
                if ((piece_mask & 1) != 0) {
                    if (random_piece == 0)
                        break;
                    random_piece--;
                }
                piece_mask >>= 1;
                selected_piece++;
            } while (piece_mask != 0);
        }
        const s_muffin_definition_piece *piece_definition = &definition->pieces[selected_piece];
        if ((piece_definition->flags & 1) != 0)
            continue;
        short permutation_index = piece_definition->permutation_index;
        if (permutation_index < 0 || permutation_index >= region->permutations.count)
            continue;
        const s_render_model_permutation *permutation = &((const s_render_model_permutation *)region->permutations.address)[permutation_index];
        long marker_name = entry->marker_name;
        object_marker marker;
        if (internal_object_get_markers_by_string_id(widget_index, marker_name, &marker, 1, false) <= 0)
            continue;
        s_muffin_piece *piece = &muffin->pieces[muffin->piece_count];
        piece->flags = 0;
        if (_real_random_range(get_local_random_seed_address(), 0.0f, 1.0f) < piece_definition->immortality)
            piece->flags |= 8;
        piece->node_index = (unsigned char)selected_piece;
        piece->object_node_index = (unsigned char)marker.node_index;
        piece->mesh_index = (unsigned char)permutation->start_index;
        piece->countdown = (unsigned short)MIN(65535.0f,
            game_seconds_to_ticks_real(
                _real_random_range(get_local_random_seed_address(), piece_definition->minimum_delay, piece_definition->maximum_delay)));
        if (piece->countdown <= 0)
            piece->flags = 1;
        float scale = _real_random_range(get_local_random_seed_address(), piece_definition->minimum_scale, piece_definition->maximum_scale);
        scale = PIN(scale * 256.0f, 1.0f, 255.0f);
        piece->marker_name = marker_name;
        piece->start_time = 0;
        piece->scale = (unsigned char)scale;
        memcpy(&piece->matrix, &marker.node_to_marker, sizeof(real_matrix4x3));
        muffin->piece_count++;
    }
    return true;
}

typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0), &muffins_update_base_address> muffin_allocation_manager;
muffin_allocation_manager g_muffin_data_allocator;

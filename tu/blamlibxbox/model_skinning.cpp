// blamlibxbox/model_skinning.obj: from the records 0x15ad58 0x15ae48 0x15b0e8 0x170f30 0x1744f0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "math/matrix_math_types.h"
#include "geometry/compression_info_types.h"
#include "objects/object_datum.h"
// @address 0x15ad58 ?render_model_get_skinning_matrix_count@@YAJJJPBE@Z
// @address 0x15ae48 ?render_model_build_skinning@@YAXJJPBUreal_matrix4x3@@PBEJ_NJPAUs_model_skinning@@@Z
// @address 0x170f30 ?render_mesh_submit_compression@@YAXPBUs_compression_info@@@Z
// @address 0x1744f0 ?model_skinning_get_node_matrix@@YAPBUs_model_skinning_matrix@@PBUs_model_skinning@@JPAUreal_matrix4x3@@@Z
#include <xtl.h>
#include "rasterizer/rasterizer.h"
#include <string.h>
#include "memory/tls_globals.h"
#include "tag_files/tag_groups.h"
#include "objects/objects.h"
#include "models/model_skinning.h"
#include "models/model_types.h"
#include "models/render_model_definitions.h"
#include "objects/object_definitions.h"
#include "math/matrix_math.h"
#include "animations/animation_manager.h"
#include "math/real_matrix3x4_types.h"
#include "models/model_skinning_matrix_types.h"
#include "models/model_skinning_types.h"
#include "geometry/render_geometry_mesh_types.h"
#include "rasterizer/rasterizer_main_globals.h"
#include "objects/object_header.h"
#include "memory/static_arrays.h"
#include "animations/animation_codec_flags.h"
#include "motor/motor_system.h"
#include "objects/object_animation.h"

struct s_shader_constant_tag {
    unsigned __int64 mask;
    unsigned long index;
    s_shader_constant_tag(unsigned long start_register, unsigned long count) :
        mask(D3DTag_ShaderConstantMask(start_register, count)),
        index(D3DTag_Index(D3DTAG_VERTEXSHADERCONSTANTS)) {
    }
};

static __forceinline void set_vertex_shader_constant_xyzw(unsigned long start_register, float x, float y, float z, float w) {
    D3DDevice *device = c_rasterizer::get_device();
    device->m_Constants.VertexShaderF[start_register].x = x;
    device->m_Constants.VertexShaderF[start_register].y = y;
    device->m_Constants.VertexShaderF[start_register].z = z;
    device->m_Constants.VertexShaderF[start_register].w = w;
}

namespace {
static __forceinline s_mesh_node_map *model_skinning_get_node_map(const s_tag_block *block, long index)
{
    const long &offset = index * (long)sizeof(s_mesh_node_map);
    return (s_mesh_node_map *)((char *)block->address + offset);
}
}
long render_model_get_skinning_matrix_count(long render_model_index, long level_of_detail, const unsigned char *region_mesh_indices) {
    render_model_definition *render_model = (render_model_definition *)tag_get('mode', render_model_index);
    long matrix_count = 0;
    if (render_model->flags & 4)
        matrix_count = 1;
    else
        matrix_count += render_model->nodes.count;
    if (render_model->render_geometry.per_mesh_node_map.count > 0) {
        for (long region_index = 0; region_index < render_model->regions.count; ++region_index) {
            if (region_mesh_indices[region_index] != 255) {
                long mesh_index = region_mesh_indices[region_index];
                const s_mesh_node_map *mesh_node_map = &((s_mesh_node_map *)render_model->render_geometry.per_mesh_node_map.address)[mesh_index];
                matrix_count += mesh_node_map->node_count;
            }
        }
    }
    return matrix_count;
}
static __forceinline long render_model_build_skinning_for_base_nodes(
    long render_model_index, const real_matrix4x3 *&object_node_matrices, s_model_skinning_matrix *base_node_matrices)
{
    render_model_definition *render_model = (render_model_definition *)tag_get('mode', render_model_index);
    const render_model_node *node = (const render_model_node *)render_model->nodes.address;
    __dcbt(0, node);
    __dcbt(0, object_node_matrices);
    long node_index = 0;
    if (node_index < render_model->nodes.count) {
        s_model_skinning_matrix *destination = base_node_matrices;
        do {
            __dcbt(128, node);
            __dcbt(128, object_node_matrices);
            matrix4x3_multiply_to_matrix3x4(object_node_matrices, &node->default_inverse_matrix, (real_matrix3x4 *)destination);
            destination++;
            node++;
            object_node_matrices++;
            node_index++;
        } while (node_index < render_model->nodes.count);
    }
    return render_model->nodes.count;
}
// @address 0x15ae48 ?render_model_build_skinning@@YAXJJPBUreal_matrix4x3@@PBEJ_NJPAUs_model_skinning@@@Z
void render_model_build_skinning(long object_index, long render_model_index, const real_matrix4x3 *object_node_matrices,
    const unsigned char *region_mesh_indices, long level_of_detail, bool use_object_node_matrices, long expected_total_matrix_count,
    s_model_skinning *skinning)
{
    render_model_definition *render_model = (render_model_definition *)tag_get('mode', render_model_index);
    const real_matrix4x3 *node_matrices = object_node_matrices;
    if (!use_object_node_matrices && object_index != -1) {
        bool is_child = false;
        if (render_model_index != -1) {
            object_datum *object = object_get(object_index);
            s_object_definition *object_definition = (s_object_definition *)tag_get(0, object->definition_index);
            long model_index = object_definition->model.index;
            if (model_index != -1) {
                s_model_definition *model = (s_model_definition *)tag_get(0, model_index);
                long model_render_model_index = model->skinning_render_model_index;
                if (model_render_model_index != -1 && render_model_index == model_render_model_index)
                    is_child = true;
            }
        }
        if (!is_child)
            node_matrices = render_model_get_animated_node_matrices(object_index, render_model_index, region_mesh_indices, object_node_matrices);
    }
    s_model_skinning_matrix base_node_matrices_buffer[255];
    s_model_skinning_matrix *base_node_matrices;
    if (render_model->flags & 4)
        base_node_matrices = base_node_matrices_buffer;
    else
        base_node_matrices = skinning->base_node_matrices;
    skinning->region_count = (unsigned short)render_model->regions.count;
    long base_node_matrix_count = render_model_build_skinning_for_base_nodes(render_model_index, node_matrices, base_node_matrices);
    if (render_model->flags & 4) {
        base_node_matrix_count = 0;
        memcpy(&skinning->base_node_matrices[base_node_matrix_count], base_node_matrices, sizeof(s_model_skinning_matrix));
        base_node_matrix_count++;
    }
    skinning->base_node_matrix_count = (unsigned short)base_node_matrix_count;
    for (long region_index = 0; region_index < render_model->regions.count; region_index++) {
        unsigned char mesh_index = region_mesh_indices[region_index];
        if (mesh_index != 0xff) {
            s_mesh *mesh = &((s_mesh *)render_model->render_geometry.meshes.address)[mesh_index];
            if (render_model->render_geometry.per_mesh_node_map.count > 0) {
                const s_tag_block *mesh_node_maps = &render_model->render_geometry.per_mesh_node_map;
                s_mesh_node_map *node_map = model_skinning_get_node_map(mesh_node_maps, mesh_index);
                const unsigned char *node_indices = node_map->nodes;
                skinning->regions[region_index].first_node_matrix = (unsigned short)base_node_matrix_count;
                skinning->regions[region_index].node_matrix_count = (unsigned short)node_map->node_count;
                for (long node_index = 0; node_index < node_map->node_count; node_index++) {
                    unsigned char matrix_index = node_indices[node_index];
                    memcpy(&skinning->base_node_matrices[base_node_matrix_count], &base_node_matrices[matrix_index], sizeof(s_model_skinning_matrix));
                    base_node_matrix_count++;
                }
            } else {
                bool rigid = mesh->node_index != 0xff;
                skinning->regions[region_index].first_node_matrix = rigid ? mesh->node_index : 0;
                skinning->regions[region_index].node_matrix_count = rigid ? 1 : render_model->nodes.count;
            }
        } else {
            skinning->regions[region_index].first_node_matrix = 0;
            skinning->regions[region_index].node_matrix_count = 0;
        }
    }
}

void render_mesh_submit_compression(const s_compression_info *compression)
{
    static const s_shader_constant_tag tag(12, 3);
    set_vertex_shader_constant_xyzw(12, compression->x.upper - compression->x.lower, compression->y.upper - compression->y.lower,
        compression->z.upper - compression->z.lower, 1.0f);
    set_vertex_shader_constant_xyzw(13, compression->x.lower, compression->y.lower, compression->z.lower, 0.0f);
    set_vertex_shader_constant_xyzw(14, compression->u.upper - compression->u.lower, compression->v.upper - compression->v.lower,
        compression->u.lower, compression->v.lower);
    D3DTagCollection_Set(&c_rasterizer::get_device()->m_Pending, tag.index, tag.mask);
}
static __forceinline void model_skinning_matrix_to_real_matrix4x3(real_matrix4x3 *matrix4x3, const s_model_skinning_matrix *skinning_matrix) {
    matrix4x3->forward.i = skinning_matrix->rows[0][0];
    matrix4x3->left.i = skinning_matrix->rows[0][1];
    matrix4x3->up.i = skinning_matrix->rows[0][2];
    matrix4x3->forward.j = skinning_matrix->rows[1][0];
    matrix4x3->left.j = skinning_matrix->rows[1][1];
    matrix4x3->up.j = skinning_matrix->rows[1][2];
    matrix4x3->forward.k = skinning_matrix->rows[2][0];
    matrix4x3->left.k = skinning_matrix->rows[2][1];
    matrix4x3->up.k = skinning_matrix->rows[2][2];
    matrix4x3->position.x = skinning_matrix->rows[0][3];
    matrix4x3->position.y = skinning_matrix->rows[1][3];
    matrix4x3->position.z = skinning_matrix->rows[2][3];
    matrix4x3->scale = 1.0f;
}
const s_model_skinning_matrix *model_skinning_get_node_matrix(const s_model_skinning *skinning, long node_index, real_matrix4x3 *matrix4x3) {
    const s_model_skinning_matrix *node_matrix = &skinning->base_node_matrices[node_index];
    if (matrix4x3)
        model_skinning_matrix_to_real_matrix4x3(matrix4x3, node_matrix);
    return node_matrix;
}

static __forceinline bool model_skinning_can_interpolate(long object_index)
{
    object_datum *object = (object_datum *)object_get_and_verify_type(object_index, -1);
    return object->node_orientations.offset != -1;
}

// @address 0x15b0e8 render_model_get_animated_node_matrices
extern "C" const real_matrix4x3 *render_model_get_animated_node_matrices(
    long object_index, long render_model_index, const unsigned char *region_mesh_indices, const real_matrix4x3 *object_node_matrices)
{
    const real_matrix4x3 *result = object_node_matrices;
    if (model_skinning_can_interpolate(object_index)) {
        s_object_header *header = object_header_get(object_index);
        object_datum *object = header->datum;
        if (!(bool)((object->flags >> 14) & 1)) {
            const s_object_definition *definition = (const s_object_definition *)tag_get('obje', object->definition_index);
            if (definition->model.index != -1) {
                const s_model_definition *model = (const s_model_definition *)tag_get('hlmt', definition->model.index);
                c_animation_manager *manager = (c_animation_manager *)object_header_block_get(object, &object->animation);
                const render_model_definition *render_model = (const render_model_definition *)tag_get('mode', model->render_model_index);
                if (!(bool)((((object_datum *)object_get_and_verify_type(object_index, -1))->flags >> 29) & 1) &&
                    (!TEST_BIT(1 << definition->object_type, 0) || motor_system_is_relaxed(object_index) == 0))
                {
                    bool animate = TEST_BIT(object->flags, 30);
                    flags256 render_node_mask;
                    memset(&render_node_mask, 0, sizeof(render_node_mask));
                    bool adjust = animation_manager_update_facial_channels(manager, &render_node_mask);
                    if (animate || adjust) {
                        float distance = model->render_time_animation_distance;
                        if (distance == 0.0f)
                            distance = 7.5f;
                        float blend_ratio = 1.0f;
                        if (object_compute_animation_blend(object_index, distance * 0.9f, distance, &blend_ratio)) {
                            real_matrix4x3 *matrices = (real_matrix4x3 *)object_header_block_get(object, &object->node_matrices);
                            long matrix_count = object->node_matrices.size / sizeof(real_matrix4x3);
                            long orientation_count = object->node_orientations.size / sizeof(real_orientation);
                            real_orientation *orientations = (real_orientation *)object_header_block_get(object, &object->node_orientations);
                            flags256 node_mask = model->node_mask;
                            if (adjust) {
                                flags256 reverse_mask = model->node_mask;
                                reverse_mask.and_with(&render_node_mask);
                                object_reverse_compute_node_orientations_from_matrices(object_index, reverse_mask.m_storage);
                                node_mask.or_with(&render_node_mask);
                            }
                            if (animate)
                                internal_object_compute_animated_node_orientations(object_index, blend_ratio,
                                    (const render_model_definition *)render_model, manager, (const c_static_flags<255> *)&model->node_mask,
                                    matrix_count, orientations, false);
                            if (adjust && manager->m_graph_tag_index != -1)
                                manager->apply_facial_node_orientations(render_model, blend_ratio,
                                    (const c_static_flags<255> *)&render_node_mask, orientation_count, orientations, NULL, object_index);
                            unsigned long prefetch_size = orientation_count * sizeof(real_matrix4x3);
                            if (prefetch_size > 1024)
                                prefetch_size = 1024;
                            for (long offset = 0; offset < (long)prefetch_size; offset += 128)
                                __dcbt(offset, matrices);
                            const render_model_node *node = (const render_model_node *)render_model->nodes.address + 1;
                            for (long node_index = 1; node_index < orientation_count; ++node, ++node_index) {
                                const short &parent_node_index = node->parent_node_index;
                                if (node_mask.m_storage[node_index >> 5] & (1UL << (node_index & 31)))
                                    matrix4x3_multiply_by_orientation(&matrices[parent_node_index], &orientations[node_index], &matrices[node_index]);
                            }
                            if (TEST_BIT(1 << header->object_type, 0))
                                unit_animation_apply_eye_tracking(object_index, 0, orientation_count, matrices);
                            object->flags |= 1UL << 14;
                            result = matrices;
                        }
                    } else if (TEST_BIT(1 << header->object_type, 0)) {
                        real_matrix4x3 *matrices = (real_matrix4x3 *)object_header_block_get(object, &object->node_matrices);
                        long matrix_count = object->node_matrices.size / sizeof(real_matrix4x3);
                        unit_animation_apply_eye_tracking(object_index, 0, matrix_count, matrices);
                        object->flags |= 1UL << 14;
                        result = matrices;
                    }
                }
            }
        }
    }
    return result;
}

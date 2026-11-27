// blamlibxbox/render_models.obj: from the records 0x9cff8 0x5777b8
// @flags /TP /O2
#include "math/matrix_math_types.h"
#include "math/real_math.h"
#include "tag_files/tag_types.h"
#include "objects/object_placement.h"
#include "models/render_models.h"
#include "math/matrix_math.h"
#include "models/render_model_marker_types.h"
#include "models/render_model_marker_group_types.h"

#include "models/render_model_definitions.h"
#include "tag_files/tag_groups.h"
#include "memory/datum_index.h"

// The index of the entry named `name` in the tag's entries block, or -1.
// @address 0x9cff8 ?render_model_find_marker_group_by_name@@YAJJJ@Z
long render_model_find_marker_group_by_name(long tag_index, long name)
{
    long result = -1;
    if (tag_index != -1 && name != -1 && name != 0) {
        render_model_definition *definition = (render_model_definition *)global_tag_instances[datum_index_to_absolute_index(tag_index)].definition;
        for (int i = 0; i < definition->marker_groups.count; i++) {
            if (((render_model_marker_group *)definition->marker_groups.address)[i].name == name) {
                result = i;
                break;
            }
        }
    }
    return result;
}

// @address 0x5777b8 render_model_get_markers_by_group_index
long render_model_get_markers_by_group_index(long render_model_definition_index, long marker_group_index, const char *region_permutation_indices,
                  long node_remap_count, const long *node_remapping_table, long node_count,
                  const real_matrix4x3 *node_matrices, bool mirror, object_marker *markers, long maximum_marker_count)
{
    long marker_count = 0;

    if (marker_group_index != -1) {
        render_model_definition *model = (render_model_definition *)tag_get('mode', render_model_definition_index);
        render_model_marker_group *marker_group = (render_model_marker_group *)tag_block_get_element_with_size(
            &model->marker_groups, marker_group_index, sizeof(render_model_marker_group));
        long marker_index;

        for (marker_index = 0; marker_index < marker_group->markers.count; marker_index++) {
            render_model_marker *marker = (render_model_marker *)tag_block_get_element_with_size(
                &marker_group->markers, marker_index, sizeof(render_model_marker));
            bool valid = true;

            if (region_permutation_indices && marker->region_index != -1) {
                valid = region_permutation_indices[marker->region_index] == marker->permutation_index;
            }

            if (valid) {
                if (marker_count < maximum_marker_count) {
                    object_marker *object_marker = &markers[marker_count];
                    marker_count++;
                    long node_index = marker->node_index;

                    if (node_remapping_table) {
                        node_index = node_remapping_table[marker->node_index];
                    }
                    object_marker->node_index = (short)node_index;
                    matrix4x3_from_point_and_quaternion(&object_marker->node_to_marker, &marker->translation, &marker->rotation);
                    if (node_matrices) {
                        matrix4x3_compute_product(&node_matrices[object_marker->node_index], &object_marker->node_to_marker, &object_marker->matrix);
                    } else {
                        real_matrix4x3 default_node_matrix;
                        render_model_node *node = (render_model_node *)tag_block_get_element_with_size(&model->nodes, node_index, sizeof(render_model_node));
                        matrix4x3_from_point_and_quaternion(&default_node_matrix, &node->default_translation, &node->default_rotation);
                        matrix4x3_compute_product(&default_node_matrix, &object_marker->node_to_marker, &object_marker->matrix);
                    }
                    object_marker->scale = marker->scale;
                    if (mirror) {
                        negate_vector3d(&object_marker->matrix.left, &object_marker->matrix.left);
                    }
                } else {
                    break;
                }
            }
        }
    }

    return marker_count;
}

#ifndef MODELS_RENDER_MODELS_H
#define MODELS_RENDER_MODELS_H

// Functions defined in tu/blamlibxbox/render_models.cpp that other units call.

long render_model_find_marker_group_by_name(long tag_index, long name);

struct object_marker;
struct real_matrix4x3;

extern "C" long render_model_get_markers_by_group_index(long render_model_definition_index, long marker_group_index,
	const char *region_permutation_indices, long node_remap_count, const long *node_remapping_table, long node_count,
	const real_matrix4x3 *node_matrices, bool mirror, object_marker *markers, long maximum_marker_count);

#endif // MODELS_RENDER_MODELS_H

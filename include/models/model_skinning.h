#ifndef MODELS_MODEL_SKINNING_H
#define MODELS_MODEL_SKINNING_H

// Functions defined in tu/blamlibxbox/model_skinning.cpp.

struct real_matrix4x3;
struct s_compression_info;
struct s_model_skinning;
struct s_model_skinning_matrix;

long render_model_get_skinning_matrix_count(long render_model_index, long level_of_detail, const unsigned char *region_mesh_indices);
void render_model_build_skinning(long object_index, long render_model_index, const real_matrix4x3 *object_node_matrices,
    const unsigned char *region_mesh_indices, long level_of_detail, bool use_object_node_matrices,
    long expected_total_matrix_count, s_model_skinning *skinning);
void render_mesh_submit_compression(const s_compression_info *compression);
const s_model_skinning_matrix *model_skinning_get_node_matrix(const s_model_skinning *skinning, long node_index, real_matrix4x3 *matrix4x3);
extern "C" const real_matrix4x3 *render_model_get_animated_node_matrices(long object_index, long render_model_index,
    const unsigned char *region_mesh_indices, const real_matrix4x3 *object_node_matrices);

#include "animations/animation_codec_flags.h"
#include "memory/static_arrays.h"
class c_animation_manager;
extern "C" bool animation_manager_update_facial_channels(c_animation_manager *manager, flags256 *node_mask);
void unit_animation_apply_eye_tracking(long object_index, const c_static_flags<255> *node_mask, long node_count, real_matrix4x3 *matrices);

#endif // MODELS_MODEL_SKINNING_H

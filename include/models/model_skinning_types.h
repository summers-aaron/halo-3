#ifndef MODELS_MODEL_SKINNING_TYPES_H
#define MODELS_MODEL_SKINNING_TYPES_H

#include "models/model_skinning_matrix_types.h"

struct s_model_skinning_region
{
    unsigned short first_node_matrix;
    unsigned short node_matrix_count;
};

struct s_model_skinning
{
    unsigned short base_node_matrix_count;
    unsigned short region_count;
    s_model_skinning_region regions[16];
    s_model_skinning_matrix base_node_matrices[1];
};

#endif // MODELS_MODEL_SKINNING_TYPES_H

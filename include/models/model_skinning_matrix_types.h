#ifndef MODELS_MODEL_SKINNING_MATRIX_TYPES_H
#define MODELS_MODEL_SKINNING_MATRIX_TYPES_H

struct s_model_skinning_matrix
{
    float rows[3][4];
};

typedef char model_skinning_matrix_size[(sizeof(s_model_skinning_matrix) == 0x30) ? 1 : -1];

#endif // MODELS_MODEL_SKINNING_MATRIX_TYPES_H

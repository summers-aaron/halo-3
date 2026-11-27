#ifndef MODELS_RENDER_MODEL_MARKER_TYPES_H
#define MODELS_RENDER_MODEL_MARKER_TYPES_H

#include "math/real_math_types.h"

struct render_model_marker {       // 0x24
    char region_index;
    char permutation_index;
    unsigned char node_index;
    char pad3;
    real_point3d translation;
    real_quaternion rotation;
    float scale;
};

#endif // MODELS_RENDER_MODEL_MARKER_TYPES_H

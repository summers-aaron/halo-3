#ifndef MODELS_RENDER_MODEL_MARKER_GROUP_TYPES_H
#define MODELS_RENDER_MODEL_MARKER_GROUP_TYPES_H

#include "memory/data_types.h"

struct render_model_marker_group { // 0x10
    long name;
    s_tag_block markers;
};

#endif // MODELS_RENDER_MODEL_MARKER_GROUP_TYPES_H

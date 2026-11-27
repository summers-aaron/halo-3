#ifndef RENDER_RENDER_FIRST_PERSON_GLOBALS_TYPES_H
#define RENDER_RENDER_FIRST_PERSON_GLOBALS_TYPES_H

#include "render/render_first_person_model_types.h"
#include <stddef.h>

// setup_camera resets the object and count, then builds at most four models.
struct s_render_object_first_person_globals
{
    long object_index;
    long model_count;
    render_first_person_model models[4];
};

typedef char first_person_globals_models_offset[(offsetof(s_render_object_first_person_globals, models) == 8) ? 1 : -1];
typedef char first_person_globals_size[(sizeof(s_render_object_first_person_globals) == 13368) ? 1 : -1];

#endif

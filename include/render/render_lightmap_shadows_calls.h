#ifndef RENDER_RENDER_LIGHTMAP_SHADOWS_CALLS_H
#define RENDER_RENDER_LIGHTMAP_SHADOWS_CALLS_H

struct render_first_person_model;

extern "C" long render_camera_build_first_person_models(long user_index, long object_index, long maximum_model_count,
    render_first_person_model *models);

#endif

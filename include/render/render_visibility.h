#ifndef RENDER_RENDER_VISIBILITY_H
#define RENDER_RENDER_VISIBILITY_H

#include "objects/object_reference_types.h"

// Functions defined in tu/graphicslibxbox/render_visibility.cpp that other units call.

struct render_camera;
struct render_projection;
struct visibility_projection;

void render_visibility_build_projection(const render_camera *render_camera, const render_projection *render_projection,
    s_cluster_reference cluster_reference, visibility_projection *visibility_projection);

#endif // RENDER_RENDER_VISIBILITY_H

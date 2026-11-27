#ifndef ANIMATIONS_ANIMATION_COMPUTE_ORIENTATIONS_WORKSPACE_H
#define ANIMATIONS_ANIMATION_COMPUTE_ORIENTATIONS_WORKSPACE_H

#include "math/real_math_types.h"

struct s_compute_node_orientations_workspace
{
    real_orientation orientations[255];
    real_orientation corrections[255];
};
extern "C" s_compute_node_orientations_workspace g_compute_node_orientations_workspaces[2];

#endif

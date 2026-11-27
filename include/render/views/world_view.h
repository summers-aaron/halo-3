#ifndef RENDER_VIEWS_WORLD_VIEW_H
#define RENDER_VIEWS_WORLD_VIEW_H

#include "render/render_view.h"
#include "objects/cluster_reference.h"

class c_world_view : public c_view
{
public:
    __forceinline c_world_view();
    using c_view::rasterizer_camera;
    using c_view::rasterizer_projection;
    unsigned char starting_cluster_invalid;
    s_cluster_reference starting_cluster;
    void get_starting_cluster(s_cluster_reference *starting_cluster);
    __forceinline const render_camera *get_render_camera() const { return &render_camera_state; }
    __forceinline const render_projection *get_render_projection() const { return &render_projection_state; }
};

typedef char world_view_size[(sizeof(void *) != 4 || ((sizeof(c_world_view) == 0x298))) ? 1 : -1];
typedef char world_view_cluster_offset[(sizeof(void *) != 4 || ((offsetof(c_world_view, starting_cluster) == 0x295))) ? 1 : -1];

#endif

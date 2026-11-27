#ifndef RENDER_VIEWS_OCCLUSION_VIEW_H
#define RENDER_VIEWS_OCCLUSION_VIEW_H

#include "render/views/lightmap_shadows_view.h"

// March constructor 0x8243e120 constructs the lightmap-shadows base.
// This view is embedded in the player-view state.
class c_occlusion_view : public c_lightmap_shadows_view
{
public:
    __forceinline c_occlusion_view() {}
    virtual void render_setup();
    virtual void render_submit_visibility();
    virtual void render(const real_point3d *point);
    virtual void compute_visibility(const real_point3d *point);
};

typedef char occlusion_view_size[(sizeof(void *) != 4 || ((sizeof(c_occlusion_view) == 0x2d8))) ? 1 : -1];

#endif

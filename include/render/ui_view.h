#ifndef RENDER_UI_VIEW_H
#define RENDER_UI_VIEW_H

#include "render/render_view.h"

class c_ui_view : public c_view
{
public:
    using c_view::rasterizer_camera;
    __forceinline c_ui_view() : m_is_screenshot(false) {}
    virtual void render();
    virtual void render_setup() {}
    virtual void compute_visibility() {}
    virtual void render_submit_visibility() {}
    bool m_is_screenshot;
    long m_render_target;
    long m_user_index;
};

#endif

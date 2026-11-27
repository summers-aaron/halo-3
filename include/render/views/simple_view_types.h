#ifndef RENDER_VIEWS_SIMPLE_VIEW_TYPES_H
#define RENDER_VIEWS_SIMPLE_VIEW_TYPES_H

#include "render/render_view.h"

struct s_observer_result;

class c_fullscreen_view : public c_view
{
public:
    virtual void render();
    virtual void render_setup();
    static void render_blank_frame(const real_rgb_color *color);
    void setup_camera(const s_observer_result *observer);
private:
    char data[0x2a0 - sizeof(c_view)];
};

class c_first_person_view : public c_view
{
public:
    __forceinline c_first_person_view();
    using c_view::rasterizer_camera;
    render_camera *original_camera;

    static float m_fov_scale;
};

class c_reflection_view : public c_view
{
public:
    virtual void render() {}
    virtual void render_setup() {}
    virtual void compute_visibility() {}
    virtual void render_submit_visibility() {}
};

typedef char fullscreen_view_size[(sizeof(c_fullscreen_view) == 0x2a0) ? 1 : -1];
typedef char first_person_view_size[(sizeof(void *) != 4 || ((sizeof(c_first_person_view) == 0x298))) ? 1 : -1];
typedef char reflection_view_size[(sizeof(void *) != 4 || ((sizeof(c_reflection_view) == 0x294))) ? 1 : -1];

#endif

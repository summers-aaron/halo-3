#ifndef RENDER_RENDER_VIEW_H
#define RENDER_RENDER_VIEW_H

#include "render/render_camera_types.h"
#include "render/render_view_stack_globals.h"

class c_view
{
public:
    static __forceinline void begin(c_view *view)
    {
        if (g_view_stack_top < 3)
        {
            g_view_stack_top++;
            g_view_stack[g_view_stack_top] = view;
            g_view_stack[g_view_stack_top]->render_setup();
        }
    }
    static __forceinline long get_current_stack_level() { return g_view_stack_top; }
    static __forceinline void end()
    {
        g_view_stack_top--;
        if (g_view_stack_top >= 0)
            g_view_stack[g_view_stack_top]->render_setup();
    }
    static __forceinline void abort_current_view_stack() { g_view_stack_top = -1; }
public:
    __forceinline c_view() {}
    __forceinline const render_camera *get_rasterizer_camera() const { return &rasterizer_camera; }
    __forceinline render_camera *get_rasterizer_camera_modifiable() { return &rasterizer_camera; }
    __forceinline const render_projection *get_rasterizer_projection() const { return &rasterizer_projection; }
    __forceinline render_projection *get_rasterizer_projection_modifiable() { return &rasterizer_projection; }
    virtual void render() {}
    virtual void render_setup() {}
    virtual void compute_visibility() {}
    virtual void render_submit_visibility() {}
protected:
    render_camera rasterizer_camera;
    render_projection rasterizer_projection;
public:
    render_camera render_camera_state;
    render_projection render_projection_state;
private:
    static long g_view_stack_top;
};


#endif // RENDER_RENDER_VIEW_H

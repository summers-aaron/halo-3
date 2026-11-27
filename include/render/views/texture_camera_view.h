#ifndef RENDER_VIEWS_TEXTURE_CAMERA_VIEW_H
#define RENDER_VIEWS_TEXTURE_CAMERA_VIEW_H

#include "render/views/player_view_types.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "render/render_texture_camera.h"

class c_texture_camera_view : public c_player_view
{
    friend class c_player_view;
private:
    __forceinline c_texture_camera_view() : c_player_view()
    {
        initialize();
    }
public:
    c_rasterizer_texture_ref m_texture;
    c_rasterizer_texture_ref m_albedo_texture;
    c_rasterizer_texture_ref m_normal_texture;
    // March set_render_target overloads store the index between textures and surfaces.
    long m_render_target_index;
    union { D3DSurface *m_render_surface; D3DSurface *m_accumulation_surface; };
    D3DSurface *m_albedo_surface;
    D3DSurface *m_normal_surface;
    D3DSurface *m_depth_surface;
    union { c_rasterizer::e_surface m_depth_surface_index; c_rasterizer::e_surface m_surface; };
    unsigned long m_flags;
    long m_view_index;
    void (*m_callback)(void *context);
    void *m_callback_context;
public:
    virtual void render();
    virtual void render_submit_visibility();
    virtual void compute_visibility(long user_index);
public:
  virtual void render_setup();
    void setup_camera(const s_observer_result *observer, long user_index);
    __forceinline void initialize()
    {
        m_render_target_index = -1;
        m_accumulation_surface = 0;
        m_albedo_surface = 0;
        m_normal_surface = 0;
        m_depth_surface = 0;
        m_surface = (c_rasterizer::e_surface)3;
        m_flags = 0;
        m_view_index = -1;
        m_callback = 0;
        m_callback_context = 0;
    }
    __forceinline void set_render_flags(unsigned long flags) { m_flags = flags; }
    __forceinline void set_render_target(c_rasterizer_texture_ref accumulation_texture, c_rasterizer_texture_ref albedo_texture,
        c_rasterizer_texture_ref normal_texture, long render_target_index, c_rasterizer::e_surface surface)
    {
        m_texture = accumulation_texture;
        m_albedo_texture = albedo_texture;
        m_normal_texture = normal_texture;
        m_render_target_index = render_target_index;
        m_surface = surface;
    }
    __forceinline void set_render_target(D3DSurface *accumulation_surface, D3DSurface *albedo_surface, D3DSurface *normal_surface,
        long render_target_index, D3DSurface *depth_surface)
    {
        m_accumulation_surface = accumulation_surface;
        m_albedo_surface = albedo_surface;
        m_normal_surface = normal_surface;
        m_render_target_index = render_target_index;
        m_depth_surface = depth_surface;
    }
    __forceinline void render_texture_camera(long view_index) { texture_camera_view_render_texture_camera(this, view_index); }
};

#endif

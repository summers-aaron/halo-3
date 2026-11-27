#ifndef RENDER_RENDER_PLAYER_VIEW_DEPENDENCIES_H
#define RENDER_RENDER_PLAYER_VIEW_DEPENDENCIES_H

struct D3DSurface;
struct D3DDevice;
extern "C" void rasterizer_set_depth_stencil_surface_direct(D3DSurface *surface);

extern "C" void render_objects_update_visible_subparts(unsigned int p1);

struct s_cluster_reference;
struct c_atmosphere_setting;
struct s_weighted_atmosphere_parameters;

extern "C" void render_objects_accumulate_atmosphere(s_cluster_reference cluster_reference,
    const c_atmosphere_setting *setting, s_weighted_atmosphere_parameters *parameters, float weight);

union real_rectangle2d;
// Pending XDK state-call adoption; preserve the accepted caller signatures.
extern "C" {
void D3DDevice_SetRenderState_ColorWriteEnable2(struct D3DDevice *device, unsigned long mask);
}

#endif // RENDER_RENDER_PLAYER_VIEW_DEPENDENCIES_H

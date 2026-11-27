#ifndef RENDER_RENDER_PLAYER_VIEW_H
#define RENDER_RENDER_PLAYER_VIEW_H

class c_lights_view;
struct D3DSurface;

void render_player_view_build_lighting(c_lights_view *lights, long user, long window, D3DSurface *surface0, D3DSurface *surface1, D3DSurface *surface2);

#endif // RENDER_RENDER_PLAYER_VIEW_H

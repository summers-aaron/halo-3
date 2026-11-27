#ifndef OBJECTS_EMBLEMS_H
#define OBJECTS_EMBLEMS_H

// Functions defined in tu/objectslibxbox/emblems.cpp that other units call.

struct emblem_info;

// match: 0x3672a0: __restrict on info keeps the shipped PIBU mangling; plain pointers emit PBU and miss the @address name
bool emblem_set_render_constants_for_emblem(const emblem_info *__restrict info);

union real_point2d;
struct D3DTexture;

extern "C" void emblem_draw_screen_quad(void *loading_screen, const real_point2d *position, const real_point2d *anchor, D3DTexture *texture, float scale);

#endif // OBJECTS_EMBLEMS_H

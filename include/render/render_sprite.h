#ifndef RENDER_RENDER_SPRITE_H
#define RENDER_RENDER_SPRITE_H

// Functions defined in tu/graphicslibxbox/render_sprite.cpp that other units call.

union real_argb_color;
union real_point3d;
union real_vector3d;

void build_sprite(long bitmap_tag_index, short orientation, short sequence_index, short sprite_index,
	const real_point3d *point, const real_vector3d *vector, float rotation, float width, unsigned long flags, real_argb_color *color);

#endif // RENDER_RENDER_SPRITE_H

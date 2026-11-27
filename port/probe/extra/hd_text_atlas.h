#ifndef PORT_PROBE_HD_TEXT_ATLAS_H
#define PORT_PROBE_HD_TEXT_ATLAS_H

struct D3DTexture;
union real_rectangle2d;

struct s_port_ui_hd_glyph
{
    int width;
    int height;
    int bearing_x;
    int bearing_y;
    float advance;
};

bool port_menu_hd_text_enabled();
bool port_ui_hd_text_enabled();
bool port_ui_hd_text_prepare_atlas();
// Allocates the HD and SDF glyph atlases' texture memory; call once at shell start, while the 16 MB each needs below
// 4 GB is still free.
void port_ui_hd_text_reserve_atlases();
D3DTexture *port_ui_hd_text_glyph(unsigned int pixel_height, unsigned int codepoint,
    s_port_ui_hd_glyph *out_glyph, real_rectangle2d *uv);

#endif

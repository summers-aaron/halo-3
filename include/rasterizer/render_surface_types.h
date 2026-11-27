#ifndef RASTERIZER_RENDER_SURFACE_TYPES_H
#define RASTERIZER_RENDER_SURFACE_TYPES_H

#include <stddef.h>

struct D3DSurface;
struct D3DTexture;

#pragma bitfield_order(push)
#pragma bitfield_order(lsb_to_msb)
struct s_texture_header {
    union {
        unsigned int words[13];
        struct {
            unsigned int fetch_constant_head[10];
            int : 13;
            int exp_adjust : 6;
            int : 13;
        };
    };
};
#pragma bitfield_order(pop)
struct s_surface_description {
    unsigned char depth_stencil;
    long width;
    long height;
    int format;
    unsigned char texture_srgb;
    unsigned int exponent_bias;
    unsigned int resolve_width;
    unsigned int resolve_height;
    int resolve_format;
    unsigned char srgb;
    unsigned int color_exponent_bias;
    unsigned int resolve_exponent_bias;
    int multisample;
    int alias_surface;
    unsigned int alias_offset;
    unsigned int base;
    unsigned int hierarchical_z;
};

typedef char surface_description_size[(sizeof(s_surface_description) == 0x44) ? 1 : -1];
typedef char surface_description_resolve_format_offset[(offsetof(s_surface_description, resolve_format) == 0x20) ? 1 : -1];
typedef char surface_description_resolve_exponent_bias_offset[(offsetof(s_surface_description, resolve_exponent_bias) == 0x2c) ? 1 : -1];
typedef char surface_description_alias_surface_offset[(offsetof(s_surface_description, alias_surface) == 0x34) ? 1 : -1];

class c_render_surface {
public:
    long create(const s_surface_description &description, long surface_index);
    void create_resolve_resources(const s_surface_description &description);
    D3DSurface *surface;
    s_texture_header *resource;
    s_texture_header texture;
    D3DSurface *resolve;
    s_surface_description description;
    long index;
    char *first_address;
    char *last_address;
    unsigned long clear_mask[7];
    __forceinline const s_surface_description &get_surface_description() { return description; }
    __forceinline D3DSurface *get_d3d_surface() { return surface; }
    __forceinline D3DTexture *get_d3d_texture() { return (D3DTexture *)resource; }
};

typedef char render_surface_size[(sizeof(void *) != 4 || ((sizeof(c_render_surface) == 0xac))) ? 1 : -1];
typedef char render_surface_resolve_offset[(sizeof(void *) != 4 || ((offsetof(c_render_surface, resolve) == 0x3c))) ? 1 : -1];
typedef char render_surface_description_offset[(sizeof(void *) != 4 || ((offsetof(c_render_surface, description) == 0x40))) ? 1 : -1];
typedef char render_surface_index_offset[(sizeof(void *) != 4 || ((offsetof(c_render_surface, index) == 0x84))) ? 1 : -1];
typedef char render_surface_first_address_offset[(sizeof(void *) != 4 || ((offsetof(c_render_surface, first_address) == 0x88))) ? 1 : -1];
typedef char render_surface_last_address_offset[(sizeof(void *) != 4 || ((offsetof(c_render_surface, last_address) == 0x8c))) ? 1 : -1];
typedef char render_surface_mask_offset[(sizeof(void *) != 4 || ((offsetof(c_render_surface, clear_mask) == 0x90))) ? 1 : -1];

struct s_surface_group_description {
    long flags;
    unsigned char depth_stencil;
    float resolve_width;
    float resolve_height;
    int resolve_format;
    unsigned int color_exponent_bias;
    unsigned char srgb;
    int multisample;
    float width;
    float height;
    int format;
    unsigned int exponent_bias;
    unsigned char texture_srgb;
    unsigned int resolve_exponent_bias;
    unsigned int base;
    unsigned int hierarchical_z;
    int alias_surface;
    long reserved;
};

typedef char surface_group_description_size[(sizeof(s_surface_group_description) == 0x48) ? 1 : -1];
typedef char surface_group_resolve_width_offset[(offsetof(s_surface_group_description, resolve_width) == 0x8) ? 1 : -1];
typedef char surface_group_multisample_offset[(offsetof(s_surface_group_description, multisample) == 0x1c) ? 1 : -1];
typedef char surface_group_resolve_exponent_bias_offset[(offsetof(s_surface_group_description, resolve_exponent_bias) == 0x34) ? 1 : -1];

#endif

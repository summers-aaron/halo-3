// graphicslibxbox/rasterizer_render_targets.obj: from the records 0x14c8a8 0x14c918 0x151df0 0x168618 0x16b718 0x16cd20 0x16d238 0x16d278 0x16ef90 0x177748 0x178708 0x178860 0x17f900 0x17fd48 0x2a46e8 0x44fa48 0x450090 0x450168 0x4502d0 0x4504f8 0x450558 0x4505b0
// @flags /TP /O2
// @address 0x5a7618 D3DResource_Release
// @address 0x5a4d88 D3DSurface_GetDesc
// @address 0x86d80 XGSetTextureHeader
// @address 0x177870 D3DDevice_Resolve
// @address 0x58e5c0 memset
// @address 0x8c469c ?g_color_surfaces@c_rasterizer@@0PAW4e_surface@1@A
#include "rasterizer/xenon/rasterizer_d3d_calls.h"
#include "rasterizer/rasterizer_main_globals.h"
#include "rasterizer/rasterizer_render_targets.h"
// @address 0x14c918 ?get_specialized_surface@c_render_surface_group@@QAAPAVc_render_surface@@W4e_surface@c_rasterizer@@@Z
// @address 0x450090 ?release@c_render_surface_group@@QAAJXZ
// @address 0x450168 ?create_resolve_resources@c_render_surface@@QAAXABUs_surface_description@@@Z
// @address 0x177748 ?resolve_surface@c_rasterizer@@SAXW4e_surface@1@JPBTrectangle2d@@JJ@Z
#include "math/real_math.h"
#include <string.h>
#include <math.h>
#include "rasterizer/rasterizer_textures_cache.h"
#include "rasterizer/xenon/rasterizer_texture_description.h"
#include "rasterizer/dx9/rasterizer_dx9_device.h"
#include "rasterizer/xenon/rasterizer_xenon_d3d.h"

extern "C" {
struct D3DDevice;
}
class c_render_surface;
class c_rasterizer_texture_ref;
#include "rasterizer/rasterizer_render_target_types.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#if !defined(_XBOX) || defined(__clang__)
#include "rasterizer/rasterizer_device_globals.h"
#endif
extern "C" {
// @address 0x8c46b8 g_antialiasing_mode
int g_antialiasing_mode;
// @address 0x72cd3a g_rasterizer_using_albedo_sampler
bool g_rasterizer_using_albedo_sampler = true;
// @address 0x832a88 g_rasterizer_surface_groups
c_render_surface_group g_rasterizer_surface_groups[68];
}


#include "cseries/xenon_platform_api.h"
#include "cseries/crt_xdk.h"
#include "rasterizer/xenon/rasterizer_render_target_resources.h"
struct D3DSurface;
struct D3DTexture;

enum _D3DFORMAT { _render_target_format_unknown = 0 };
typedef _D3DFORMAT D3DFORMAT;
enum _D3DMULTISAMPLE_TYPE { _render_target_multisample_none = 0 };
typedef _D3DMULTISAMPLE_TYPE D3DMULTISAMPLE_TYPE;
#include "rasterizer/rasterizer_d3d_allocations.h"


extern "C" {

struct D3DBaseTexture;
}


#include "rasterizer/render_surfaces_interface.h"

struct s_resolve_rect {
    long x1;
    long y1;
    long x2;
    long y2;
};
struct s_resolve_point {
    long x;
    long y;
};


#include "rasterizer/xenon/rasterizer_tiling_settings_types.h"




// @address 0x14c8a8 ?set_render_target@c_rasterizer@@SAXJW4e_surface@1@@Z
void c_rasterizer::set_render_target(long index, e_surface surface)
{
    g_color_surfaces[index] = surface;
    c_render_surface *render_surface = c_render_surfaces_interface::get_render_surface(surface);
    D3DSurface *d3d_surface = render_surface->get_d3d_surface();
    long result = c_rasterizer::get_device()->SetRenderTarget(index, d3d_surface);
}

c_render_surface *c_render_surface_group::get_specialized_surface(c_rasterizer::e_surface mode)
{
    c_render_surface_group *slot = this;
    int index = 0;
    if (slot->description.flags & 1)
        index = c_rasterizer::g_screen_resolution;
    if (slot->description.flags & 2) {
        int variant = c_rasterizer::g_tiling_configuration;
        if (!(D3DDevice_GetDeviceState(c_rasterizer::get_device()) & 1) && (variant == 1 || variant == 2))
            variant = 0;
        switch (g_antialiasing_mode) {
        case 1:
            variant = 3;
            break;
        case 2:
            variant = 4;
            break;
        case 3:
            variant = 5;
            break;
        }
        index = index * 6 + variant;
    }
    if (slot->description.flags & 8) {
        index *= 4;
        switch (mode) {
        case c_rasterizer::_surface_accum_HDR:
        case c_rasterizer::_surface_accum_HDR_secondary:
            if (c_rasterizer::g_HDR_alpha_blend && c_rasterizer::g_HDR_render_mode == 2)
                index += 3;
            else
                index += c_rasterizer::g_HDR_render_mode;
            break;
        case c_rasterizer::_surface_accum_LDR:
        case c_rasterizer::_surface_accum_LDR_secondary:
            if (c_rasterizer::g_HDR_alpha_blend && c_rasterizer::g_LDR_render_mode == 2)
                index += 3;
            else
                index += c_rasterizer::g_LDR_render_mode;
            break;
        }
    }
    if (slot->description.flags & 0x20)
        index = index * 4 + g_antialiasing_mode;
    return &slot->surfaces[index];
}

// @address 0x168618 ?set_depth_stencil_surface@c_rasterizer@@SAXW4e_surface@1@@Z
void c_rasterizer::set_depth_stencil_surface(e_surface surface)
{
    g_depth_stencil_surface = surface;
    c_render_surface *depth_surface = 0;
    if (surface != _surface_none)
        depth_surface = c_render_surfaces_interface::get_render_surface(surface);
    long result = 0;
    if (g_color_surfaces[0] != _surface_none && depth_surface) {
        c_render_surface *color_surface = c_render_surfaces_interface::get_render_surface(g_color_surfaces[0]);
        result = Direct3D_CheckDepthStencilMatch(0, 1, 0x28280186,
            color_surface->get_surface_description().resolve_format,
            depth_surface->get_surface_description().resolve_format);
    }
    if (depth_surface && depth_surface->get_surface_description().resolve_format == 0x1a220197)
        c_rasterizer::m_use_floating_point_z_buffer = true;
    else
        c_rasterizer::m_use_floating_point_z_buffer = false;
    c_render_surface *render_surface = c_render_surfaces_interface::get_render_surface(surface);
    D3DSurface *d3d_surface = render_surface->get_d3d_surface();
    result = c_rasterizer::get_device()->SetDepthStencilSurface(d3d_surface);
}
// @address 0x16b718 ?lock_surface_as_texture@c_rasterizer@@SA_NW4e_surface@1@KPAPADPAJ@Z
bool c_rasterizer::lock_surface_as_texture(e_surface surface, unsigned long flags, char **pixels, long *pitch)
{
    c_render_surface *render_surface = c_render_surfaces_interface::get_render_surface(surface);
    _D3DLOCKED_RECT locked;
    long result = render_surface->get_d3d_texture()->LockRect(0, &locked, 0, 16);
    if (result == 0) {
        *pixels = (char *)locked.pBits;
        *pitch = locked.Pitch;
    } else {
        *pixels = 0;
        *pitch = 0;
    }
    return result == 0;
}

// @address 0x16cd20 rasterizer_set_default_render_targets
extern "C" void rasterizer_set_default_render_targets(void)
{
    c_rasterizer::set_depth_stencil_surface((c_rasterizer::e_surface)3);
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)13);
    c_rasterizer::set_render_target(1, (c_rasterizer::e_surface)0);
    c_rasterizer::set_render_target(2, (c_rasterizer::e_surface)0);
    c_rasterizer::set_render_target(3, (c_rasterizer::e_surface)0);
}

// @address 0x16d238 ?get_surface_height@c_rasterizer@@SAJW4e_surface@1@@Z
long c_rasterizer::get_surface_height(e_surface surface)
{
    c_render_surface *render_surface = c_render_surfaces_interface::get_render_surface(surface);
    return render_surface->get_surface_description().width;
}

// @address 0x16d278 ?get_surface_width@c_rasterizer@@SAJW4e_surface@1@@Z
long c_rasterizer::get_surface_width(e_surface surface)
{
    c_render_surface *render_surface = c_render_surfaces_interface::get_render_surface(surface);
    return render_surface->get_surface_description().height;
}

// @address 0x16ef90 ?get_render_target_alpha_multiplier@c_rasterizer@@SAMJ@Z
float c_rasterizer::get_render_target_alpha_multiplier(long render_target)
{
    c_render_surface *render_surface = c_render_surfaces_interface::get_render_surface(g_color_surfaces[render_target]);
    return 1.0f / (float)(1 << render_surface->get_surface_description().color_exponent_bias);
}

extern "C" {
// @address 0x832a68 g_rasterizer_resolved_surface_mask
}

void c_rasterizer::resolve_surface(e_surface slot_index, long target, const rectangle2d *bounds, long x, long y)
{
    bool rebound = false;
    c_render_surface *handle = g_rasterizer_surface_groups[slot_index].get_specialized_surface(slot_index);
    if (handle->resolve != handle->surface) {
        D3DDevice_SetRenderTarget_External(c_rasterizer::get_device(), target, handle->resolve);
        rebound = true;
    }
    D3DBaseTexture *destination = (D3DBaseTexture *)&handle->texture;
    long resolve_target = target;
    if (target == 4)
        resolve_target = 20;
    unsigned long flags = (handle->description.resolve_exponent_bias << 26) | resolve_target;
    s_resolve_rect rect;
    s_resolve_rect *source = 0;
    if (bounds) {
        source = &rect;
        rect.x1 = bounds->x0;
        rect.x2 = bounds->x1;
        rect.y1 = bounds->y0;
        rect.y2 = bounds->y1;
    }
    s_resolve_point point;
    point.x = x;
    point.y = y;
    D3DDevice_Resolve(c_rasterizer::get_device(), flags, source, destination, &point, 0, 0, 0, 1.0f, 0, 0);
    rasterizer_mark_surface_resolved(handle);
    if (rebound)
        D3DDevice_SetRenderTarget_External(c_rasterizer::get_device(), target, handle->surface);
}

// @address 0x178708 rasterizer_mark_surface_resolved
void rasterizer_mark_surface_resolved(c_render_surface *surface)
{
    for (long word = 0; word < 7; word++)
        g_rasterizer_resolved_surface_mask[word] &= ~surface->clear_mask[word];
    g_rasterizer_resolved_surface_mask[surface->index >> 5] |= 1 << (surface->index & 31);
}

// @address 0x178860 ?set_using_albedo_sampler@c_rasterizer@@SAX_N@Z
void c_rasterizer::set_using_albedo_sampler(bool using_albedo_sampler)
{
    g_rasterizer_using_albedo_sampler = using_albedo_sampler;
    int sampler_value = !using_albedo_sampler;
    set_pixel_shader_constant_bool(0, 1, &sampler_value);
}

// @address 0x17f900 rasterizer_is_surface_resolved
extern "C" bool rasterizer_is_surface_resolved(c_rasterizer::e_surface surface)
{
    c_render_surface *handle = c_render_surfaces_interface::get_render_surface(surface);
    long index = handle->index;
    return (g_rasterizer_resolved_surface_mask[index >> 5] & (1 << (index & 31))) != 0;
}

// @address 0x17fd48 ?set_aliased_surface_as_texture@c_rasterizer@@SAXJW4e_surface@1@@Z
void c_rasterizer::set_aliased_surface_as_texture(long sampler_index, e_surface surface)
{
    x_last_sampler_textures[sampler_index] = -1;
    c_render_surface *render_surface = c_render_surfaces_interface::get_render_surface(surface);
    c_rasterizer::get_device()->SetTexture(sampler_index, render_surface->get_d3d_texture());
}
// @address 0x2a46e8 rasterizer_clear_postprocess_target
extern "C" void rasterizer_clear_postprocess_target(unsigned long color)
{
    c_rasterizer::set_render_target(0, (c_rasterizer::e_surface)30);
    c_rasterizer::clear(15, color, 1.0f, 0);
    c_rasterizer::resolve_surface((c_rasterizer::e_surface)30, 0, 0, 0, 0);
}

long c_render_surface_group::release()
{
  unsigned int *resource;
  int surface_index;
  c_render_surface *surface;

  surface_index = 0;
  if (0 < count) {
    do {
      // match: 0x450090: retain the member-reference boundary for commutative add ordering.
      c_render_surface *const &surface_array = surfaces;
      surface = &surface_array[surface_index];
      resource = (unsigned int *)surface->resource;
      if (resource != 0) {
        if ((*resource & 0x100000) == 0) {
          if (surface->description.alias_surface == 0) {
            XPhysicalFree((void *)(size_t)(resource[8] & 0xfffff000));
          }
          RtlFreeHeap(GetProcessHeap(),0,resource);
        }
        else {
          D3DResource_Release(resource);
        }
        surface->resource = 0;
      }
      if (surface->resolve != surface->surface) {
        if (surface->resolve != 0) {
          D3DResource_Release(surface->resolve);
        }
        surface->resolve = 0;
      }
      if (surface->surface != 0) {
        D3DResource_Release(surface->surface);
        surface->surface = 0;
      }
      surface_index = surface_index + 1;
    } while (surface_index < count);
  }
  return 0;
}

void c_render_surface::create_resolve_resources(const s_surface_description &description)
{
    c_render_surface *self = this;
    if (self->resource && (self->resource->words[8] & 0xfffff000)) {
        int format = description.format;
        if (description.srgb)
            format |= 0x7e00;
        XGSetTextureHeader(description.width, description.height, 1, 1, format,
                      0, 0, 0, 0, &self->texture, 0, 0);
        self->texture.words[8] = (self->texture.words[8] & 0xfff) |
                                (self->resource->words[8] & 0xfffff000);
        self->texture.words[12] = (self->texture.words[12] & 0xfff) |
                                 (self->resource->words[12] & 0xfffff000);
        self->texture.words[7] = (self->texture.words[7] & ~0x300u) |
                                ((self->texture.words[7] & 0xc) << 6);
    } else {
        memset(&self->texture, 0, sizeof(self->texture));
    }
    if (self->surface) {
        int format = description.resolve_format;
        if (description.srgb)
            format |= 0x7e00;
        if (format == 0x1a2201bf) {
            s_surface_parameters parameters;
            parameters.base = description.base;
            parameters.flags = 0;
            parameters.hierarchical_z = description.hierarchical_z;
            int resolve_format = description.format == 0x28280136 ?
                                 0x1a2001b6 : 0x1a2201bf;
            c_d3d_resource_allocator::create_render_target(description.resolve_width, description.resolve_height,
                          (D3DFORMAT)resolve_format, (D3DMULTISAMPLE_TYPE)description.multisample, 0, 0,
                          &self->resolve, &parameters);
        } else {
            self->resolve = self->surface;
        }
    } else {
        self->resolve = 0;
    }
}
// @address 0x4504f8 rasterizer_set_depth_stencil_surface_direct
extern "C" void rasterizer_set_depth_stencil_surface_direct(D3DSurface *surface)
{
    s_render_targets_surface_desc desc;
    D3DSurface_GetDesc(surface, &desc);
    c_rasterizer::set_use_floating_point_z_buffer(desc.format == 0x1a220197);
    c_rasterizer::get_device()->SetDepthStencilSurface(surface);
}

// @address 0x450558 rasterizer_set_back_buffer_render_target
extern "C" void rasterizer_set_back_buffer_render_target(long, long)
{
    c_rasterizer::g_color_surfaces[0] = (c_rasterizer::e_surface)1;
    c_render_surface *render_surface = c_render_surfaces_interface::get_render_surface((c_rasterizer::e_surface)1);
    D3DSurface *resolve_surface = render_surface->resolve;
    if (resolve_surface)
        c_rasterizer::get_device()->SetRenderTarget(0, resolve_surface);
}


inline unsigned long c_rasterizer_texture_ref::get_d3d_format()
{
    D3DBaseTexture *texture = get_d3d_texture();
    unsigned long format = 0;
    s_render_targets_surface_desc desc;
    if (texture && get_texture_level_desc(texture, 0, &desc))
        format = desc.format;
    return format;
}

// @address 0x4505b0 ?resolve_entire_surface@c_rasterizer@@SAXVc_rasterizer_texture_ref@@J@Z
void c_rasterizer::resolve_entire_surface(c_rasterizer_texture_ref texture, long source_target)
{
    unsigned long flags = source_target;
    unsigned long format = texture.get_d3d_format();
    if (format == 0x1a20015a || format == 0x2d22ab9f)
        flags |= 0xec000000;
    D3DDevice_Resolve(c_rasterizer::get_device(), flags, 0, texture.get_d3d_texture(), 0, 0, 0, 0, 1.0f, 0, 0);
}

__forceinline void c_rasterizer::set_pixel_shader_constant_bool(long start, long count, const int *values)
{
    D3DDevice_SetPixelShaderConstantB(c_rasterizer::get_device(), start, values, count);
}

extern "C" {
// @address 0x8c4714 g_rasterizer_surface_count
// @address 0x8c485c g_d3d_resource_allocator_device
}

static __forceinline int rasterizer_render_targets_srgb_format(int format)
{
    return g_gputextureformat_to_srgb_gputextureformat_table[format & 63] | (format & ~63) | 0x7e00;
}

static __forceinline long rasterizer_render_targets_create_depth(unsigned int width, unsigned int height, int format,
    int multisample, D3DSurface **surface, const s_surface_parameters *parameters)
{
    *surface = 0;
    long result = 0;
    if (g_d3d_resource_allocator_device) {
        *surface = D3DDevice_CreateSurface(width, height, format, multisample, parameters);
        if (*surface)
            result = 0;
        else
            result = 0x8007000e;
    }
    return result;
}

#if !defined(_XBOX) || defined(__clang__)
// PC (port/GAME-CHANGES.md, "Custom resolutions"): k_render_surface_group_descriptions has surfaces share memory: an
// alias surface's texture lies in its host's, at its start or at its end (flag 64), and resolving one surface marks
// those it overlaps unresolved (rasterizer_main_compute_surface_overlaps). At 1152x640 every alias fits its host. At a
// custom resolution, screen-sized aliases outgrow the two hosts that are not screen-sized, the shadow map (2, 512x512:
// the scene depth's 4x views 4, 5, 6 and the scope depth 53 / 54) and the water ripple surface (56, 800x800: 66 at its
// end), running past or before them over other memory; and a split screen's two halves (W x H/2 at a host's start and
// at its end) overlap where H/2 rows are padded to a 32-row texture tile, which marks one player's surfaces unresolved
// when the other's resolve. Such a host is given the memory its aliases need laid out as retail lays them out
// (rasterizer_render_targets_footprint): the surfaces that share memory at 1152x640 do, and no others. The memory a
// surface's texture is given when that is more than its own size (0: its own), by
// c_render_surface_group::g_surface_buffer index; c_render_surface_group::create sets it before the surface is made.
static unsigned int g_rasterizer_render_targets_backing_size[NUMBEROF(c_render_surface_group::g_surface_buffer)];

extern "C" void rasterizer_render_targets_clear_backing_sizes(void)
{
    memset(g_rasterizer_render_targets_backing_size, 0, sizeof(g_rasterizer_render_targets_backing_size));
}

extern "C" void port_write_error(const char *text);

static unsigned int rasterizer_render_targets_texture_size(long width, long height, int format)
{
    unsigned int size = 0;
    if (width > 0 && height > 0)
        XGSetTextureHeader(width, height, 1, 0, (D3DFORMAT)format, 0, 0, 0, 0, 0, &size, 0);
    return size;
}
#endif

// @address 0x4502d0 ?create@c_render_surface@@QAAJABUs_surface_description@@J@Z
long c_render_surface::create(const s_surface_description &description, long surface_index)
{
    surface = 0;
    resource = 0;
    resolve = 0;
    index = g_rasterizer_surface_count;
    const s_surface_description *source = &description;
    memcpy(&this->description, source, sizeof(description));
    long result = 0;
    if (description.width > 0 && description.height > 0) {
        int format = description.format;
        unsigned long flags = 0;
        void *data = 0;
        if (description.texture_srgb)
            format = rasterizer_render_targets_srgb_format(format);
        if (description.alias_surface) {
            flags = 3;
            c_render_surface *alias = (c_render_surface *)g_rasterizer_surface_groups[description.alias_surface].surfaces;
            data = (void *)(size_t)((alias->resource->words[8] & 0xfffff000) + description.alias_offset);
        }
#if !defined(_XBOX) || defined(__clang__)
        else if (g_rasterizer_render_targets_backing_size[surface_index] != 0) {
            // PC: a host its aliases outgrow (g_rasterizer_render_targets_backing_size): its texture at the start of
            // memory of that size, freed as its own (c_render_surface_group::dispose)
            data = allocate_d3d_memory_tracked(g_rasterizer_render_targets_backing_size[surface_index]);
            if (data)
                flags = 3;
            else
                g_rasterizer_render_targets_backing_size[surface_index] = 0;
        }
#endif
        s_texture_header *created = 0;
        result = c_d3d_resource_allocator::create_texture(description.width, description.height, 1, 1, (D3DFORMAT)format, 0,
                             (D3DTexture **)&created, 0, (unsigned char *)data, flags);
#if !defined(_XBOX) || defined(__clang__)
        // PC: create_texture reports success when its memory could not be allocated (the 360's never failed) and
        // leaves the texture unset: an out-of-memory failure, not a texture to fill in
        if (result >= 0 && created == 0) {
            port_write_error("port: rasterizer: no memory below 4 GB for a render target at this resolution\n");
            result = 0x8007000e; // E_OUTOFMEMORY
        }
#endif
        if (result >= 0) {
            // match: 0x4502d0: restrict on d keeps exponent_bias in r9 with the exp_adjust destination in r10 across the rlwimi insert
            const s_surface_description *__restrict d = &description; created->exp_adjust = d->exponent_bias;
            resource = created;
        } else {
            resource = 0;
            return result;
        }
    }
    if ((int)description.resolve_width > 0 && (int)description.resolve_height > 0) {
        int format = description.resolve_format;
        if (description.srgb)
            format |= 0x7e00;
        s_surface_parameters parameters;
        parameters.base = description.base;
        parameters.flags = description.color_exponent_bias;
        parameters.hierarchical_z = description.hierarchical_z;
        if (description.depth_stencil) {
            D3DSurface *created;
            long status = rasterizer_render_targets_create_depth(description.resolve_width, description.resolve_height,
                format, description.multisample, &created, &parameters);
            result |= status;
            if (status >= 0) {
                surface = created;
            } else {
                surface = 0;
                return result;
            }
        } else {
            D3DSurface *created = 0;
            long status = c_d3d_resource_allocator::create_render_target(description.resolve_width, description.resolve_height,
                (D3DFORMAT)format, (D3DMULTISAMPLE_TYPE)description.multisample, 0, 1, &created, &parameters);
            result |= status;
            if (status >= 0)
                surface = created;
            else
                surface = 0;
        }
    }
    create_resolve_resources(description);
    return result;
}

// @address 0x8293e8 ?g_surface_buffer@c_render_surface_group@@2PAVc_render_surface@@A
c_render_surface c_render_surface_group::g_surface_buffer[224];


// @rdata 0x820254f8..0x82025888
static const s_rasterizer_tiling_settings k_rasterizer_render_targets_tiling_settings[6] = {
    {0x480, 0x280, 0x480, 0x280, 0x1, 0x0,
        {{0x0, 0x0, 0x480, 0x280}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0},
            {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}}},
    {0x480, 0x280, 0x480, 0x140, 0x2, 0x1,
        {{0x0, 0x0, 0x480, 0x140}, {0x0, 0x140, 0x480, 0x280}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0},
            {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}}},
    {0x480, 0x280, 0x480, 0xa0, 0x4, 0x2,
        {{0x0, 0x0, 0x480, 0xa0}, {0x0, 0xa0, 0x480, 0x140}, {0x0, 0x140, 0x480, 0x1e0}, {0x0, 0x1e0, 0x480, 0x280}, {0x0, 0x0, 0x0, 0x0},
            {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}}},
    {0x480, 0x140, 0x480, 0x140, 0x1, 0x1,
        {{0x0, 0x0, 0x480, 0x140}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0},
            {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}}},
    {0x240, 0x140, 0x240, 0x140, 0x1, 0x2,
        {{0x0, 0x0, 0x240, 0x140}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0},
            {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}}},
    {0x360, 0x140, 0x360, 0x140, 0x1, 0x1,
        {{0x0, 0x0, 0x360, 0x140}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0},
            {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}, {0x0, 0x0, 0x0, 0x0}}},
};
// @rdata-end

#if !defined(_XBOX) || defined(__clang__)
// PC (port/GAME-CHANGES.md, "Custom resolutions"): the tiling settings at the size the game renders at. The table above
// is the 360's, made for its one 1152x640 frame; c_rasterizer::render_globals is that size unless a custom resolution
// gave it another before the device was made (c_rasterizer::initialize_device, which then calls
// rasterizer_render_targets_update_tiling_settings). Each size and tile edge is scaled from 1152x640 to the render size
// and rounded once, so the tiles still meet edge to edge; a tile's size (resolve_width / resolve_height) is then that of
// its largest tile. At 1152x640 this is the retail table exactly.
static s_rasterizer_tiling_settings g_rasterizer_render_targets_scaled_tiling_settings[6];
static bool g_rasterizer_render_targets_tiling_settings_scaled;

static __forceinline long rasterizer_render_targets_scale_length(long length, long size, long authored)
{
    return (length * size + authored / 2) / authored;
}

extern "C" void rasterizer_render_targets_update_tiling_settings(void)
{
    const long width = c_rasterizer::render_globals.width;
    const long height = c_rasterizer::render_globals.height;
    for (long configuration = 0; configuration < 6; configuration++)
    {
        const s_rasterizer_tiling_settings *authored = &k_rasterizer_render_targets_tiling_settings[configuration];
        s_rasterizer_tiling_settings *scaled = &g_rasterizer_render_targets_scaled_tiling_settings[configuration];
        *scaled = *authored;
        scaled->width = rasterizer_render_targets_scale_length(authored->width, width, 1152);
        scaled->height = rasterizer_render_targets_scale_length(authored->height, height, 640);
        scaled->resolve_width = rasterizer_render_targets_scale_length(authored->resolve_width, width, 1152);
        scaled->resolve_height = rasterizer_render_targets_scale_length(authored->resolve_height, height, 640);
        for (long tile = 0; tile < (long)NUMBEROF(scaled->tile_rects); tile++)
        {
            D3DRECT *rect = &scaled->tile_rects[tile];
            rect->x1 = rasterizer_render_targets_scale_length(authored->tile_rects[tile].x1, width, 1152);
            rect->y1 = rasterizer_render_targets_scale_length(authored->tile_rects[tile].y1, height, 640);
            rect->x2 = rasterizer_render_targets_scale_length(authored->tile_rects[tile].x2, width, 1152);
            rect->y2 = rasterizer_render_targets_scale_length(authored->tile_rects[tile].y2, height, 640);
            if (tile < scaled->tile_count)
            {
                if (rect->x2 - rect->x1 > scaled->resolve_width)
                    scaled->resolve_width = rect->x2 - rect->x1;
                if (rect->y2 - rect->y1 > scaled->resolve_height)
                    scaled->resolve_height = rect->y2 - rect->y1;
            }
        }
    }
    g_rasterizer_render_targets_tiling_settings_scaled = true;
}
#endif

const s_rasterizer_tiling_settings *rasterizer_xenon_main_get_tiling_settings(long screen_resolution, long tiling_configuration)
{
#if !defined(_XBOX) || defined(__clang__)
    if (g_rasterizer_render_targets_tiling_settings_scaled)
        return &g_rasterizer_render_targets_scaled_tiling_settings[screen_resolution * 6 + tiling_configuration];
#endif
    return &k_rasterizer_render_targets_tiling_settings[screen_resolution * 6 + tiling_configuration];
}

static __forceinline long rasterizer_render_targets_scaled_size(float scale, float size)
{
    if (!(scale > 1.0f))
        scale = 1.0f;
    return (long)(float)floor(size / scale + 0.5f);
}

#if defined(_XBOX) && !defined(__clang__)
// (retail's c_render_surface_group::create; on PC rasterizer_render_targets_surface_description takes the tiling sizes)
static __forceinline void rasterizer_render_targets_create_tiling_surface_desc(s_surface_description *description, long tiling_configuration)
{
    const s_rasterizer_tiling_settings *tiling_settings = &k_rasterizer_render_targets_tiling_settings[tiling_configuration];
    description->resolve_width = tiling_settings->resolve_width;
    description->resolve_height = tiling_settings->resolve_height;
    description->width = tiling_settings->width;
    description->height = tiling_settings->height;
    description->multisample = tiling_settings->multisample;
}
#endif

#if !defined(_XBOX) || defined(__clang__)
// The full-screen view's depth-of-field source (render_player_view.cpp, player_view_resolve_depth_of_field_source): the
// size of the 32-pixel aligned depth region it resolves and of the region the scene texture is copied from.
extern "C" void player_view_depth_of_field_source_extents(const rectangle2d *window_bounds, const rectangle2d *title_safe_bounds,
    long *resolve_width, long *resolve_height, long *width, long *height);

// PC (port/GAME-CHANGES.md, "Custom resolutions"): the surfaces k_render_surface_group_descriptions gives in pixels of
// the 360's 1152x640 frame although they are the screen's, at the size the game renders at (the same sizes at 1152x640):
//   5, 6     the scene depth's 4x multisampled views (576x320, 432x320) over the 1x depth buffer's EDRAM: a 4x pixel is
//            2x2 samples, so half of 1152x640 (and 3/8 x 1/2) of the render size;
//   53, 54   the depth the scope screens' scene texture is resolved through (and its A8R8G8B8 header), 320x192: the
//            full-screen view's depth-of-field region (half its quarter-screen centre, cropped to 9:5) 32-pixel aligned;
//   55       that scene texture, 288x160: the region itself.
// The shadow map (2) and the water ripple surfaces (56, 57: 800x800) are not the screen's.
static __forceinline void rasterizer_render_targets_screen_size(long surface, s_surface_description *description)
{
    const long width = c_rasterizer::render_globals.width;
    const long height = c_rasterizer::render_globals.height;
    switch (surface)
    {
    case 2:
        description->width = description->height = description->resolve_width = description->resolve_height =
            c_rasterizer::render_globals.shadow_size;
        break;
    case 5:
    case 6:
        description->width = rasterizer_render_targets_scale_length(description->width, width, 1152);
        description->height = rasterizer_render_targets_scale_length(description->height, height, 640);
        description->resolve_width = rasterizer_render_targets_scale_length(description->resolve_width, width, 1152);
        description->resolve_height = rasterizer_render_targets_scale_length(description->resolve_height, height, 640);
        break;
    case 53:
    case 54:
    case 55:
    {
        rectangle2d window_bounds;
        window_bounds.x0 = 0;
        window_bounds.y0 = 0;
        window_bounds.x1 = (short)width;
        window_bounds.y1 = (short)height;
        rectangle2d title_safe_bounds;
        c_rasterizer::get_fullscreen_render_title_safe_pixel_bounds(&title_safe_bounds);
        long resolve_width, resolve_height, region_width, region_height;
        player_view_depth_of_field_source_extents(&window_bounds, &title_safe_bounds, &resolve_width, &resolve_height,
            &region_width, &region_height);
        description->width = description->resolve_width = surface == 55 ? region_width : resolve_width;
        description->height = description->resolve_height = surface == 55 ? region_height : resolve_height;
        break;
    }
    }
}

static long rasterizer_render_targets_group_count(const s_surface_group_description *group_description)
{
    long count = 1;
    if (group_description->flags & 2)
        count = 6;
    if (group_description->flags & 8)
        count *= 4;
    if (group_description->flags & 32)
        count *= 4;
    return count;
}

// A group's surface as c_render_surface_group::create describes it (retail's loop, in its #else), at the size the game
// renders at or, with native, at the 360's 1152x640 with its tiling settings (retail's sizes, for the memory plan:
// rasterizer_render_targets_footprint). Each surface is described from its group alone: retail carries one description
// through the loop, but every group of more than one surface (flags 2, 8, 32) is a fraction of the screen (flag 1),
// whose sizes each pass sets again, and the other fields each pass sets are set for every surface.
static void rasterizer_render_targets_surface_description(const s_surface_group_description *group_description,
    long surface, long surface_index, bool native, s_surface_description *description)
{
    memset(description, 0, sizeof(*description));
    description->depth_stencil = group_description->depth_stencil;
    description->width = (long)group_description->width;
    description->height = (long)group_description->height;
    description->format = group_description->format;
    description->texture_srgb = group_description->texture_srgb;
    description->exponent_bias = group_description->exponent_bias;
    description->resolve_width = (long)group_description->resolve_width;
    description->resolve_height = (long)group_description->resolve_height;
    description->resolve_format = group_description->resolve_format;
    description->srgb = group_description->srgb;
    description->color_exponent_bias = group_description->color_exponent_bias;
    description->resolve_exponent_bias = group_description->resolve_exponent_bias;
    description->multisample = group_description->multisample;
    description->alias_surface = surface_index > 0 ? surface : group_description->alias_surface;
    description->alias_offset = 0;
    description->base = group_description->base;
    description->hierarchical_z = group_description->hierarchical_z;
    long index = surface_index;
    long view = 0;
    long tiling_configuration = 0;
    long level = 0;
    const long flags = group_description->flags;
    if (flags & 32)
    {
        level = surface_index % 4;
        index = surface_index / 4;
    }
    if (flags & 8)
    {
        view = surface_index % 4;
        index = surface_index / 4;
    }
    if (flags & 2)
    {
        tiling_configuration = index % 6;
        index = index / 6;
    }
    // a size given as a fraction of the screen (flag 1) is one of the size the game renders at, and so are the few
    // screen-derived surfaces the table gives in 1152x640 pixels (rasterizer_render_targets_screen_size)
    if (flags & 1)
    {
        const float screen_width = native ? 1152.0f : (float)c_rasterizer::render_globals.width;
        const float screen_height = native ? 640.0f : (float)c_rasterizer::render_globals.height;
        description->resolve_width = rasterizer_render_targets_scaled_size(group_description->resolve_width, screen_width);
        description->resolve_height = rasterizer_render_targets_scaled_size(group_description->resolve_height, screen_height);
        description->width = rasterizer_render_targets_scaled_size(group_description->width, screen_width);
        description->height = rasterizer_render_targets_scaled_size(group_description->height, screen_height);
    }
    else if (!native)
    {
        rasterizer_render_targets_screen_size(surface, description);
    }
    if (surface == 4 || surface == 52)
    {
        switch (level)
        {
        case 1:
        case 3:
            description->resolve_height *= 2;
            description->height *= 2;
            break;
        case 2:
            description->resolve_width *= 2;
            description->resolve_height *= 2;
            description->width *= 2;
            description->height *= 2;
            break;
        }
    }
    if (flags & 32)
    {
        switch (level)
        {
        case 1:
            description->resolve_height = (long)description->resolve_height / 2;
            description->height = description->height / 2;
            break;
        case 2:
            description->resolve_width = (long)description->resolve_width / 2;
            description->resolve_height = (long)description->resolve_height / 2;
            description->width = description->width / 2;
            description->height = description->height / 2;
            break;
        case 3:
            description->resolve_width = (long)(description->resolve_width * 3) / 4;
            description->resolve_height = (long)description->resolve_height / 2;
            description->width = description->width * 3 / 4;
            description->height = description->height / 2;
            break;
        }
    }
    if (flags & 8)
    {
        // retail's switch: every view 0x18280186, unbiased, sRGB but view 2
        description->resolve_format = 0x18280186;
        description->format = 0x18280186;
        description->color_exponent_bias = 0;
        description->resolve_exponent_bias = 0;
        description->exponent_bias = 0;
        description->srgb = view != 2;
        description->texture_srgb = view != 2;
    }
    if (flags & 2)
    {
        // the table at the render size (rasterizer_render_targets_update_tiling_settings), or the 360's
        const s_rasterizer_tiling_settings *tiling_settings = native
            ? &k_rasterizer_render_targets_tiling_settings[tiling_configuration]
            : rasterizer_xenon_main_get_tiling_settings(0, tiling_configuration);
        description->resolve_width = tiling_settings->resolve_width;
        description->resolve_height = tiling_settings->resolve_height;
        description->width = tiling_settings->width;
        description->height = tiling_settings->height;
        description->multisample = tiling_settings->multisample;
        if (!(flags & 4))
        {
            description->width = description->resolve_width;
            description->height = description->resolve_height;
        }
    }
    if (flags & 16)
    {
        if (description->multisample == 0 || description->multisample == 1)
            description->resolve_width = (description->resolve_width + 79) / 80 * 80;
        else if (description->multisample == 2)
            description->resolve_width = (description->resolve_width + 39) / 40 * 40;
    }
}

static unsigned int rasterizer_render_targets_surface_size(long surface, long surface_index, bool native)
{
    s_surface_description description;
    rasterizer_render_targets_surface_description(&k_render_surface_group_descriptions[surface], surface, surface_index,
        native, &description);
    return rasterizer_render_targets_texture_size(description.width, description.height, description.format);
}

static unsigned int rasterizer_render_targets_footprint(long surface);

// The most memory taken at the start of a surface's memory by a surface retail makes at most native_limit bytes: its
// group's other surfaces, and the surfaces of the groups placed at its start (without flag 64), with what lies in them.
static unsigned int rasterizer_render_targets_largest_start(long surface, unsigned int native_limit)
{
    unsigned int largest = 0;
    const long count = rasterizer_render_targets_group_count(&k_render_surface_group_descriptions[surface]);
    for (long surface_index = 1; surface_index < count; surface_index++)
    {
        const unsigned int size = rasterizer_render_targets_surface_size(surface, surface_index, false);
        if (size > largest && rasterizer_render_targets_surface_size(surface, surface_index, true) <= native_limit)
            largest = size;
    }
    for (long alias = 1; alias < (long)NUMBEROF(k_render_surface_group_descriptions); alias++)
    {
        const s_surface_group_description *alias_description = &k_render_surface_group_descriptions[alias];
        if (alias == surface || alias_description->alias_surface != surface || (alias_description->flags & 64))
            continue;
        if (rasterizer_render_targets_surface_size(alias, 0, true) <= native_limit)
        {
            const unsigned int footprint = rasterizer_render_targets_footprint(alias);
            if (footprint > largest)
                largest = footprint;
        }
        const unsigned int start = rasterizer_render_targets_largest_start(alias, native_limit);
        if (start > largest)
            largest = start;
    }
    return largest;
}

// The memory a group's first surface needs for itself and everything retail places in it, at the size the game renders
// at (g_rasterizer_render_targets_backing_size): its own texture, its largest surface at the start, each surface placed
// at its end (flag 64, with what lies in that one), and, for every start and end surface that retail keeps apart (the
// two fit in the host at 1152x640: a split screen's halves), both, so they stay apart. Retail's sizes are all multiples
// of the 32x32 texture tiles, so a texture's memory is its pixels' and these pairs fill the host at most; at another
// size a half screen's rows are padded to a tile and the two halves can need more than the full screen's texture.
// Surfaces that share memory in retail still do. At 1152x640 this is the texture's own size (nothing grows).
static unsigned int rasterizer_render_targets_footprint(long surface)
{
    const unsigned int native_size = rasterizer_render_targets_surface_size(surface, 0, true);
    unsigned int footprint = rasterizer_render_targets_surface_size(surface, 0, false);
    const unsigned int start = rasterizer_render_targets_largest_start(surface, 0xffffffff);
    if (start > footprint)
        footprint = start;
    for (long alias = 1; alias < (long)NUMBEROF(k_render_surface_group_descriptions); alias++)
    {
        const s_surface_group_description *alias_description = &k_render_surface_group_descriptions[alias];
        if (alias == surface || alias_description->alias_surface != surface || !(alias_description->flags & 64))
            continue;
        const unsigned int end = rasterizer_render_targets_footprint(alias);
        if (end > footprint)
            footprint = end;
        const unsigned int native_end = rasterizer_render_targets_surface_size(alias, 0, true);
        if (native_end <= native_size)
        {
            const unsigned int apart = rasterizer_render_targets_largest_start(surface, native_size - native_end);
            if (apart != 0 && apart + end > footprint)
                footprint = apart + end;
        }
    }
    return footprint;
}
#endif

// @address 0x44fa48 ?create@c_render_surface_group@@QAAJABUs_surface_group_description@@W4e_surface@c_rasterizer@@PAUD3DSurface@@@Z
long c_render_surface_group::create(const s_surface_group_description &surface_group_description, c_rasterizer::e_surface surface, D3DSurface *d3d_surface)
{
    c_render_surface_group *group = this;
    group->count = 1;
    memcpy(&group->description, &surface_group_description, sizeof(group->description));
    long group_flags = group->description.flags;
    if (group_flags & 2)
        group->count = 6;
    if (group_flags & 8)
        group->count *= 4;
    if (group_flags & 32)
        group->count *= 4;
    group->surfaces = &c_render_surface_group::g_surface_buffer[g_rasterizer_surface_count];
#if !defined(_XBOX) || defined(__clang__)
    // PC (port/GAME-CHANGES.md, "Custom resolutions"): each surface as retail describes it, at the size the game renders
    // at (rasterizer_render_targets_surface_description); a surface placed at its host's end (flag 64) at the end of the
    // memory the host holds for itself and its aliases, and a host given that memory (rasterizer_render_targets_footprint,
    // g_rasterizer_render_targets_backing_size). At 1152x640 the descriptions, offsets and memory are retail's.
    long result = 0;
    for (long surface_index = 0; surface_index < group->count; surface_index++, g_rasterizer_surface_count++)
    {
        s_surface_description description;
        rasterizer_render_targets_surface_description(&group->description, surface, surface_index, false, &description);
        unsigned int backing_size = 0;
        if (surface_index == 0 && (group->description.flags & 64))
        {
            description.alias_offset = rasterizer_render_targets_footprint(description.alias_surface)
                - rasterizer_render_targets_footprint(surface);
        }
        else if (surface_index == 0 && description.alias_surface == 0 && surface != 0)
        {
            const unsigned int footprint = rasterizer_render_targets_footprint(surface);
            if (footprint > rasterizer_render_targets_texture_size(description.width, description.height, description.format))
            {
                backing_size = footprint;
                char text[112];
                csnzprintf(text, sizeof(text), "port: rasterizer: surface %ld holds its aliases in %u bytes\n", (long)surface,
                    footprint);
                port_write_error(text);
            }
        }
        g_rasterizer_render_targets_backing_size[g_rasterizer_surface_count] = backing_size;
        result |= group->surfaces[surface_index].create(description, g_rasterizer_surface_count);
    }
    return result;
#else
    s_surface_description description = { 0 };
    description.depth_stencil = group->description.depth_stencil;
    description.width = (long)group->description.width;
    description.height = (long)group->description.height;
    description.format = group->description.format;
    description.texture_srgb = group->description.texture_srgb;
    description.exponent_bias = group->description.exponent_bias;
    description.resolve_width = (long)group->description.resolve_width;
    description.resolve_height = (long)group->description.resolve_height;
    description.resolve_format = group->description.resolve_format;
    description.srgb = group->description.srgb;
    description.color_exponent_bias = group->description.color_exponent_bias;
    description.resolve_exponent_bias = group->description.resolve_exponent_bias;
    description.multisample = group->description.multisample;
    description.alias_surface = group->description.alias_surface;
    description.alias_offset = 0;
    description.base = group->description.base;
    description.hierarchical_z = group->description.hierarchical_z;
    long result = 0;
    for (long surface_index = 0; surface_index < group->count; surface_index++, g_rasterizer_surface_count++)
    {
        long index = surface_index;
        long view = 0;
        long tiling_configuration = 0;
        long level = 0;
        long flags = group->description.flags;
        if (flags & 32)
        {
            level = surface_index % 4;
            index = surface_index / 4;
        }
        if (flags & 8)
        {
            view = surface_index % 4;
            index = surface_index / 4;
        }
        if (flags & 2)
        {
            tiling_configuration = index % 6;
            index = index / 6;
        }
        if (flags & 1)
        {
            description.resolve_width = rasterizer_render_targets_scaled_size(group->description.resolve_width, 1152.0f);
            description.resolve_height = rasterizer_render_targets_scaled_size(group->description.resolve_height, 640.0f);
            description.width = rasterizer_render_targets_scaled_size(group->description.width, 1152.0f);
            description.height = rasterizer_render_targets_scaled_size(group->description.height, 640.0f);
        }
        if (surface == 4 || surface == 52)
        {
            switch (level)
            {
            case 1:
            case 3:
                description.resolve_height *= 2;
                description.height *= 2;
                break;
            case 2:
                description.resolve_width *= 2;
                description.resolve_height *= 2;
                description.width *= 2;
                description.height *= 2;
                break;
            }
        }
        flags = group->description.flags;
        if (flags & 32)
        {
            switch (level)
            {
            case 1:
                description.resolve_height = (long)description.resolve_height / 2;
                description.height = description.height / 2;
                break;
            case 2:
                description.resolve_width = (long)description.resolve_width / 2;
                description.resolve_height = (long)description.resolve_height / 2;
                description.width = description.width / 2;
                description.height = description.height / 2;
                break;
            case 3:
                description.resolve_width = (long)(description.resolve_width * 3) / 4;
                description.resolve_height = (long)description.resolve_height / 2;
                description.width = description.width * 3 / 4;
                description.height = description.height / 2;
                break;
            }
        }
        if (flags & 8)
        {
            switch (view)
            {
            case 0:
            case 1:
                description.resolve_format = 0x18280186;
                description.format = 0x18280186;
                description.color_exponent_bias = 0;
                description.resolve_exponent_bias = 0;
                description.exponent_bias = 0;
                description.srgb = 1;
                description.texture_srgb = 1;
                break;
            case 2:
                description.resolve_format = 0x18280186;
                description.format = 0x18280186;
                description.color_exponent_bias = 0;
                description.resolve_exponent_bias = 0;
                description.exponent_bias = 0;
                description.srgb = 0;
                description.texture_srgb = 0;
                break;
            case 3:
                description.resolve_format = 0x18280186;
                description.format = 0x18280186;
                description.color_exponent_bias = 0;
                description.resolve_exponent_bias = 0;
                description.exponent_bias = 0;
                description.srgb = 1;
                description.texture_srgb = 1;
                break;
            }
        }
        if (flags & 2)
        {
            rasterizer_render_targets_create_tiling_surface_desc(&description, tiling_configuration);
            if (!(flags & 4))
            {
                description.width = description.resolve_width;
                description.height = description.resolve_height;
            }
        }
        if (flags & 16)
        {
            switch (description.multisample)
            {
            case 0:
                description.resolve_width = (description.resolve_width + 79) / 80 * 80;
                break;
            case 1:
                description.resolve_width = (description.resolve_width + 79) / 80 * 80;
                break;
            case 2:
                description.resolve_width = (description.resolve_width + 39) / 40 * 40;
                break;
            }
        }
        if (flags & 64)
        {
            s_render_surface_texture_desc texture_desc;
            c_render_surface *alias = (c_render_surface *)g_rasterizer_surface_groups[description.alias_surface].surfaces;
            XGGetTextureDesc(alias->resource, 0, &texture_desc);
            unsigned int original_size = 0;
            XGSetTextureHeader(texture_desc.width, texture_desc.height, 1, 0, texture_desc.format, 0, 0, 0, 0, 0, &original_size, 0);
            unsigned int current_size = 0;
            XGSetTextureHeader(description.width, description.height, 1, 0, description.format, 0, 0, 0, 0, 0, &current_size, 0);
            description.alias_offset = original_size - current_size;
        }
        if (surface_index > 0)
        {
            description.alias_surface = surface;
            description.alias_offset = 0;
        }
        result |= group->surfaces[surface_index].create(description, g_rasterizer_surface_count);
    }
    return result;
#endif
}

extern "C" {
// @address 0x8c4688 g_rasterizer_viewport_offset
real_vector2d g_rasterizer_viewport_offset;
// @address 0x151ef0 D3DDevice_SetViewportF
}
// @address 0x151df0 ?set_viewport@c_rasterizer@@SAXABTrectangle2d@@MM@Z
void c_rasterizer::set_viewport(union rectangle2d const &p0, float p1, float p2)
{
    long vp[10];
    short sx1 = p0.x1;
    long *cursor = &vp[1];
    short sx0 = p0.x0;
    short sy1 = p0.y1;
    short sy0 = p0.y0;
    int x0 = sx0;
    int y0 = sy0;
    int w = sx1 - x0;
    int h = sy1 - y0;
    int n = 6;
    do
    {
        *cursor = 0;
        ++cursor;
    } while (--n > 0);
    float *fvp = (float *)vp;
    fvp[4] = 0.0f;
    fvp[5] = 1.0f;
    fvp[2] = (float)(unsigned int)w;
    fvp[3] = (float)(unsigned int)h;
    fvp[0] = (float)(unsigned int)x0 + g_rasterizer_viewport_offset.i;
    fvp[1] = (float)(unsigned int)y0 + g_rasterizer_viewport_offset.j;
    D3DDevice_SetViewportF(c_rasterizer::get_device(), vp);
}

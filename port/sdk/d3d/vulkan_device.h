#ifndef PORT_D3D_VULKAN_DEVICE_H
#define PORT_D3D_VULKAN_DEVICE_H

/*
 * The PC renderer behind the Xbox D3D device (port/sdk/d3d/null_device.cpp). With HALO3_VULKAN=1 a graphics backend
 * (gpu_backend.h: Vulkan, vulkan_gpu.cpp; HALO3_RENDERER / halo3.ini [display] renderer picks it) comes up with the
 * shader pack (port/shaders/shader_pack.h) and the D3D device forwards its GPU work here: render targets, clears,
 * resolves, draws, presentation. Without it the device stays null. (The port_vk_* names are from when Vulkan was the
 * only backend: nothing here is Vulkan's.)
 *
 * Split of work. null_device.cpp is built like the game, against the XDK headers: it owns the D3DDevice and the
 * resource headers the game's inline code reads and writes, and turns them into the plain descriptions below.
 * vulkan_device.cpp, the Xenos GPU frontend, is host code (halo3_gpu, no XDK headers): it owns every GPU object and
 * the caches, through the backend seam (gpu_backend.h) alone.
 * Everything here is C with 4-byte members (pointers, 32-bit integers, floats) so the game's MS-ABI code and host
 * code agree on it on both targets.
 *
 * What maps to what:
 *   - shaders: a D3DVertexShader / D3DPixelShader is registered with its Xenos microcode (CreateVertexShader /
 *     CreatePixelShader functions, or the tag objects XGRegisterVertexShader / XGRegisterPixelShader see); the
 *     microcode hash finds its SPIR-V, MaterialParameters gathers and sampler map in the pack;
 *   - constants: the gathers read the D3DDevice's own register shadows (m_Constants), where the game's inline
 *     SetVertexShaderConstantF etc. write, through the pointers given once by port_vk_set_shadows;
 *   - render targets and depth/stencil surfaces: one image per D3DSurface (EDRAM aliasing between surfaces
 *     at one EDRAM base is not modelled: each surface is its own image);
 *   - textures: converted from their Xenos layout (port/data/texture_convert.h) on first use and cached by header,
 *     addresses and generation; a texture a Resolve wrote is sampled from the resolved image;
 *   - vertex and index data: copied (and swapped per element while in Xenos byte order) into a per-frame ring.
 *
 * That is the version 2 (DX11) shader pack. With a version 3 pack (HALO3_SHADER_PACK=...halo3_shaders_xenos.pack, told
 * apart by its header) the shaders are the retail microcode translated to SPIR-V (port/shaders/xenos_spirv_abi.h; a
 * version 4 pack holds the same translated to DXIL, for Direct3D 12): the register shadows go to the shaders verbatim,
 * the fetch constants' textures bind by fetch constant, vertex data is read raw (by device address on Vulkan, through
 * buffer views on Direct3D 12), memexport writes the exported memory's device copy, and tessellated water runs on the
 * tessellator (vulkan_device.cpp, "shader pack version 3").
 *
 * HALO3_WINDOW=1 opens a window (HALO3_RES=WxH, default the back buffer size) and presents into it;
 * HALO3_DUMP_FRAMES=<dir> writes every presented frame as <dir>/frame_%04u.ppm.
 * HALO3_PRESENT_RESOLVE=<n> (diagnostics) presents the frame's n-th Resolve destination (from 0) instead of the front
 * buffer, to look at the intermediate targets.
 */

#include "d3d_port_texture.h"

typedef __UINT64_TYPE__ uint64_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Returns 1 when the renderer is up, 0 when it is not wanted (HALO3_VULKAN unset) or could not start (the reason is
 * logged through port_write_error). Safe to call more than once: later calls report the first result. */
int PORT_MS_ABI port_vulkan_device_startup(unsigned int back_buffer_width, unsigned int back_buffer_height);
/* 1 once port_vulkan_device_startup succeeded. */
int PORT_MS_ABI port_vulkan_device_active(void);
extern int g_port_vulkan_device_started;
static inline int port_vulkan_device_active_fast(void) { return __atomic_load_n(&g_port_vulkan_device_started, __ATOMIC_ACQUIRE) > 0; }
/* Waits for the GPU and releases every GPU object (tests; the game never shuts the device down). */
void PORT_MS_ABI port_vulkan_device_shutdown(void);

/* ---- register shadows (the D3DDevice's m_Constants) ---- */

struct s_port_vk_shadows {
    const float *vertex_float;      /* 256 float4 (VertexShaderF) */
    const float *pixel_float;       /* 256 float4 (PixelShaderF) */
    const uint32_t *vertex_bool;    /* 128 bits: register r is bit r % 32 of dword r / 32 */
    const uint32_t *pixel_bool;
    const uint32_t *vertex_int;     /* 16 GPU integer constants (count, start, step bytes) */
    const uint32_t *pixel_int;
};
void PORT_MS_ABI port_vk_set_shadows(const struct s_port_vk_shadows *shadows);

/* ---- shaders ---- */

#define PORT_VK_STAGE_VERTEX 0u
#define PORT_VK_STAGE_PIXEL 1u

/* Associates a shader object with its physical microcode (see shader_pack_hash_microcode for the byte order). */
void PORT_MS_ABI port_vk_shader_register(const void *object, uint32_t stage, const void *microcode, uint32_t size_bytes,
                             uint32_t words_in_host_order);
void PORT_MS_ABI port_vk_shader_forget(const void *object);
/* The pack shader of a registered object (SHADER_PACK_NOT_FOUND if unknown or the pack has no counterpart). */
uint32_t PORT_MS_ABI port_vk_shader_pack_index(const void *object);

/* ---- surfaces ---- */

struct s_port_vk_surface {
    const void *key;                /* the D3DSurface */
    uint32_t width;
    uint32_t height;
    uint32_t d3d_format;            /* Xbox D3DFORMAT */
    uint32_t depth;                 /* 1 for a depth/stencil surface */
    uint32_t edram_base;            /* where in EDRAM it lives (ColorBase / DepthBase), 0xffffffff for a texture level */
    int color_exp_bias;             /* ColorExpBias (RB_COLOR_INFO): color writes are scaled by 2^bias; 0 for depth */
    uint32_t msaa_samples;          /* MsaaSamples (RB_SURFACE_INFO, D3DMULTISAMPLE_TYPE): 0 one sample per pixel,
                                       1 two, 2 four; 0 for a texture level */
    uint32_t reserved;              /* keeps sizeof a multiple of 8 across -fmax-type-align=4 and LP64 */
};

unsigned int PORT_MS_ABI port_vk_render_resolution_scale(void);
void PORT_MS_ABI port_vk_set_back_buffer(const struct s_port_vk_surface *surface);
/* Resizes the back buffer and tears down resolution-dependent GPU targets and resolved textures (between frames).
 * res_scale == 0 reads the configured [display] res_scale / HALO3_RES_SCALE (for native resolution). */
void PORT_MS_ABI port_vk_resize_back_buffer(unsigned int width, unsigned int height, unsigned int res_scale);
/* MS-ABI wrapper around port_config_revision() so game code can poll live setting changes safely on 64-bit Linux. */
uint32_t PORT_MS_ABI port_vk_config_revision(void);
/* index 0..3; NULL unbinds. */
void PORT_MS_ABI port_vk_set_render_target(uint32_t index, const struct s_port_vk_surface *surface);
void PORT_MS_ABI port_vk_set_depth_stencil(const struct s_port_vk_surface *surface);

/* ---- textures and samplers ---- */

struct s_port_vk_texture {
    const void *key;                /* the D3DBaseTexture header */
    uint32_t generation;            /* changes when the game rewrites the data (LockRect) */
    uint32_t reserved;              /* aligns source (pointers inside) to 8 bytes across -fmax-type-align=4 and LP64 */
    struct s_port_d3d_texture_source source;
};

/* Sampler state in D3D's (Xbox) values. */
struct s_port_vk_sampler_state {
    uint32_t mag_filter;            /* D3DTEXF_POINT 0, LINEAR 1, ANISOTROPIC 4 */
    uint32_t min_filter;
    uint32_t mip_filter;            /* POINT 0, LINEAR 1, NONE 2 */
    uint32_t address_u;             /* D3DTADDRESS_WRAP 0, MIRROR 1, CLAMP 2, MIRRORONCE 3, BORDER 6 */
    uint32_t address_v;
    uint32_t address_w;
    uint32_t max_anisotropy;        /* 1..16 */
    uint32_t border_color;          /* GPUBORDERCOLOR: 0 transparent black, 1 opaque black, 2 opaque white */
    float lod_bias;
    uint32_t min_mip_level;         /* most detailed level the sampler may use */
    uint32_t max_mip_level;         /* least detailed */
    /* The filters between the layers of a stacked texture (the fetch constant's VolMagFilter / VolMinFilter, POINT 0,
     * LINEAR 1): the translated shaders' own (a host sampler has none for array layers), given them in the fetch
     * constant they read (xe_fetch_constants). */
    uint32_t mag_filter_z;
    uint32_t min_filter_z;
};

struct s_port_vk_sampler {
    struct s_port_vk_texture texture;
    struct s_port_vk_sampler_state state;
    uint32_t bound;                 /* 1 when a texture is set */
};

/* D3D sampler slots, the 26 texture fetch constants: 0..15 pixel shader samplers (16..19 unused), 20..25 vertex shader
 * samplers. A vertex shader's sampler register r is D3D sampler 20 + r: render_method_submit adds 20 for a vertex-stage
 * texture (tu/graphicslibxbox/render_method_submit.cpp, `entry->flags & 1`), and the game binds the retail vertex
 * shaders' samplers so (decorator wind_texture, register 0: sampler 20; particle_update sampler_weather_occlusion and
 * the water vs_tex_ripple_buffer_slope_height, register 1: sampler 21). */
#define PORT_VK_SAMPLER_COUNT 26
#define PORT_VK_VERTEX_SAMPLER_BASE 20

/* The game rewrote a texture's or buffer's data: the device's copies made from it are converted again at their next
 * use. Not for a D3DLOCK_READONLY buffer lock, which leaves the bytes as they were (port_vk_buffer_lock). */
void PORT_MS_ABI port_vk_resource_touched(const void *key);
/* The game placed a vertex or index buffer over [base, base + size) (XGSetVertexBufferHeader / XGSetIndexBufferHeader
 * with an address, XGOffsetResourceAddress). A header inside the memory of an earlier memory export (a version 3 pack
 * keeps a device copy per exported range) views what the GPU wrote there (the water's per-draw header over its
 * tessellation factors), and the copy stays. Any other placement over exported memory is a new buffer holding what
 * the game loaded or wrote there: the copy is dropped, and the next export or fetch of the memory reads the guest bytes
 * again. */
void PORT_MS_ABI port_vk_memory_placed(const void *base, uint32_t size);
/* D3DVertexBuffer_Lock / D3DIndexBuffer_Lock of [base, base + size): the CPU is about to read the bytes and, unless
 * read_only (D3DLOCK_READONLY), to write them. What memory exports wrote there is read back into the memory first (the
 * frame finished and waited for, as the Xbox Lock waits for the GPU), and a writing lock leaves the memory to the CPU:
 * draws read what it writes, and the next export starts from it (the water writes its interaction events into the
 * buffer its previous ripple export wrote). Nothing happens for memory no export wrote, nor on a thread other than the
 * one recording the frames (the update thread locks the particle queue buffers, which no export writes). Returns
 * nonzero when an export was read back into the memory: its bytes changed even under a read-only lock. */
int PORT_MS_ABI port_vk_buffer_lock(const void *base, uint32_t size, int read_only);
/* D3D*Texture_LockRect / LockBox without D3DLOCK_READONLY: the CPU is about to write the texture's texels. It writes
 * them in host order, so the format's GPUENDIAN swap (which undoes the Xbox CPU's big-endian stores) must not be
 * applied to that memory until a Resolve writes it again. Also counts as port_vk_resource_touched. */
void PORT_MS_ABI port_vk_texture_written(const void *texture_header);
/* The CPU wrote texels into [base, base + size) without a lock: the Bink library decoding a movie frame into the planes
 * the game registered (port/sdk/bink/bink_read.cpp, BinkDoFrame), which the game shows as linear textures it never
 * locks. Every texture whose level 0 or mips start in the range is converted again at its next bind, its texels
 * read in host order, as after port_vk_texture_written. */
void PORT_MS_ABI port_vk_texels_written(const void *base, uint32_t size);
/* The game's resource pager rewrote [base, base + size): a page read from the map and made ready
 * (tag_resource_page_table_set_page_ready) or moved by compaction (lruv_move_tag_resource_page). Its texels are in the
 * Xenos byte order again, whatever the CPU or a Resolve left there before (the pager reuses memory the Bink planes,
 * locked textures and earlier pages held), so what port_vk_texture_written / port_vk_texels_written and Resolve
 * recorded about addresses in the range no longer holds: those records are dropped, as are the device copies of memory
 * exports over it. Any thread (the pager's). */
void PORT_MS_ABI port_vk_memory_streamed(const void *base, uint32_t size);

/* ---- draws ---- */

/* The render state a pipeline depends on, in D3D's (Xbox) values (D3DRS_*). */
struct s_port_vk_render_state {
    uint32_t z_enable, z_write_enable, z_func;
    /* RB_BLENDCONTROL0..3 (GPU_BLENDCONTROL, the D3DBLENDSTATE layout): color source / op / destination in bits
     * 0-4 / 5-7 / 8-12, alpha in 16-20 / 21-23 / 24-28, D3DBLEND and D3DBLENDOP values. One per render target, as
     * the Xenos blends; 0x00010001 (ONE, ADD, ZERO for both) is no blending. */
    uint32_t blend_control[4];
    uint32_t cull_mode, fill_mode;
    uint32_t color_write_enable[4];
    uint32_t scissor_test_enable;
    uint32_t stencil_enable, two_sided_stencil_mode;
    uint32_t stencil_fail, stencil_z_fail, stencil_pass, stencil_func, stencil_ref, stencil_mask, stencil_write_mask;
    uint32_t ccw_stencil_fail, ccw_stencil_z_fail, ccw_stencil_pass, ccw_stencil_func, ccw_stencil_ref,
             ccw_stencil_mask, ccw_stencil_write_mask;
    uint32_t depth_bias;            /* float bits (D3DRS_DEPTHBIAS) */
    uint32_t slope_scale_depth_bias;/* float bits */
    uint32_t alpha_test_enable, alpha_func, alpha_ref;
    uint32_t primitive_reset_enable;
    uint32_t blend_factor;          /* D3DRS_BLENDFACTOR, a D3DCOLOR (RB_BLEND_RED..ALPHA): the D3DBLEND_BLENDFACTOR /
                                     * CONSTANTALPHA constant */
};

struct s_port_vk_stream {
    const void *key;                /* the D3DVertexBuffer, or NULL for user-pointer data */
    const void *data;               /* the buffer's data (offset 0) */
    uint32_t size;                  /* bytes of data */
    uint32_t offset;                /* SetStreamSource offset */
    uint32_t stride;
    uint32_t big_endian;            /* 1: Xenos byte order (d3d_port_buffer.h), swapped per element of the declaration */
};

struct s_port_vk_indices {
    const void *key;                /* the D3DIndexBuffer, or NULL for user-pointer data */
    const void *data;
    uint32_t size;
    uint32_t index_size;            /* 2 or 4 */
    uint32_t big_endian;
    uint32_t reserved;              /* keeps sizeof a multiple of 8 across -fmax-type-align=4 and LP64 */
};

#define PORT_VK_MAX_STREAMS 16

struct s_port_vk_draw {
    const void *vertex_shader;      /* registered shader objects */
    const void *pixel_shader;
    const void *declaration;        /* D3DVERTEXELEMENT9[] ending at stream 0xff (or NULL) */
    struct s_port_vk_render_state state;
    float viewport[6];              /* x, y, width, height, min z, max z; width 0: the whole render target */
    int scissor[4];             /* left, top, right, bottom */
    uint32_t stream_mask;
    struct s_port_vk_stream streams[PORT_VK_MAX_STREAMS];
    struct s_port_vk_indices indices;
    struct s_port_vk_sampler samplers[PORT_VK_SAMPLER_COUNT];
    /* User clip planes (D3DDevice_SetClipPlane, D3DRS_CLIPPLANEENABLE): clip-space planes, a point is kept where
     * dot(position, plane) >= 0. The Xbox clips in fixed function; Bungie's DX11 shaders write SV_ClipDistance from
     * PC-only cbuffer members instead (clip_plane.fx v_clip_plane, dynamic_light_clip.fx
     * v_dynamic_light_clip_plane[6]), which the renderer fills from these. */
    uint32_t clip_plane_enable;     /* bit i: plane i */
    float clip_planes[6][4];
    /* The screen origin of the predicated tile being drawn (0, 0 outside tiling): the negated window offset. On the Xbox
     * vPos is tile-relative and Bungie's shaders add p_tiling_vpos_offset; the DX11 build (-Dpc) never tiles, so the
     * renderer adds this to SV_Position instead (VULKAN_SPEC_FRAGCOORD_OFFSET). */
    int fragment_offset[2];
    /* PA_SU_VTX_CNTL.PIX_CENTER: 0.5 with D3DRS_HALFPIXELOFFSET FALSE (D3D9 integer pixel centres, the XDK default),
     * 0 with TRUE. Added to the viewport origin, as xenia's GetHostViewportInfo does. */
    float pixel_center_offset;
    /* PA_SU_POINT_MINMAX as sprite diameters in guest pixels: the range the Xenos clamps a vertex shader's point size
     * (oPts.x, a diameter) to. The register holds radii in 12.4 fixed point; the Xbox D3D writes D3DRS_POINTSIZE_MIN /
     * _MAX x 16 into them (retail defaults 1 and 64: diameters 2 and 128). */
    float point_size_min, point_size_max;
    /* Monotonic generation counters (0 = unversioned / always dirty): */
    uint32_t state_gen;             /* pipeline render state and shader identity */
    uint32_t stream_gen;            /* vertex streams or stream_mask */
    uint32_t vs_float_gen;          /* VertexShaderF constants */
    uint32_t ps_float_gen;          /* PixelShaderF constants */
    uint32_t bool_loop_gen;         /* VertexShaderB / PixelShaderB / VertexShaderI / PixelShaderI constants */
    uint32_t sampler_gen[PORT_VK_SAMPLER_COUNT]; /* per-slot sampler/texture generation */
    uint32_t samplers_any_gen;      /* incremented whenever any sampler slot changes */
    uint32_t vp_gen;                /* viewport / scissor / window generation (separate from pipeline state_gen) */
    uint32_t reserved_pad;          /* keeps pointer fields 8-byte aligned across -fmax-type-align=4 and LP64 */
    const uint32_t *vs_chunk_gen;   /* 64-entry per-4-register dirty generation (NULL: unversioned) */
    const uint32_t *ps_chunk_gen;   /* 64-entry per-4-register dirty generation (NULL: unversioned) */
};

/* Twelve scalar groups and two 26-slot fields. */
#define PORT_VK_DIRTY_VS          (1ull << 0)
#define PORT_VK_DIRTY_PS          (1ull << 1)
#define PORT_VK_DIRTY_VDECL       (1ull << 2)
#define PORT_VK_DIRTY_BLEND       (1ull << 3)
#define PORT_VK_DIRTY_DS          (1ull << 4)
#define PORT_VK_DIRTY_RAST        (1ull << 5)
#define PORT_VK_DIRTY_VP          (1ull << 6)
#define PORT_VK_DIRTY_RT          (1ull << 7)
#define PORT_VK_DIRTY_STREAMS     (1ull << 8)
#define PORT_VK_DIRTY_IB          (1ull << 9)
#define PORT_VK_DIRTY_VS_CONST    (1ull << 10)
#define PORT_VK_DIRTY_PS_CONST    (1ull << 11)
#define PORT_VK_DIRTY_TEXTURE(i)  (1ull << (12u + (i)))
#define PORT_VK_DIRTY_SAMPLER(i)  (1ull << (38u + (i)))
#define PORT_VK_DIRTY_TEXTURES    ((PORT_VK_DIRTY_SAMPLER(0) - 1u) & ~(PORT_VK_DIRTY_TEXTURE(0) - 1u))
#define PORT_VK_DIRTY_SAMPLERS    (~(PORT_VK_DIRTY_SAMPLER(0) - 1u))
#define PORT_VK_DIRTY_PIPELINE    (PORT_VK_DIRTY_VS | PORT_VK_DIRTY_PS | PORT_VK_DIRTY_BLEND | PORT_VK_DIRTY_DS | \
                                   PORT_VK_DIRTY_RAST | PORT_VK_DIRTY_RT)

/* Borrowed for one call. Stream 0 and the stream mask replace the packet's geometry.
 * A NULL index or declaration pointer inherits that part of the packet. */
struct s_port_vk_geometry {
    struct s_port_vk_stream stream0;
    const struct s_port_vk_indices *indices;
    const void *declaration;
    uint32_t stream_mask;
    uint32_t reserved;
};

/* HAL ownership transfers and last-resource releases bound the lifetime of queued guest memory. */
void PORT_MS_ABI port_vk_record_acquire(void);
void PORT_MS_ABI port_vk_record_drain(void);
void PORT_MS_ABI port_vk_record_fence(uint32_t fence);
int PORT_MS_ABI port_vk_record_fence_pending(uint32_t fence, int wait);
void PORT_MS_ABI port_vk_invalidate_draw_caches(void);
/* The producer consumes this epoch on the render thread. Writers publish after changing a resource header. */
extern uint32_t g_port_vk_resource_touch_epoch;
void PORT_MS_ABI port_vk_texture_header_changed(void);

/* D3DPRIMITIVETYPE values (Xbox). */
#define PORT_VK_PT_POINTLIST 1u
#define PORT_VK_PT_LINELIST 2u
#define PORT_VK_PT_LINESTRIP 3u
#define PORT_VK_PT_TRIANGLELIST 4u
#define PORT_VK_PT_TRIANGLEFAN 5u
#define PORT_VK_PT_TRIANGLESTRIP 6u
#define PORT_VK_PT_RECTLIST 8u
#define PORT_VK_PT_QUADLIST 13u

/* Draws `count` vertices from `start` (indexed: `count` indices from `start`, base_vertex added to each). */
void PORT_MS_ABI port_vk_draw(const struct s_port_vk_draw *draw, uint64_t dirty_mask, uint32_t primitive_type,
                              uint32_t start, uint32_t count, int base_vertex, uint32_t indexed,
                              const struct s_port_vk_geometry *geometry);
/* D3DDevice_DrawIndexedTessellatedVertices: `index_count` patches from `start_index` of the draw's index buffer
 * (D3DTM_ADAPTIVE: it holds the per-edge factors, three a triangle patch), numbered from base_patch; mode
 * D3DRS_TESSELLATIONMODE, levels D3DRS_MIN/MAXTESSELLATIONLEVEL. Only adaptive triangle patches are drawn. Version 2
 * pack: Halo 3's water patches (a vertex shader of vertex type water) expanded on the CPU into the vertices Bungie's
 * DX11 water shaders read (vulkan_device.cpp, "Halo 3 water"). Version 3 pack: the tessellator, the factors as its
 * index buffer, the retail vertex shader as the evaluation stage. */
void PORT_MS_ABI port_vk_draw_tessellated(const struct s_port_vk_draw *draw, uint64_t dirty_mask,
                                          uint32_t primitive_type, int base_patch, uint32_t start_index,
                                          uint32_t index_count, uint32_t mode, float minimum_level, float maximum_level);

/* ---- clears, resolves, presentation ---- */

#define PORT_VK_CLEAR_TARGETS 0x0fu     /* D3DCLEAR_TARGET0..3 */
#define PORT_VK_CLEAR_ZBUFFER 0x10u
#define PORT_VK_CLEAR_STENCIL 0x20u

/* Clears the bound targets: rects are (x0, y0, x1, y1) each; count 0 clears everything. */
void PORT_MS_ABI port_vk_clear(uint32_t flags, const float color[4], float z, uint32_t stencil, const int *rects,
                   uint32_t rect_count);

/* An EDRAM clear (D3DDevice_EndTiling's per-tile clear): clears the rectangle `rect` (x0, y0, x1, y1, in EDRAM surface
 * coordinates from each base) of the EDRAM under the bound targets, so the surface placed at one of those bases next
 * (the tile-size G-buffer and depth surfaces) starts from the cleared value, whichever surface the game binds there.
 * Applied when such a surface is next drawn into or resolved. */
void PORT_MS_ABI port_vk_clear_edram(uint32_t flags, const float color[4], float z, uint32_t stencil, const int rect[4]);

/* D3DQUERYTYPE_OCCLUSION: Issue(D3DISSUE_BEGIN) .. Issue(D3DISSUE_END) brackets the draws whose passing samples the
 * query counts (accumulate != 0: a later tile of a tiled query, added to the earlier tiles' counts). result: 1 and the
 * count (D3DOCCLUSIONSTATS::ZPass) once the frame that drew them has finished on the GPU, 0 while pending. */
void PORT_MS_ABI port_vk_query_begin(const void *query, uint32_t accumulate);
void PORT_MS_ABI port_vk_query_end(const void *query);
int PORT_MS_ABI port_vk_query_result(const void *query, uint32_t *out_samples);

/* D3DDevice_Resolve: copies a rectangle (NULL: the whole surface) of render target `source_index` into a texture
 * level / slice at dest_x, dest_y; flags: D3DRESOLVE_* (0x100 clear the render target after, 0x200 clear
 * depth/stencil after, 0x4 resolve depth). */
void PORT_MS_ABI port_vk_resolve(uint32_t flags, uint32_t source_index, const int *source_rect,
                     const struct s_port_vk_texture *destination, uint32_t dest_x, uint32_t dest_y,
                     uint32_t dest_level, uint32_t dest_slice, const float clear_color[4], float clear_z,
                     uint32_t clear_stencil);

/* The display controller's gamma LUT (D3DDevice_SetGammaRamp / D3DDevice_SetPWLGamma): the console applies it as it
 * scans the front buffer out, so every presented frame goes through it (shown and dumped with it applied). words:
 * a D3DGAMMARAMP (red[256], green[256], blue[256] 16-bit words) for k_port_vk_display_gamma_ramp, a D3DPWLGAMMA
 * (red, green, blue: 128 { Base, Delta } word pairs) for k_port_vk_display_gamma_pwl; none (identity) otherwise. */
enum e_port_vk_display_gamma {
    k_port_vk_display_gamma_none = 0,
    k_port_vk_display_gamma_ramp = 1,
    k_port_vk_display_gamma_pwl = 2,
};
void PORT_MS_ABI port_vk_set_display_gamma(uint32_t mode, const uint16_t *words);
/* The display VdGetCurrentDisplayGamma reports (the XDK adapts every gamma ramp to it): type 1 sRGB, 2 BT.709 (an
 * HDTV; the default, as Halo3Rex and xenia report), 3 the power curve `power`, other values linear. HALO3_DISPLAY_GAMMA=
 * <type>[,<power>] overrides it. */
void PORT_MS_ABI port_vk_display_gamma_query(uint32_t *type, float *power);

/* D3DDevice_Swap / Present: ends the frame, shows the front buffer (window) and dumps it (HALO3_DUMP_FRAMES), both
 * through the display gamma. front_buffer: the texture D3DDevice_Swap shows (the game resolves its frame into it)
 * when a Resolve wrote it; NULL shows the back buffer (D3DDevice_Present). */
void PORT_MS_ABI port_vk_presented_extent(int *width, int *height); // leaves the supplied size before the first present
void PORT_MS_ABI port_vk_present(const void *front_buffer);

/* Frames presented so far. */
uint32_t PORT_MS_ABI port_vk_frame_count(void);

/* The frame rate (HALO3_FPS): 30 (the default) shows one frame per 30 Hz game tick, as the console; 31..1000 shows
 * that many frames a second and 0 as many as the host can (the vertical blanks the frames wait for run at 1 kHz). Above
 * 30 the game still simulates 30 ticks a second and draws the frames between two ticks interpolated (the game's
 * render-side interpolation); the swapchain presents without vsync (MAILBOX, else IMMEDIATE) unless HALO3_VSYNC=1. Any
 * other value logs a warning and keeps 30. Unless the launch set HALO3_FPS, it follows halo3.ini's fps_cap as the
 * Guide's Settings page changes it (port/sdk/runtime/port_config.h), and so do the rates below. Works with or without
 * a running renderer. */
uint32_t PORT_MS_ABI port_vk_frame_rate(void);

/* The rate of the D3D device's vertical blanks (D3DDevice_SetVerticalBlankCallback), which pace the game's frames:
 * 60 Hz (the console's display) at HALO3_FPS=30, else HALO3_FPS (1000 for 0). The video mode keeps reporting 60 Hz: the
 * game derives its 30 Hz tick rate from it (game_options_initialize_default). */
uint32_t PORT_MS_ABI port_vk_vertical_blank_rate(void);

/* Whether the game interpolates the frames between two 30 Hz ticks (its render-side interpolation): above HALO3_FPS=30,
 * unless HALO3_INTERPOLATE=0 (the frames between two ticks are then drawn as
 * published: every frame of a tick shows that tick's poses, as a 60 Hz console frame repeats its tick). */
uint32_t PORT_MS_ABI port_vk_frame_interpolation(void);

/* The least number of vertical blanks between two frames the D3D device lets the game show
 * (D3DDevice_SynchronizeToPresentationInterval): HALO3_FRAME_BLANKS 1..4, default 2 at HALO3_FPS=30 (the console's
 * cadence of one 30 Hz game tick a frame), else 1 (a frame every blank of port_vk_vertical_blank_rate). Works with or
 * without a running renderer. */
uint32_t PORT_MS_ABI port_vk_frame_blanks(void);

/* The last presented frame (after the display gamma) as 8-bit B, G, R, A rows (width * 4 bytes each), for tests;
 * NULL before the first present (frames are read back only with HALO3_DUMP_FRAMES). */
const void * PORT_MS_ABI port_vk_last_frame(uint32_t *out_width, uint32_t *out_height);

/* D3D*Texture_LockRect: the CPU is about to read level / layer of a texture's memory. On the console the GPU writes that
 * memory itself; here what a Resolve wrote is an image and what a memory export wrote is the export's device copy, so:
 *   - resolved memory: the first lock reads the resolve back at once (a sync point) and marks the memory, every later
 *     resolve into it is also copied back, and each later lock writes the latest finished copy into the memory (tiled
 *     as the header says);
 *   - exported memory (a texture export, port_vk_begin_texture_export, or any export the level's bytes overlap): the
 *     frame is finished and waited for, and the copy's bytes over the level (all of a texture export's) are written
 *     into the memory with the GPUENDIAN swap undone (the host order the CPU reads in); the CPU then holds that memory
 *     until the GPU exports into it again.
 * Nothing happens for memory neither wrote. */
void PORT_MS_ABI port_vk_texture_lock(const void *texture_header, uint32_t level, uint32_t layer);

/* ---- explicit compute shaders (the Xbox's memexport updates) ---- */

/* A device-local storage buffer kept across frames (particle, contrail, beam, light volume and ripple state). data
 * NULL: zero-filled. NULL when the renderer is not up. */
struct s_port_vk_storage;
struct s_port_vk_storage * PORT_MS_ABI port_vulkan_storage_create(uint32_t size, const void *data);
void PORT_MS_ABI port_vulkan_storage_destroy(struct s_port_vk_storage *storage);
/* Reads bytes back: submits the frame being recorded and waits for the GPU first (a sync point; tests, debugging).
 * Returns 1 on success. */
int PORT_MS_ABI port_vulkan_storage_read(struct s_port_vk_storage *storage, uint32_t offset, uint32_t size, void *out);

/* A storage buffer for one binding of the compute shader: binding 300 + u# (RWStructuredBuffer) or 100 + t#
 * (StructuredBuffer); range 0 means to the end of the buffer. */
struct s_port_vk_compute_resource {
    struct s_port_vk_storage *storage;
    uint32_t binding;
    uint32_t offset;
    uint32_t range;
    uint32_t reserved;              /* keeps sizeof a multiple of 8 across -fmax-type-align=4 and LP64 */
};

/* Bytes written into a cbuffer after the register gathers: members the Xbox registers do not carry (the PC-only
 * index ranges and counts of each update). name_hash: shader_pack_hash_name of the cbuffer's name. */
struct s_port_vk_compute_constants {
    uint32_t name_hash;
    uint32_t offset;
    const void *data;
    uint32_t size;
    uint32_t reserved;              /* keeps sizeof a multiple of 8 across -fmax-type-align=4 and LP64 */
};

struct s_port_vk_compute_dispatch {
    const struct s_port_vk_compute_resource *resources;
    const struct s_port_vk_compute_constants *constants;
    uint32_t explicit_index;        /* rasterizer_globals explicit shader slot of the compute shader */
    uint32_t groups_x, groups_y, groups_z;
    uint32_t resource_count;
    uint32_t constant_count;
};

/* Records the dispatch into the current frame (outside rendering): the pack's compute pipeline for the slot, its
 * cbuffers gathered from the vertex shader register shadows plus `constants`, its storage buffers from `resources`,
 * full memory barriers before and after. Returns 1 when recorded, 0 when skipped (reported once). */
int PORT_MS_ABI port_vulkan_dispatch_explicit_compute(const struct s_port_vk_compute_dispatch *dispatch);

/* D3DDevice_BeginExport / EndExport: `base` (`size` bytes, a vertex buffer's data) is the memory the draws in between
 * export into. Their memexport vertex shaders run as the pack's compute shaders (the microcode table maps each to its
 * explicit slot's DX11 update) writing a storage buffer kept for that memory, which later draws reading the memory as
 * a vertex stream get as their StructuredBuffer t16. xbox_stride: the stride of the stream bound to the same buffer
 * when the export begins (the Xbox record size), 0 if none. Only export slot 0 is modelled. */
void PORT_MS_ABI port_vk_begin_export(uint32_t index, const void *base, uint32_t size, uint32_t xbox_stride);
/* D3DDevice_BeginExport of a texture (the tiled screenshot: rasterizer_export_screenshot_tile exports each tile's pixels
 * from its pixel shader into the screenshot buffer, which a LIN_A8R8G8B8 header describes): port_vk_begin_export over
 * the texture's base level memory. The CPU sees that memory in host order (the texture format's GPUENDIAN swap undone),
 * the device copy in the order the Xenos writes it, so the copy is made from the memory with the swap applied, a
 * texture fetch of the memory (the export shader adds the pixel already there) samples the copy, and a lock reads the
 * copy back with the swap undone (port_vk_texture_lock). Ended by port_vk_end_export. */
void PORT_MS_ABI port_vk_begin_texture_export(uint32_t index, const void *texture_header);
void PORT_MS_ABI port_vk_end_export(uint32_t index);
/* The records draws exported into `base` so far (tests, debugging): `count` records from `record`, `*out_stride` bytes
 * each: the PC record layout of the compute shaders with a version 2 pack; with a version 3 pack the guest bytes as
 * the Xenos wrote them (formats and byte order of the game's stream constants), in 16-byte units. Submits and waits
 * like port_vulkan_storage_read. Returns 1 on success, 0 when the memory has no export storage or the range is outside
 * it. */
int PORT_MS_ABI port_vk_export_read(const void *base, uint32_t record, uint32_t count, void *out, uint32_t *out_stride);

#ifdef __cplusplus
}
#endif

#endif /* PORT_D3D_VULKAN_DEVICE_H */

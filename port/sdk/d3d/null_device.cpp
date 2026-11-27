// port/sdk/d3d: the Direct3D devices (plan stage 3: a headless game with null graphics; with HALO3_VULKAN=1 the GPU
// work goes to the Vulkan renderer, vulkan_device.h). Built like the game, against the XDK's d3d9.h, because the
// game's inline D3D code (d3d9.h D3DINLINE functions compiled into the game's units) reads and writes the device
// structure and resource headers directly.
//
// Two devices, as on the console (rasterizer_xenon_main.cpp c_rasterizer::initialize_device):
//   - the D3DDEVTYPE_HAL device: its state reaches the renderer; draws, clears and resolves render;
//   - the D3DDEVTYPE_COMMAND_BUFFER device (g_command_buffer_device): it only records. Between
//     BeginCommandBuffer and EndCommandBuffer everything set on it and every draw becomes a command stream in the
//     D3DCommandBuffer; D3DCommandBuffer_GetClone copies that stream into the game's own header / physical memory
//     (render_method_submit.cpp's cached mesh draws) and D3DDevice_RunCommandBuffer replays it on the HAL device.
// Each device has its own D3DDevice (m_Constants, m_Pending, the register shadows) and its own D3D state (render and
// sampler states, shaders, streams, textures, surfaces); recording never touches the HAL device or the renderer.
//
// Command streams (s_recording): dword ops, `opcode | dwords << 8` then the arguments. State set through
// out-of-line setters is recorded when it is set. State the game writes inline into the register shadows
// (SetVertexShaderConstantF / SetPixelShaderConstantF / ..B / ..I, sampler states, SetTexture,
// m_Constants.TextureFetch[] fields) is recorded the way Xbox D3D builds its command buffers: at each draw (and
// before a nested RunCommandBuffer, and at the end of the stream) every register group whose m_Pending bit is set
// is written from the device's shadow, and the bits are cleared. Only pending registers: the game clears pending
// bits on purpose for registers it hands to the GPU (render_player_view.cpp: the skinning constants c16..c227 while
// they are GPU-owned), so a value comparison of the shadows would send registers the console does not send.
//
// Predicated tiling (BeginTiling .. EndTiling): the HAL device records the bracket like a command buffer and
// replays it once per tile at EndTiling, as the console does: the tile's EDRAM window (window offset -tile origin,
// window scissor the tile's size; viewport, scissor, clear and resolve rectangles move with it, so the tile's pixels
// land at the render target's origin as in EDRAM, VPOS is tile relative and the shaders' p_tiling_vpos_offset (c108)
// makes it a screen position again), the per-tile clear, and SetPredication: a command recorded under a
// predication mask runs only for the tiles the mask selects (D3DPRED_TILE(t) = 3 << 2t). Tiled surfaces are tile
// sized (UI shell: 4 tiles of 1152x160). The Vulkan UI shell can promote a verified stream to full-frame backing. The window applies
// only while the bracket's own surfaces are bound (window_applies): a shadow map rendered inside the bracket is
// rendered, cleared and resolved whole in every tile, as Xbox D3D does.
//
// Resources (textures, surfaces, vertex/index buffers, shaders, vertex declarations, queries, command buffers) are
// backed by real host allocations with populated Xenon headers (xgraphics.cpp) so the rasterizer's initialization,
// surface-alias calculations, and CPU-side buffer locks succeed without a GPU.
#include <xtl.h>
#include <xgraphics.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vulkan_device.h"
#include "d3d_port_buffer.h"
#include "d3d9/device.h"
#include "render/render_mesh.h"
#include "render/render_objects_submission_globals.h"
#include "rasterizer/rasterizer_device_states.h"
#include "multithreading/threads_externs.h"

extern "C" int PORT_MS_ABI port_vk_buffer_was_mapped(const void *, const void *);
extern "C" void PORT_MS_ABI port_vk_buffer_written(const void *, const void *);
extern "C" void PORT_MS_ABI port_vk_record_resource_idle(const void *, const void *, uint32_t);
extern "C" void PORT_MS_ABI port_vk_texture_lock_mode(const void *, uint32_t, uint32_t, int);
extern "C" void PORT_MS_ABI port_vk_record_release(void);
extern "C" int PORT_MS_ABI port_vk_query_result_flush(const void *, uint32_t *);
extern "C" void *XPhysicalAlloc(SIZE_T dwSize, ULONG_PTR ulPhysicalAddress, ULONG_PTR ulAlignment, DWORD flProtect);
extern "C" void XPhysicalFree(void *lpAddress);
extern "C" void port_write_error(const char *text);
// port/sdk/kernel time_*.c: sleeps until QueryPerformanceCounter reads at least `counter`, as exactly as the host's
// timers allow (Win32 Sleep has millisecond steps, 15.6 ms ones at Windows' default timer resolution).
extern "C" void port_sleep_until_counter(long long counter);
extern "C" int PORT_MS_ABI port_config_resolution(unsigned int *width, unsigned int *height);
#ifdef HALO3_64BIT
extern "C" int PORT_MS_ABI port_vk_shell_available();
extern "C" void PORT_MS_ABI port_vk_shell_trace(int, unsigned, unsigned, unsigned, const void *);
extern "C" void PORT_MS_ABI port_vk_shell_analyze_shader(const void *, const uint32_t *, uint32_t, int, int);
extern "C" int PORT_MS_ABI port_vk_shell_begin(const s_port_vk_surface *, uint32_t, uint32_t);
extern "C" void PORT_MS_ABI port_vk_shell_apply_begin_clear(const s_port_vk_surface *, uint32_t);
extern "C" void PORT_MS_ABI port_vk_shell_end(const s_port_vk_surface *, uint32_t, uint32_t, const void *);
extern "C" int PORT_MS_ABI port_vk_shell_draw(const void *, const void *, uint32_t, uint32_t, uint32_t,
                                               int, int, const float *, uint32_t, uint32_t, const uint32_t *, int);
#endif

namespace {

const int k_render_state_count = D3DRS_MAX / 4;   // the state values are pre-scaled by 4, the stride of the tables
const int k_sampler_state_count = D3DSAMP_MAX / 4;
const int k_sampler_count = 32;
const int k_fetch_count = GPU_D3D_TEXTURE_FETCH_CONSTANT_COUNT; // D3D samplers 0..25 = texture fetch constants 0..25
const int k_stream_count = PORT_VK_MAX_STREAMS;
const int k_flow_dwords = GPU_D3D_VERTEX_CONSTANTB_COUNT / 32 + GPU_D3D_PIXEL_CONSTANTB_COUNT / 32 +
                          GPU_D3D_VERTEX_CONSTANTI_COUNT + GPU_D3D_PIXEL_CONSTANTI_COUNT;
const int k_max_tiles = 16;

// D3DTAG bit `bit` of a D3DTAGCOLLECTION mask word (D3DTAG_MASKENCODE(bit, bit): bit 0 is the word's MSB).
inline UINT64 tag_bit(DWORD bit) { return 1ull << (63u - bit); }
const DWORD k_tag_texture_fetch_first = 32;  // D3DTAG_TEXTUREFETCHCONSTANTS, m_Mask[3]
const DWORD k_tag_flow = 7;                  // D3DTAG_FLOWCONSTANTS, m_Mask[4]
const DWORD k_tag_clip_plane_first = 9;      // D3DTAG_CLIPPLANE0, m_Mask[4]
const DWORD k_tag_pixel_shader = 43;         // D3DTAG_PIXELSHADER, m_Mask[2]
const DWORD k_tag_vertex_shader = 44;        // D3DTAG_VERTEXSHADER, m_Mask[2]
const DWORD k_tag_blend_control[4] = { 53, 61, 62, 63 }; // D3DTAG_BLENDCONTROL0..3, m_Mask[2]

// RB_BLENDCONTROL of "no blending": ONE, ADD, ZERO for color and alpha (D3DRS_ALPHABLENDENABLE FALSE).
const DWORD k_blend_control_disabled = 0x00010001u;

struct s_stream_binding
{
    D3DVertexBuffer *buffer;
    UINT offset;
    UINT stride;
};

// One device's D3D state: what the getters return and what the renderer draws with.
struct s_device_state
{
    DWORD render_states[k_render_state_count];
    DWORD sampler_states[k_sampler_count][k_sampler_state_count];
    D3DSurface *render_targets[4];
    D3DSurface *depth_stencil;
    D3DVertexShader *vertex_shader;
    D3DPixelShader *pixel_shader;
    D3DVertexDeclaration *vertex_declaration;
    s_stream_binding streams[k_stream_count];
    D3DIndexBuffer *index_buffer;
    D3DBaseTexture *textures[k_fetch_count];
    // What each texture fetch constant still describes once SetTexture(NULL) has dropped its texture (unbound_valid).
    // Xbox D3D's D3DDevice_SetTexture (d3d9 texture.obj, retail 0x82167a60) with a NULL texture only stores the NULL
    // reference and fences the old texture: it neither rewrites the fetch constant nor marks it pending, so the GPU
    // keeps sampling the last texture loaded into it. Halo 3 relies on that: clear_sampler_textures (lightmap shadows,
    // between the albedo and static lighting passes) unbinds every sampler, and the lighting entry points of
    // per-pixel lightmapped geometry fetch the lightprobe (samplers 12, 13) the albedo pass bound without binding it
    // again (GAME-CHANGES.md, section M).
    s_port_vk_texture unbound[k_fetch_count];
    bool unbound_valid[k_fetch_count];
    float viewport[6];
    RECT scissor;
    DWORD blend_control[4];     // RB_BLENDCONTROL0..3, mirrored in the device's m_ControlPacket
    DWORD predication;          // SetPredication's mask
    // The tiling bracket's surfaces (Xbox D3D's CDevice +0x31b0..+0x31c0): the ones bound at BeginTiling, or the ones
    // SetSurfaces registered since with its tiling flag. Only meaningful inside a bracket (window_applies).
    D3DSurface *tiling_render_targets[4];
    D3DSurface *tiling_depth_stencil;
};

struct s_recording;
struct s_vertex_shader_binding;


struct s_port_device
{
    D3D::CDevice *device;
    bool hal;                   // the D3DDEVTYPE_HAL device: its draws reach the renderer
    s_device_state state;
    s_recording *recording;     // the stream being recorded (BeginCommandBuffer / BeginTiling), or NULL
    bool recording_noop;        // BeginCommandBuffer while !gpu(): bracket open without recording ops
    DWORD *ring;
    DWORD gpr_flags, gpr_vertex, gpr_pixel; // SetShaderGPRAllocation
    s_port_vk_draw draw;
    uint64_t dirty_mask;
    uint32_t resource_touch_epoch;
    const s_vertex_shader_binding *cached_binding;
    uint32_t vs_chunk_gen[64];
    uint32_t ps_chunk_gen[64];
};

inline uint32_t next_gen(uint32_t &g)
{
    if (++g == 0) {
        port_vk_invalidate_draw_caches();
        g = 1;
    }
    return g;
}

inline void mark_chunks_range(uint32_t chunks[64], uint32_t gen, DWORD first, DWORD count)
{
    if (count == 0)
        return;
    DWORD c0 = first >> 2;
    DWORD c1 = (first + count - 1u) >> 2;
    if (c1 >= 64u)
        c1 = 63u;
    for (DWORD c = c0; c <= c1; c++)
        chunks[c] = gen;
}

inline void mark_chunks_mask(uint32_t chunks[64], uint32_t gen, UINT64 mask)
{
    while (mask != 0) {
        const DWORD g = (DWORD)__builtin_clzll(mask);
        mask &= ~(1ull << (63u - g));
        chunks[g] = gen;
    }
}

inline bool equal_vectors(const void *a, const void *b, DWORD count)
{
    const UINT64 *pa = (const UINT64 *)a;
    const UINT64 *pb = (const UINT64 *)b;
    const DWORD n = count * 2u;
    for (DWORD i = 0; i < n; i++)
        if (pa[i] != pb[i])
            return false;
    return true;
}

void publish_draw(s_port_device *pd);
void publish_window(s_port_device *pd);

bool publishes_draw(const s_port_device *pd) { return pd->hal && !pd->recording; }

void dirty_pipeline(s_port_device *pd, uint64_t bits)
{
    pd->dirty_mask |= bits;
    next_gen(pd->draw.state_gen);
}

void change_float_chunks(s_port_device *pd, bool pixel, UINT64 mask)
{
    pd->dirty_mask |= pixel ? PORT_VK_DIRTY_PS_CONST : PORT_VK_DIRTY_VS_CONST;
    mark_chunks_mask(pixel ? pd->ps_chunk_gen : pd->vs_chunk_gen,
                     next_gen(pixel ? pd->draw.ps_float_gen : pd->draw.vs_float_gen), mask);
}

void change_float_range(s_port_device *pd, bool pixel, DWORD first, DWORD count)
{
    if (count == 0)
        return;
    pd->dirty_mask |= pixel ? PORT_VK_DIRTY_PS_CONST : PORT_VK_DIRTY_VS_CONST;
    mark_chunks_range(pixel ? pd->ps_chunk_gen : pd->vs_chunk_gen,
                      next_gen(pixel ? pd->draw.ps_float_gen : pd->draw.vs_float_gen), first, count);
}

void change_flow(s_port_device *pd)
{
    pd->dirty_mask |= PORT_VK_DIRTY_VS_CONST | PORT_VK_DIRTY_PS_CONST;
    next_gen(pd->draw.bool_loop_gen);
}

// The game's view of a device is D3D::CDevice (include/d3d9/device.h): the public D3DDevice and the private state
// after it that d3d9/query.obj reads and writes (the occlusion query bookkeeping at 0x2f08..0x2f51).
__declspec(align(128)) D3D::CDevice g_hal_device;
__declspec(align(128)) D3D::CDevice g_command_buffer_device_object;

// The command rings the game's own packet writers fill (d3d_device_issue_occlusion_query: __storewordupdate through
// m_pRing). No GPU reads them on this port: the packets the D3D calls stand for go to the renderer through the calls
// themselves, so KickOff only hands the writer a fresh segment. m_pRing points one dword before the next write.
const DWORD k_ring_dwords = 1024;
const DWORD k_ring_guarantee_dwords = 32;  // d3d9.h: below m_pRingGuarantee, 32 dwords are always free
DWORD g_hal_ring[k_ring_dwords];
DWORD g_command_buffer_ring[k_ring_dwords];

s_port_device g_hal = { &g_hal_device, true };
s_port_device g_recorder = { &g_command_buffer_device_object, false };

D3DSurface g_back_buffer;

struct s_null_object : D3DResource
{
    DWORD payload[32];
};

static_assert(sizeof(void *) != 8 || sizeof(s_xenos_texture_desc) == 28, "ABI size mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_d3d_texture_source) == 72 &&
    offsetof(s_port_d3d_texture_source, base) == 0 && offsetof(s_port_d3d_texture_source, mip) == 8 &&
    offsetof(s_port_d3d_texture_source, desc) == 16 && offsetof(s_port_d3d_texture_source, identifier) == 44 &&
    offsetof(s_port_d3d_texture_source, fetch_constant) == 48), "s_port_d3d_texture_source 64-bit ABI mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_surface) == 40 &&
    offsetof(s_port_vk_surface, key) == 0 && offsetof(s_port_vk_surface, msaa_samples) == 32), "s_port_vk_surface 64-bit ABI mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_texture) == 88 &&
    offsetof(s_port_vk_texture, key) == 0 && offsetof(s_port_vk_texture, generation) == 8 &&
    offsetof(s_port_vk_texture, source) == 16), "s_port_vk_texture 64-bit ABI mismatch");
static_assert(sizeof(void *) != 8 || sizeof(s_port_vk_sampler_state) == 52, "s_port_vk_sampler_state size mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_sampler) == 144 &&
    offsetof(s_port_vk_sampler, texture) == 0 && offsetof(s_port_vk_sampler, state) == 88 &&
    offsetof(s_port_vk_sampler, bound) == 140), "s_port_vk_sampler 64-bit ABI mismatch");
static_assert(sizeof(void *) != 8 || sizeof(s_port_vk_render_state) == 148, "s_port_vk_render_state size mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_stream) == 32 &&
    offsetof(s_port_vk_stream, key) == 0 && offsetof(s_port_vk_stream, data) == 8), "s_port_vk_stream 64-bit ABI mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_indices) == 32 &&
    offsetof(s_port_vk_indices, key) == 0 && offsetof(s_port_vk_indices, data) == 8), "s_port_vk_indices 64-bit ABI mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_draw) == 4776 &&
    offsetof(s_port_vk_draw, state) == 24 && offsetof(s_port_vk_draw, streams) == 216 &&
    offsetof(s_port_vk_draw, indices) == 728 && offsetof(s_port_vk_draw, samplers) == 760 &&
    offsetof(s_port_vk_draw, clip_plane_enable) == 4504 && offsetof(s_port_vk_draw, point_size_max) == 4620 &&
    offsetof(s_port_vk_draw, samplers_any_gen) == 4748 && offsetof(s_port_vk_draw, vp_gen) == 4752 &&
    offsetof(s_port_vk_draw, vs_chunk_gen) == 4760 && offsetof(s_port_vk_draw, ps_chunk_gen) == 4768),
    "s_port_vk_draw 64-bit ABI mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_geometry) == 56 &&
    offsetof(s_port_vk_geometry, stream0) == 0 && offsetof(s_port_vk_geometry, indices) == 32 &&
    offsetof(s_port_vk_geometry, declaration) == 40 && offsetof(s_port_vk_geometry, stream_mask) == 48),
    "s_port_vk_geometry 64-bit ABI mismatch");
static_assert(PORT_VK_SAMPLER_COUNT == 26 && PORT_VK_DIRTY_SAMPLER(25) == (1ull << 63),
    "draw dirty mask ABI mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_compute_resource) == 24 &&
    offsetof(s_port_vk_compute_resource, storage) == 0 && offsetof(s_port_vk_compute_resource, binding) == 8),
    "s_port_vk_compute_resource 64-bit ABI mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_compute_constants) == 24 &&
    offsetof(s_port_vk_compute_constants, data) == 8), "s_port_vk_compute_constants 64-bit ABI mismatch");
static_assert(sizeof(void *) != 8 || (sizeof(s_port_vk_compute_dispatch) == 40 &&
    offsetof(s_port_vk_compute_dispatch, resources) == 0 && offsetof(s_port_vk_compute_dispatch, constants) == 8 &&
    offsetof(s_port_vk_compute_dispatch, explicit_index) == 16), "s_port_vk_compute_dispatch 64-bit ABI mismatch");

bool gpu() { return port_vulkan_device_active_fast() != 0; }

s_port_device *port_of(D3DDevice *device)
{
    return device == &g_command_buffer_device_object ? &g_recorder : &g_hal;
}

// The device's GPU work goes to the renderer now: the HAL device, not recording.
bool live(const s_port_device *pd) { return pd->hal && pd->recording == 0 && gpu(); }

// This file is built with the game's flags, and -fno-builtin makes every memcpy / memset in it a C library call, even
// a 4-byte one. The per-draw paths below (state for each draw, its 26 samplers, constants, replayed command buffers)
// therefore copy and clear with the compiler's builtins, which expand fixed sizes inline, as port/sdk/vmx does.
// memcmp has no such builtin (__builtin_memcmp is still the library call): fixed-size compares are written out.
DWORD swap32(DWORD v) { return (v >> 24) | ((v >> 8) & 0xff00u) | ((v << 8) & 0xff0000u) | (v << 24); }
DWORD float_bits(float f) { DWORD d; __builtin_memcpy(&d, &f, 4); return d; }
float bits_float(DWORD d) { float f; __builtin_memcpy(&f, &d, 4); return f; }

// A PA_SU_POINT_MINMAX field as the retail D3D writes it from D3DRS_POINTSIZE_MIN / _MAX
// (D3DDevice_SetRenderState_PointSizeMin / Max, default.pe 0x825a5588 / 0x825a55e0): the low 16 bits of
// fctiwz(value * 16) (saturated to the int range, NaN -> 0x80000000), a radius in 12.4 fixed point. Returned as the
// sprite diameter in pixels the Xenos clamps a vertex shader's point size to.
float point_minmax_diameter(DWORD state)
{
    const float scaled = bits_float(state) * 16.0f;
    const int fixed = !(scaled == scaled) || scaled <= -2147483648.0f ? -2147483647 - 1
                      : scaled >= 2147483648.0f                       ? 2147483647
                                                                      : (int)scaled;
    return (float)(WORD)fixed * (2.0f / 16.0f);
}

// count float4 registers (the game's constant data is only 4-byte aligned): 16-byte moves instead of a library call
// for the one to a few registers most calls set.
void copy_vectors(void *destination, const void *source, DWORD count)
{
    char *d = (char *)destination;
    const char *s = (const char *)source;
    for (DWORD i = 0; i < count; i++)
        __builtin_memcpy(d + 16u * i, s + 16u * i, 16);
}

void reset_ring(s_port_device *pd)
{
    pd->device->m_pRing = (PRING)&pd->ring[0];
    pd->device->m_pRingLimit = (PRING)&pd->ring[k_ring_dwords - 1];
    pd->device->m_pRingGuarantee = (PRING)&pd->ring[k_ring_dwords - 1 - k_ring_guarantee_dwords];
}

void *alloc_zeroed(size_t bytes)
{
    return RtlAllocateHeap(GetProcessHeap(), 0x08 /* HEAP_ZERO_MEMORY */, bytes);
}

D3DSurface *make_texture_surface_view(D3DBaseTexture *parent, UINT array_index, UINT level)
{
    D3DSurface *surf = (D3DSurface *)alloc_zeroed(sizeof(D3DSurface));
    if (!surf)
        return 0;
    surf->Common = D3DCOMMON_TYPE_SURFACE | D3DCOMMON_D3DCREATED | D3DCOMMON_SURFACE_FROM_TEXTURE;
    surf->ReferenceCount = 1;
    surf->BaseFlush = D3DFLUSH_INITIAL_VALUE;
    surf->Parent = parent;
    surf->MipLevel = level;
    surf->ArrayIndex = array_index;
    return surf;
}

// Reported once per id (port_write_error), for what the device does not do.
bool g_reported[16];
void report_once(DWORD id, const char *text)
{
    if (id < 16 && g_reported[id])
        return;
    if (id < 16)
        g_reported[id] = true;
    port_write_error("d3d device: ");
    port_write_error(text);
    port_write_error("\n");
}

// ---- sampler state: the D3D values (sampler_states) and the texture fetch constants the inline setters write ----

GPUTEXTURE_FETCH_CONSTANT *fetch_of(s_port_device *pd, DWORD sampler)
{
    return &pd->device->m_Constants.TextureFetch[GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(sampler)];
}

void mark_sampler_pending(s_port_device *pd, DWORD sampler)
{
    const DWORD fetch = GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(sampler);
    if (fetch < (DWORD)k_fetch_count)
        pd->device->m_Pending.m_Mask[3] |= tag_bit(k_tag_texture_fetch_first + fetch);
}

DWORD aniso_level(DWORD max_anisotropy)
{
    // GPUANISOFILTER: disabled, 1:1, 2:1, 4:1, 8:1, 16:1
    return max_anisotropy <= 1 ? 1u : max_anisotropy <= 2 ? 2u : max_anisotropy <= 4 ? 3u : max_anisotropy <= 8 ? 4u : 5u;
}

void update_aniso(s_port_device *pd, DWORD sampler)
{
    const DWORD *row = pd->state.sampler_states[sampler];
    const bool aniso = row[D3DSAMP_MAGFILTER / 4] == D3DTEXF_ANISOTROPIC || row[D3DSAMP_MINFILTER / 4] == D3DTEXF_ANISOTROPIC;
    fetch_of(pd, sampler)->AnisoFilter = aniso ? aniso_level(row[D3DSAMP_MAXANISOTROPY / 4]) : 0u;
}

void update_volume_filter(s_port_device *pd, DWORD sampler)
{
    // VolMagFilter / VolMinFilter (filtering along Z: between a volume's slices, a stacked texture's layers):
    // MAGFILTERZ / MINFILTERZ with SEPARATEZFILTERENABLE, else MAGFILTER / MINFILTER (point or not, one bit each)
    const DWORD *row = pd->state.sampler_states[sampler];
    const bool separate = row[D3DSAMP_SEPARATEZFILTERENABLE / 4] != 0;
    GPUTEXTURE_FETCH_CONSTANT *f = fetch_of(pd, sampler);
    f->VolMagFilter = row[(separate ? D3DSAMP_MAGFILTERZ : D3DSAMP_MAGFILTER) / 4] == D3DTEXF_POINT ? 0u : 1u;
    f->VolMinFilter = row[(separate ? D3DSAMP_MINFILTERZ : D3DSAMP_MINFILTER) / 4] == D3DTEXF_POINT ? 0u : 1u;
}

void apply_sampler_state(s_port_device *pd, DWORD sampler, DWORD type, DWORD value)
{
    if (sampler >= (DWORD)k_sampler_count || type / 4 >= (DWORD)k_sampler_state_count)
        return;
    if (sampler < (DWORD)k_fetch_count && !pd->recording) {
        const GPUTEXTURE_FETCH_CONSTANT *cf = fetch_of(pd, sampler);
        bool same = false;
        switch (type) {
        case D3DSAMP_ADDRESSU: same = cf->ClampX == value; break;
        case D3DSAMP_ADDRESSV: same = cf->ClampY == value; break;
        case D3DSAMP_ADDRESSW: same = cf->ClampZ == value; break;
        case D3DSAMP_MIPFILTER: same = cf->MipFilter == value; break;
        case D3DSAMP_BORDERCOLOR: same = cf->BorderColor == (value == 0 ? GPUBORDERCOLOR_ABGR_BLACK : GPUBORDERCOLOR_ABGR_WHITE); break;
        case D3DSAMP_TRILINEARTHRESHOLD: same = cf->TriClamp == value; break;
        case D3DSAMP_HGRADIENTEXPBIAS: same = cf->GradExpAdjustH == (INT)value; break;
        case D3DSAMP_VGRADIENTEXPBIAS: same = cf->GradExpAdjustV == (INT)value; break;
        case D3DSAMP_WHITEBORDERCOLORW: same = cf->ForceBCWToMax == value; break;
        case D3DSAMP_POINTBORDERENABLE: same = cf->ClampPolicy == (value ? 0u : 1u); break;
        default: same = pd->state.sampler_states[sampler][type / 4] == value; break;
        }
        if (same) {
            pd->state.sampler_states[sampler][type / 4] = value;
            return;
        }
    }
    pd->state.sampler_states[sampler][type / 4] = value;
    if (sampler >= (DWORD)k_fetch_count)
        return;
    GPUTEXTURE_FETCH_CONSTANT *f = fetch_of(pd, sampler);
    switch (type) {
    case D3DSAMP_ADDRESSU: f->ClampX = value; break;
    case D3DSAMP_ADDRESSV: f->ClampY = value; break;
    case D3DSAMP_ADDRESSW: f->ClampZ = value; break;
    case D3DSAMP_MAGFILTER:
        f->MagFilter = value == D3DTEXF_POINT ? 0u : 1u;
        update_aniso(pd, sampler);
        update_volume_filter(pd, sampler);
        break;
    case D3DSAMP_MINFILTER:
        f->MinFilter = value == D3DTEXF_POINT ? 0u : 1u;
        update_aniso(pd, sampler);
        update_volume_filter(pd, sampler);
        break;
    case D3DSAMP_MAGFILTERZ:
    case D3DSAMP_MINFILTERZ:
    case D3DSAMP_SEPARATEZFILTERENABLE: update_volume_filter(pd, sampler); break;
    case D3DSAMP_MIPFILTER: f->MipFilter = value; break;
    case D3DSAMP_MAXANISOTROPY: update_aniso(pd, sampler); break;
    case D3DSAMP_MIPMAPLODBIAS: {
        float bias;
        __builtin_memcpy(&bias, &value, 4);
        int fixed = (int)(bias * 32.0f + (bias < 0 ? -0.5f : 0.5f)); // LODBias: signed, 5 fraction bits
        f->LODBias = fixed < -512 ? -512 : fixed > 511 ? 511 : fixed;
        break;
    }
    case D3DSAMP_BORDERCOLOR: f->BorderColor = value == 0 ? GPUBORDERCOLOR_ABGR_BLACK : GPUBORDERCOLOR_ABGR_WHITE; break;
    case D3DSAMP_TRILINEARTHRESHOLD: f->TriClamp = value; break;
    case D3DSAMP_HGRADIENTEXPBIAS: f->GradExpAdjustH = (INT)value; break;
    case D3DSAMP_VGRADIENTEXPBIAS: f->GradExpAdjustV = (INT)value; break;
    case D3DSAMP_WHITEBORDERCOLORW: f->ForceBCWToMax = value; break;
    case D3DSAMP_POINTBORDERENABLE: f->ClampPolicy = value ? 0u : 1u; break;
    default: break;
    }
    mark_sampler_pending(pd, sampler);
}

DWORD read_sampler_state(s_port_device *pd, DWORD sampler, DWORD type)
{
    if (sampler >= (DWORD)k_sampler_count || type / 4 >= (DWORD)k_sampler_state_count)
        return 0;
    if (sampler >= (DWORD)k_fetch_count)
        return pd->state.sampler_states[sampler][type / 4];
    const GPUTEXTURE_FETCH_CONSTANT *f = fetch_of(pd, sampler);
    switch (type) { // the states the game's inline code sets live only in the fetch constant
    case D3DSAMP_ADDRESSU: return f->ClampX;
    case D3DSAMP_ADDRESSV: return f->ClampY;
    case D3DSAMP_ADDRESSW: return f->ClampZ;
    case D3DSAMP_MIPFILTER: return f->MipFilter;
    case D3DSAMP_BORDERCOLOR: return f->BorderColor == GPUBORDERCOLOR_ABGR_BLACK ? 0u : 1u;
    case D3DSAMP_TRILINEARTHRESHOLD: return f->TriClamp;
    case D3DSAMP_HGRADIENTEXPBIAS: return (DWORD)f->GradExpAdjustH;
    case D3DSAMP_VGRADIENTEXPBIAS: return (DWORD)f->GradExpAdjustV;
    case D3DSAMP_WHITEBORDERCOLORW: return f->ForceBCWToMax;
    case D3DSAMP_POINTBORDERENABLE: return f->ClampPolicy ? 0u : 1u;
    default: return pd->state.sampler_states[sampler][type / 4];
    }
}

// The sampler states kept only in sampler_states (everything else is read back from the fetch constant): what a
// recorded sampler carries besides its fetch constant and texture.
const DWORD k_row_states[] = { D3DSAMP_MAGFILTER, D3DSAMP_MINFILTER, D3DSAMP_MAXANISOTROPY, D3DSAMP_MIPMAPLODBIAS,
                               D3DSAMP_MAXMIPLEVEL, D3DSAMP_MAGFILTERZ, D3DSAMP_MINFILTERZ,
                               D3DSAMP_SEPARATEZFILTERENABLE, D3DSAMP_MINMIPLEVEL, D3DSAMP_ANISOTROPYBIAS };
const int k_row_state_count = sizeof(k_row_states) / sizeof(k_row_states[0]);

DWORD host_address(DWORD clamp)
{
    // GPUCLAMP -> D3DTADDRESS (the halfway clamps behave as their edge forms; mirror-to-border as border)
    switch (clamp) {
    case 4: return D3DTADDRESS_CLAMP;
    case 5: return D3DTADDRESS_MIRRORONCE;
    case 7: return D3DTADDRESS_BORDER;
    default: return clamp;
    }
}

void sampler_state_for_host(s_port_device *pd, DWORD sampler, s_port_vk_sampler_state *out)
{
    const GPUTEXTURE_FETCH_CONSTANT *f = fetch_of(pd, sampler);
    const DWORD *row = pd->state.sampler_states[sampler];
    out->mag_filter = row[D3DSAMP_MAGFILTER / 4];
    out->min_filter = row[D3DSAMP_MINFILTER / 4];
    out->mip_filter = f->MipFilter;
    out->address_u = host_address(f->ClampX);
    out->address_v = host_address(f->ClampY);
    out->address_w = host_address(f->ClampZ);
    out->max_anisotropy = row[D3DSAMP_MAXANISOTROPY / 4];
    out->border_color = f->BorderColor == GPUBORDERCOLOR_ABGR_BLACK ? 0u : f->BorderColor == GPUBORDERCOLOR_ABGR_WHITE ? 2u : 1u;
    DWORD bias = row[D3DSAMP_MIPMAPLODBIAS / 4];
    __builtin_memcpy(&out->lod_bias, &bias, 4);
    out->min_mip_level = row[D3DSAMP_MAXMIPLEVEL / 4]; // D3D: the most detailed level allowed
    out->max_mip_level = 15;
    out->mag_filter_z = f->VolMagFilter;
    out->min_filter_z = f->VolMinFilter;
}

// ---- blending: RB_BLENDCONTROL0..3 ----

void write_blend_control(s_port_device *pd, DWORD index, DWORD value)
{
    pd->state.blend_control[index] = value;
    if (publishes_draw(pd) && pd->draw.state.blend_control[index] != value) {
        pd->draw.state.blend_control[index] = value;
        dirty_pipeline(pd, PORT_VK_DIRTY_BLEND);
    }
    GPU_CONTROLPACKET &packet = pd->device->m_ControlPacket;
    GPU_BLENDCONTROL *controls[4] = { &packet.BlendControl0, &packet.BlendControl1, &packet.BlendControl2,
                                      &packet.BlendControl3 };
    controls[index]->dword = value;
    pd->device->m_Pending.m_Mask[2] |= tag_bit(k_tag_blend_control[index]);
}

// The register value of the D3DRS blend states, as d3d9/state.obj computes it (D3DDevice_SetRenderState_SrcBlend,
// AlphaBlendEnable, ...): color source / op / destination in the low half; the alpha half is its own states with
// D3DRS_SEPARATEALPHABLENDENABLE, otherwise the color ones, with D3DBLEND_SRCALPHASAT (meaningless for alpha) as ONE.
DWORD blend_control_of_render_states(const s_port_device *pd)
{
    const DWORD *rs = pd->state.render_states;
    const DWORD color = (rs[D3DRS_SRCBLEND / 4] & 0x1fu) | (rs[D3DRS_BLENDOP / 4] & 7u) << 5 |
                        (rs[D3DRS_DESTBLEND / 4] & 0x1fu) << 8;
    DWORD alpha;
    if (rs[D3DRS_SEPARATEALPHABLENDENABLE / 4]) {
        alpha = (rs[D3DRS_SRCBLENDALPHA / 4] & 0x1fu) | (rs[D3DRS_BLENDOPALPHA / 4] & 7u) << 5 |
                (rs[D3DRS_DESTBLENDALPHA / 4] & 0x1fu) << 8;
    } else {
        alpha = color;
        if ((alpha & 0x1fu) == D3DBLEND_SRCALPHASAT)
            alpha = (alpha & ~0x1fu) | D3DBLEND_ONE;
        if (((alpha >> 8) & 0x1fu) == D3DBLEND_SRCALPHASAT)
            alpha = (alpha & ~0x1f00u) | (D3DBLEND_ONE << 8);
    }
    return color | alpha << 16;
}

// ---- render states ----

void publish_render_state(s_port_device *pd, DWORD index, DWORD value)
{
    s_port_vk_draw &d = pd->draw;
    switch (index * 4) {
#define DRAW_RS(reg, field, group) \
    case D3DRS_##reg: \
        if (d.state.field != value) { \
            d.state.field = value; \
            dirty_pipeline(pd, PORT_VK_DIRTY_##group); \
        } \
        break
    DRAW_RS(ZENABLE, z_enable, DS);
    DRAW_RS(ZWRITEENABLE, z_write_enable, DS);
    DRAW_RS(ZFUNC, z_func, DS);
    DRAW_RS(CULLMODE, cull_mode, RAST);
    DRAW_RS(FILLMODE, fill_mode, RAST);
    DRAW_RS(COLORWRITEENABLE, color_write_enable[0], BLEND);
    DRAW_RS(COLORWRITEENABLE1, color_write_enable[1], BLEND);
    DRAW_RS(COLORWRITEENABLE2, color_write_enable[2], BLEND);
    DRAW_RS(COLORWRITEENABLE3, color_write_enable[3], BLEND);
    DRAW_RS(STENCILENABLE, stencil_enable, DS);
    DRAW_RS(TWOSIDEDSTENCILMODE, two_sided_stencil_mode, DS);
    DRAW_RS(STENCILFAIL, stencil_fail, DS);
    DRAW_RS(STENCILZFAIL, stencil_z_fail, DS);
    DRAW_RS(STENCILPASS, stencil_pass, DS);
    DRAW_RS(STENCILFUNC, stencil_func, DS);
    DRAW_RS(STENCILREF, stencil_ref, DS);
    DRAW_RS(STENCILMASK, stencil_mask, DS);
    DRAW_RS(STENCILWRITEMASK, stencil_write_mask, DS);
    DRAW_RS(CCW_STENCILFAIL, ccw_stencil_fail, DS);
    DRAW_RS(CCW_STENCILZFAIL, ccw_stencil_z_fail, DS);
    DRAW_RS(CCW_STENCILPASS, ccw_stencil_pass, DS);
    DRAW_RS(CCW_STENCILFUNC, ccw_stencil_func, DS);
    DRAW_RS(CCW_STENCILREF, ccw_stencil_ref, DS);
    DRAW_RS(CCW_STENCILMASK, ccw_stencil_mask, DS);
    DRAW_RS(CCW_STENCILWRITEMASK, ccw_stencil_write_mask, DS);
    DRAW_RS(DEPTHBIAS, depth_bias, RAST);
    DRAW_RS(SLOPESCALEDEPTHBIAS, slope_scale_depth_bias, RAST);
    DRAW_RS(PRIMITIVERESETENABLE, primitive_reset_enable, RAST);
    DRAW_RS(BLENDFACTOR, blend_factor, BLEND);
#undef DRAW_RS
    case D3DRS_SCISSORTESTENABLE:
    case D3DRS_HALFPIXELOFFSET:
        publish_window(pd);
        break;
    case D3DRS_ALPHATESTENABLE:
    case D3DRS_ALPHAFUNC: {
        if ((index == D3DRS_ALPHATESTENABLE / 4 ? d.state.alpha_test_enable : d.state.alpha_func) != value)
            pd->dirty_mask |= PORT_VK_DIRTY_RAST;
        const bool specialized = d.state.alpha_test_enable && (d.state.alpha_func & 7u) != 7u;
        if (index == D3DRS_ALPHATESTENABLE / 4)
            d.state.alpha_test_enable = value;
        else
            d.state.alpha_func = value;
        if (specialized != (d.state.alpha_test_enable && (d.state.alpha_func & 7u) != 7u))
            dirty_pipeline(pd, PORT_VK_DIRTY_RAST);
        break;
    }
    case D3DRS_ALPHAREF:
        if (d.state.alpha_ref != value) {
            d.state.alpha_ref = value;
            pd->dirty_mask |= PORT_VK_DIRTY_RAST;
        }
        break;
    case D3DRS_CLIPPLANEENABLE:
        if (d.clip_plane_enable != (value & 0x3fu)) pd->dirty_mask |= PORT_VK_DIRTY_RAST;
        d.clip_plane_enable = value & 0x3fu;
        break;
    case D3DRS_POINTSIZE_MIN:
    case D3DRS_POINTSIZE_MAX: {
        float &limit = index == D3DRS_POINTSIZE_MIN / 4 ? d.point_size_min : d.point_size_max;
        const float diameter = point_minmax_diameter(value);
        if (limit != diameter) {
            limit = diameter;
            pd->dirty_mask |= PORT_VK_DIRTY_RAST;
        }
        break;
    }
    default:
        break;
    }
}

void apply_render_state(s_port_device *pd, DWORD index, DWORD value)
{
    if (index >= (DWORD)k_render_state_count)
        return;
    const bool changed = pd->state.render_states[index] != value;
    pd->state.render_states[index] = value;
    switch (index * 4) {
    case D3DRS_ALPHABLENDENABLE:
        // the blend states write all four render targets' RB_BLENDCONTROL; disabling writes "no blending"
        for (DWORD i = 0; i < 4; i++)
            write_blend_control(pd, i, value ? blend_control_of_render_states(pd) : k_blend_control_disabled);
        break;
    case D3DRS_SEPARATEALPHABLENDENABLE:
    case D3DRS_SRCBLEND:
    case D3DRS_DESTBLEND:
    case D3DRS_BLENDOP:
    case D3DRS_SRCBLENDALPHA:
    case D3DRS_DESTBLENDALPHA:
    case D3DRS_BLENDOPALPHA:
        // with blending off the registers keep what they have (a SetBlendState of another target included)
        if (pd->state.render_states[D3DRS_ALPHABLENDENABLE / 4])
            for (DWORD i = 0; i < 4; i++)
                write_blend_control(pd, i, blend_control_of_render_states(pd));
        break;
    default:
        if (changed && publishes_draw(pd))
            publish_render_state(pd, index, value);
        break;
    }
}

void reset_device_state(s_port_device *pd)
{
    memset(&pd->state, 0, sizeof(pd->state));
    DWORD *rs = pd->state.render_states;
    rs[D3DRS_ZWRITEENABLE / 4] = TRUE;
    rs[D3DRS_ZFUNC / 4] = D3DCMP_LESSEQUAL;
    rs[D3DRS_CULLMODE / 4] = D3DCULL_CCW;
    rs[D3DRS_FILLMODE / 4] = D3DFILL_SOLID;
    rs[D3DRS_SRCBLEND / 4] = D3DBLEND_ONE;
    rs[D3DRS_DESTBLEND / 4] = D3DBLEND_ZERO;
    rs[D3DRS_SRCBLENDALPHA / 4] = D3DBLEND_ONE;
    rs[D3DRS_DESTBLENDALPHA / 4] = D3DBLEND_ZERO;
    rs[D3DRS_ALPHAFUNC / 4] = D3DCMP_ALWAYS;
    rs[D3DRS_STENCILFUNC / 4] = D3DCMP_ALWAYS;
    rs[D3DRS_CCW_STENCILFUNC / 4] = D3DCMP_ALWAYS;
    rs[D3DRS_STENCILMASK / 4] = 0xff;
    rs[D3DRS_STENCILWRITEMASK / 4] = 0xff;
    rs[D3DRS_CCW_STENCILMASK / 4] = 0xff;
    rs[D3DRS_CCW_STENCILWRITEMASK / 4] = 0xff;
    rs[D3DRS_COLORWRITEENABLE / 4] = 0xf;
    rs[D3DRS_COLORWRITEENABLE1 / 4] = 0xf;
    rs[D3DRS_COLORWRITEENABLE2 / 4] = 0xf;
    rs[D3DRS_COLORWRITEENABLE3 / 4] = 0xf;
    // the retail D3D's render state defaults (its table of getter, setter and default per state, default.pe
    // 0x82722270-0x8272229c): point size 1, minimum 1, maximum 64
    rs[D3DRS_POINTSIZE / 4] = float_bits(1.0f);
    rs[D3DRS_POINTSIZE_MIN / 4] = float_bits(1.0f);
    rs[D3DRS_POINTSIZE_MAX / 4] = float_bits(64.0f);
    for (DWORD i = 0; i < 4; i++)
        write_blend_control(pd, i, k_blend_control_disabled);
    for (int s = 0; s < k_sampler_count; s++) {
        apply_sampler_state(pd, (DWORD)s, D3DSAMP_MAXANISOTROPY, 1);
        apply_sampler_state(pd, (DWORD)s, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
        apply_sampler_state(pd, (DWORD)s, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
        apply_sampler_state(pd, (DWORD)s, D3DSAMP_MINFILTER, D3DTEXF_POINT);
        apply_sampler_state(pd, (DWORD)s, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
        apply_sampler_state(pd, (DWORD)s, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
        apply_sampler_state(pd, (DWORD)s, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
    }
    pd->gpr_flags = 0;
    pd->gpr_vertex = 64;
    pd->gpr_pixel = 64;
    publish_draw(pd);
}

// ---- descriptions for the Vulkan renderer ----

void surface_info(D3DSurface *surface, DWORD depth, s_port_vk_surface *out)
{
    out->key = surface;
    out->depth = depth;
    if (surface->Common & D3DCOMMON_SURFACE_FROM_TEXTURE) {
        XGTEXTURE_DESC td;
        XGGetTextureDesc(surface->Parent, surface->MipLevel, &td);
        out->width = td.Width;
        out->height = td.Height;
        out->d3d_format = (DWORD)td.Format;
        out->edram_base = 0xffffffffu;
        out->color_exp_bias = 0;
        out->msaa_samples = 0;
    } else {
        out->width = surface->Width;
        out->height = surface->Height;
        out->d3d_format = (DWORD)surface->Format;
        out->edram_base = depth ? surface->DepthInfo.DepthBase : surface->ColorInfo.ColorBase;
        // a 6-bit signed field
        out->color_exp_bias = depth ? 0 : (int)((surface->ColorInfo.ColorExpBias ^ 0x20u) & 0x3fu) - 0x20;
        out->msaa_samples = surface->SurfaceInfo.MsaaSamples;
    }
    out->reserved = 0;
}

bool texture_info(D3DBaseTexture *texture, s_port_vk_texture *out)
{
    out->key = texture;
    out->generation = 0;
    out->reserved = 0;
    return port_d3d_texture_get_source(texture, &out->source) != 0;
}

// Header words used to derive a bound texture's description. The description itself lives in the HAL packet.
struct s_texture_header {
    const D3DBaseTexture *texture;
    DWORD common, identifier;
    DWORD format[6];
};
s_texture_header g_texture_headers[PORT_VK_SAMPLER_COUNT];

bool texture_header_unchanged(const s_texture_header &header, const D3DBaseTexture *texture)
{
    return header.texture == texture && header.common == texture->Common && header.identifier == texture->Identifier &&
           __builtin_memcmp(header.format, texture->Format.dword, sizeof(header.format)) == 0;
}

// Whether a vertex / index buffer's memory is in Xenos byte order (d3d_port_buffer.h): a header the game built with
// XGSet*BufferHeader, while paged resources still arrive unconverted.
uint32_t xenos_order(const D3DResource *buffer)
{
    return (buffer->Identifier & PORT_D3D_BUFFER_ID_KIND_MASK) == PORT_D3D_BUFFER_ID_XENOS_ORDER &&
           !port_resource_paged_data_is_host_order();
}

// ---- the EDRAM window of the tile being replayed (predicated tiling) ----

struct s_window
{
    bool active;
    int x, y;                   // the tile's screen origin (the window offset is its negation)
    int width, height;          // the tile's size: the window scissor
};
s_window g_window;

// Whether what the device renders now goes through the tile's window. Xbox D3D moves rendering into the tile (the
// window offset and the tile's window scissor, D3D::SetSurfaceClip 0x8214c5c8; the clear rectangles,
// D3D::ClearSurface 0x821695c8; the resolve source and destination, D3DDevice_Resolve 0x82177e7c) only while every
// bound surface is the bracket's own in its slot, or none. With any other surface bound it sends window offset 0 and
// the surface's own scissor, unpredicated, so every tile renders, clears and resolves that surface whole. Halo 3
// renders its lightmap shadow maps that way inside the UI shell's bracket (render_occlusions_setup_depth_target binds
// the 512x512 shadow depth, surface 2) and the shadow_apply pass of each tile reads the whole map; moved into the
// window, each tile drew and resolved only its own 160 rows of the map, and shadow_apply read the other rows from
// whatever was resolved there before (another object's map, an earlier tile's): square shadow cut-outs on the lobby
// Spartans.
bool window_applies(const s_port_device *pd)
{
    if (!g_window.active)
        return false;
    const s_device_state &st = pd->state;
    for (int i = 0; i < 4; i++)
        if (st.render_targets[i] && st.render_targets[i] != st.tiling_render_targets[i])
            return false;
    return !st.depth_stencil || st.depth_stencil == st.tiling_depth_stencil;
}

// Moves a screen rectangle into the window, when the device renders through it, and clips it to the window. Returns
// false when nothing is left.
bool window_rect(const s_port_device *pd, int rect[4])
{
    if (!window_applies(pd))
        return rect[2] > rect[0] && rect[3] > rect[1];
    rect[0] -= g_window.x;
    rect[2] -= g_window.x;
    rect[1] -= g_window.y;
    rect[3] -= g_window.y;
    if (rect[0] < 0) rect[0] = 0;
    if (rect[1] < 0) rect[1] = 0;
    if (rect[2] > g_window.width) rect[2] = g_window.width;
    if (rect[3] > g_window.height) rect[3] = g_window.height;
    return rect[2] > rect[0] && rect[3] > rect[1];
}

// D3D's private part of a vertex shader object, in the XDK's layout (titles read it: the bound flag says whether
// D3DVertexShader_Bind is still needed). The object is 0x368 bytes of D3D state, the D3DResource
// first and the binding slots from +0x28, followed by the copy of the microcode container whose first word is the
// container magic with the shader's flags in its low byte; 0x40 is "bound". A tag object (XGRegisterVertexShader)
// keeps that word in the Xbox's byte order, an object CreateVertexShader made keeps the order of the function it
// was given. Bind records the declaration and stream strides in the binding slots, where the XDK keeps its patched
// fetch state: a reloaded tag object loses it with the rest of the object, as on the Xbox.
const DWORD k_vertex_shader_binding = 0x28;
const DWORD k_vertex_shader_container = 0x368;
const DWORD k_vertex_shader_bound = 0x40;
const DWORD k_vertex_binding_marker = 0x50564231u; // 'PVB1'

struct s_vertex_shader_binding
{
    DWORD marker;
    D3DVertexDeclaration *declaration;
    DWORD strides[k_stream_count];  // bytes; 0 where the declaration reads no data from the stream
};
static_assert(k_vertex_shader_binding + sizeof(s_vertex_shader_binding) <= k_vertex_shader_container,
    "the binding record fits the XDK's binding slots");

const s_vertex_shader_binding *vertex_shader_binding(const D3DVertexShader *shader)
{
    if (!shader)
        return 0;
    const s_vertex_shader_binding *b = (const s_vertex_shader_binding *)((const char *)shader + k_vertex_shader_binding);
    return b->marker == k_vertex_binding_marker ? b : 0;
}

// The declaration a draw fetches with: the device's, or with none set the one the vertex shader was bound to.
D3DVertexDeclaration *draw_declaration(const s_device_state &st, const s_vertex_shader_binding **out_binding)
{
    *out_binding = 0;
    if (st.vertex_declaration)
        return st.vertex_declaration;
    const s_vertex_shader_binding *b = vertex_shader_binding(st.vertex_shader);
    *out_binding = b;
    return b ? b->declaration : 0;
}

void publish_window(s_port_device *pd)
{
    if (!publishes_draw(pd))
        return;
    const s_device_state &st = pd->state;
    s_port_vk_draw &d = pd->draw;
    float viewport[6];
    __builtin_memcpy(viewport, st.viewport, sizeof(viewport));
    int scissor[] = { (int)st.scissor.left, (int)st.scissor.top, (int)st.scissor.right, (int)st.scissor.bottom };
    const bool windowed = window_applies(pd);
    const int fragment_offset[] = { windowed ? g_window.x : 0, windowed ? g_window.y : 0 };
    const DWORD scissor_enable = st.render_states[D3DRS_SCISSORTESTENABLE / 4];
    const DWORD effective_enable = windowed ? 1u : scissor_enable;
    const float center = st.render_states[D3DRS_HALFPIXELOFFSET / 4] ? 0.0f : 0.5f;
    if (windowed) {
        if (viewport[2] > 0.0f && viewport[3] > 0.0f) {
            viewport[0] -= (float)g_window.x;
            viewport[1] -= (float)g_window.y;
        }
        if (!scissor_enable) {
            scissor[0] = g_window.x;
            scissor[1] = g_window.y;
            scissor[2] = g_window.x + g_window.width;
            scissor[3] = g_window.y + g_window.height;
        }
        window_rect(pd, scissor);
        if (scissor[2] < scissor[0])
            scissor[2] = scissor[0];
        if (scissor[3] < scissor[1])
            scissor[3] = scissor[1];
    }
    const bool viewport_changed = __builtin_memcmp(viewport, d.viewport, sizeof(viewport)) != 0;
    const bool scissor_changed = __builtin_memcmp(scissor, d.scissor, sizeof(scissor)) != 0;
    const bool origin_changed = __builtin_memcmp(fragment_offset, d.fragment_offset, sizeof(fragment_offset)) != 0;
    if (!viewport_changed && !scissor_changed && !origin_changed &&
        effective_enable == d.state.scissor_test_enable && center == d.pixel_center_offset)
        return;
    if (viewport_changed)
        __builtin_memcpy(d.viewport, viewport, sizeof(viewport));
    if (scissor_changed)
        __builtin_memcpy(d.scissor, scissor, sizeof(scissor));
    if (origin_changed)
        __builtin_memcpy(d.fragment_offset, fragment_offset, sizeof(fragment_offset));
    if (d.state.scissor_test_enable != effective_enable)
        d.state.scissor_test_enable = effective_enable;
    if (d.pixel_center_offset != center)
        d.pixel_center_offset = center;
    pd->dirty_mask |= PORT_VK_DIRTY_VP;
    next_gen(d.vp_gen);
}

bool publish_stream(s_port_device *pd, DWORD index)
{
    const s_stream_binding &binding = pd->state.streams[index];
    D3DVertexBuffer *vb = binding.buffer;
    s_port_vk_stream stream = {};
    if (vb) {
        // CPU-created buffers may be persistently mapped and rewritten without another Lock.
        stream.data = (const void *)((size_t)(DWORD)vb->Format.BaseAddress << 2);
        stream.key = ((vb->Common & D3DCOMMON_D3DCREATED) || port_vk_buffer_was_mapped(vb, stream.data)) ? 0 : vb;
        stream.size = vb->Format.Size * 4u;
        stream.offset = binding.offset;
        stream.stride = binding.stride ? binding.stride : pd->cached_binding ? pd->cached_binding->strides[index] : 0u;
        stream.big_endian = xenos_order(vb);
    }
    s_port_vk_draw &d = pd->draw;
    const uint32_t bit = 1u << index;
    if (((d.stream_mask & bit) != 0) == (vb != 0) &&
        __builtin_memcmp(&d.streams[index], &stream, sizeof(stream)) == 0)
        return false;
    d.streams[index] = stream;
    d.stream_mask = (d.stream_mask & ~bit) | (vb ? bit : 0u);
    pd->dirty_mask |= PORT_VK_DIRTY_STREAMS;
    next_gen(d.stream_gen);
    return true;
}

void publish_declaration(s_port_device *pd)
{
    const s_vertex_shader_binding *binding;
    D3DVertexDeclaration *declaration = draw_declaration(pd->state, &binding);
    const void *elements = declaration ? (const void *)(size_t)((s_null_object *)declaration)->payload[0] : 0;
    if (pd->draw.declaration != elements) {
        pd->draw.declaration = elements;
        pd->dirty_mask |= PORT_VK_DIRTY_VDECL;
    }
    pd->cached_binding = binding;
    for (uint32_t mask = pd->draw.stream_mask; mask != 0; mask &= mask - 1u) {
        const DWORD index = (DWORD)__builtin_ctz(mask);
        if (pd->state.streams[index].stride == 0)
            publish_stream(pd, index);
    }
}

void apply_vertex_shader(s_port_device *pd, D3DVertexShader *shader)
{
    pd->state.vertex_shader = shader;
    if (publishes_draw(pd) && pd->draw.vertex_shader != shader) {
        pd->draw.vertex_shader = shader;
        dirty_pipeline(pd, PORT_VK_DIRTY_VS);
        publish_declaration(pd);
    }
}

void apply_pixel_shader(s_port_device *pd, D3DPixelShader *shader)
{
    pd->state.pixel_shader = shader;
    if (publishes_draw(pd) && pd->draw.pixel_shader != shader) {
        pd->draw.pixel_shader = shader;
        dirty_pipeline(pd, PORT_VK_DIRTY_PS);
    }
}

void apply_declaration(s_port_device *pd, D3DVertexDeclaration *declaration)
{
    pd->state.vertex_declaration = declaration;
    if (publishes_draw(pd))
        publish_declaration(pd);
}

bool apply_stream(s_port_device *pd, DWORD index, D3DVertexBuffer *buffer, UINT offset, UINT stride)
{
    s_stream_binding &binding = pd->state.streams[index];
    const bool changed = binding.buffer != buffer || binding.offset != offset || binding.stride != stride;
    if (changed)
        binding = { buffer, offset, stride };
    const bool packet_changed = publishes_draw(pd) && publish_stream(pd, index);
    return changed || packet_changed;
}

void apply_indices(s_port_device *pd, D3DIndexBuffer *buffer)
{
    pd->state.index_buffer = buffer;
    if (!publishes_draw(pd))
        return;
    s_port_vk_indices indices = {};
    if (buffer) {
        indices.data = (const void *)(size_t)buffer->Address;
        indices.key = ((buffer->Common & D3DCOMMON_D3DCREATED) || port_vk_buffer_was_mapped(buffer, indices.data)) ? 0 : buffer;
        indices.size = buffer->Size;
        indices.index_size = (buffer->Common & D3DINDEXBUFFER_INDEX32) ? 4u : 2u;
        indices.big_endian = xenos_order(buffer);
    }
    if (__builtin_memcmp(&pd->draw.indices, &indices, sizeof(indices)) != 0) {
        pd->draw.indices = indices;
        pd->dirty_mask |= PORT_VK_DIRTY_IB;
    }
}

bool apply_viewport(s_port_device *pd, const float viewport[6])
{
    if (__builtin_memcmp(pd->state.viewport, viewport, sizeof(pd->state.viewport)) == 0)
        return false;
    __builtin_memcpy(pd->state.viewport, viewport, sizeof(pd->state.viewport));
    publish_window(pd);
    return true;
}

bool apply_scissor(s_port_device *pd, const RECT &rect)
{
    RECT &old = pd->state.scissor;
    if (old.left == rect.left && old.top == rect.top && old.right == rect.right && old.bottom == rect.bottom)
        return false;
    old = rect;
    publish_window(pd);
    return true;
}

void dirty_sampler(s_port_device *pd, DWORD slot, uint64_t bits)
{
    pd->dirty_mask |= bits;
    next_gen(pd->draw.sampler_gen[slot]);
    next_gen(pd->draw.samplers_any_gen);
}

void publish_texture(s_port_device *pd, DWORD slot)
{
    if (slot >= PORT_VK_SAMPLER_COUNT)
        return;
    const s_device_state &st = pd->state;
    D3DBaseTexture *texture = st.textures[slot];
    s_texture_header &header = g_texture_headers[slot];
    s_port_vk_sampler &s = pd->draw.samplers[slot];
    if (texture && texture_header_unchanged(header, texture))
        return;
    s_port_vk_texture info = {};
    bool bound = false;
    header.texture = texture;
    if (texture) {
        header.common = texture->Common;
        header.identifier = texture->Identifier;
        __builtin_memcpy(header.format, texture->Format.dword, sizeof(header.format));
        bound = texture_info(texture, &info);
    } else if (st.unbound_valid[slot]) {
        bound = true;
        info = st.unbound[slot];
    }
    if (s.bound != bound || __builtin_memcmp(&s.texture, &info, sizeof(info)) != 0) {
        s.bound = bound;
        s.texture = info;
        dirty_sampler(pd, slot, PORT_VK_DIRTY_TEXTURE(slot));
    }
}

void publish_sampler(s_port_device *pd, DWORD slot)
{
    if (slot >= PORT_VK_SAMPLER_COUNT)
        return;
    s_port_vk_sampler_state state;
    sampler_state_for_host(pd, slot, &state);
    if (__builtin_memcmp(&pd->draw.samplers[slot].state, &state, sizeof(state)) != 0) {
        pd->draw.samplers[slot].state = state;
        dirty_sampler(pd, slot, PORT_VK_DIRTY_SAMPLER(slot));
    }
}

void apply_texture(s_port_device *pd, DWORD slot, D3DBaseTexture *texture)
{
    s_device_state &st = pd->state;
    if (texture)
        st.unbound_valid[slot] = false;
    else if (st.textures[slot] && gpu())
        st.unbound_valid[slot] = texture_info(st.textures[slot], &st.unbound[slot]);
    st.textures[slot] = texture;
    if (publishes_draw(pd))
        publish_texture(pd, slot);
}

// Restores and resets publish all restored shadow groups once, outside the draw boundary.
void publish_draw(s_port_device *pd)
{
    if (!publishes_draw(pd))
        return;
    if (pd->draw.vertex_shader != pd->state.vertex_shader)
        apply_vertex_shader(pd, pd->state.vertex_shader);
    else
        publish_declaration(pd);
    apply_pixel_shader(pd, pd->state.pixel_shader);
    for (DWORD i = 0; i < (DWORD)k_render_state_count; i++)
        if (i != D3DRS_SCISSORTESTENABLE / 4 && i != D3DRS_HALFPIXELOFFSET / 4)
            publish_render_state(pd, i, pd->state.render_states[i]);
    if (__builtin_memcmp(pd->draw.clip_planes, pd->device->m_ClipPlanes, sizeof(pd->draw.clip_planes)) != 0) {
        __builtin_memcpy(pd->draw.clip_planes, pd->device->m_ClipPlanes, sizeof(pd->draw.clip_planes));
        pd->dirty_mask |= PORT_VK_DIRTY_RAST;
    }
    publish_window(pd);
    for (DWORD i = 0; i < PORT_VK_MAX_STREAMS; i++)
        publish_stream(pd, i);
    apply_indices(pd, pd->state.index_buffer);
    for (DWORD i = 0; i < PORT_VK_SAMPLER_COUNT; i++) {
        publish_texture(pd, i);
        publish_sampler(pd, i);
    }
    change_float_chunks(pd, false, ~0ull);
    change_float_chunks(pd, true, ~0ull);
    change_flow(pd);
    pd->dirty_mask |= PORT_VK_DIRTY_RT;
}

// Resource placement and streaming notifications may come from the pager thread. Only inspect bound headers on the
// render thread, and only after a notification. Same-pointer SetTexture also checks the header directly.
void refresh_placed_resources(s_port_device *pd)
{
    const uint32_t epoch = __atomic_load_n(&g_port_vk_resource_touch_epoch, __ATOMIC_ACQUIRE);
    if (pd->resource_touch_epoch == epoch)
        return;
    if (epoch < pd->resource_touch_epoch)
        port_vk_invalidate_draw_caches();
    pd->resource_touch_epoch = epoch;
    for (DWORD i = 0; i < PORT_VK_SAMPLER_COUNT; i++)
        if (pd->state.textures[i])
            publish_texture(pd, i);
    if (pd->state.index_buffer)
        apply_indices(pd, pd->state.index_buffer);
    for (uint32_t mask = pd->draw.stream_mask; mask != 0; mask &= mask - 1u)
        publish_stream(pd, (DWORD)__builtin_ctz(mask));
}

uint64_t take_draw_dirty(s_port_device *pd)
{
    D3DTAGCOLLECTION &pending = pd->device->m_Pending;
    const uint32_t fetch_mask = 0xffffffffu << (32u - PORT_VK_SAMPLER_COUNT);
    uint32_t fetches = (uint32_t)pending.m_Mask[3] & fetch_mask;
    if (fetches != 0) {
        pending.m_Mask[3] &= ~(UINT64)fetches;
        while (fetches) {
            const DWORD i = (DWORD)__builtin_clz(fetches);
            fetches &= ~(0x80000000u >> i);
            publish_texture(pd, i);
            publish_sampler(pd, i);
        }
    }
    for (DWORD stage = 0; stage < 2; stage++) {
        const UINT64 mask = pending.m_Mask[stage];
        if (mask != 0) {
            pending.m_Mask[stage] = 0;
            change_float_chunks(pd, stage != 0, mask);
        }
    }
    if (pending.m_Mask[4] & tag_bit(k_tag_flow)) {
        pending.m_Mask[4] &= ~tag_bit(k_tag_flow);
        change_flow(pd);
    }
    const uint64_t dirty = pd->dirty_mask;
    pd->dirty_mask = 0;
    return dirty;
}

// Shaders: the microcode a D3D shader object runs, registered with the renderer by hash.
const DWORD k_container_magic = 0x102a1100u;

void register_shader(const void *object, uint32_t stage, const DWORD *header, const void *physical, bool host)
{
    const auto word = [&](DWORD i) { return host ? header[i] : swap32(header[i]); };
    const DWORD size = word(2);
    port_vk_shader_register(object, stage, physical, size, host);
#ifdef HALO3_64BIT
    if (stage == PORT_VK_STAGE_PIXEL && physical && word(1) >= 0x24) {
        const DWORD record = word(6);
        if (!(record & 3) && record >= 0x24 && record <= word(1) - 8) {
            const DWORD offset = word(record / 4), bytes = word(record / 4 + 1);
            if (!(offset & 3) && bytes && !(bytes % 12) && offset <= size && bytes <= size - offset) {
                const DWORD defs = word(5);
                const bool dynamic_loops = !defs || (!(defs & 3) && defs <= word(1) - 20 && !word(defs / 4 + 2));
                port_vk_shell_analyze_shader(object, (const uint32_t *)physical + offset / 4, bytes / 4, host, dynamic_loops);
            }
        }
    }
#endif
}

// A CreateVertexShader / CreatePixelShader function: the microcode container (magic, virtual size, physical size,
// ...) followed by the physical part, as 32-bit words in host order (the game's own arrays) or as the big-endian
// bytes of a cache file.
void register_function(const void *object, uint32_t stage, const DWORD *function)
{
    if (!gpu() || !function)
        return;
    const bool host = (function[0] & 0xffffff00u) == k_container_magic;
    const bool big = (swap32(function[0]) & 0xffffff00u) == k_container_magic;
    if (!host && !big)
        return;
    const DWORD virtual_size = host ? function[1] : swap32(function[1]);
    register_shader(object, stage, function, (const char *)function + virtual_size, host);
}

// A shader object in tag data (XGRegister*Shader): the cache file keeps it in the Xbox's byte order
// (port_cache_swap_tags leaves D3D shader objects alone), with the microcode container right after the object
// (+0x28 for a pixel shader, +0x368 for a vertex shader) and the physical microcode at the address in Identifier.
void register_tag_object(const void *object, uint32_t stage)
{
    if (!gpu() || !object)
        return;
    const DWORD container = stage == PORT_VK_STAGE_PIXEL ? 0x28u : 0x368u;
    const DWORD *words = (const DWORD *)((const char *)object + container);
    const DWORD identifier = ((const D3DResource *)object)->Identifier;
    if ((words[0] & 0xffffff00u) == k_container_magic)
        register_shader(object, stage, words, (const void *)(size_t)identifier, true);
    else if ((swap32(words[0]) & 0xffffff00u) == k_container_magic)
        register_shader(object, stage, words, (const void *)(size_t)swap32(identifier), false);
}

void forward_render_target(DWORD index, D3DSurface *surface)
{
    if (!surface) {
        port_vk_set_render_target(index, 0);
        return;
    }
    s_port_vk_surface info;
    surface_info(surface, 0, &info);
    port_vk_set_render_target(index, &info);
}

void forward_depth_stencil(D3DSurface *surface)
{
    s_port_vk_surface info;
    if (surface)
        surface_info(surface, 1, &info);
    port_vk_set_depth_stencil(surface ? &info : 0);
}

void forward_surfaces(s_port_device *pd)
{
    if (!live(pd))
        return;
    for (DWORD i = 0; i < 4; i++)
        forward_render_target(i, pd->state.render_targets[i]);
    forward_depth_stencil(pd->state.depth_stencil);
}

// ---- command streams ----

enum e_op
{
    k_op_nop = 0,
    k_op_render_state,          // index (D3DRS_* / 4), value
    k_op_sampler,               // sampler, texture, fetch constant [6], k_row_states values
    k_op_constants_f,           // stage (0 vertex, 1 pixel), first register, count, count float4
    k_op_flow,                  // VertexShaderB, PixelShaderB, VertexShaderI, PixelShaderI (k_flow_dwords)
    k_op_gpu_constants_f,       // stage, first register, count, count float4 (16-byte aligned): GpuBegin/GpuLoad
    k_op_vertex_shader,         // shader
    k_op_pixel_shader,          // shader
    k_op_declaration,           // declaration
    k_op_stream,                // stream, buffer, offset, stride
    k_op_indices,               // buffer
    k_op_viewport,              // x, y, width, height, min z, max z (float bits)
    k_op_scissor,               // left, top, right, bottom
    k_op_render_target,         // index, surface
    k_op_depth_stencil,         // surface
    k_op_blend_state,           // render target, RB_BLENDCONTROL
    k_op_clip_plane,            // index, plane [4] (float bits)
    k_op_predication,           // mask
    k_op_unset_all,
    k_op_draw,                  // primitive, start, count, base vertex, indexed
    k_op_draw_up,               // primitive, vertex count, stride, bytes, vertex data
    k_op_draw_indexed_up,       // primitive, min index, vertex count, index count, index format, stride,
                                // vertex bytes, index bytes, vertex data, index data
    k_op_clear,                 // flags, color, z, stencil, rect count, rects [4 each]
    k_op_clearf,                // flags, has rect, rect [4], color [4], z, stencil
    k_op_resolve,               // flags, has rect, rect [4], texture, has point, x, y, level, slice, has color,
                                // color [4], z, stencil
    k_op_run_command_buffer,    // command buffer, predication select
    k_op_query,                 // query, D3DISSUE_* flags
    k_op_tiling_surfaces,       // render targets [4], depth/stencil: SetSurfaces' tiling registration (in a bracket)
    k_op_count
};

const DWORD k_payload_magic = 0x48334342u;   // 'H3CB'
const DWORD k_payload_version = 1;
const DWORD k_payload_header_dwords = 4;     // magic, version, op dwords, reserved

struct s_port_command_buffer;

// A command stream being recorded: the payload header then the ops (16-byte aligned, so GPU constant data inside it
// is aligned for the game's vector writes).
struct s_recording
{
    DWORD *ops;
    DWORD count;                // dwords, the payload header included
    DWORD capacity;
    void *allocation;
    bool failed;                // out of memory: the stream is cut short (reported)
    s_port_command_buffer *owner;
    // BeginCommandBuffer's inherit tags: non-inherited state the stream must set before its first draw
    bool force_vertex_shader, force_pixel_shader;
    DWORD force_blend;          // bit i: RB_BLENDCONTROLi
};

bool recording_reserve(s_recording *r, DWORD dwords)
{
    if (r->failed)
        return false;
    if (r->count + dwords <= r->capacity)
        return true;
    DWORD capacity = r->capacity ? r->capacity : 1024u;
    while (capacity < r->count + dwords)
        capacity *= 2;
    void *allocation = RtlAllocateHeap(GetProcessHeap(), 0, capacity * 4u + 16u);
    if (!allocation) {
        r->failed = true;
        report_once(0, "out of memory recording a command stream (the stream is cut short)");
        return false;
    }
    DWORD *ops = (DWORD *)(((size_t)allocation + 15u) & ~(size_t)15u);
    if (r->ops)
        memcpy(ops, r->ops, r->count * 4u);
    if (r->allocation)
        RtlFreeHeap(GetProcessHeap(), 0, r->allocation);
    r->allocation = allocation;
    r->ops = ops;
    r->capacity = capacity;
    return true;
}

void recording_reset(s_recording *r)
{
    r->failed = false;
    r->count = 0;
    if (recording_reserve(r, k_payload_header_dwords)) {
        memset(r->ops, 0, k_payload_header_dwords * 4u);
        r->count = k_payload_header_dwords;
    }
    r->force_vertex_shader = r->force_pixel_shader = false;
    r->force_blend = 0;
}

// Appends an op with `args` argument dwords; returns them (valid until the next append), or NULL.
DWORD *record(s_recording *r, DWORD op, DWORD args)
{
    if (!recording_reserve(r, 1u + args))
        return 0;
    DWORD *out = r->ops + r->count;
    out[0] = op | (1u + args) << 8;
    r->count += 1u + args;
    return out + 1;
}

// As record, with the last `data` dwords 16-byte aligned (NOP padding before the op).
DWORD *record_aligned(s_recording *r, DWORD op, DWORD head, DWORD data)
{
    while (((r->count + 1u + head) & 3u) != 0) {
        if (!record(r, k_op_nop, 0))
            return 0;
    }
    return record(r, op, head + data);
}

void finish_payload(s_recording *r)
{
    if (r->count < k_payload_header_dwords)
        return;
    r->ops[0] = k_payload_magic;
    r->ops[1] = k_payload_version;
    r->ops[2] = r->count - k_payload_header_dwords;
    r->ops[3] = 0;
}

DWORD payload_bytes(const s_recording *r) { return (r->count * 4u + 31u) & ~31u; }

void record_sampler(s_port_device *pd, DWORD sampler)
{
    DWORD *a = record(pd->recording, k_op_sampler, 2u + 6u + (DWORD)k_row_state_count);
    if (!a)
        return;
    a[0] = sampler;
    a[1] = (DWORD)(size_t)pd->state.textures[sampler];
    __builtin_memcpy(a + 2, fetch_of(pd, sampler), 24);
    for (int i = 0; i < k_row_state_count; i++)
        a[8 + i] = pd->state.sampler_states[sampler][k_row_states[i] / 4];
}

// Writes the device's pending register groups into the stream (Xbox D3D's flush of lazy state at a draw): float
// constants by group of four registers (m_Mask[0] vertex, [1] pixel), texture fetch constants (m_Mask[3], with
// the sampler's texture and D3D sampler states), the flow constants (m_Mask[4]), and the state BeginCommandBuffer
// found not inherited. The pending bits written are cleared.
void capture_pending(s_port_device *pd)
{
    s_recording *r = pd->recording;
    D3D::CDevice *d = pd->device;
    if (r->force_vertex_shader) {
        if (DWORD *a = record(r, k_op_vertex_shader, 1))
            a[0] = (DWORD)(size_t)pd->state.vertex_shader;
        r->force_vertex_shader = false;
    }
    if (r->force_pixel_shader) {
        if (DWORD *a = record(r, k_op_pixel_shader, 1))
            a[0] = (DWORD)(size_t)pd->state.pixel_shader;
        r->force_pixel_shader = false;
    }
    for (DWORD i = 0; i < 4; i++) {
        if (r->force_blend & (1u << i)) {
            if (DWORD *a = record(r, k_op_blend_state, 2)) {
                a[0] = i;
                a[1] = pd->state.blend_control[i];
            }
        }
    }
    r->force_blend = 0;
    for (DWORD stage = 0; stage < 2; stage++) {
        const UINT64 mask = d->m_Pending.m_Mask[stage];
        d->m_Pending.m_Mask[stage] = 0;
        const D3DVECTOR4 *f = stage ? d->m_Constants.PixelShaderF : d->m_Constants.VertexShaderF;
        for (DWORD g = 0; g < 64;) {
            if ((mask & tag_bit(g)) == 0) {
                g++;
                continue;
            }
            DWORD e = g;
            while (e < 64 && (mask & tag_bit(e)) != 0)
                e++;
            const DWORD first = 4u * g, count = 4u * (e - g);
            if (DWORD *a = record(r, k_op_constants_f, 3u + 4u * count)) {
                a[0] = stage;
                a[1] = first;
                a[2] = count;
                copy_vectors(a + 3, &f[first], count);
            }
            g = e;
        }
    }
    const UINT64 fetch_pending = d->m_Pending.m_Mask[3];
    for (DWORD fetch = 0; fetch < (DWORD)k_fetch_count; fetch++) {
        const UINT64 bit = tag_bit(k_tag_texture_fetch_first + fetch);
        if (fetch_pending & bit) {
            record_sampler(pd, fetch); // texture fetch constant f is D3D sampler f
            d->m_Pending.m_Mask[3] &= ~bit;
        }
    }
    if (d->m_Pending.m_Mask[4] & tag_bit(k_tag_flow)) {
        d->m_Pending.m_Mask[4] &= ~tag_bit(k_tag_flow);
        if (DWORD *a = record(r, k_op_flow, (DWORD)k_flow_dwords))
            __builtin_memcpy(a, d->m_Constants.VertexShaderB, k_flow_dwords * 4u);
    }
}

// ---- command buffers ----

// The header part of a command buffer: the XDK's D3DCommandBuffer (D3DResource, m_Inherited, m_Persisted), then
// the port's: where the command stream (the physical part) is. A clone's Identifier is free for the game, which
// stores the physical part's address in it (render_method_submit.cpp); the stream is found through `payload`.
const DWORD k_command_buffer_magic = 0x63627566u; // 'cbuf'

struct s_port_command_buffer
{
    D3DCommandBuffer header;
    DWORD magic;
    const DWORD *payload;       // payload header + ops
    DWORD payload_size;         // bytes (32-byte multiple)
    s_recording *recording;     // a created command buffer's stream (NULL in a clone)
    DWORD run_epoch;            // the tiling bracket that runs it (D3DResource_IsBusy until that bracket ends)
};

s_port_command_buffer *port_command_buffer(D3DCommandBuffer *buffer)
{
    s_port_command_buffer *cb = (s_port_command_buffer *)buffer;
    return cb && cb->magic == k_command_buffer_magic ? cb : 0;
}

// ---- predicated tiling ----

struct s_snapshot
{
    s_device_state state;
    D3DConstants constants;
    float clip_planes[D3DMAXUSERCLIPPLANES][4];
};

struct s_tiling
{
    bool active;
    bool direct_single_tile;    // Count == 1 fast path: execute live without recording/snapshotting/replaying
    DWORD epoch;                // counts brackets
    DWORD flags;                // D3DTILING_*
    DWORD count;
    int rects[k_max_tiles][4];  // x1, y1, x2, y2
    bool clear_color_given;
    float clear_color[4];
    float clear_z;
    DWORD clear_stencil;
    s_recording recording;
    s_snapshot begin;           // the device when the bracket began: each tile starts from it
};
s_tiling g_tiling;
s_snapshot g_tiling_end;
#ifdef HALO3_64BIT
struct s_shell_view {
    unsigned role;
    D3DSurface *surface;
    D3DBaseTexture *resolve;
    DWORD base_page;
};
struct s_shell_bracket {
    bool tagged;
    unsigned count;
    s_shell_view views[16];
};
s_shell_bracket g_shell;
#endif

inline bool direct_tile_predicated_out(const s_port_device *pd)
{
    return pd->hal && g_tiling.direct_single_tile &&
           pd->state.predication != 0 && (pd->state.predication & D3DPRED_TILE(0)) == 0;
}

void snapshot_take(s_port_device *pd, s_snapshot *out)
{
    out->state = pd->state;
    memcpy(&out->constants, &pd->device->m_Constants, sizeof(out->constants));
    memcpy(out->clip_planes, pd->device->m_ClipPlanes, sizeof(out->clip_planes));
}

void snapshot_restore(s_port_device *pd, const s_snapshot *in)
{
    pd->state = in->state;
    if (publishes_draw(pd)) pd->dirty_mask = ~0ull;
    memcpy(&pd->device->m_Constants, &in->constants, sizeof(in->constants));
    memcpy(pd->device->m_ClipPlanes, in->clip_planes, sizeof(in->clip_planes));
    for (DWORD i = 0; i < 4; i++)
        write_blend_control(pd, i, in->state.blend_control[i]);
    publish_draw(pd);
    forward_surfaces(pd);
}

// ---- applying state and commands (directly, or when a stream is replayed) ----

void apply_render_target(s_port_device *pd, DWORD index, D3DSurface *surface)
{
    if (index >= 4)
        return;
    if (pd->state.render_targets[index] == surface)
        return;
    pd->state.render_targets[index] = surface;
    if (publishes_draw(pd)) {
        pd->dirty_mask |= PORT_VK_DIRTY_RT;
        publish_window(pd);
    }
    if (live(pd))
        forward_render_target(index, surface);
}

void apply_depth_stencil(s_port_device *pd, D3DSurface *surface)
{
    if (pd->state.depth_stencil == surface)
        return;
    pd->state.depth_stencil = surface;
    if (publishes_draw(pd)) {
        pd->dirty_mask |= PORT_VK_DIRTY_RT;
        publish_window(pd);
    }
    if (live(pd))
        forward_depth_stencil(surface);
}

// The surfaces the tiling bracket renders through its tiles' windows from now on (window_applies).
void apply_tiling_surfaces(s_port_device *pd, D3DSurface *const render_targets[4], D3DSurface *depth_stencil)
{
    for (DWORD i = 0; i < 4; i++)
        pd->state.tiling_render_targets[i] = render_targets[i];
    pd->state.tiling_depth_stencil = depth_stencil;
    publish_window(pd);
}

void apply_clip_plane(s_port_device *pd, DWORD index, const float *plane)
{
    if (index >= D3DMAXUSERCLIPPLANES)
        return;
    __builtin_memcpy(pd->device->m_ClipPlanes[index], plane, 16);
    pd->device->m_Pending.m_Mask[4] |= tag_bit(k_tag_clip_plane_first + index);
    if (publishes_draw(pd)) {
        __builtin_memcpy(pd->draw.clip_planes[index], plane, 16);
        pd->dirty_mask |= PORT_VK_DIRTY_RAST;
    }
}

void apply_unset_all(s_port_device *pd)
{
    s_device_state &st = pd->state;
    for (int i = 0; i < 4; i++)
        st.render_targets[i] = 0;
    st.depth_stencil = 0;
    st.vertex_shader = 0;
    st.pixel_shader = 0;
    st.vertex_declaration = 0;
    st.index_buffer = 0;
    memset(st.streams, 0, sizeof(st.streams));
    memset(st.textures, 0, sizeof(st.textures));
    publish_draw(pd);
    forward_surfaces(pd);
}

void apply_draw(s_port_device *pd, DWORD primitive, DWORD start, DWORD count, INT base_vertex, DWORD indexed)
{
    if (!live(pd) || direct_tile_predicated_out(pd))
        return;
    refresh_placed_resources(pd);
    port_vk_draw(&pd->draw, take_draw_dirty(pd), primitive, start, count, base_vertex, indexed, 0);
}

void apply_draw_up(s_port_device *pd, DWORD primitive, DWORD vertex_count, const void *vertices, DWORD stride)
{
    if (!live(pd) || !vertices || direct_tile_predicated_out(pd))
        return;
    refresh_placed_resources(pd);
    const uint64_t dirty = take_draw_dirty(pd) | PORT_VK_DIRTY_STREAMS;
    const s_port_vk_geometry geometry = { { 0, vertices, vertex_count * stride, 0, stride, 0 }, 0, 0,
                                          pd->draw.stream_mask | 1u, 0 };
    port_vk_draw(&pd->draw, dirty, primitive, 0, vertex_count, 0, 0, &geometry);
}

void apply_draw_indexed_up(s_port_device *pd, DWORD primitive, DWORD vertex_bytes, DWORD index_count,
                          const void *indices, DWORD index_format, const void *vertices, DWORD stride)
{
    if (!live(pd) || !vertices || !indices || direct_tile_predicated_out(pd))
        return;
    refresh_placed_resources(pd);
    const uint64_t dirty = take_draw_dirty(pd) | PORT_VK_DIRTY_STREAMS | PORT_VK_DIRTY_IB;
    const uint32_t index_size = index_format == D3DFMT_INDEX32 ? 4u : 2u;
    const s_port_vk_indices index = { 0, indices, index_count * index_size, index_size, 0, 0 };
    const s_port_vk_geometry geometry = { { 0, vertices, vertex_bytes, 0, stride, 0 }, &index, 0,
                                          pd->draw.stream_mask | 1u, 0 };
    port_vk_draw(&pd->draw, dirty, primitive, 0, index_count, 0, 1, &geometry);
}

// rects: count rectangles (x1, y1, x2, y2), screen coordinates; count 0 clears the whole surface (the window).
void apply_clear(s_port_device *pd, DWORD flags, const float color[4], float z, DWORD stencil, const int *rects,
                 DWORD count)
{
    if (!live(pd) || direct_tile_predicated_out(pd))
        return;
    // D3D9 / Xbox 360 D3D::ClearSurface clips clears to the current viewport, and to the scissor rectangle when
    // D3DRS_SCISSORTESTENABLE is set. Halo 3's cinematic_clear_letterbox_bars relies on this: it sets the viewport
    // and scissor to each letterbox bar and calls Clear(0, NULL, ...) on the shared albedo/LDR EDRAM target so that
    // decorators already rendered into the letterboxed scene are preserved.
    const s_device_state &st = pd->state;
    int clip[4] = { 0, 0, 0xffff, 0xffff };
    bool has_clip = false;
    if (st.viewport[2] > 0.0f && st.viewport[3] > 0.0f) {
        clip[0] = (int)st.viewport[0];
        clip[1] = (int)st.viewport[1];
        clip[2] = (int)(st.viewport[0] + st.viewport[2]);
        clip[3] = (int)(st.viewport[1] + st.viewport[3]);
        has_clip = true;
    }
    if (st.render_states[D3DRS_SCISSORTESTENABLE / 4] != 0) {
        if (st.scissor.left > clip[0]) clip[0] = st.scissor.left;
        if (st.scissor.top > clip[1]) clip[1] = st.scissor.top;
        if (st.scissor.right < clip[2]) clip[2] = st.scissor.right;
        if (st.scissor.bottom < clip[3]) clip[3] = st.scissor.bottom;
        has_clip = true;
    }
    if (has_clip && (clip[2] <= clip[0] || clip[3] <= clip[1]))
        return;

    int moved[4 * 16];
    DWORD n = 0;
    if (count != 0) {
        for (DWORD i = 0; i < count && i < 16; i++) {
            int r[4] = { rects[4 * i], rects[4 * i + 1], rects[4 * i + 2], rects[4 * i + 3] };
            if (has_clip) {
                if (clip[0] > r[0]) r[0] = clip[0];
                if (clip[1] > r[1]) r[1] = clip[1];
                if (clip[2] < r[2]) r[2] = clip[2];
                if (clip[3] < r[3]) r[3] = clip[3];
            }
            if (window_rect(pd, r))
                memcpy(&moved[4 * n++], r, sizeof(r));
        }
        if (n == 0)
            return; // every rectangle is outside the viewport, scissor, or tile
    } else if (has_clip) {
        int r[4] = { clip[0], clip[1], clip[2], clip[3] };
        if (!window_rect(pd, r))
            return; // outside the tile
        memcpy(moved, r, sizeof(r));
        n = 1;
    } else if (window_applies(pd)) {
        moved[0] = 0;
        moved[1] = 0;
        moved[2] = g_window.width;
        moved[3] = g_window.height;
        n = 1;
    }
    port_vk_clear(flags, color, z, stencil, n ? moved : 0, n);
}

// The tile a tiling bracket's replay is in (D3DDevice_EndTiling), 0 outside one: a tiled occlusion query adds the
// later tiles' samples to the first's.
DWORD g_tiling_replay_tile;

void apply_query(s_port_device *pd, D3DQuery *query, DWORD flags)
{
    if (!live(pd) || !query || ((s_null_object *)query)->payload[0] != D3DQUERYTYPE_OCCLUSION ||
        direct_tile_predicated_out(pd))
        return;
    if (flags & D3DISSUE_BEGIN)
        port_vk_query_begin(query, g_tiling_replay_tile != 0 ? 1u : 0u);
    else if (flags & D3DISSUE_END)
        port_vk_query_end(query);
}

void apply_resolve(s_port_device *pd, DWORD flags, const int *source_rect, D3DBaseTexture *destination,
                   const int *dest_point, DWORD level, DWORD slice, const float *clear_color, float clear_z,
                   DWORD clear_stencil)
{
    if (!live(pd) || direct_tile_predicated_out(pd))
        return;
    int rect[4] = { 0, 0, 0, 0 };
    int point[2] = { 0, 0 };
    bool has_rect = source_rect != 0;
    if (has_rect)
        memcpy(rect, source_rect, sizeof(rect));
    if (dest_point)
        memcpy(point, dest_point, sizeof(point));
    if (window_applies(pd)) {
        // in a tile the source is a screen rectangle (NULL: the tile); it is read from the window
        if (!has_rect) {
            rect[0] = g_window.x;
            rect[1] = g_window.y;
            rect[2] = g_window.x + g_window.width;
            rect[3] = g_window.y + g_window.height;
            if (!dest_point) {
                point[0] = rect[0];
                point[1] = rect[1];
            }
            has_rect = true;
        }
        const int x1 = rect[0], y1 = rect[1];
        if (!window_rect(pd, rect))
            return;
        point[0] += rect[0] + g_window.x - x1; // what the window clipped off the left / top moves the destination
        point[1] += rect[1] + g_window.y - y1;
    }
    s_port_vk_texture texture;
    const bool has_destination = destination && texture_info(destination, &texture);
    const float color[4] = { clear_color ? clear_color[0] : 0.0f, clear_color ? clear_color[1] : 0.0f,
                             clear_color ? clear_color[2] : 0.0f, clear_color ? clear_color[3] : 0.0f };
    port_vk_resolve(flags, flags & 3u, has_rect ? rect : 0, has_destination ? &texture : 0, (uint32_t)point[0],
                    (uint32_t)point[1], level, slice, color, clear_z, clear_stencil);
}

void execute(s_port_device *pd, const DWORD *ops, DWORD count, DWORD select);

// Runs a command buffer's stream on a device. select: the predication select (tiles) it runs for.
void run_command_buffer(s_port_device *pd, s_port_command_buffer *cb, DWORD select)
{
    const DWORD *payload = cb->payload;
    if (!payload || payload[0] != k_payload_magic || payload[1] != k_payload_version) {
        report_once(1, "RunCommandBuffer of a command buffer without a recorded stream (skipped)");
        return;
    }
    // registers the command buffer does not persist go back to what the device had (the game persists all)
    const D3DTAGCOLLECTION &persist = cb->header.m_Persisted;
    const bool persist_all = (persist.m_Mask[0] & persist.m_Mask[1] & persist.m_Mask[2] & persist.m_Mask[3] &
                              persist.m_Mask[4]) == ~0ull;
    s_snapshot *before = persist_all ? 0 : (s_snapshot *)__builtin_alloca(sizeof(s_snapshot));
    if (before)
        snapshot_take(pd, before);
    const DWORD predication = pd->state.predication;
    pd->state.predication = 0;
    execute(pd, payload + k_payload_header_dwords, payload[2], select);
    pd->state.predication = predication;
    if (before) {
        D3DConstants &c = pd->device->m_Constants;
        D3DVECTOR4 *const dst[] = { c.VertexShaderF, c.PixelShaderF };
        const D3DVECTOR4 *const src[] = { before->constants.VertexShaderF, before->constants.PixelShaderF };
        for (DWORD stage = 0; stage < 2; stage++) {
            UINT64 changed = 0;
            for (UINT64 mask = ~persist.m_Mask[stage]; mask != 0;) {
                const DWORD group = (DWORD)__builtin_clzll(mask);
                const UINT64 bit = tag_bit(group);
                mask &= ~bit;
                const DWORD first = group * 4u;
                if (!equal_vectors(&dst[stage][first], &src[stage][first], 4)) {
                    copy_vectors(&dst[stage][first], &src[stage][first], 4);
                    changed |= bit;
                }
            }
            if (changed)
                change_float_chunks(pd, stage != 0, changed);
        }
        for (DWORD f = 0; f < (DWORD)k_fetch_count; f++) {
            if ((persist.m_Mask[3] & tag_bit(k_tag_texture_fetch_first + f)) == 0) {
                c.TextureFetch[f] = before->constants.TextureFetch[f];
                pd->state.textures[f] = before->state.textures[f];
                pd->state.unbound[f] = before->state.unbound[f];
                pd->state.unbound_valid[f] = before->state.unbound_valid[f];
                memcpy(pd->state.sampler_states[f], before->state.sampler_states[f], sizeof(pd->state.sampler_states[f]));
                publish_texture(pd, f);
                publish_sampler(pd, f);
            }
        }
        if ((persist.m_Mask[4] & tag_bit(k_tag_flow)) == 0 &&
            __builtin_memcmp(c.VertexShaderB, before->constants.VertexShaderB, k_flow_dwords * 4u) != 0) {
            memcpy(c.VertexShaderB, before->constants.VertexShaderB, k_flow_dwords * 4u);
            change_flow(pd);
        }
        if ((persist.m_Mask[2] & tag_bit(k_tag_vertex_shader)) == 0) {
            apply_vertex_shader(pd, before->state.vertex_shader);
        }
        if ((persist.m_Mask[2] & tag_bit(k_tag_pixel_shader)) == 0) {
            apply_pixel_shader(pd, before->state.pixel_shader);
        }
        for (DWORD i = 0; i < 4; i++)
            if ((persist.m_Mask[2] & tag_bit(k_tag_blend_control[i])) == 0)
                write_blend_control(pd, i, before->state.blend_control[i]);
        report_once(2, "a command buffer that persists only part of its state (constants, samplers, flow constants, "
                       "shaders and blending are restored; other state persists)");
    }
}

// Applies one op (its predication already checked). select: the predication select of the replay.
void apply_op(s_port_device *pd, DWORD op, const DWORD *a, DWORD args, DWORD select)
{
    s_device_state &st = pd->state;
    D3D::CDevice *d = pd->device;
    switch (op) {
    case k_op_render_state:
        apply_render_state(pd, a[0], a[1]);
        break;
    case k_op_sampler: {
        const DWORD sampler = a[0];
        if (sampler >= (DWORD)k_fetch_count)
            break;
        apply_texture(pd, sampler, (D3DBaseTexture *)(size_t)a[1]);
        __builtin_memcpy(fetch_of(pd, sampler), a + 2, 24);
        for (int i = 0; i < k_row_state_count; i++)
            st.sampler_states[sampler][k_row_states[i] / 4] = a[8 + i];
        publish_sampler(pd, sampler);
        break;
    }
    case k_op_constants_f:
    case k_op_gpu_constants_f: {
        D3DVECTOR4 *f = a[0] ? d->m_Constants.PixelShaderF : d->m_Constants.VertexShaderF;
        const DWORD first = a[1], count = a[2];
        if (first + count <= 256 && 3u + 4u * count <= args) {
            if (!equal_vectors(&f[first], a + 3, count)) {
                copy_vectors(&f[first], a + 3, count);
                change_float_range(pd, a[0] != 0, first, count);
            }
        }
        break;
    }
    case k_op_flow:
        if (__builtin_memcmp(d->m_Constants.VertexShaderB, a, k_flow_dwords * 4u) != 0) {
            __builtin_memcpy(d->m_Constants.VertexShaderB, a, k_flow_dwords * 4u);
            change_flow(pd);
        }
        break;
    case k_op_vertex_shader: apply_vertex_shader(pd, (D3DVertexShader *)(size_t)a[0]); break;
    case k_op_pixel_shader: apply_pixel_shader(pd, (D3DPixelShader *)(size_t)a[0]); break;
    case k_op_declaration: apply_declaration(pd, (D3DVertexDeclaration *)(size_t)a[0]); break;
    case k_op_stream:
        if (a[0] < (DWORD)k_stream_count)
            apply_stream(pd, a[0], (D3DVertexBuffer *)(size_t)a[1], a[2], a[3]);
        break;
    case k_op_indices: apply_indices(pd, (D3DIndexBuffer *)(size_t)a[0]); break;
    case k_op_viewport: {
        float viewport[6];
        for (int i = 0; i < 6; i++)
            viewport[i] = bits_float(a[i]);
        apply_viewport(pd, viewport);
        break;
    }
    case k_op_scissor: {
        const RECT scissor = { (LONG)a[0], (LONG)a[1], (LONG)a[2], (LONG)a[3] };
        apply_scissor(pd, scissor);
        break;
    }
    case k_op_render_target: apply_render_target(pd, a[0], (D3DSurface *)(size_t)a[1]); break;
    case k_op_depth_stencil: apply_depth_stencil(pd, (D3DSurface *)(size_t)a[0]); break;
    case k_op_blend_state:
        if (a[0] < 4)
            write_blend_control(pd, a[0], a[1]);
        break;
    case k_op_clip_plane: {
        float plane[4];
        __builtin_memcpy(plane, a + 1, 16);
        apply_clip_plane(pd, a[0], plane);
        break;
    }
    case k_op_unset_all: apply_unset_all(pd); break;
    case k_op_draw: apply_draw(pd, a[0], a[1], a[2], (INT)a[3], a[4]); break;
    case k_op_draw_up: apply_draw_up(pd, a[0], a[1], a + 4, a[2]); break;
    case k_op_draw_indexed_up: {
        const DWORD vertex_dwords = (a[6] + 3u) / 4u;
        apply_draw_indexed_up(pd, a[0], a[6], a[3], a + 8 + vertex_dwords, a[4], a + 8, a[5]);
        break;
    }
    case k_op_clear: {
        float color[4];
        const DWORD c = a[1];
        color[0] = ((c >> 16) & 0xff) / 255.0f;
        color[1] = ((c >> 8) & 0xff) / 255.0f;
        color[2] = (c & 0xff) / 255.0f;
        color[3] = (c >> 24) / 255.0f;
        apply_clear(pd, a[0], color, bits_float(a[2]), a[3], (const int *)(a + 5), a[4]);
        break;
    }
    case k_op_clearf: {
        float color[4];
        __builtin_memcpy(color, a + 6, 16);
        apply_clear(pd, a[0], color, bits_float(a[10]), a[11], (const int *)(a + 2), a[1] ? 1u : 0u);
        break;
    }
    case k_op_resolve: {
        float color[4];
        __builtin_memcpy(color, a + 12, 16);
        apply_resolve(pd, a[0], a[1] ? (const int *)(a + 2) : 0, (D3DBaseTexture *)(size_t)a[6],
                      a[7] ? (const int *)(a + 8) : 0, a[10], a[11], color, bits_float(a[16]), a[17]);
        break;
    }
    case k_op_query: apply_query(pd, (D3DQuery *)(size_t)a[0], a[1]); break;
    case k_op_tiling_surfaces: {
        D3DSurface *render_targets[4];
        for (DWORD i = 0; i < 4; i++)
            render_targets[i] = (D3DSurface *)(size_t)a[i];
        apply_tiling_surfaces(pd, render_targets, (D3DSurface *)(size_t)a[4]);
        break;
    }
    case k_op_run_command_buffer: {
        s_port_command_buffer *cb = port_command_buffer((D3DCommandBuffer *)(size_t)a[0]);
        if (cb)
            run_command_buffer(pd, cb, select & a[1]);
        break;
    }
    default:
        break;
    }
}

void execute(s_port_device *pd, const DWORD *ops, DWORD count, DWORD select)
{
    for (DWORD i = 0; i < count;) {
        const DWORD op = ops[i] & 0xffu, length = ops[i] >> 8;
        if (length == 0 || i + length > count || op >= (DWORD)k_op_count) {
            report_once(3, "a malformed command stream (replay stopped)");
            return;
        }
        if (op == k_op_predication)
            pd->state.predication = ops[i + 1];
        else if (op != k_op_nop && (pd->state.predication == 0 || (pd->state.predication & select) != 0))
            apply_op(pd, op, ops + i + 1, length - 1u, select);
        i += length;
    }
}

#ifdef HALO3_64BIT
struct s_shell_plan {
    s_port_vk_surface targets[16];
    unsigned target_count, draws, resolves;
    int width, height, band_height;
    D3DSurface *bound[5], *registered[5], *setup_color;
    D3DVertexDeclaration *declaration;
    const void *vs, *ps;
    DWORD sampler_pages[k_fetch_count], point_samples;
    uint32_t flow[k_flow_dwords], floats[2][256][4], different[2][8], different_samplers, frame_samplers;
    bool different_flow;
    float viewport[4], screen_texel[4];
    RECT scissor;
    bool scissor_on, stencil_on, origin, shadow_pending, shadow_ready;
    unsigned first_shadow_size, shadow_size, resolved_roles, cleared_roles, op;
    bool cleared_depth;
    DWORD resolve_pages[3];
};

const s_shell_view *shell_view(D3DSurface *surface)
{
    for (unsigned i = 0; surface && i < g_shell.count; ++i)
        if (surface == g_shell.views[i].surface)
            return &g_shell.views[i];
    return 0;
}

// Only attachments actually accessed are enlarged. SetRenderTarget also records transient bindings on its
// way to a registered MRT set. The independent caster depth must be bound alone, outside the tile window.
bool shell_attachments(s_shell_plan &p, unsigned roles[5])
{
    for (unsigned i = 0; i < 5; ++i) {
        D3DSurface *s = p.bound[i];
        if (!s)
            continue;
        const s_shell_view *v = shell_view(s);
        if (!v || s->Common & D3DCOMMON_SURFACE_FROM_TEXTURE)
            return false;
        roles[i] = v->role;
        if (v->role == 2) {
            if (i != 4 || p.registered[i] == s || roles[0] || roles[1] || roles[2] || roles[3])
                return false;
            continue;
        }
        if (s != p.registered[i] || (i == 4) != (v->role == 3) ||
            s->SurfaceInfo.MsaaSamples != D3DMULTISAMPLE_4_SAMPLES ||
            s->SurfaceInfo.SurfacePitch != (unsigned)(p.width + 39) / 40 * 40 ||
            s->Height != p.band_height || (s->Width != p.width &&
            !(v->role == 8 && s->Width == (p.width + 39) / 40 * 40)))
            return false;
        if (v->role == 8) {
            // The setup quad writes only the first strip of this independent color view of depth.
            if (s->ColorInfo.ColorBase != g_tiling.begin.state.depth_stencil->DepthInfo.DepthBase ||
                i || p.bound[1] || p.bound[2] || p.bound[3] || p.bound[4]) return false;
            p.setup_color = s;
        }
        for (unsigned j = 0; j < i && i < 4; ++j)
            if (p.bound[j] && p.bound[j] != s && p.bound[j]->ColorInfo.ColorBase == s->ColorInfo.ColorBase)
                return false;
        unsigned n = 0;
        while (n < p.target_count && p.targets[n].key != s)
            ++n;
        if (n == p.target_count)
            surface_info(s, i == 4, &p.targets[p.target_count++]);
    }
    return roles[0] || roles[1] || roles[2] || roles[3] || roles[4];
}

// The shell setup uses pixel_copy's screen-space rectangle (x,y,1,1). Its bottom edge is above the
// next strip's first sample, including the D3D half-pixel viewport shift. Check the actual UP payload
// and POSITION0 declaration before allowing the shader's strip-local VPOS.
bool shell_setup_quad(const s_shell_plan &p, DWORD op, const DWORD *a)
{
    if (p.draws || op != k_op_draw_up || a[0] != D3DPT_RECTLIST || a[1] != 3 || a[2] != 20 || a[3] != 60 ||
        p.viewport[1] != 0 || p.viewport[3] != p.band_height || a[5] != a[10] ||
        !(bits_float(a[5]) >= 1 && bits_float(a[15]) > -1 && bits_float(a[15]) < 0))
        return false;
    D3DVertexDeclaration *decl = p.declaration;
    if (!decl) {
        const auto *b = vertex_shader_binding((D3DVertexShader *)p.vs);
        if (b) decl = b->declaration;
    }
    if (!decl) return false;
    const auto *object = (const s_null_object *)decl;
    const auto *e = (const D3DVERTEXELEMENT9 *)(size_t)object->payload[0];
    for (unsigned i = 0; e && i < object->payload[1]; ++i)
        if (e[i].Usage == D3DDECLUSAGE_POSITION && !e[i].UsageIndex)
            return !e[i].Stream && !e[i].Offset && e[i].Type == D3DDECLTYPE_FLOAT2;
    return false;
}

bool shell_draw(s_shell_plan &p, const unsigned roles[5])
{
    const bool shadow = roles[4] == 2;
    const bool first_band = roles[0] == 8;
    if (shadow && (!p.shadow_pending || p.stencil_on || p.viewport[0] < 0 || p.viewport[1] < 0 ||
        p.viewport[0] + p.viewport[2] > p.shadow_size || p.viewport[1] + p.viewport[3] > p.shadow_size))
        return false;
    if (p.different_flow) return false;
    for (const auto &stage : p.different)
        for (auto mask : stage)
            if (mask) return false;
    uint32_t scene = 0, forbidden = p.different_samplers;
    for (unsigned s = 0; s < k_fetch_count; ++s) {
        if (!p.sampler_pages[s])
            continue;
        for (unsigned i = 0; i < g_shell.count; ++i) {
            const s_shell_view &v = g_shell.views[i];
            if (v.base_page != p.sampler_pages[s])
                continue;
            if (v.role == 2) {
                if (!p.shadow_ready || shadow)
                    forbidden |= 1u << s;
            } else {
                scene |= 1u << s;
                // Accumulators reuse albedo/normal memory. Only the completed G-buffer snapshots may be
                // read inside this bracket, and each read must stay within the fragment's own band.
                const unsigned producer = v.role == 3 || v.role == 8 ? 3 :
                                          v.role == 14 || v.role == 19 ? 19 : 17;
                if (shadow || !(p.frame_samplers & (1u << s)) || !(p.resolved_roles & (1u << producer)) ||
                    (p.resolved_roles & ((1u << 13) | (1u << 14))))
                    forbidden |= 1u << s;
            }
        }
    }
    if (p.viewport[2] <= 0 || p.viewport[3] <= 0)
        return false; // an implicit attachment-sized viewport would change when enlarged
    ++p.draws;
    return port_vk_shell_draw(p.vs, p.ps, scene, forbidden, p.point_samples, p.origin, shadow,
                              p.screen_texel, p.width, p.height, p.flow + k_flow_dwords - 32, first_band) != 0;
}

struct s_shell_cursor {
    const DWORD *words;
    DWORD left;
    bool take(DWORD &op, const DWORD *&a, DWORD &args) {
        if (!left)
            return false;
        op = words[0] & 255;
        const DWORD n = words[0] >> 8;
        if (!n || n > left || op >= k_op_count)
            return false;
        a = words + 1;
        args = n - 1;
        words += n;
        left -= n;
        return true;
    }
    bool expect(DWORD op, DWORD args, const DWORD *&a) {
        DWORD next, size;
        do {
            if (!take(next, a, size))
                return false;
        } while (next == k_op_nop && !size);
        return next == op && size == args;
    }
};

bool shell_frame_texture(const GPUTEXTURE_FETCH_CONSTANT &f, const s_shell_plan &p)
{
    return f.Dimension == GPUDIMENSION_2D && !f.Stacked && !f.MaxMipLevel &&
           f.Size.TwoD.Width + 1u == (unsigned)p.width && f.Size.TwoD.Height + 1u == (unsigned)p.height;
}

// Resolve flushes pending constants and samplers under the first tile predicate. A differing value can
// survive across the copies, but must be overwritten (or an unused sampler) before a shared draw.
bool shell_registers(s_shell_plan &p, DWORD op, const DWORD *a, DWORD n, bool first_tile_only)
{
    if (op == k_op_sampler) {
        if (n != 8 + k_row_state_count || a[0] >= k_fetch_count) return false;
        const auto *tex = (const D3DBaseTexture *)(size_t)a[1];
        const DWORD bit = 1u << a[0];
        if (tex) {
            p.sampler_pages[a[0]] = tex->Format.BaseAddress;
            p.frame_samplers = (p.frame_samplers & ~bit) | (shell_frame_texture(tex->Format, p) ? bit : 0u);
            if (!first_tile_only) p.different_samplers &= ~bit;
        }
        if (first_tile_only) p.different_samplers |= bit;
        p.point_samples = (p.point_samples & ~bit) | (!a[8] && !a[9] ? bit : 0u);
        return true;
    }
    if (op == k_op_flow) {
        if (n != k_flow_dwords) return false;
        p.different_flow = first_tile_only && (p.different_flow || memcmp(p.flow, a, sizeof(p.flow)) != 0);
        memcpy(p.flow, a, sizeof(p.flow));
        return true;
    }
    if ((op != k_op_constants_f && op != k_op_gpu_constants_f) || n < 3 || a[0] > 1 ||
        a[1] > 256 || a[2] > 256 - a[1] || n != 3 + 4 * a[2]) return false;
    for (unsigned i = 0; i < a[2]; ++i) {
        const unsigned reg = a[1] + i;
        const uint32_t bit = 1u << (reg % 32);
        auto &diff = p.different[a[0]][reg / 32];
        auto &value = p.floats[a[0]][reg];
        const DWORD *next = a + 3 + 4 * i;
        if (!first_tile_only) diff &= ~bit;
        else if (memcmp(value, next, sizeof(value))) diff |= bit;
        memcpy(value, next, sizeof(value));
    }
    if (a[0] && a[1] <= 108 && a[1] + a[2] > 108 && !first_tile_only) p.origin = false;
    if (a[0] && a[1] <= 7 && a[1] + a[2] > 7)
        memcpy(p.screen_texel, p.floats[1][7], 16);
    return true;
}

// Match precisely the two four-way predicated sequences emitted by begin_tiling and resolve_surface_tiled.
// Nothing can interleave the four copies. Resolve flags, texture, level, slice and clear values agree.
bool shell_group(s_shell_cursor &c, s_shell_plan &p, bool constants)
{
    const DWORD *first = 0, *a;
    for (unsigned t = 0; t < 4; ++t) {
        if (t && (!c.expect(k_op_predication, 1, a) || a[0] != D3DPRED_TILE(t)))
            return false;
        if (constants) {
            if (!c.expect(k_op_gpu_constants_f, 19, a) || a[0] != 1 || a[1] != 108 || a[2] != 4 ||
                a[3] || a[4] != float_bits((float)g_tiling.rects[t][1]) || a[5] || a[6] ||
                a[7] != float_bits(1.f / p.width) || a[8] != float_bits(1.f / p.band_height) ||
                a[9] != float_bits(0.5f / p.width) || a[10] != float_bits(0.5f / p.band_height))
                return false;
            if (!t) memcpy(p.floats[1][108], a + 3, 64);
            for (unsigned i = 11; i < 19; ++i)
                if (a[i])
                    return false;
        } else {
            while (!t && c.left && (c.words[0] & 255) != k_op_resolve) {
                DWORD op, n;
                if (!c.take(op, a, n) || ((op != k_op_nop || n) && !shell_registers(p, op, a, n, true)))
                    return false;
            }
            if (!c.expect(k_op_resolve, 18, a) || !a[1] || !a[7] || !a[6] || a[10] || a[11] ||
                (a[0] & ~0xfc000077u) || memcmp(a + 2, g_tiling.rects[t], 16) ||
                a[8] != a[2] || a[9] != a[3] ||
                (first && (a[0] != first[0] || a[6] != first[6] || memcmp(a + 10, first + 10, 32))))
                return false;
            if (!first) {
                unsigned roles[5] = {};
                if (!shell_attachments(p, roles))
                    return false;
                const unsigned slot = a[0] & 7;
                const s_shell_view *v = slot < 5 ? shell_view(p.bound[slot]) : 0;
                if (!v || (v->role != 3 && v->role != 17 && v->role != 19 && v->role != 13 && v->role != 14) ||
                    !v->base_page)
                    return false;
                if ((v->role == 14 || v->role == 19) && !(p.cleared_roles & ((1u << 14) | (1u << 19))))
                    return false;
                bool destination = false;
                for (unsigned i = 0; i < g_shell.count; ++i)
                    destination |= g_shell.views[i].role == v->role && (size_t)g_shell.views[i].resolve == a[6];
                if (!destination || !shell_frame_texture(((D3DBaseTexture *)(size_t)a[6])->Format, p))
                    return false;
                const DWORD page = ((D3DBaseTexture *)(size_t)a[6])->Format.BaseAddress;
                const unsigned family = v->role == 3 ? 0 : (v->role == 19 || v->role == 14) ? 2 : 1;
                for (unsigned i = 0; i < 3; ++i)
                    if (p.resolve_pages[i] && (p.resolve_pages[i] == page) != (i == family)) return false;
                for (unsigned i = 0; i < g_shell.count; ++i)
                    if (g_shell.views[i].role == 2 && g_shell.views[i].base_page == page) return false;
                p.resolve_pages[family] = page;
                p.resolved_roles |= 1u << v->role;
                ++p.resolves;
            }
        }
        first = a;
    }
    return c.expect(k_op_predication, 1, a) && !a[0];
}

bool shell_stream(s_shell_cursor c, s_shell_plan &p, bool child)
{
    while (c.left) {
        DWORD op, n;
        const DWORD *a;
        if (!c.take(op, a, n))
            return false;
        p.op = op;
        switch (op) {
        case k_op_nop: if (n) return false; break;
        case k_op_predication:
            if (child || n != 1 || a[0] != D3DPRED_TILE(0) || !shell_group(c, p, false))
                return false;
            break;
        case k_op_run_command_buffer: {
            if (child || n != 2)
                return false;
            auto *cb = port_command_buffer((D3DCommandBuffer *)(size_t)a[0]);
            if (!cb || !cb->payload || cb->payload_size < 16 || cb->payload[0] != k_payload_magic ||
                cb->payload[1] != k_payload_version || cb->payload[2] > cb->payload_size / 4 - 4)
                return false;
            for (int i = 0; i < 5; ++i)
                if (cb->header.m_Persisted.m_Mask[i] != ~0ull)
                    return false;
            if (!shell_stream({cb->payload + 4, cb->payload[2]}, p, true))
                return false;
            break;
        }
        case k_op_tiling_surfaces:
            if (child || n != 5) return false;
            for (int i = 0; i < 5; ++i)
                p.registered[i] = (D3DSurface *)(size_t)a[i];
            break;
        case k_op_render_target:
            if (n != 2 || a[0] >= 4) return false;
            p.bound[a[0]] = (D3DSurface *)(size_t)a[1];
            break;
        case k_op_depth_stencil:
            if (n != 1) return false;
            p.bound[4] = (D3DSurface *)(size_t)a[0];
            break;
        case k_op_vertex_shader: case k_op_pixel_shader:
            if (n != 1) return false;
            (op == k_op_vertex_shader ? p.vs : p.ps) = (const void *)(size_t)a[0];
            break;
        case k_op_viewport:
            if (n != 6) return false;
            memcpy(p.viewport, a, sizeof(p.viewport));
            break;
        case k_op_scissor:
            if (n != 4) return false;
            memcpy(&p.scissor, a, 16);
            break;
        case k_op_render_state:
            if (n != 2 || a[0] >= k_render_state_count) return false;
            if (a[0] == D3DRS_SCISSORTESTENABLE / 4) p.scissor_on = a[1] != 0;
            if (a[0] == D3DRS_STENCILENABLE / 4) p.stencil_on = a[1] != 0;
            break;
        case k_op_sampler: case k_op_gpu_constants_f: case k_op_constants_f: case k_op_flow:
            if (!shell_registers(p, op, a, n, false)) return false;
            break;
        case k_op_draw: case k_op_draw_up: case k_op_draw_indexed_up:
        case k_op_clear: case k_op_clearf: case k_op_resolve: {
            unsigned roles[5] = {};
            if (!shell_attachments(p, roles))
                return false;
            if (roles[0] == 8 && op != k_op_draw_up) return false;
            if (op == k_op_resolve) {
                const auto *v = shell_view(p.bound[4]);
                if (child || n != 18 || roles[4] != 2 || !p.shadow_pending || a[0] != 0x14 ||
                    !a[1] || a[2] || a[3] || a[4] != p.shadow_size || a[5] != p.shadow_size ||
                    a[6] != (size_t)v->resolve || a[8] || a[9] || a[10] || a[11])
                    return false;
                p.shadow_pending = false;
                p.shadow_ready = true;
            } else if (op == k_op_clear || op == k_op_clearf) {
                if ((op == k_op_clear && (n < 5 || a[4] > 16 || n != 5 + 4 * a[4])) ||
                    (op == k_op_clearf && n != 12))
                    return false;
                const bool whole_view = op == k_op_clear ? !a[4] : !a[1];
                const bool covers = !p.scissor_on || (p.scissor.left <= 0 && p.scissor.top <= 0 &&
                    p.scissor.right >= p.viewport[2] && p.scissor.bottom >= p.viewport[3]);
                if (roles[4] == 2) {
                    if (p.shadow_pending || !whole_view || !covers || !(a[0] & D3DCLEAR_ZBUFFER) ||
                        p.viewport[0] || p.viewport[1] || p.viewport[2] != p.viewport[3] ||
                        !(p.viewport[2] > 0 && p.viewport[2] <= p.bound[4]->Height))
                        return false;
                    p.shadow_size = (unsigned)p.viewport[2];
                    // Texels outside a cropped resolve survive. Replaying the casters cannot change the
                    // previous contents outside the first rectangle if every later resolve fits inside it.
                    if (!p.first_shadow_size) p.first_shadow_size = p.shadow_size;
                    if (p.shadow_size > p.first_shadow_size) return false;
                    p.shadow_pending = true;
                    p.shadow_ready = false;
                } else if (whole_view && covers && !p.viewport[0] && !p.viewport[1] &&
                           p.viewport[2] >= p.width && p.viewport[3] >= p.height) {
                    if (roles[4] == 3 && (a[0] & (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL)) ==
                                            (D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL)) p.cleared_depth = true;
                    for (unsigned i = 0; i < 4; ++i)
                        if ((a[0] & (1u << i)) && roles[i]) p.cleared_roles |= 1u << roles[i];
                }
            } else {
                if ((op == k_op_draw && n != 5) ||
                    (op == k_op_draw_up && (n < 4 || (uint64_t)a[1] * a[2] != a[3] ||
                                            ((uint64_t)a[3] + 3) / 4 != n - 4)) ||
                    (op == k_op_draw_indexed_up && (n < 8 || ((uint64_t)a[1] + a[2]) * a[5] != a[6] ||
                        (uint64_t)a[3] * (a[4] == D3DFMT_INDEX32 ? 4u : 2u) != a[7] || (((uint64_t)a[6] + 3) / 4 +
                        ((uint64_t)a[7] + 3) / 4 != n - 8))))
                    return false;
                if (roles[4] == 3 && !p.cleared_depth) return false;
                if (!(p.cleared_roles & ((1u << 14) | (1u << 19)))) {
                    for (unsigned role : roles)
                        if (role == 14 || role == 19) return false;
                }
                if ((roles[0] == 8 && !shell_setup_quad(p, op, a)) || !shell_draw(p, roles))
                    return false;
            }
            break;
        }
        case k_op_declaration:
            if (n != 1) return false;
            p.declaration = (D3DVertexDeclaration *)(size_t)a[0];
            break;
        case k_op_indices: if (n != 1) return false; break;
        case k_op_blend_state: if (n != 2) return false; break;
        case k_op_stream: if (n != 4) return false; break;
        case k_op_clip_plane: if (n != 5 || a[0] >= D3DMAXUSERCLIPPLANES) return false; break;
        default: return false; // queries, unknown state resets, or a nested predicate
        }
    }
    return true;
}

bool shell_preflight(s_shell_plan &p, const s_recording &r)
{
    if (!port_vk_shell_available() || g_tiling.count != 4 ||
        g_tiling.flags != D3DTILING_SKIP_FIRST_TILE_CLEAR || r.failed || r.count < 4)
        return false;
    p.width = g_tiling.rects[0][2];
    p.band_height = g_tiling.rects[0][3];
    p.height = g_tiling.rects[3][3];
    if (p.width <= 0 || p.width > 8192 || p.band_height <= 0 || p.band_height > 2048 ||
        (p.band_height & 1)) return false; // aligned fragment quads at every derivative seam
    for (int i = 0; i < 4; ++i) {
        const int band[4] = {0, i * p.band_height, p.width, (i + 1) * p.band_height};
        if (memcmp(band, g_tiling.rects[i], sizeof(band))) return false;
    }
    const s_device_state &s = g_tiling.begin.state;
    if (s.predication || !s.render_targets[0] || !s.depth_stencil || s.render_targets[1] ||
        s.render_targets[2] || s.render_targets[3] || !g_tiling.clear_color_given)
        return false;
    memcpy(p.bound, s.render_targets, sizeof(s.render_targets));
    p.bound[4] = s.depth_stencil;
    memcpy(p.registered, p.bound, sizeof(p.bound));
    memcpy(p.flow, g_tiling.begin.constants.VertexShaderB, sizeof(p.flow));
    memcpy(p.floats[0], g_tiling.begin.constants.VertexShaderF, sizeof(p.floats[0]));
    memcpy(p.floats[1], g_tiling.begin.constants.PixelShaderF, sizeof(p.floats[1]));
    p.declaration = s.vertex_declaration;
    p.vs = s.vertex_shader;
    p.ps = s.pixel_shader;
    p.origin = true;
    memcpy(p.viewport, s.viewport, sizeof(p.viewport));
    memcpy(p.screen_texel, &g_tiling.begin.constants.PixelShaderF[7], 16);
    p.scissor = s.scissor;
    p.scissor_on = s.render_states[D3DRS_SCISSORTESTENABLE / 4] != 0;
    p.stencil_on = s.render_states[D3DRS_STENCILENABLE / 4] != 0;
    for (unsigned i = 0; i < k_fetch_count; ++i) {
        p.sampler_pages[i] = s.textures[i] ? s.textures[i]->Format.BaseAddress :
            s.unbound_valid[i] ? (DWORD)((size_t)s.unbound[i].source.base >> 12) : 0;
        const auto &desc = s.unbound[i].source.desc;
        if (s.textures[i] ? shell_frame_texture(s.textures[i]->Format, p) :
            (s.unbound_valid[i] && desc.dimension == k_xenos_dimension_2d && desc.levels == 1 &&
             desc.width == (unsigned)p.width && desc.height == (unsigned)p.height))
            p.frame_samplers |= 1u << i;
        if (!s.sampler_states[i][D3DSAMP_MAGFILTER / 4] && !s.sampler_states[i][D3DSAMP_MINFILTER / 4])
            p.point_samples |= 1u << i;
    }
    s_shell_cursor c = {r.ops + 4, r.count - 4};
    const DWORD *a;
    if (!c.expect(k_op_predication, 1, a) || a[0] != D3DPRED_TILE(0) || !shell_group(c, p, true) ||
        !shell_stream(c, p, false) || p.shadow_pending || !p.draws || !(p.resolved_roles & (1u << 13)))
        return false;
    const DWORD color_base = s.render_targets[0]->ColorInfo.ColorBase;
    const DWORD depth_base = s.depth_stencil->DepthInfo.DepthBase;
    if (color_base == depth_base) return false;
    DWORD normal_base = ~0u;
    for (unsigned i = 0; i < p.target_count; ++i) {
        const auto &t = p.targets[i];
        const auto *v = shell_view((D3DSurface *)t.key);
        if ((v->role == 3 || v->role == 8) && t.edram_base != s.depth_stencil->DepthInfo.DepthBase)
            return false;
        if ((v->role == 13 || v->role == 17 || v->role == 18) &&
            t.edram_base != s.render_targets[0]->ColorInfo.ColorBase)
            return false;
        if (v->role == 14 || v->role == 19) {
            if (t.edram_base == depth_base || t.edram_base == color_base ||
                (normal_base != ~0u && normal_base != t.edram_base)) return false;
            normal_base = t.edram_base;
        }
        for (unsigned j = 0; j < g_shell.count; ++j)
            if (g_shell.views[j].role == 2 && t.edram_base == g_shell.views[j].surface->DepthInfo.DepthBase)
                return false;
    }
    return true;
}

void execute_shell(s_port_device *pd, const s_recording &r)
{
    // Preflight established that only the c108..111 prologue and complete resolve groups have predicates.
    // Band zero supplies the common shader constants, with a zero c108. Every rectangle retains its position.
    s_shell_cursor c = {r.ops + 4, r.count - 4};
    DWORD op, n, mask = 0;
    const DWORD *a;
    while (c.take(op, a, n)) {
        if (op == k_op_predication)
            mask = a[0];
        else if (!mask || mask == D3DPRED_TILE(0) || op == k_op_resolve)
            apply_op(pd, op, a, n, D3DPRED_TILE(0));
    }
}

bool try_execute_shell(s_port_device *pd, const s_recording &r)
{
    s_shell_plan shell = {};
    const bool single_pass = shell_preflight(shell, r) && port_vk_shell_begin(shell.targets, shell.target_count, shell.height);
    port_vk_shell_trace(single_pass, shell.draws, shell.resolves, shell.op, shell.ps);
    if (!single_pass) return false;
    g_window = {true, 0, 0, shell.width, shell.height};
    snapshot_restore(pd, &g_tiling.begin);
    const int later_bands[] = {0, shell.band_height, shell.width, shell.height};
    port_vk_clear_edram((g_tiling.clear_color_given ? (DWORD)D3DCLEAR_TARGET : 0u) |
        D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, g_tiling.clear_color, g_tiling.clear_z,
        g_tiling.clear_stencil, later_bands);
    port_vk_shell_apply_begin_clear(shell.targets, shell.target_count);
    execute_shell(pd, r);
    port_vk_shell_end(shell.targets, shell.target_count, g_tiling.rects[3][1], shell.setup_color);
    return true;
}
#endif

// The draw / command ops of a recording device: the pending state first, then the op.
bool recording_command(s_port_device *pd)
{
    if (!pd->recording)
        return false;
    capture_pending(pd);
    return true;
}

// ---- vertical blank (60 Hz, as the console's display; HALO3_FPS: port_vk_vertical_blank_rate) ----

struct s_vblank
{
    volatile LONG started;
    D3DVBLANKCALLBACK volatile callback;
    volatile LONG swaps;        // D3DDevice_Swap calls so far
    LONG swaps_shown;           // the swap the last vertical blank showed
    volatile DWORD count;
    volatile DWORD swap_count;  // the blank count at the last D3DDevice_Swap / Present
    HANDLE blank_event;         // set at every blank (auto reset), for D3DDevice_SynchronizeToPresentationInterval
};
s_vblank g_vblank;

// The vertical blank interrupt: D3D calls the title's callback with the blank's counter and the most recent swap
// shown (D3DVBLANK_SWAPDONE when this blank showed a new one). rasterizer_globals.cpp counts blanks, paces frames
// on them (main_time.cpp waits for events[3] / events[4], which the callback sets) and times presents by swap.
// Above HALO3_FPS=30 the blanks come at the frame rate (port_vk_vertical_blank_rate): the game measures a frame's time
// in blanks of that rate (port_d3d_vertical_blank_rate below), while the video mode keeps reporting 60 Hz.
DWORD WINAPI vblank_thread(LPVOID)
{
    LARGE_INTEGER frequency, now, next;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&next);
    const char *fast_env = getenv("HALO3_FAST_SIM");
    const bool fast_sim = !gpu() && fast_env != 0 && fast_env[0] != '\0' && fast_env[0] != '0';
    for (;;) {
        // the rate as it is now: the frame rate cap changes live (Guide > Settings, port_vk_frame_rate)
        const LONGLONG rate = fast_sim ? 600ll : (LONGLONG)port_vk_vertical_blank_rate();
        if (rate >= 500ll) {
            Sleep(1); // high-rate callbacks can use a coarse wait without spinning
            QueryPerformanceCounter(&next); // anchor precise pacing here if the cap is enabled again
        } else {
            const LONGLONG period = frequency.QuadPart / rate;
            next.QuadPart += period;
            port_sleep_until_counter(next.QuadPart);
            QueryPerformanceCounter(&now);
            if (now.QuadPart - next.QuadPart > frequency.QuadPart / 15)
                next = now; // the host stalled (four 60 Hz blanks): no burst of catch-up blanks
        }
        D3DVBLANKDATA data;
        data.VBlank = ++g_vblank.count;
        const LONG swaps = g_vblank.swaps;
        data.Flags = swaps != g_vblank.swaps_shown ? D3DVBLANK_SWAPDONE : 0;
        g_vblank.swaps_shown = swaps;
        data.Swap = (DWORD)swaps;
        D3DVBLANKCALLBACK callback = g_vblank.callback;
        if (callback)
            callback(&data);
        if (g_vblank.blank_event)
            SetEvent(g_vblank.blank_event);
    }
}

// ---- display ----

struct s_display
{
    bool ramp_set, pwl_set;
    D3DGAMMARAMP ramp;          // SetGammaRamp (8-bit display formats)
    D3DPWLGAMMA pwl;            // SetPWLGamma (the 7e3 / 10-bit front buffer)
};
s_display g_display;

} // namespace

// ---- the command buffer device's and the HAL device's creation ----

namespace {

template <int k_state> void WINAPI null_set_render_state(D3DDevice *device, DWORD value);
template <int k_state> DWORD WINAPI null_get_render_state(D3DDevice *device)
{
    return port_of(device)->state.render_states[k_state];
}
template <int k_state> void WINAPI null_set_sampler_state(D3DDevice *device, DWORD sampler, DWORD value)
{
    apply_sampler_state(port_of(device), sampler, (DWORD)k_state * 4u, value);
}
template <int k_state> DWORD WINAPI null_get_sampler_state(D3DDevice *device, DWORD sampler)
{
    return read_sampler_state(port_of(device), sampler, (DWORD)k_state * 4u);
}

void set_render_state(D3DDevice *device, DWORD index, DWORD value)
{
    s_port_device *pd = port_of(device);
    apply_render_state(pd, index, value);
    if (pd->recording) {
        if (DWORD *a = record(pd->recording, k_op_render_state, 2)) {
            a[0] = index;
            a[1] = value;
        }
    }
}

template <int k_state> void WINAPI null_set_render_state(D3DDevice *device, DWORD value)
{
    set_render_state(device, (DWORD)k_state, value);
}

template <int k_count> struct null_state_tables
{
    static void render(D3DDevice *device)
    {
        null_state_tables<k_count - 1>::render(device);
        device->m_SetRenderStateCall[k_count - 1] = &null_set_render_state<k_count - 1>;
        device->m_GetRenderStateCall[k_count - 1] = &null_get_render_state<k_count - 1>;
    }
    static void sampler(D3DDevice *device)
    {
        null_state_tables<k_count - 1>::sampler(device);
        device->m_SetSamplerStateCall[k_count - 1] = &null_set_sampler_state<k_count - 1>;
        device->m_GetSamplerStateCall[k_count - 1] = &null_get_sampler_state<k_count - 1>;
    }
};
template <> struct null_state_tables<0>
{
    static void render(D3DDevice *) {}
    static void sampler(D3DDevice *) {}
};

void initialize_device(s_port_device *pd, DWORD *ring)
{
    D3D::CDevice *d = pd->device;
    memset(d, 0, sizeof(*d));
    null_state_tables<k_render_state_count>::render(d);
    null_state_tables<k_sampler_state_count>::sampler(d);
    d->m_ReferenceCount = 1;
    pd->ring = ring;
    pd->recording = 0;
    pd->recording_noop = false;
    reset_ring(pd);
    pd->draw.vs_chunk_gen = pd->vs_chunk_gen;
    pd->draw.ps_chunk_gen = pd->ps_chunk_gen;
    reset_device_state(pd);
    memset(&d->m_Pending, 0, sizeof(d->m_Pending));
}

} // namespace

extern "C" {

HRESULT WINAPI Direct3D_CreateDevice(UINT Adapter, D3DDEVTYPE DeviceType, void *hUnusedFocusWindow, DWORD BehaviorFlags,
    D3DPRESENT_PARAMETERS *pPresentationParameters, D3DDevice **ppReturnedDeviceInterface)
{
    (void)Adapter; (void)hUnusedFocusWindow; (void)BehaviorFlags;
    if (!ppReturnedDeviceInterface)
        return D3DERR_INVALIDCALL;

    // The command buffer device (no presentation parameters, no surfaces): a recorder of its own.
    if (DeviceType == D3DDEVTYPE_COMMAND_BUFFER) {
        initialize_device(&g_recorder, g_command_buffer_ring);
        *ppReturnedDeviceInterface = &g_command_buffer_device_object;
        return S_OK;
    }

    initialize_device(&g_hal, g_hal_ring);
    UINT width = pPresentationParameters && pPresentationParameters->BackBufferWidth ? pPresentationParameters->BackBufferWidth : 1280;
    UINT height = pPresentationParameters && pPresentationParameters->BackBufferHeight ? pPresentationParameters->BackBufferHeight : 720;
    D3DFORMAT fmt = pPresentationParameters && pPresentationParameters->BackBufferFormat ? pPresentationParameters->BackBufferFormat : D3DFMT_A8R8G8B8;
    XGSetSurfaceHeader(width, height, fmt, D3DMULTISAMPLE_NONE, 0, &g_back_buffer, 0);
    g_hal.state.render_targets[0] = &g_back_buffer;
    // D3DRS_PRESENTINTERVAL starts as the parameters' PresentationInterval (D3DPRESENT_INTERVAL_DEFAULT is ONE).
    g_hal.state.render_states[D3DRS_PRESENTINTERVAL / 4] =
        pPresentationParameters != 0 ? pPresentationParameters->PresentationInterval : D3DPRESENT_INTERVAL_DEFAULT;

    // The PC renderer (HALO3_VULKAN=1): this device keeps every header and shadow the game's inline code uses and
    // forwards the GPU work (vulkan_device.h).
    if (port_vulkan_device_startup(width, height)) {
        s_port_vk_shadows shadows;
        shadows.vertex_float = &g_hal_device.m_Constants.VertexShaderF[0].x;
        shadows.pixel_float = &g_hal_device.m_Constants.PixelShaderF[0].x;
        shadows.vertex_bool = (const uint32_t *)g_hal_device.m_Constants.VertexShaderB;
        shadows.pixel_bool = (const uint32_t *)g_hal_device.m_Constants.PixelShaderB;
        shadows.vertex_int = (const uint32_t *)g_hal_device.m_Constants.VertexShaderI;
        shadows.pixel_int = (const uint32_t *)g_hal_device.m_Constants.PixelShaderI;
        port_vk_set_shadows(&shadows);
        s_port_vk_surface back;
        surface_info(&g_back_buffer, 0, &back);
        port_vk_set_back_buffer(&back);
        port_vk_set_render_target(0, &back);
    }

    *ppReturnedDeviceInterface = &g_hal_device;
    return S_OK;
}

HRESULT WINAPI Direct3D_CheckDeviceFormat(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage,
    D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
{
    (void)Adapter; (void)DeviceType; (void)AdapterFormat; (void)Usage; (void)RType; (void)CheckFormat;
    return S_OK;
}

HRESULT WINAPI Direct3D_CheckDeviceMultiSampleType(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat,
    BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType, DWORD *pQualityLevels)
{
    (void)Adapter; (void)DeviceType; (void)SurfaceFormat; (void)Windowed; (void)MultiSampleType;
    if (pQualityLevels)
        *pQualityLevels = 1;
    return S_OK;
}

HRESULT WINAPI Direct3D_CheckDepthStencilMatch(UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat,
    D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat)
{
    (void)Adapter; (void)DeviceType; (void)AdapterFormat; (void)RenderTargetFormat; (void)DepthStencilFormat;
    return S_OK;
}

void WINAPI D3DDevice_GetDisplayMode(D3DDevice *pDevice, UINT UnusedSwapChain, D3DDISPLAYMODE *pMode)
{
    (void)pDevice; (void)UnusedSwapChain;
    if (!pMode)
        return;
    memset(pMode, 0, sizeof(*pMode));
    pMode->Width = 1280;
    pMode->Height = 720;
    pMode->RefreshRate = 60;
    pMode->Format = D3DFMT_A8R8G8B8;
}

void WINAPI D3DDevice_GetDeviceCaps(D3DDevice *pDevice, D3DCAPS9 *pCaps)
{
    (void)pDevice;
    if (!pCaps)
        return;
    memset(pCaps, 0, sizeof(*pCaps));
    pCaps->DeviceType = D3DDEVTYPE_HAL;
    pCaps->MaxTextureWidth = 4096;
    pCaps->MaxTextureHeight = 4096;
    pCaps->MaxVolumeExtent = 512;
    pCaps->MaxSimultaneousTextures = 16;
    pCaps->NumSimultaneousRTs = 4;
    pCaps->MaxUserClipPlanes = D3DMAXUSERCLIPPLANES;
}

ULONG WINAPI D3DDevice_AddRef(D3DDevice *pDevice)
{
    return pDevice ? ++pDevice->m_ReferenceCount : 1;
}

ULONG WINAPI D3DDevice_Release(D3DDevice *pDevice)
{
    return pDevice && pDevice->m_ReferenceCount > 1 ? --pDevice->m_ReferenceCount : 0;
}

// D3DDEVICESTATE_*: inside a tiling bracket (the HAL device), recording a command buffer (the recording device).
DWORD WINAPI D3DDevice_GetDeviceState(D3DDevice *pDevice)
{
    s_port_device *pd = port_of(pDevice);
    DWORD state = 0;
    if (pd->hal && g_tiling.active)
        state |= D3DDEVICESTATE_TILING_BRACKET;
    if (!pd->hal && (pd->recording || pd->recording_noop))
        state |= D3DDEVICESTATE_COMMANDBUFFER_BRACKET;
    return state;
}

ULONG WINAPI D3DResource_AddRef(D3DResource *pThis)
{
    return pThis ? ++pThis->ReferenceCount : 1;
}

static void wait_resource_storage(D3DResource *resource)
{
    if (!gpu() || !resource) return;
    switch (resource->Common & D3DCOMMON_TYPE_MASK) {
    case D3DCOMMON_TYPE_VERTEXBUFFER: {
        const auto *vb = (D3DVertexBuffer *)resource;
        port_vk_record_resource_idle(0, (const void *)((size_t)vb->Format.BaseAddress << 2), vb->Format.Size * 4u);
        break;
    }
    case D3DCOMMON_TYPE_INDEXBUFFER: {
        const auto *ib = (D3DIndexBuffer *)resource;
        port_vk_record_resource_idle(0, (const void *)(size_t)ib->Address, ib->Size);
        break;
    }
    case D3DCOMMON_TYPE_TEXTURE:
        port_vk_record_resource_idle(resource, 0, 0);
        break;
    }
}

ULONG WINAPI D3DResource_Release(D3DResource *pThis)
{
    if (!pThis)
        return 0;
    if (pThis->ReferenceCount > 1)
        return --pThis->ReferenceCount;
    wait_resource_storage(pThis);
    pThis->ReferenceCount = 0;
    if ((pThis->Common & D3DCOMMON_TYPE_MASK) == D3DCOMMON_TYPE_SURFACE &&
        (pThis->Common & D3DCOMMON_D3DCREATED) != 0 &&
        (pThis->Common & D3DCOMMON_SURFACE_FROM_TEXTURE) == 0 &&
        pThis != (D3DResource *)&g_back_buffer) {
        bool bound = (g_hal.state.depth_stencil == (D3DSurface *)pThis);
        for (DWORD i = 0; i < 4; i++) {
            if (g_hal.state.render_targets[i] == (D3DSurface *)pThis)
                bound = true;
        }
        if (!bound) {
            pThis->Common &= ~D3DCOMMON_D3DCREATED;
            RtlFreeHeap(GetProcessHeap(), 0, pThis);
        }
    }
    return 0;
}

D3DBaseTexture *WINAPI D3DDevice_CreateTexture(DWORD Width, DWORD Height, DWORD Depth, DWORD Levels, DWORD Usage,
    D3DFORMAT Format, D3DPOOL UnusedPool, D3DRESOURCETYPE D3DType)
{
    D3DBaseTexture *tex = (D3DBaseTexture *)alloc_zeroed(sizeof(D3DTexture));
    if (!tex)
        return 0;
    UINT size = 0;
    switch (D3DType) {
    case D3DRTYPE_CUBETEXTURE:
        size = XGSetCubeTextureHeader(Width, Levels, Usage, Format, UnusedPool, 0, XGHEADER_CONTIGUOUS_MIP_OFFSET,
            (IDirect3DCubeTexture9 *)tex, 0, 0);
        break;
    case D3DRTYPE_VOLUMETEXTURE:
        size = XGSetVolumeTextureHeader(Width, Height, Depth, Levels, Usage, Format, UnusedPool, 0,
            XGHEADER_CONTIGUOUS_MIP_OFFSET, (IDirect3DVolumeTexture9 *)tex, 0, 0);
        break;
    case D3DRTYPE_ARRAYTEXTURE:
        size = XGSetArrayTextureHeader(Width, Height, Depth, Levels, Usage, Format, UnusedPool, 0,
            XGHEADER_CONTIGUOUS_MIP_OFFSET, 0, (IDirect3DArrayTexture9 *)tex, 0, 0);
        break;
    case D3DRTYPE_LINETEXTURE:
        size = XGSetLineTextureHeader(Width, Levels, Usage, Format, UnusedPool, 0, XGHEADER_CONTIGUOUS_MIP_OFFSET,
            (IDirect3DLineTexture9 *)tex, 0, 0);
        break;
    default:
        size = XGSetTextureHeader(Width, Height, Levels, Usage, Format, UnusedPool, 0, XGHEADER_CONTIGUOUS_MIP_OFFSET,
            0, (IDirect3DTexture9 *)tex, 0, 0);
        break;
    }
    tex->Common |= D3DCOMMON_D3DCREATED;
    void *mem = XPhysicalAlloc(size ? size : 4096u, ~0ul, 4096u, PAGE_READWRITE);
    if (!mem) {
        RtlFreeHeap(GetProcessHeap(), 0, tex);
        return 0;
    }
    XGOffsetResourceAddress(tex, mem);
    return tex;
}

D3DSurface *WINAPI D3DDevice_CreateSurface(DWORD Width, DWORD Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample,
    CONST D3DSURFACE_PARAMETERS *pParameters)
{
    D3DSurface *surf = (D3DSurface *)alloc_zeroed(sizeof(D3DSurface));
    if (!surf)
        return 0;
    XGSetSurfaceHeader(Width, Height, Format, MultiSample, pParameters, surf, 0);
    surf->Common |= D3DCOMMON_D3DCREATED;
    return surf;
}

D3DVertexBuffer *WINAPI D3DDevice_CreateVertexBuffer(UINT Length, DWORD Usage, D3DPOOL UnusedPool)
{
    D3DVertexBuffer *vb = (D3DVertexBuffer *)alloc_zeroed(sizeof(D3DVertexBuffer));
    if (!vb)
        return 0;
    UINT alloc_len = Length ? Length : 16u;
    void *mem = XPhysicalAlloc(alloc_len, ~0ul, 4096u, PAGE_READWRITE);
    if (!mem) {
        RtlFreeHeap(GetProcessHeap(), 0, vb);
        return 0;
    }
    XGSetVertexBufferHeader(alloc_len, Usage, UnusedPool, 0, vb);
    vb->Common |= D3DCOMMON_D3DCREATED;
    vb->Identifier = 0; // filled through Lock by the CPU: host order (d3d_port_buffer.h)
    XGOffsetResourceAddress(vb, mem);
    return vb;
}

D3DIndexBuffer *WINAPI D3DDevice_CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL UnusedPool)
{
    D3DIndexBuffer *ib = (D3DIndexBuffer *)alloc_zeroed(sizeof(D3DIndexBuffer));
    if (!ib)
        return 0;
    UINT alloc_len = Length ? Length : 16u;
    void *mem = XPhysicalAlloc(alloc_len, ~0ul, 4096u, PAGE_READWRITE);
    if (!mem) {
        RtlFreeHeap(GetProcessHeap(), 0, ib);
        return 0;
    }
    XGSetIndexBufferHeader(alloc_len, Usage, Format, UnusedPool, 0, ib);
    ib->Common |= D3DCOMMON_D3DCREATED;
    ib->Identifier = 0; // host order (d3d_port_buffer.h)
    XGOffsetResourceAddress(ib, mem);
    return ib;
}

D3DVertexDeclaration *WINAPI D3DDevice_CreateVertexDeclaration(CONST D3DVERTEXELEMENT9 *pVertexElements)
{
    s_null_object *decl = (s_null_object *)alloc_zeroed(sizeof(s_null_object));
    if (!decl)
        return 0;
    // a copy of the elements, up to and including the D3DDECL_END marker (stream 0xff): what the renderer reads
    UINT count = 0;
    while (pVertexElements && count < 64 && pVertexElements[count].Stream != 0xff)
        count++;
    D3DVERTEXELEMENT9 *elements = (D3DVERTEXELEMENT9 *)alloc_zeroed((count + 1) * sizeof(D3DVERTEXELEMENT9));
    if (elements && pVertexElements) {
        memcpy(elements, pVertexElements, (count + 1) * sizeof(D3DVERTEXELEMENT9));
        elements[count].Stream = 0xff;
    }
    decl->payload[0] = (DWORD)(size_t)elements;
    decl->payload[1] = count;
    decl->Common = D3DCOMMON_TYPE_VERTEXDECLARATION | D3DCOMMON_D3DCREATED;
    decl->ReferenceCount = 1;
    return (D3DVertexDeclaration *)decl;
}

// The object in the XDK's layout (vertex_shader_binding): the D3D state, then the copy of the function's microcode
// container (its virtual part) whose first word holds the shader's flags.
D3DVertexShader *WINAPI D3DDevice_CreateVertexShader(CONST DWORD *pFunction)
{
    DWORD container_size = 4;
    if (pFunction) {
        if ((pFunction[0] & 0xffffff00u) == k_container_magic)
            container_size = pFunction[1];
        else if ((swap32(pFunction[0]) & 0xffffff00u) == k_container_magic)
            container_size = swap32(pFunction[1]);
        if (container_size < 4 || container_size > 0x10000u)
            container_size = 4;
    }
    s_null_object *sh = (s_null_object *)alloc_zeroed(k_vertex_shader_container + container_size);
    if (!sh)
        return 0;
    if (pFunction)
        memcpy((char *)sh + k_vertex_shader_container, pFunction, container_size);
    sh->Common = D3DCOMMON_TYPE_VERTEXSHADER | D3DCOMMON_D3DCREATED;
    sh->ReferenceCount = 1;
    register_function(sh, PORT_VK_STAGE_VERTEX, pFunction);
    return (D3DVertexShader *)sh;
}

D3DPixelShader *WINAPI D3DDevice_CreatePixelShader(CONST DWORD *pFunction)
{
    s_null_object *sh = (s_null_object *)alloc_zeroed(sizeof(s_null_object));
    if (!sh)
        return 0;
    sh->Common = D3DCOMMON_TYPE_PIXELSHADER | D3DCOMMON_D3DCREATED;
    sh->ReferenceCount = 1;
    register_function(sh, PORT_VK_STAGE_PIXEL, pFunction);
    return (D3DPixelShader *)sh;
}

D3DConstantBuffer *WINAPI D3DDevice_CreateConstantBuffer(UINT VectorCount, DWORD Usage)
{
    D3DConstantBuffer *cb = (D3DConstantBuffer *)alloc_zeroed(64);
    if (!cb)
        return 0;
    void *mem = XPhysicalAlloc(VectorCount ? VectorCount * 16u : 16u, ~0ul, 4096u, PAGE_READWRITE);
    XGSetConstantBufferHeader(VectorCount, Usage, (UINT)(size_t)mem, cb);
    return cb;
}

// Size: the console's command memory for the buffer; here the stream's first allocation (it grows as needed).
D3DCommandBuffer *WINAPI D3DDevice_CreateCommandBuffer(UINT Size, DWORD Flags)
{
    (void)Flags;
    s_port_command_buffer *cb = (s_port_command_buffer *)alloc_zeroed(sizeof(s_port_command_buffer));
    s_recording *r = (s_recording *)alloc_zeroed(sizeof(s_recording));
    if (!cb || !r)
        return 0;
    cb->header.Common = D3DCOMMON_TYPE_COMMANDBUFFER | D3DCOMMON_D3DCREATED;
    cb->header.ReferenceCount = 1;
    cb->magic = k_command_buffer_magic;
    cb->recording = r;
    r->owner = cb;
    recording_reserve(r, Size / 4u > k_payload_header_dwords ? Size / 4u : 1024u);
    recording_reset(r);
    finish_payload(r);
    cb->payload = r->ops;
    cb->payload_size = payload_bytes(r);
    return (D3DCommandBuffer *)cb;
}

D3DQuery *WINAPI D3DDevice_CreateQueryTiled(D3DDevice *pDevice, D3DQUERYTYPE Type, DWORD TileCount)
{
    (void)pDevice;
    s_null_object *q = (s_null_object *)alloc_zeroed(sizeof(s_null_object));
    if (!q)
        return 0;
    q->Common = D3DCOMMON_D3DCREATED;
    q->ReferenceCount = 1;
    q->payload[0] = (DWORD)Type;
    q->payload[1] = TileCount;
    return (D3DQuery *)q;
}

D3DQuery *WINAPI D3DDevice_CreateQuery(D3DDevice *pDevice, D3DQUERYTYPE Type)
{
    return D3DDevice_CreateQueryTiled(pDevice, Type, 1);
}

HRESULT WINAPI D3DQuery_GetData(D3DQuery *pThis, void *pData, DWORD Size, DWORD GetDataFlags)
{
    if (pData && Size > 0)
        memset(pData, 0, Size);
    if (!pThis || !gpu() || ((s_null_object *)pThis)->payload[0] != D3DQUERYTYPE_OCCLUSION)
        return S_OK;
    uint32_t samples = 0;
    const bool flush = (GetDataFlags & 1u) != 0; // D3DGETDATA_FLUSH
    if (!(flush ? port_vk_query_result_flush(pThis, &samples) : port_vk_query_result(pThis, &samples)))
        return S_FALSE; // rasterizer_occlusions_retrieve keeps it pending and asks again next frame
    if (pData && Size >= sizeof(DWORD))
        ((D3DOCCLUSIONSTATS *)pData)->ZPass = samples;
    return S_OK;
}

D3DSurface *WINAPI D3DDevice_GetBackBuffer(D3DDevice *pDevice)
{
    (void)pDevice;
    D3DResource_AddRef(&g_back_buffer);
    return &g_back_buffer;
}

// ---- surfaces ----

void WINAPI D3DDevice_SetRenderTarget_External(D3DDevice *pDevice, DWORD RenderTargetIndex, D3DSurface *pRenderTarget)
{
    s_port_device *pd = port_of(pDevice);
    if (RenderTargetIndex >= 4)
        return;
    apply_render_target(pd, RenderTargetIndex, pRenderTarget);
    if (pd->recording) {
        if (DWORD *a = record(pd->recording, k_op_render_target, 2)) {
            a[0] = RenderTargetIndex;
            a[1] = (DWORD)(size_t)pRenderTarget;
        }
    }
    // A new render target 0 (or depth-stencil when RT0 is NULL) resets the viewport and the scissor rectangle to the
    // whole surface, as the XDK's SetRenderTarget / SetDepthStencilSurface does (D3D::SetSurfaceInfo: scissor
    // {0, 0, 0xffff, 0xffff}, viewport {0, 0, 0xffff, 0xffff, 0, 1} clamped to the surface by SetViewportF);
    // full-screen and depth-only passes draw and clear with no viewport of their own.
    if (RenderTargetIndex == 0) {
        D3DSurface *surface = pRenderTarget ? pRenderTarget : pd->state.depth_stencil;
        if (surface) {
            s_port_vk_surface info;
            surface_info(surface, 0, &info);
            D3DVIEWPORT9 viewport;
            viewport.X = 0;
            viewport.Y = 0;
            viewport.Width = info.width < 0xffffu ? info.width : 0xffffu;
            viewport.Height = info.height < 0xffffu ? info.height : 0xffffu;
            viewport.MinZ = 0.0f;
            viewport.MaxZ = 1.0f;
            D3DDevice_SetViewport(pDevice, &viewport);
            RECT scissor = { 0, 0, 0xffff, 0xffff };
            D3DDevice_SetScissorRect(pDevice, &scissor);
        }
    }
}

D3DSurface *WINAPI D3DDevice_GetRenderTarget(D3DDevice *pDevice, DWORD RenderTargetIndex)
{
    s_port_device *pd = port_of(pDevice);
    D3DSurface *rt = RenderTargetIndex < 4 ? pd->state.render_targets[RenderTargetIndex] : 0;
    if (rt)
        D3DResource_AddRef(rt);
    return rt;
}

void WINAPI D3DDevice_SetDepthStencilSurface(D3DDevice *pDevice, D3DSurface *pZStencilSurface)
{
    s_port_device *pd = port_of(pDevice);
    apply_depth_stencil(pd, pZStencilSurface);
    if (pd->recording) {
        if (DWORD *a = record(pd->recording, k_op_depth_stencil, 1))
            a[0] = (DWORD)(size_t)pZStencilSurface;
    }
    if (pd->state.render_targets[0] == 0 && pZStencilSurface) {
        s_port_vk_surface info;
        surface_info(pZStencilSurface, 0, &info);
        D3DVIEWPORT9 viewport;
        viewport.X = 0;
        viewport.Y = 0;
        viewport.Width = info.width < 0xffffu ? info.width : 0xffffu;
        viewport.Height = info.height < 0xffffu ? info.height : 0xffffu;
        viewport.MinZ = 0.0f;
        viewport.MaxZ = 1.0f;
        D3DDevice_SetViewport(pDevice, &viewport);
        RECT scissor = { 0, 0, 0xffff, 0xffff };
        D3DDevice_SetScissorRect(pDevice, &scissor);
    }
}

D3DSurface *WINAPI D3DDevice_GetDepthStencilSurface(D3DDevice *pDevice)
{
    s_port_device *pd = port_of(pDevice);
    if (pd->state.depth_stencil)
        D3DResource_AddRef(pd->state.depth_stencil);
    return pd->state.depth_stencil;
}

// The depth/stencil surface and the four render targets at once. With Flags bit 0 inside a tiling bracket
// (c_rasterizer::refresh_tiling_render_targets) they also become the bracket's surfaces, the ones its tiles render
// through their windows (window_applies; retail 0x825a6790 keeps them at CDevice +0x31b0..+0x31c0, only while a
// bracket is open). Each tile's replay repeats the registration where the game made it.
void WINAPI D3DDevice_SetSurfaces(D3DDevice *pDevice, CONST D3DSURFACES *pSurfaces, DWORD Flags)
{
    if (!pSurfaces)
        return;
    s_port_device *pd = port_of(pDevice);
    if ((Flags & 1u) != 0 && pd->hal && g_tiling.active) {
        apply_tiling_surfaces(pd, pSurfaces->pRenderTarget, pSurfaces->pDepthStencilSurface);
        if (pd->recording == &g_tiling.recording) {
            if (DWORD *a = record(pd->recording, k_op_tiling_surfaces, 5)) {
                for (DWORD i = 0; i < 4; i++)
                    a[i] = (DWORD)(size_t)pSurfaces->pRenderTarget[i];
                a[4] = (DWORD)(size_t)pSurfaces->pDepthStencilSurface;
            }
        }
    }
    D3DDevice_SetDepthStencilSurface(pDevice, pSurfaces->pDepthStencilSurface);
    for (DWORD i = 0; i < 4; i++)
        D3DDevice_SetRenderTarget_External(pDevice, i, pSurfaces->pRenderTarget[i]);
}

DWORD WINAPI D3DBaseTexture_GetLevelCount(D3DBaseTexture *pThis)
{
    return pThis ? pThis->Format.MaxMipLevel + 1u : 1u;
}

D3DSurface *WINAPI D3DTexture_GetSurfaceLevel(D3DTexture *pThis, UINT Level)
{
    return pThis ? make_texture_surface_view(pThis, 0, Level) : 0;
}

D3DSurface *WINAPI D3DCubeTexture_GetCubeMapSurface(D3DCubeTexture *pThis, D3DCUBEMAP_FACES FaceType, UINT Level)
{
    return pThis ? make_texture_surface_view(pThis, (UINT)FaceType, Level) : 0;
}

DWORD WINAPI D3DArrayTexture_GetArraySize(D3DArrayTexture *pThis)
{
    return pThis ? pThis->Format.Size.Stack.Depth + 1u : 1u;
}

D3DSurface *WINAPI D3DArrayTexture_GetArraySurface(D3DArrayTexture *pThis, UINT ArrayIndex, UINT Level)
{
    return pThis ? make_texture_surface_view(pThis, ArrayIndex, Level) : 0;
}

void WINAPI D3DSurface_GetDesc(D3DSurface *pThis, D3DSURFACE_DESC *pDesc)
{
    if (!pThis || !pDesc)
        return;
    memset(pDesc, 0, sizeof(*pDesc));
    if (pThis->Common & D3DCOMMON_SURFACE_FROM_TEXTURE) {
        XGTEXTURE_DESC td;
        XGGetTextureDesc(pThis->Parent, pThis->MipLevel, &td);
        pDesc->Format = td.Format;
        pDesc->Type = D3DRTYPE_SURFACE;
        pDesc->Width = td.Width;
        pDesc->Height = td.Height;
        pDesc->MultiSampleType = D3DMULTISAMPLE_NONE;
    } else {
        pDesc->Format = pThis->Format;
        pDesc->Type = D3DRTYPE_SURFACE;
        pDesc->Width = pThis->Width;
        pDesc->Height = pThis->Height;
        pDesc->MultiSampleType = D3DMULTISAMPLE_NONE;
    }
}

// ---- locks ----

// The bytes a buffer lock covers: SizeToLock from OffsetToLock, 0 meaning the rest of the buffer (`length` bytes).
static UINT lock_size(UINT length, UINT OffsetToLock, UINT SizeToLock)
{
    if (OffsetToLock >= length)
        return 0;
    return SizeToLock == 0 || SizeToLock > length - OffsetToLock ? length - OffsetToLock : SizeToLock;
}

// A buffer Lock's effect on the device: what memory exports wrote there is read back for the CPU to read
// (port_vk_buffer_lock), and the device's copies of the buffer are made again from its bytes at their next draw unless
// the lock leaves them as they were: D3DLOCK_READONLY with no export read back. The game read-locks the structure's
// render geometry to sample object lighting (geometry_sampling.cpp read_lock, flags 0x11); counting that as a write
// demoted those vertex buffers from their device copies to a per-draw copy of the draw's vertex range, which the water's
// per-vertex shading (the 2-4 player tuning) cannot use: it fetches mesh vertices by the indices in its triangle stream.
static void buffer_lock_gpu(const D3DResource *buffer, char *base, UINT offset, UINT size, DWORD Flags)
{
    const int read_only = (Flags & D3DLOCK_READONLY) != 0;
    const int read_back = port_vk_buffer_lock(base + offset, size, read_only);
    if (!read_only && !(buffer->Common & D3DCOMMON_D3DCREATED))
        port_vk_buffer_written(buffer, base);
    else if (read_back || !read_only)
        port_vk_resource_touched(buffer);
}

void *WINAPI D3DVertexBuffer_Lock(D3DVertexBuffer *pThis, UINT OffsetToLock, UINT SizeToLock, DWORD Flags)
{
    if (!pThis)
        return 0;
    char *base = (char *)((size_t)pThis->Format.BaseAddress << 2);
    if (gpu())
        buffer_lock_gpu(pThis, base, OffsetToLock, lock_size(pThis->Format.Size * 4u, OffsetToLock, SizeToLock), Flags);
    return base + OffsetToLock;
}

void *WINAPI D3DIndexBuffer_Lock(D3DIndexBuffer *pThis, UINT OffsetToLock, UINT SizeToLock, DWORD Flags)
{
    if (!pThis)
        return 0;
    char *base = (char *)(size_t)pThis->Address;
    if (gpu())
        buffer_lock_gpu(pThis, base, OffsetToLock, lock_size(pThis->Size, OffsetToLock, SizeToLock), Flags);
    return base + OffsetToLock;
}

void WINAPI D3DTexture_LockRect(D3DTexture *pThis, UINT Level, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags)
{
    (void)pRect; (void)Flags;
    if (!pThis || !pLockedRect)
        return;
    if (gpu()) {
        port_vk_texture_lock_mode(pThis, Level, 0, (Flags & D3DLOCK_READONLY) != 0);
        if ((Flags & D3DLOCK_READONLY) == 0)
            port_vk_texture_written(pThis);
    }
    XGTEXTURE_DESC desc;
    XGGetTextureDesc(pThis, Level, &desc);
    UINT base = (Level > 0 && pThis->Format.MipAddress != 0)
        ? (pThis->Format.MipAddress << 12)
        : (pThis->Format.BaseAddress << 12);
    pLockedRect->Pitch = (INT)desc.RowPitch;
    pLockedRect->pBits = (char *)(size_t)base + XGGetMipLevelOffset(pThis, 0, Level);
}

void WINAPI D3DCubeTexture_LockRect(D3DCubeTexture *pThis, D3DCUBEMAP_FACES FaceType, UINT Level,
    D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags)
{
    (void)pRect; (void)Flags;
    if (!pThis || !pLockedRect)
        return;
    if (gpu()) {
        port_vk_texture_lock_mode(pThis, Level, (UINT)FaceType, (Flags & D3DLOCK_READONLY) != 0);
        if ((Flags & D3DLOCK_READONLY) == 0)
            port_vk_texture_written(pThis);
    }
    XGTEXTURE_DESC desc;
    XGGetTextureDesc(pThis, Level, &desc);
    UINT base = (Level > 0 && pThis->Format.MipAddress != 0)
        ? (pThis->Format.MipAddress << 12)
        : (pThis->Format.BaseAddress << 12);
    pLockedRect->Pitch = (INT)desc.RowPitch;
    pLockedRect->pBits = (char *)(size_t)base + XGGetMipLevelOffset(pThis, (UINT)FaceType, Level);
}

void WINAPI D3DArrayTexture_LockRect(D3DArrayTexture *pThis, UINT ArrayIndex, UINT Level,
    D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags)
{
    (void)pRect; (void)Flags;
    if (!pThis || !pLockedRect)
        return;
    if (gpu()) {
        port_vk_texture_lock_mode(pThis, Level, ArrayIndex, (Flags & D3DLOCK_READONLY) != 0);
        if ((Flags & D3DLOCK_READONLY) == 0)
            port_vk_texture_written(pThis);
    }
    XGTEXTURE_DESC desc;
    XGGetTextureDesc(pThis, Level, &desc);
    UINT base = (Level > 0 && pThis->Format.MipAddress != 0)
        ? (pThis->Format.MipAddress << 12)
        : (pThis->Format.BaseAddress << 12);
    pLockedRect->Pitch = (INT)desc.RowPitch;
    pLockedRect->pBits = (char *)(size_t)base + XGGetMipLevelOffset(pThis, ArrayIndex, Level);
}

void WINAPI D3DVolumeTexture_LockBox(D3DVolumeTexture *pThis, UINT Level, D3DLOCKED_BOX *pLockedVolume,
    CONST D3DBOX *pBox, DWORD Flags)
{
    (void)pBox; (void)Flags;
    if (!pThis || !pLockedVolume)
        return;
    if (gpu()) {
        port_vk_texture_lock_mode(pThis, Level, 0, (Flags & D3DLOCK_READONLY) != 0);
        if ((Flags & D3DLOCK_READONLY) == 0)
            port_vk_texture_written(pThis);
    }
    XGTEXTURE_DESC desc;
    XGGetTextureDesc(pThis, Level, &desc);
    UINT base = (Level > 0 && pThis->Format.MipAddress != 0)
        ? (pThis->Format.MipAddress << 12)
        : (pThis->Format.BaseAddress << 12);
    pLockedVolume->RowPitch = (INT)desc.RowPitch;
    pLockedVolume->SlicePitch = (INT)desc.SlicePitch;
    pLockedVolume->pBits = (char *)(size_t)base + XGGetMipLevelOffset(pThis, 0, Level);
}

void WINAPI D3DSurface_LockRect(D3DSurface *pThis, D3DLOCKED_RECT *pLockedRect, CONST RECT *pRect, DWORD Flags)
{
    if (!pThis || !pLockedRect)
        return;
    if (pThis->Common & D3DCOMMON_SURFACE_FROM_TEXTURE) {
        D3DTexture_LockRect((D3DTexture *)pThis->Parent, pThis->MipLevel, pLockedRect, pRect, Flags);
        return;
    }
    static unsigned char s_scratch_surface[4096];
    pLockedRect->Pitch = 256;
    pLockedRect->pBits = s_scratch_surface;
}

void WINAPI D3DVertexBuffer_Unlock(D3DVertexBuffer *pThis) { (void)pThis; }
void WINAPI D3DIndexBuffer_Unlock(D3DIndexBuffer *pThis) { (void)pThis; }
void WINAPI D3DTexture_UnlockRect(D3DTexture *pThis, UINT Level) { (void)pThis; (void)Level; }

void WINAPI D3DTexture_GetLevelDesc(D3DTexture *pThis, UINT Level, D3DSURFACE_DESC *pDesc)
{
    if (!pThis || !pDesc)
        return;
    memset(pDesc, 0, sizeof(*pDesc));
    XGTEXTURE_DESC td;
    XGGetTextureDesc(pThis, Level, &td);
    pDesc->Format = td.Format;
    pDesc->Type = D3DRTYPE_TEXTURE;
    pDesc->Width = td.Width;
    pDesc->Height = td.Height;
    pDesc->MultiSampleType = D3DMULTISAMPLE_NONE;
}

void WINAPI D3DCubeTexture_GetLevelDesc(D3DCubeTexture *pThis, UINT Level, D3DSURFACE_DESC *pDesc)
{
    if (!pThis || !pDesc)
        return;
    memset(pDesc, 0, sizeof(*pDesc));
    XGTEXTURE_DESC td;
    XGGetTextureDesc(pThis, Level, &td);
    pDesc->Format = td.Format;
    pDesc->Type = D3DRTYPE_CUBETEXTURE;
    pDesc->Width = td.Width;
    pDesc->Height = td.Height;
    pDesc->MultiSampleType = D3DMULTISAMPLE_NONE;
}

D3DRESOURCETYPE WINAPI D3DResource_GetType(D3DResource *pThis)
{
    if (!pThis)
        return D3DRTYPE_SURFACE;
    switch (pThis->Common & D3DCOMMON_TYPE_MASK) {
    case D3DCOMMON_TYPE_VERTEXBUFFER:   return D3DRTYPE_VERTEXBUFFER;
    case D3DCOMMON_TYPE_INDEXBUFFER:    return D3DRTYPE_INDEXBUFFER;
    case D3DCOMMON_TYPE_TEXTURE:        return D3DRTYPE_TEXTURE;
    case D3DCOMMON_TYPE_SURFACE:        return D3DRTYPE_SURFACE;
    case D3DCOMMON_TYPE_VERTEXDECLARATION: return D3DRTYPE_VERTEXDECLARATION;
    case D3DCOMMON_TYPE_VERTEXSHADER:   return D3DRTYPE_VERTEXSHADER;
    case D3DCOMMON_TYPE_PIXELSHADER:    return D3DRTYPE_PIXELSHADER;
    case D3DCOMMON_TYPE_CONSTANTBUFFER: return D3DRTYPE_CONSTANTBUFFER;
    case D3DCOMMON_TYPE_COMMANDBUFFER:  return D3DRTYPE_COMMANDBUFFER;
    default:                            return D3DRTYPE_SURFACE;
    }
}

// The GPU work of this port's device is done when its call returns, except what a tiling bracket defers to
// EndTiling: a command buffer run inside the open bracket is busy until then (render_method_submit.cpp rebuilds a
// cached command buffer in place only when it is not busy).
BOOL WINAPI D3DResource_IsBusy(D3DResource *pThis)
{
    if (!pThis || (pThis->Common & D3DCOMMON_TYPE_MASK) != D3DCOMMON_TYPE_COMMANDBUFFER)
        return FALSE;
    const s_port_command_buffer *cb = port_command_buffer((D3DCommandBuffer *)pThis);
    return cb && g_tiling.active && cb->run_epoch == g_tiling.epoch ? TRUE : FALSE;
}

void WINAPI D3DResource_BlockUntilNotBusy(D3DResource *pThis)
{
    wait_resource_storage(pThis);
    if (D3DResource_IsBusy(pThis))
        report_once(4, "BlockUntilNotBusy on a command buffer the open tiling bracket runs (it runs at EndTiling)");
}

ULONG WINAPI D3DQuery_Release(D3DQuery *pThis)
{
    return D3DResource_Release((D3DResource *)pThis);
}

void WINAPI D3DQuery_Issue(D3DQuery *pThis, DWORD IssueFlags)
{
    if (!gpu())
        return;
    // into the open tiling bracket's or command buffer's stream when one is being recorded: it runs with the draws
    s_port_device *pd = &g_hal;
    if (pd->recording) {
        if (DWORD *a = record(pd->recording, k_op_query, 2)) {
            a[0] = (DWORD)(size_t)pThis;
            a[1] = IssueFlags;
        }
        return;
    }
    apply_query(pd, pThis, IssueFlags);
}

HRESULT WINAPI D3DDevice_Reset(D3DDevice *pDevice, D3DPRESENT_PARAMETERS *pPresentationParameters)
{
    (void)pDevice;
    if (!pPresentationParameters)
        return S_OK;
    UINT width = pPresentationParameters->BackBufferWidth ? pPresentationParameters->BackBufferWidth : 1280;
    UINT height = pPresentationParameters->BackBufferHeight ? pPresentationParameters->BackBufferHeight : 720;
    D3DFORMAT fmt = pPresentationParameters->BackBufferFormat ? pPresentationParameters->BackBufferFormat : D3DFMT_A8R8G8B8;
    XGSetSurfaceHeader(width, height, fmt, D3DMULTISAMPLE_NONE, 0, &g_back_buffer, 0);
    g_hal.state.render_targets[0] = &g_back_buffer;
    for (DWORD i = 1; i < 4; i++)
        g_hal.state.render_targets[i] = 0;
    g_hal.state.depth_stencil = 0;
    for (DWORD i = 0; i < 4; i++)
        g_hal.state.tiling_render_targets[i] = 0;
    g_hal.state.tiling_depth_stencil = 0;
    for (DWORD i = 0; i < k_fetch_count; i++) {
        g_hal.state.textures[i] = 0;
        g_hal.state.unbound_valid[i] = false;
    }
    __builtin_memset(g_hal.state.unbound, 0, sizeof(g_hal.state.unbound));
    g_hal.state.render_states[D3DRS_PRESENTINTERVAL / 4] = pPresentationParameters->PresentationInterval;
    publish_draw(&g_hal);
    port_vk_invalidate_draw_caches();
    if (port_vulkan_device_active()) {
        unsigned int custom_w = 0, custom_h = 0;
        const unsigned int res_scale = port_config_resolution(&custom_w, &custom_h) ? 1u : 0u;
        port_vk_resize_back_buffer(width, height, res_scale);
        s_port_vk_surface back;
        surface_info(&g_back_buffer, 0, &back);
        port_vk_set_back_buffer(&back);
        port_vk_set_render_target(0, &back);
    }
    return S_OK;
}

void WINAPI D3DDevice_AcquireThreadOwnership(D3DDevice *pDevice)
{
    if (pDevice == g_hal.device) port_vk_record_acquire();
}
void WINAPI D3DDevice_ReleaseThreadOwnership(D3DDevice *pDevice)
{
    if (pDevice == g_hal.device) port_vk_record_release();
}
void WINAPI D3DDevice_Suspend(D3DDevice *pDevice)
{
    if (pDevice == g_hal.device) port_vk_record_drain();
}
void WINAPI D3DDevice_BlockUntilIdle(D3DDevice *pDevice) { (void)pDevice; port_vk_record_drain(); }
// The CPU waits until the frame being finished may be shown: D3DRS_PRESENTINTERVAL vertical blanks after the
// previous Swap (ONE: the next blank, TWO: the second, ...; IMMEDIATE: no wait). This is what paces the game's frames
// to the display: the title submits at most one frame per presentation interval. Without blanks (no vertical blank
// callback installed yet) nothing waits.
void WINAPI D3DDevice_SynchronizeToPresentationInterval(D3DDevice *pDevice)
{
    const DWORD interval = port_of(pDevice)->state.render_states[D3DRS_PRESENTINTERVAL / 4];
    DWORD blanks = 1;
    if (interval & D3DPRESENT_INTERVAL_IMMEDIATE)
        blanks = 0;
    else if (interval & D3DPRESENT_INTERVAL_FOUR)
        blanks = 4;
    else if (interval & D3DPRESENT_INTERVAL_THREE)
        blanks = 3;
    else if (interval & D3DPRESENT_INTERVAL_TWO)
        blanks = 2;
    // The console's frame cadence. Halo 3 asks for D3DPRESENT_INTERVAL_DEFAULT (one blank) and leaves its frame rate
    // to the GPU, which never draws a frame of it in under two blanks: every frame shows one new 30 Hz game tick
    // (main_time_update measures elapsed blanks; game_time_update makes one tick of two). A host fast enough to show
    // every blank would show each tick twice: the scripted cameras (camera_scripting.cpp mode 3/5, the main menu's
    // camera_set_animation_relative) and every object move only on ticks, so every other frame repeats the last, and
    // effects that compare a frame with the previous one alternate (c_screen_postprocess::postprocess_player_view's
    // motion blur: last_world_to_view == world_to_view, no blur; then a whole tick of motion, blur). HALO3_FRAME_BLANKS
    // (port_vk_frame_blanks, default 2) is the least number of blanks a frame takes; 1 shows frames at the display's rate.
    // Above HALO3_FPS=30 the blanks come at the frame rate and the default is 1: the game draws the frames between two
    // ticks interpolated (its render-side interpolation), so no frame repeats the last.
    const DWORD minimum_blanks = port_vk_frame_blanks();
    if (port_vk_frame_rate() == 0u && minimum_blanks <= 1u)
        return;
    if (blanks != 0 && blanks < minimum_blanks)
        blanks = minimum_blanks;
    if (blanks == 0 || !g_vblank.started)
        return;
    const DWORD start = GetTickCount();
    // woken by the blank itself (no polling quantum: at 240 Hz a blank is 4.2 ms); a set event left from an earlier
    // blank only makes the loop look again
    while ((DWORD)(g_vblank.count - g_vblank.swap_count) < blanks && GetTickCount() - start < 100) {
        if (g_vblank.blank_event)
            WaitForSingleObject(g_vblank.blank_event, 10);
        else
            Sleep(1);
    }
}

// ---- presentation, vertical blank, gamma ----

void WINAPI D3DDevice_Present(D3DDevice *pDevice)
{
    (void)pDevice;
    InterlockedIncrement(&g_vblank.swaps);
    g_vblank.swap_count = g_vblank.count;
    if (gpu())
        port_vk_present(0);
}

void WINAPI D3DDevice_Swap(D3DDevice *pDevice, D3DBaseTexture *pFrontBuffer, CONST D3DVIDEO_SCALER_PARAMETERS *pParameters)
{
    (void)pDevice; (void)pParameters;
    if (gpu())
        port_vk_present(pFrontBuffer);
    InterlockedIncrement(&g_vblank.swaps); // shown at the next vertical blank
    g_vblank.swap_count = g_vblank.count;
}

// The rate of the vertical blanks above, for the game's frame clock (main_time_update divides the blanks a frame took
// by it; on the console that is the display's refresh rate, rasterizer_get_refresh_rate). A port extension: the game
// calls it only in its PC build (port/GAME-CHANGES.md section M).
extern "C" unsigned long port_d3d_vertical_blank_rate(void)
{
    return port_vk_vertical_blank_rate();
}

// Whether the game draws frames between two ticks (HALO3_FPS above 30): main_loop_body then publishes every frame in
// which game time advanced, not only those that ran a tick. A port extension (port/GAME-CHANGES.md section M).
extern "C" unsigned long port_d3d_frames_between_ticks(void)
{
    return port_vk_frame_rate() != 30u;
}

// Whether those frames are interpolated (port_vk_frame_interpolation; HALO3_INTERPOLATE=0 draws them as published):
// the game's render-side interpolation asks before each frame it draws.
extern "C" unsigned long port_d3d_frame_interpolation(void)
{
    return port_vk_frame_interpolation();
}

// rasterizer_globals_initialize installs rasterizer_vblank_callback (it reads D3DVBLANKDATA::Swap). The blanks run
// on their own thread from the first install on; NULL uninstalls.
void WINAPI D3DDevice_SetVerticalBlankCallback(D3DDevice *pDevice, D3DVBLANKCALLBACK pCallback)
{
    (void)pDevice;
    g_vblank.callback = pCallback;
    if (pCallback && InterlockedCompareExchange(&g_vblank.started, 1, 0) == 0) {
        g_vblank.blank_event = CreateEvent(0, FALSE, FALSE, 0);
        HANDLE thread = CreateThread(0, 0, vblank_thread, 0, 0, 0);
        if (thread)
            CloseHandle(thread);
        else
            report_once(5, "the vertical blank thread did not start (no vertical blank callbacks)");
    }
}

// The display's gamma: the display controller's LUT, applied to every presented frame (port_vk_set_display_gamma).
// Halo 3's PWL ramp decodes the 7e3 front buffer to linear and applies the display gamma (rasterizer_LUT_control).
//
// The XDK's D3D adapts every ramp to the display before it reaches the LUT (retail d3d9 state.obj: SetGammaRamp at
// 0x825a65a0 and SetPWLGamma at 0x825a6668 ignore their flags and map each 10-bit value through a 1024-entry table
// built at 0x825ac738). The game's ramps are authored for an sRGB display; the table re-encodes them for the display
// VdGetCurrentDisplayGamma reports: type 1 sRGB (identity), 2 BT.709 (an HDTV, what Halo3Rex and xenia report and
// what the reference frames were captured with), 3 a power curve, anything else linear. HALO3_DISPLAY_GAMMA=<type>
// [,<power>] picks another display (default 2).
struct s_display_gamma_table {
    bool built;
    DWORD type;
    float power;
    WORD table[1024];
};
static s_display_gamma_table g_display_gamma;

// The table builder's curves (0x825ac5d8 sRGB, 0x825ac688 BT.709), with its precision: single-precision arithmetic,
// pow in double, constants as the listing loads them.
// fmsubs a * b - c, rounded once: the product of two floats is exact in double.
static float display_fmsubs(float a, float b, float c)
{
    return (float)((double)a * (double)b - (double)c);
}

static float display_srgb_to_linear(float x)
{
    if ((double)x > 0.04045)
        return (float)pow((double)((x + 0.055f) * 0.947867334f), (double)2.4f);
    return x * 0.0773993805f;
}

static float display_linear_to_srgb(float x)
{
    if ((double)x > 0.0031308)
        return display_fmsubs((float)pow((double)x, (double)0.416666657f), 1.055f, 0.055f);
    return x * 12.92f;
}

static float display_linear_to_bt709(float x)
{
    if (!((double)x < 0.018))
        return display_fmsubs((float)pow((double)x, (double)0.45f), 1.099f, 0.099f);
    return x * 4.5f;
}

// VdGetCurrentDisplayGamma as the port reports it (see above; port_vk_display_gamma_query reads the setting).
static void display_gamma_query(DWORD *type, float *power)
{
    uint32_t t = 2;
    port_vk_display_gamma_query(&t, power);
    *type = t;
}

static const WORD *display_gamma_table(void)
{
    DWORD type;
    float power;
    display_gamma_query(&type, &power);
    s_display_gamma_table &t = g_display_gamma;
    if (t.built && t.type == type && t.power == power)
        return t.table;
    for (int i = 0; i < 1024; i++) {
        float v = display_srgb_to_linear((float)i * 0.000977517106f); // i / 1023
        switch (type) {
        case 1: v = display_linear_to_srgb(v); break;
        case 2: v = display_linear_to_bt709(v); break;
        case 3: v = (float)pow((double)v, (double)(1.0f / power)); break;
        default: break;
        }
        const int code = (int)((double)v * 1023.0 + 0.5); // fmadd (the product is exact in double), fctiwz
        t.table[i] = (WORD)(code < 0 ? 0 : code > 1023 ? 1023 : code);
    }
    t.type = type;
    t.power = power;
    t.built = true;
    return t.table;
}

void WINAPI D3DDevice_SetGammaRamp(D3DDevice *pDevice, DWORD UnusedFlags, CONST D3DGAMMARAMP *pRamp)
{
    (void)pDevice; (void)UnusedFlags;
    if (!pRamp)
        return;
    g_display.ramp = *pRamp;
    g_display.ramp_set = true;
    g_display.pwl_set = false;
    if (!gpu())
        return;
    // each 16-bit word's top 10 bits through the table, back in the top 10 bits (0x825ac8c8)
    const WORD *table = display_gamma_table();
    D3DGAMMARAMP display;
    for (int i = 0; i < 256; i++) {
        display.red[i] = (WORD)(table[pRamp->red[i] >> 6] << 6);
        display.green[i] = (WORD)(table[pRamp->green[i] >> 6] << 6);
        display.blue[i] = (WORD)(table[pRamp->blue[i] >> 6] << 6);
    }
    port_vk_set_display_gamma(k_port_vk_display_gamma_ramp, &display.red[0]);
}

void WINAPI D3DDevice_GetGammaRamp(D3DDevice *pDevice, D3DGAMMARAMP *pRamp)
{
    (void)pDevice;
    if (!pRamp)
        return;
    if (g_display.ramp_set) {
        *pRamp = g_display.ramp;
        return;
    }
    for (DWORD i = 0; i < 256; i++) // identity
        pRamp->red[i] = pRamp->green[i] = pRamp->blue[i] = (WORD)(i * 0x0101u);
}

void WINAPI D3DDevice_SetPWLGamma(D3DDevice *pDevice, DWORD Flags, CONST D3DPWLGAMMA *pRamp)
{
    (void)pDevice; (void)Flags;
    if (!pRamp)
        return;
    g_display.pwl = *pRamp;
    g_display.pwl_set = true;
    g_display.ramp_set = false;
    if (!gpu())
        return;
    // each segment's start and end (Base, Base + Delta, 10-bit, the end clamped to 1023) through the table
    // (0x825ac960 / 0x825ac588)
    const WORD *table = display_gamma_table();
    D3DPWLGAMMA display;
    D3DPWLGAMMAENTRY *out[3] = { display.red, display.green, display.blue };
    const D3DPWLGAMMAENTRY *in[3] = { pRamp->red, pRamp->green, pRamp->blue };
    for (int c = 0; c < 3; c++)
        for (int i = 0; i < 128; i++) {
            const DWORD start = (DWORD)in[c][i].Base >> 6;
            DWORD end = start + ((DWORD)in[c][i].Delta >> 6);
            if ((end & 0xffffu) > 1023u)
                end = 1023u;
            out[c][i].Base = (WORD)(table[start] << 6);
            out[c][i].Delta = (WORD)(((DWORD)table[end & 0xffffu] - table[start]) << 6);
        }
    port_vk_set_display_gamma(k_port_vk_display_gamma_pwl, &display.red[0].Base);
}

void WINAPI D3DDevice_GetPWLGamma(D3DDevice *pDevice, D3DPWLGAMMA *pRamp)
{
    (void)pDevice;
    if (!pRamp)
        return;
    if (g_display.pwl_set) {
        *pRamp = g_display.pwl;
        return;
    }
    for (DWORD i = 0; i < 128; i++) { // identity: 128 segments of 512 (10-bit input, 16-bit output)
        D3DPWLGAMMAENTRY e = { (WORD)(i * 512u), 512 };
        pRamp->red[i] = pRamp->green[i] = pRamp->blue[i] = e;
    }
}

// GPR split between vertex and pixel shaders: (0, 0) asks for the default, an even split of the 128.
void WINAPI D3DDevice_SetShaderGPRAllocation(D3DDevice *pDevice, DWORD Flags, DWORD VertexShaderCount, DWORD PixelShaderCount)
{
    s_port_device *pd = port_of(pDevice);
    if (VertexShaderCount == 0 && PixelShaderCount == 0)
        VertexShaderCount = PixelShaderCount = 64;
    pd->gpr_flags = Flags;
    pd->gpr_vertex = VertexShaderCount;
    pd->gpr_pixel = PixelShaderCount;
}

void WINAPI D3DDevice_GetShaderGPRAllocation(D3DDevice *pDevice, DWORD *pFlags, DWORD *pVertexShaderCount,
    DWORD *pPixelShaderCount)
{
    s_port_device *pd = port_of(pDevice);
    if (pFlags)
        *pFlags = pd->gpr_flags;
    if (pVertexShaderCount)
        *pVertexShaderCount = pd->gpr_vertex;
    if (pPixelShaderCount)
        *pPixelShaderCount = pd->gpr_pixel;
}

static volatile LONG g_null_fence_counter = 1;

DWORD WINAPI D3DDevice_InsertFence(D3DDevice *pDevice)
{
    (void)pDevice;
    const DWORD fence = (DWORD)InterlockedIncrement(&g_null_fence_counter);
    port_vk_record_fence(fence);
    return fence;
}

void WINAPI D3DDevice_BlockOnFence(DWORD Fence) { port_vk_record_fence_pending(Fence, 1); }
BOOL WINAPI D3DDevice_IsFencePending(DWORD Fence) { return port_vk_record_fence_pending(Fence, 0); }

// ---- clears and resolves ----

void WINAPI D3DDevice_Clear(D3DDevice *pDevice, DWORD Count, CONST D3DRECT *pRects, DWORD Flags, D3DCOLOR Color,
    float Z, DWORD Stencil, BOOL EDRAMClear)
{
    (void)EDRAMClear;
    if (!gpu())
        return;
    s_port_device *pd = port_of(pDevice);
    const DWORD n = pRects ? (Count < 16 ? Count : 16) : 0;
    int rects[4 * 16];
    for (DWORD i = 0; i < n; i++) {
        rects[4 * i] = pRects[i].x1;
        rects[4 * i + 1] = pRects[i].y1;
        rects[4 * i + 2] = pRects[i].x2;
        rects[4 * i + 3] = pRects[i].y2;
    }
    if (recording_command(pd)) {
        if (DWORD *a = record(pd->recording, k_op_clear, 5u + 4u * n)) {
            a[0] = Flags;
            a[1] = Color;
            a[2] = float_bits(Z);
            a[3] = Stencil;
            a[4] = n;
            memcpy(a + 5, rects, n * 16u);
        }
        return;
    }
    const float color[4] = { ((Color >> 16) & 0xff) / 255.0f, ((Color >> 8) & 0xff) / 255.0f, (Color & 0xff) / 255.0f,
                             (Color >> 24) / 255.0f };
    apply_clear(pd, Flags, color, Z, Stencil, rects, n);
}

void WINAPI D3DDevice_ClearF(D3DDevice *pDevice, DWORD Flags, CONST D3DRECT *pRect, CONST D3DVECTOR4 *pColor,
    float Z, DWORD Stencil)
{
    if (!gpu())
        return;
    s_port_device *pd = port_of(pDevice);
    int rect[4] = { 0, 0, 0, 0 };
    if (pRect) {
        rect[0] = pRect->x1;
        rect[1] = pRect->y1;
        rect[2] = pRect->x2;
        rect[3] = pRect->y2;
    }
    const float color[4] = { pColor ? pColor->x : 0.0f, pColor ? pColor->y : 0.0f, pColor ? pColor->z : 0.0f,
                             pColor ? pColor->w : 0.0f };
    if (recording_command(pd)) {
        if (DWORD *a = record(pd->recording, k_op_clearf, 12)) {
            a[0] = Flags;
            a[1] = pRect ? 1u : 0u;
            __builtin_memcpy(a + 2, rect, 16);
            __builtin_memcpy(a + 6, color, 16);
            a[10] = float_bits(Z);
            a[11] = Stencil;
        }
        return;
    }
    apply_clear(pd, Flags, color, Z, Stencil, rect, pRect ? 1u : 0u);
}

void WINAPI D3DDevice_Resolve(D3DDevice *pDevice, DWORD Flags, CONST D3DRECT *pSourceRect, D3DBaseTexture *pDestTexture,
    CONST D3DPOINT *pDestPoint, UINT DestLevel, UINT DestSliceOrFace, CONST D3DVECTOR4 *pClearColor, float ClearZ,
    DWORD ClearStencil, CONST D3DRESOLVE_PARAMETERS *pParameters)
{
    (void)pParameters;
    if (!gpu())
        return;
    s_port_device *pd = port_of(pDevice);
    int rect[4] = { 0, 0, 0, 0 };
    if (pSourceRect) {
        rect[0] = pSourceRect->x1;
        rect[1] = pSourceRect->y1;
        rect[2] = pSourceRect->x2;
        rect[3] = pSourceRect->y2;
    }
    int point[2] = { pDestPoint ? (int)pDestPoint->x : 0, pDestPoint ? (int)pDestPoint->y : 0 };
    const float color[4] = { pClearColor ? pClearColor->x : 0.0f, pClearColor ? pClearColor->y : 0.0f,
                             pClearColor ? pClearColor->z : 0.0f, pClearColor ? pClearColor->w : 0.0f };
    if (recording_command(pd)) {
        if (DWORD *a = record(pd->recording, k_op_resolve, 18)) {
            a[0] = Flags;
            a[1] = pSourceRect ? 1u : 0u;
            __builtin_memcpy(a + 2, rect, 16);
            a[6] = (DWORD)(size_t)pDestTexture;
            a[7] = pDestPoint ? 1u : 0u;
            a[8] = (DWORD)point[0];
            a[9] = (DWORD)point[1];
            a[10] = DestLevel;
            a[11] = DestSliceOrFace;
            __builtin_memcpy(a + 12, color, 16);
            a[16] = float_bits(ClearZ);
            a[17] = ClearStencil;
        }
        return;
    }
    apply_resolve(pd, Flags, pSourceRect ? rect : 0, pDestTexture, pDestPoint ? point : 0, DestLevel, DestSliceOrFace,
                  color, ClearZ, ClearStencil);
}

// ---- draws ----

void WINAPI D3DDevice_DrawVertices(D3DDevice *pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT VertexCount)
{
    if (!gpu())
        return;
    s_port_device *pd = port_of(pDevice);
    if (recording_command(pd)) {
        if (DWORD *a = record(pd->recording, k_op_draw, 5)) {
            a[0] = (DWORD)PrimitiveType;
            a[1] = StartVertex;
            a[2] = VertexCount;
            a[3] = 0;
            a[4] = 0;
        }
        return;
    }
    apply_draw(pd, (DWORD)PrimitiveType, StartVertex, VertexCount, 0, 0);
}

void WINAPI D3DDevice_DrawIndexedVertices(D3DDevice *pDevice, D3DPRIMITIVETYPE PrimitiveType, INT BaseVertexIndex,
    UINT StartIndex, UINT IndexCount)
{
    if (!gpu())
        return;
    s_port_device *pd = port_of(pDevice);
    if (recording_command(pd)) {
        if (DWORD *a = record(pd->recording, k_op_draw, 5)) {
            a[0] = (DWORD)PrimitiveType;
            a[1] = StartIndex;
            a[2] = IndexCount;
            a[3] = (DWORD)BaseVertexIndex;
            a[4] = 1;
        }
        return;
    }
    apply_draw(pd, (DWORD)PrimitiveType, StartIndex, IndexCount, BaseVertexIndex, 1);
}

void WINAPI D3DDevice_DrawVerticesUP(D3DDevice *pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT VertexCount,
    CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    if (!gpu())
        return;
    s_port_device *pd = port_of(pDevice);
    if (!pVertexStreamZeroData)
        return;
    if (recording_command(pd)) {
        // the data is the caller's (often its stack): the stream keeps a copy
        const DWORD bytes = VertexCount * VertexStreamZeroStride;
        if (DWORD *a = record(pd->recording, k_op_draw_up, 4u + (bytes + 3u) / 4u)) {
            a[0] = (DWORD)PrimitiveType;
            a[1] = VertexCount;
            a[2] = VertexStreamZeroStride;
            a[3] = bytes;
            memcpy(a + 4, pVertexStreamZeroData, bytes);
        }
        return;
    }
    apply_draw_up(pd, (DWORD)PrimitiveType, VertexCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

void WINAPI D3DDevice_DrawIndexedVerticesUP(D3DDevice *pDevice, D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex,
    UINT NumVertices, UINT IndexCount, CONST void *pIndexData, D3DFORMAT IndexDataFormat,
    CONST void *pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
    if (!gpu())
        return;
    s_port_device *pd = port_of(pDevice);
    if (!pVertexStreamZeroData || !pIndexData)
        return;
    const DWORD vertex_bytes = (MinVertexIndex + NumVertices) * VertexStreamZeroStride;
    const DWORD index_bytes = IndexCount * (IndexDataFormat == D3DFMT_INDEX32 ? 4u : 2u);
    if (recording_command(pd)) {
        const DWORD vertex_dwords = (vertex_bytes + 3u) / 4u, index_dwords = (index_bytes + 3u) / 4u;
        if (DWORD *a = record(pd->recording, k_op_draw_indexed_up, 8u + vertex_dwords + index_dwords)) {
            a[0] = (DWORD)PrimitiveType;
            a[1] = MinVertexIndex;
            a[2] = NumVertices;
            a[3] = IndexCount;
            a[4] = (DWORD)IndexDataFormat;
            a[5] = VertexStreamZeroStride;
            a[6] = vertex_bytes;
            a[7] = index_bytes;
            memcpy(a + 8, pVertexStreamZeroData, vertex_bytes);
            memcpy(a + 8 + vertex_dwords, pIndexData, index_bytes);
        }
        return;
    }
    apply_draw_indexed_up(pd, (DWORD)PrimitiveType, vertex_bytes, IndexCount, pIndexData, (DWORD)IndexDataFormat,
                          pVertexStreamZeroData, VertexStreamZeroStride);
}

// Tessellated patches: the Vulkan layer expands them on the CPU (port_vk_draw_tessellated). Halo 3 draws only its
// water this way (render_water.cpp render_water_draw_indexed_tessellated_primitive: D3DTPT_TRIPATCH, D3DTM_ADAPTIVE,
// levels 1..15, the factor buffer the water_tessellation pass exported as the index buffer).
void WINAPI D3DDevice_DrawIndexedTessellatedVertices(D3DDevice *pDevice, D3DTESSPRIMITIVETYPE PrimitiveType,
    INT BaseVertexIndex, UINT StartIndex, UINT IndexCount)
{
    if (!gpu())
        return;
    s_port_device *pd = port_of(pDevice);
    if (recording_command(pd)) {
        report_once(11, "a tessellated draw inside a recorded command buffer (not recorded)");
        return;
    }
    if (!live(pd) || direct_tile_predicated_out(pd))
        return;
    const s_device_state &st = pd->state;
    refresh_placed_resources(pd);
    port_vk_draw_tessellated(&pd->draw, take_draw_dirty(pd), (DWORD)PrimitiveType, BaseVertexIndex, StartIndex,
                             IndexCount,
                             st.render_states[D3DRS_TESSELLATIONMODE / 4],
                             bits_float(st.render_states[D3DRS_MINTESSELLATIONLEVEL / 4]),
                             bits_float(st.render_states[D3DRS_MAXTESSELLATIONLEVEL / 4]));
}

// Memory export (memexport): the draws between BeginExport and EndExport write the resource's memory from their shaders.
// The renderer keeps a device copy of that memory, which the exporting shaders write (port_vk_begin_export). The
// GPU-effect updates export into vertex buffers (their vertex shaders run as compute shaders); the water tessellation
// factors into an index buffer; the tiled screenshot into a texture, from its pixel shader
// (rasterizer_dx9_dynamic_geometry.cpp rasterizer_export_screenshot_tile: the screenshot buffer, which the CPU then
// locks and encodes: port_vk_begin_texture_export, port_vk_texture_lock).
void WINAPI D3DDevice_BeginExport(D3DDevice *pDevice, DWORD Index, D3DResource *pResource, DWORD Flags)
{
    (void)Flags;
    s_port_device *pd = port_of(pDevice);
    if (!gpu() || !pResource)
        return;
    if (pd->recording) {
        report_once(9, "BeginExport inside a recorded command buffer (not recorded)");
        return;
    }
    if ((pResource->Common & D3DCOMMON_TYPE_MASK) == D3DCOMMON_TYPE_INDEXBUFFER) {
        // Halo 3's water tessellation factors (render_water.cpp render_water_part, entry point water_tessellation):
        // one 32-bit float an edge into the index buffer the tessellated draw then reads. The Vulkan layer computes
        // them on the CPU (vulkan_device.cpp water_tessellation_factors).
        const D3DIndexBuffer *ib = (const D3DIndexBuffer *)pResource;
        port_vk_begin_export(Index, (const void *)(size_t)ib->Address, ib->Size, 4u);
        return;
    }
    if ((pResource->Common & D3DCOMMON_TYPE_MASK) == D3DCOMMON_TYPE_TEXTURE) {
        port_vk_begin_texture_export(Index, pResource);
        return;
    }
    if ((pResource->Common & D3DCOMMON_TYPE_MASK) != D3DCOMMON_TYPE_VERTEXBUFFER) {
        report_once(10, "BeginExport of a resource other than a vertex or index buffer or a 2D texture (not modelled)");
        return;
    }
    D3DVertexBuffer *vb = (D3DVertexBuffer *)pResource;
    // the Xbox record size: the stride of the stream the update reads the same buffer through, if one is bound
    const s_vertex_shader_binding *binding = vertex_shader_binding(pd->state.vertex_shader);
    DWORD stride = 0;
    for (int i = 0; i < k_stream_count && stride == 0; i++)
        if (pd->state.streams[i].buffer == vb)
            stride = pd->state.streams[i].stride ? pd->state.streams[i].stride : binding ? binding->strides[i] : 0u;
    port_vk_begin_export(Index, (const void *)((size_t)(DWORD)vb->Format.BaseAddress << 2), vb->Format.Size * 4u, stride);
}

void WINAPI D3DDevice_EndExport(D3DDevice *pDevice, DWORD Index, D3DResource *pResource, DWORD Flags)
{
    (void)pDevice; (void)pResource; (void)Flags;
    if (gpu())
        port_vk_end_export(Index);
}

// The game opens a survey with its own inlined writer (d3d_device_issue_occlusion_query, retail 0x8218f138): it pushes
// (flag << 6 | index) onto queued_queries at ++query_write_index (an unsigned char, no mask) and sets the index's
// pending_queries bit. D3DDevice_EndConditionalSurvey (retail 0x8218f208) pops it: query_write_index - 1, and unless
// Flags bit 0 is set, the entry's pending bit is cleared (after the end packet, which nothing reads here). Without the
// pop the index grows by one per survey (each lens flare and the distortion pass, every frame), overwrites
// query_write_index itself at queued_queries[65] and then walks past the end of the device object (g_tiling follows
// g_hal_device). Conditional rendering stays unmodelled: the bracketed draws always run.
void WINAPI D3DDevice_EndConditionalSurvey(D3DDevice *pDevice, DWORD Flags)
{
    D3D::CDevice *device = port_of(pDevice)->device;
    const unsigned char top = device->query_write_index;
    const unsigned char entry = top < sizeof(device->queued_queries) ? device->queued_queries[top] : 0;
    device->query_write_index = (unsigned char)(top - 1u);
    if ((Flags & 1u) == 0)
        device->pending_queries &= ~(1ull << (entry & 63u));
}
void WINAPI D3DDevice_BeginConditionalRendering(D3DDevice *pDevice, DWORD Identifier) { (void)pDevice; (void)Identifier; }
void WINAPI D3DDevice_EndConditionalRendering(D3DDevice *pDevice) { (void)pDevice; }
void WINAPI D3DDevice_FlushHiZStencil(D3DDevice *pDevice, D3DFHZS_FLUSHTYPE FlushType) { (void)pDevice; (void)FlushType; }

void WINAPI D3DDevice_UnsetAll(D3DDevice *pDevice)
{
    s_port_device *pd = port_of(pDevice);
    apply_unset_all(pd);
    if (pd->recording)
        record(pd->recording, k_op_unset_all, 0);
}

// ---- predicated tiling ----

#ifdef HALO3_64BIT
int port_d3d_tag_shell_bracket()
{
    static const bool enabled = [] {
        const char *v = getenv("HALO3_SHELL_SINGLE_PASS");
        return !v || v[0] != '0';
    }();
    g_shell.tagged = enabled && g_hal.recording == &g_tiling.recording && port_vk_shell_available();
    return g_shell.tagged;
}

void port_d3d_shell_surface(unsigned role, D3DSurface *surface, D3DBaseTexture *texture)
{
    if (!surface || !g_shell.tagged)
        return;
    if (g_shell.count == sizeof(g_shell.views) / sizeof(g_shell.views[0])) {
        g_shell.tagged = false;
        return;
    }
    g_shell.views[g_shell.count++] = {role, surface, texture, (DWORD)texture->Format.BaseAddress};
}
#endif

// The bracket's commands are recorded; EndTiling runs them once per tile (see the top of the file). Flags:
// D3DTILING_SKIP_FIRST_TILE_CLEAR leaves the first tile's EDRAM as it is (the surfaces' contents before the bracket),
// D3DTILING_FIRST_TILE_INHERITS_DEPTH_BUFFER clears only its color; every other tile starts cleared to pClearColor
// (render targets, when given), ClearZ and ClearStencil.
void WINAPI D3DDevice_BeginTiling(D3DDevice *pDevice, DWORD Flags, DWORD Count, CONST D3DRECT *pTileRects,
    CONST D3DVECTOR4 *pClearColor, float ClearZ, DWORD ClearStencil)
{
    s_port_device *pd = port_of(pDevice);
    if (!pd->hal || g_tiling.active || pd->recording) {
        report_once(6, "BeginTiling inside a tiling bracket or a command buffer (ignored)");
        return;
    }
    g_tiling.active = true;
#ifdef HALO3_64BIT
    g_shell = {};
#endif
    g_tiling.direct_single_tile = false;
    g_tiling.epoch++;
    if (!gpu())
        return;
    g_tiling.flags = Flags;
    g_tiling.count = Count < (DWORD)k_max_tiles ? Count : (DWORD)k_max_tiles;
    for (DWORD t = 0; t < g_tiling.count; t++) {
        g_tiling.rects[t][0] = pTileRects ? pTileRects[t].x1 : 0;
        g_tiling.rects[t][1] = pTileRects ? pTileRects[t].y1 : 0;
        g_tiling.rects[t][2] = pTileRects ? pTileRects[t].x2 : 0;
        g_tiling.rects[t][3] = pTileRects ? pTileRects[t].y2 : 0;
    }
    g_tiling.clear_color_given = pClearColor != 0;
    if (pClearColor) {
        g_tiling.clear_color[0] = pClearColor->x;
        g_tiling.clear_color[1] = pClearColor->y;
        g_tiling.clear_color[2] = pClearColor->z;
        g_tiling.clear_color[3] = pClearColor->w;
    }
    g_tiling.clear_z = ClearZ;
    g_tiling.clear_stencil = ClearStencil;
    // the surfaces bound now are the bracket's (retail 0x825a9dbc), until SetSurfaces registers others
    apply_tiling_surfaces(pd, pd->state.render_targets, pd->state.depth_stencil);
    if (g_tiling.count == 1) {
        // Single-tile bracket (e.g. 4K / non-MSAA): execute live without snapshotting, recording, or replaying.
        g_tiling.direct_single_tile = true;
        const int *rect = g_tiling.rects[0];
        g_window.active = true;
        g_window.x = rect[0];
        g_window.y = rect[1];
        g_window.width = rect[2] - rect[0];
        g_window.height = rect[3] - rect[1];
        publish_window(pd);
        DWORD clear = (g_tiling.clear_color_given ? (DWORD)D3DCLEAR_TARGET : 0u) | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
        if (g_tiling.flags & D3DTILING_SKIP_FIRST_TILE_CLEAR)
            clear = 0;
        if (g_tiling.flags & D3DTILING_FIRST_TILE_INHERITS_DEPTH_BUFFER)
            clear &= ~(DWORD)(D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
        if (clear && live(pd)) {
            const int tile_rect[4] = { 0, 0, g_window.width, g_window.height };
            port_vk_clear_edram(clear, g_tiling.clear_color, g_tiling.clear_z, g_tiling.clear_stencil, tile_rect);
        }
        return;
    }
    snapshot_take(pd, &g_tiling.begin);
    // the begin snapshot holds every shadow value, sent or pending: the stream starts with nothing pending
    for (int i = 0; i < 5; i++)
        if (i != 2)
            pd->device->m_Pending.m_Mask[i] = 0;
    recording_reset(&g_tiling.recording);
    pd->recording = &g_tiling.recording;
}

HRESULT WINAPI D3DDevice_EndTiling(D3DDevice *pDevice, DWORD ResolveFlags, CONST D3DRECT *pResolveRects,
    D3DBaseTexture *pDestTexture, CONST D3DVECTOR4 *pClearColor, float ClearZ, DWORD ClearStencil,
    CONST D3DRESOLVE_PARAMETERS *pParameters)
{
    (void)pParameters;
    s_port_device *pd = port_of(pDevice);
    if (!gpu()) {
        if (!pd->hal || !g_tiling.active) {
            report_once(7, "EndTiling without BeginTiling (ignored)");
            return D3DERR_INVALIDCALL;
        }
        pd->state.predication = 0;
        g_tiling.active = false;
        g_tiling.direct_single_tile = false;
        return S_OK;
    }
    if (!pd->hal || !g_tiling.active) {
        report_once(7, "EndTiling without BeginTiling (ignored)");
        return D3DERR_INVALIDCALL;
    }
    if (g_tiling.direct_single_tile) {
        pd->state.predication = 0;
        if (ResolveFlags && pDestTexture) {
            const float resolve_color[4] = { pClearColor ? pClearColor->x : 0.0f, pClearColor ? pClearColor->y : 0.0f,
                                             pClearColor ? pClearColor->z : 0.0f, pClearColor ? pClearColor->w : 0.0f };
            const int *source = g_tiling.rects[0];
            int resolve_rect[4];
            if (pResolveRects) {
                resolve_rect[0] = pResolveRects[0].x1;
                resolve_rect[1] = pResolveRects[0].y1;
                resolve_rect[2] = pResolveRects[0].x2;
                resolve_rect[3] = pResolveRects[0].y2;
                source = resolve_rect;
            }
            const int point[2] = { source[0], source[1] };
            apply_resolve(pd, ResolveFlags, source, pDestTexture, point, 0, 0, resolve_color, ClearZ, ClearStencil);
        }
        g_window.active = false;
        g_tiling.direct_single_tile = false;
        g_tiling.active = false;
        publish_window(pd);
        return S_OK;
    }
    if (pd->recording != &g_tiling.recording) {
        report_once(7, "EndTiling without BeginTiling (ignored)");
        return D3DERR_INVALIDCALL;
    }
    s_recording *r = &g_tiling.recording;
    finish_payload(r);
    pd->recording = 0;
    snapshot_take(pd, &g_tiling_end);
    const float resolve_color[4] = { pClearColor ? pClearColor->x : 0.0f, pClearColor ? pClearColor->y : 0.0f,
                                     pClearColor ? pClearColor->z : 0.0f, pClearColor ? pClearColor->w : 0.0f };
#ifdef HALO3_64BIT
    if (!g_shell.tagged || ResolveFlags || pDestTexture || pResolveRects || pParameters || !try_execute_shell(pd, *r))
#endif
    for (DWORD t = 0; t < g_tiling.count; t++) {
        g_tiling_replay_tile = t;
        const int *rect = g_tiling.rects[t];
        g_window.active = true;
        g_window.x = rect[0];
        g_window.y = rect[1];
        g_window.width = rect[2] - rect[0];
        g_window.height = rect[3] - rect[1];
        snapshot_restore(pd, &g_tiling.begin);
        // the tile's EDRAM starts cleared (but the first tile's, as the flags say)
        DWORD clear = (g_tiling.clear_color_given ? (DWORD)D3DCLEAR_TARGET : 0u) | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL;
        if (t == 0 && (g_tiling.flags & D3DTILING_SKIP_FIRST_TILE_CLEAR))
            clear = 0;
        if (t == 0 && (g_tiling.flags & D3DTILING_FIRST_TILE_INHERITS_DEPTH_BUFFER))
            clear &= ~(DWORD)(D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL);
        // an EDRAM clear: it hits whatever surface the tile's commands place over the bound targets' EDRAM (the
        // tile-size G-buffer, light and depth surfaces), not the full-size surfaces bound at BeginTiling
        if (clear && live(pd)) {
            const int tile_rect[4] = { 0, 0, g_window.width, g_window.height };
            port_vk_clear_edram(clear, g_tiling.clear_color, g_tiling.clear_z, g_tiling.clear_stencil, tile_rect);
        }
        execute(pd, r->ops + k_payload_header_dwords, r->count - k_payload_header_dwords, D3DPRED_TILE(t));
        pd->state.predication = 0;
        if (ResolveFlags && pDestTexture) {
            const int *source = rect;
            int resolve_rect[4];
            if (pResolveRects) {
                resolve_rect[0] = pResolveRects[t].x1;
                resolve_rect[1] = pResolveRects[t].y1;
                resolve_rect[2] = pResolveRects[t].x2;
                resolve_rect[3] = pResolveRects[t].y2;
                source = resolve_rect;
            }
            const int point[2] = { source[0], source[1] };
            apply_resolve(pd, ResolveFlags, source, pDestTexture, point, 0, 0, resolve_color, ClearZ, ClearStencil);
        }
    }
    g_tiling_replay_tile = 0;
    g_window.active = false;
    snapshot_restore(pd, &g_tiling_end);
    g_tiling.active = false;
    return S_OK;
}

// Commands recorded after this run only for the tiles the mask selects (D3DPRED_TILE(t)); 0 runs them for all.
// Outside a tiling bracket every tile is selected.
void WINAPI D3DDevice_SetPredication(D3DDevice *pDevice, DWORD PredicationMask)
{
    s_port_device *pd = port_of(pDevice);
    pd->state.predication = PredicationMask;
    if (pd->recording) {
        if (DWORD *a = record(pd->recording, k_op_predication, 1))
            a[0] = PredicationMask;
    }
}

// ---- command buffers ----

// pInheritTags: the registers the stream takes from the device that runs it (NULL: none); the rest are pending
// from the start, so the stream sets them at its first draw. pPersistTags: what stays set on that device after it
// (NULL: nothing). Tiling rects: the recorded stream is run inside the tiling bracket, once per tile.
void WINAPI D3DDevice_BeginCommandBuffer(D3DDevice *pDevice, D3DCommandBuffer *pCommandBuffer, DWORD Flags,
    CONST D3DTAGCOLLECTION *pInheritTags, CONST D3DTAGCOLLECTION *pPersistTags, CONST D3DRECT *pTilingRects, DWORD TileCount)
{
    (void)Flags; (void)pTilingRects; (void)TileCount;
    s_port_device *pd = port_of(pDevice);
    s_port_command_buffer *cb = port_command_buffer(pCommandBuffer);
    if (!cb || !cb->recording || pd->recording || pd->recording_noop || pd->hal) {
        report_once(8, "BeginCommandBuffer on a device or command buffer that cannot record (ignored)");
        return;
    }
    static const D3DTAGCOLLECTION k_none = { { 0, 0, 0, 0, 0 } };
    const D3DTAGCOLLECTION &inherit = pInheritTags ? *pInheritTags : k_none;
    cb->header.m_Inherited = inherit;
    cb->header.m_Persisted = pPersistTags ? *pPersistTags : k_none;
    s_recording *r = cb->recording;
    recording_reset(r);
    if (!gpu()) {
        finish_payload(r);
        cb->payload = r->ops;
        cb->payload_size = payload_bytes(r);
        pd->recording_noop = true;
        return;
    }
    D3D::CDevice *d = pd->device;
    for (int i = 0; i < 5; i++)
        d->m_Pending.m_Mask[i] = ~inherit.m_Mask[i];
    r->force_vertex_shader = (inherit.m_Mask[2] & tag_bit(k_tag_vertex_shader)) == 0;
    r->force_pixel_shader = (inherit.m_Mask[2] & tag_bit(k_tag_pixel_shader)) == 0;
    for (DWORD i = 0; i < 4; i++)
        if ((inherit.m_Mask[2] & tag_bit(k_tag_blend_control[i])) == 0)
            r->force_blend |= 1u << i;
    pd->recording = r;
}

HRESULT WINAPI D3DDevice_EndCommandBuffer(D3DDevice *pDevice)
{
    s_port_device *pd = port_of(pDevice);
    if (pd->recording_noop) {
        pd->recording_noop = false;
        return S_OK;
    }
    s_recording *r = pd->recording;
    if (!r || !r->owner)
        return D3DERR_INVALIDCALL;
    capture_pending(pd); // what is pending at the end is part of the stream (and persists on the running device)
    finish_payload(r);
    s_port_command_buffer *cb = r->owner;
    cb->payload = r->ops;
    cb->payload_size = payload_bytes(r);
    pd->recording = 0;
    return r->failed ? E_OUTOFMEMORY : S_OK;
}

// Header part: the D3DCommandBuffer and where the stream is; physical part: the stream (payload header + ops),
// 32-byte multiple. Either may be NULL (render_method_submit.cpp asks for the sizes first).
void WINAPI D3DCommandBuffer_GetClone(D3DCommandBuffer *pThis, DWORD Flags, D3DCommandBuffer *pHeaderPart,
    DWORD *pHeaderSize, void *pPhysicalPart, DWORD *pPhysicalSize)
{
    (void)Flags;
    s_port_command_buffer *source = port_command_buffer(pThis);
    if (!source) {
        if (pHeaderSize)
            *pHeaderSize = 0;
        if (pPhysicalSize)
            *pPhysicalSize = 0;
        return;
    }
    const DWORD payload_size = source->payload_size;
    if (pHeaderSize)
        *pHeaderSize = sizeof(s_port_command_buffer);
    if (pPhysicalSize)
        *pPhysicalSize = payload_size;
    if (pPhysicalPart && source->payload) {
        const DWORD used = (source->payload[2] + k_payload_header_dwords) * 4u;
        memcpy(pPhysicalPart, source->payload, used);
        memset((char *)pPhysicalPart + used, 0, payload_size - used);
    }
    if (pHeaderPart) {
        s_port_command_buffer *clone = (s_port_command_buffer *)pHeaderPart;
        memset(clone, 0, sizeof(*clone));
        clone->header.Common = D3DCOMMON_TYPE_COMMANDBUFFER;
        clone->header.ReferenceCount = 1;
        clone->header.Identifier = (DWORD)(size_t)pPhysicalPart;
        clone->header.m_Inherited = source->header.m_Inherited;
        clone->header.m_Persisted = source->header.m_Persisted;
        clone->magic = k_command_buffer_magic;
        clone->payload = (const DWORD *)pPhysicalPart;
        clone->payload_size = pPhysicalPart ? payload_size : 0;
    }
}

// PredicationSelect: the tiles the stream's predicated commands run for (the game: 0x3fffffff, all).
void WINAPI D3DDevice_RunCommandBuffer(D3DDevice *pDevice, D3DCommandBuffer *pD3dCommandBuffer, DWORD PredicationSelect)
{
    if (!gpu())
        return;
    s_port_device *pd = port_of(pDevice);
    s_port_command_buffer *cb = port_command_buffer(pD3dCommandBuffer);
    if (!cb) {
        report_once(1, "RunCommandBuffer of a command buffer without a recorded stream (skipped)");
        return;
    }
    if (recording_command(pd)) {
        if (DWORD *a = record(pd->recording, k_op_run_command_buffer, 2)) {
            a[0] = (DWORD)(size_t)cb;
            a[1] = PredicationSelect;
        }
        if (pd->hal && g_tiling.active)
            cb->run_epoch = g_tiling.epoch;
        return;
    }
    if (pd->hal && g_tiling.direct_single_tile) {
        if (direct_tile_predicated_out(pd))
            return;
        run_command_buffer(pd, cb, PredicationSelect & D3DPRED_TILE(0));
        return;
    }
    run_command_buffer(pd, cb, PredicationSelect);
}

// Binds the shader to a declaration and its stream strides (one per stream the declaration reads, up to its last),
// so it draws with no vertex declaration set. The pixel shader the XDK links interpolators against needs nothing
// here: the renderer links each pipeline's stages itself.
void WINAPI D3DVertexShader_Bind(D3DVertexShader *pThis, DWORD Flags, D3DVertexDeclaration *pVertexDeclaration,
    CONST DWORD *pStreamStrides, D3DPixelShader *pPixelShader)
{
    (void)Flags; (void)pPixelShader;
    if (!pThis)
        return;
    s_vertex_shader_binding *b = (s_vertex_shader_binding *)((char *)pThis + k_vertex_shader_binding);
    memset(b, 0, sizeof(*b));
    b->marker = k_vertex_binding_marker;
    b->declaration = pVertexDeclaration;
    if (pVertexDeclaration && pStreamStrides) {
        const s_null_object *decl = (const s_null_object *)pVertexDeclaration;
        const D3DVERTEXELEMENT9 *elements = (const D3DVERTEXELEMENT9 *)(size_t)decl->payload[0];
        DWORD streams = 0;
        for (DWORD e = 0; elements && e < decl->payload[1]; e++)
            if (elements[e].Stream < k_stream_count && elements[e].Stream + 1u > streams)
                streams = elements[e].Stream + 1u;
        for (DWORD s = 0; s < streams; s++)
            b->strides[s] = pStreamStrides[s];
    }
    // the bound flag, in the low byte of the container word in the object's own byte order
    BYTE *flags = (BYTE *)pThis + k_vertex_shader_container;
    DWORD word;
    __builtin_memcpy(&word, flags, 4);
    const bool big = (word & 0xffffff00u) != k_container_magic && (swap32(word) & 0xffffff00u) == k_container_magic;
    flags[big ? 3 : 0] |= (BYTE)k_vertex_shader_bound;
    if (pThis == g_hal.state.vertex_shader && publishes_draw(&g_hal))
        publish_declaration(&g_hal);
}

// ---- shader constants ----

static __declspec(align(16)) D3DVECTOR4 g_gpu_constant_scratch[256];

// The "GPU-owned" constant paths: the registers are written by the command stream itself, not from the shadow. On
// a recording device the caller writes straight into the stream (an op whose data it fills before its next D3D
// call); otherwise into the register shadow the renderer gathers from.
D3DVECTOR4 *WINAPI D3DDevice_GpuBeginShaderConstantF4(D3DDevice *pDevice, BOOL PixelShader, UINT StartRegister, DWORD Vector4fCount)
{
    if (!gpu())
        return g_gpu_constant_scratch;
    s_port_device *pd = port_of(pDevice);
    if (StartRegister + Vector4fCount > 256)
        return g_gpu_constant_scratch;
    if (pd->recording) {
        DWORD *a = record_aligned(pd->recording, k_op_gpu_constants_f, 3, 4u * Vector4fCount);
        if (!a)
            return g_gpu_constant_scratch;
        a[0] = PixelShader ? 1u : 0u;
        a[1] = StartRegister;
        a[2] = Vector4fCount;
        memset(a + 3, 0, Vector4fCount * 16u);
        return (D3DVECTOR4 *)(a + 3);
    }
    change_float_range(pd, PixelShader != 0, StartRegister, Vector4fCount);
    return PixelShader ? &pd->device->m_Constants.PixelShaderF[StartRegister]
                       : &pd->device->m_Constants.VertexShaderF[StartRegister];
}

void WINAPI D3DDevice_GpuLoadShaderConstantF4Pointer(D3DDevice *pDevice, BOOL PixelShader, UINT StartRegister,
    CONST void *pConstantData, DWORD Vector4fCount)
{
    if (!gpu())
        return;
    s_port_device *pd = port_of(pDevice);
    if (!pConstantData || StartRegister + Vector4fCount > 256)
        return;
    if (pd->recording) {
        if (DWORD *a = record_aligned(pd->recording, k_op_gpu_constants_f, 3, 4u * Vector4fCount)) {
            a[0] = PixelShader ? 1u : 0u;
            a[1] = StartRegister;
            a[2] = Vector4fCount;
            copy_vectors(a + 3, pConstantData, Vector4fCount);
        }
        return;
    }
    D3DVECTOR4 *shadow = PixelShader ? pd->device->m_Constants.PixelShaderF : pd->device->m_Constants.VertexShaderF;
    copy_vectors(&shadow[StartRegister], pConstantData, Vector4fCount);
    change_float_range(pd, PixelShader != 0, StartRegister, Vector4fCount);
}

void WINAPI D3DDevice_SetVertexShaderConstantFN(D3DDevice *pDevice, UINT StartRegister, CONST float *pConstantData,
    DWORD Vector4fCount, UINT64 PendingMask0)
{
    if (!gpu())
        return;
    if (pConstantData && StartRegister + Vector4fCount <= 256) {
        s_port_device *pd = port_of(pDevice);
        D3DVECTOR4 *dst = &pDevice->m_Constants.VertexShaderF[StartRegister];
        if (!pd->recording && equal_vectors(dst, pConstantData, Vector4fCount))
            return;
        copy_vectors(dst, pConstantData, Vector4fCount);
        pDevice->m_Pending.m_Mask[0] |= PendingMask0;
    }
}

void WINAPI D3DDevice_SetPixelShaderConstantFN(D3DDevice *pDevice, UINT StartRegister, CONST float *pConstantData,
    DWORD Vector4fCount, UINT64 PendingMask1)
{
    if (!gpu())
        return;
    if (pConstantData && StartRegister + Vector4fCount <= 256) {
        s_port_device *pd = port_of(pDevice);
        D3DVECTOR4 *dst = &pDevice->m_Constants.PixelShaderF[StartRegister];
        if (!pd->recording && equal_vectors(dst, pConstantData, Vector4fCount))
            return;
        copy_vectors(dst, pConstantData, Vector4fCount);
        pDevice->m_Pending.m_Mask[1] |= PendingMask1;
    }
}

} // extern "C"

namespace {

// Bool registers: register r is bit r % 32 of dword r / 32 (the layout the renderer's gathers read).
bool set_bools(DWORD *bits, UINT count_bits, UINT StartRegister, CONST BOOL *pConstantData, UINT BoolCount)
{
    bool changed = false;
    for (UINT i = 0; pConstantData && i < BoolCount && StartRegister + i < count_bits; i++) {
        const UINT r = StartRegister + i;
        const DWORD mask = 1u << (r & 31u);
        const DWORD prev = bits[r >> 5];
        const DWORD next = pConstantData[i] ? (prev | mask) : (prev & ~mask);
        if (next != prev) {
            bits[r >> 5] = next;
            changed = true;
        }
    }
    return changed;
}

// Integer registers: GPU_INTEGER_CONSTANT (count, start, step in bytes 0, 1, 2).
bool set_ints(DWORD *registers, UINT count, UINT StartRegister, CONST int *pConstantData, DWORD Vector4iCount)
{
    bool changed = false;
    for (UINT i = 0; pConstantData && i < Vector4iCount && StartRegister + i < count; i++) {
        const int *v = pConstantData + 4 * i;
        const DWORD val = ((DWORD)v[0] & 0xffu) | ((DWORD)v[1] & 0xffu) << 8 | ((DWORD)v[2] & 0xffu) << 16;
        if (registers[StartRegister + i] != val) {
            registers[StartRegister + i] = val;
            changed = true;
        }
    }
    return changed;
}

void flow_pending(D3DDevice *device, bool changed)
{
    s_port_device *pd = port_of(device);
    if (!changed && !pd->recording)
        return;
    device->m_Pending.m_Mask[4] |= tag_bit(k_tag_flow);
    change_flow(pd);
}

} // namespace

extern "C" {

void WINAPI D3DDevice_SetVertexShaderConstantB(D3DDevice *pDevice, UINT StartRegister, CONST BOOL *pConstantData, UINT BoolCount)
{
    if (!gpu())
        return;
    flow_pending(pDevice, set_bools(pDevice->m_Constants.VertexShaderB, GPU_D3D_VERTEX_CONSTANTB_COUNT, StartRegister, pConstantData, BoolCount));
}

void WINAPI D3DDevice_SetPixelShaderConstantB(D3DDevice *pDevice, UINT StartRegister, CONST BOOL *pConstantData, UINT BoolCount)
{
    if (!gpu())
        return;
    flow_pending(pDevice, set_bools(pDevice->m_Constants.PixelShaderB, GPU_D3D_PIXEL_CONSTANTB_COUNT, StartRegister, pConstantData, BoolCount));
}

void WINAPI D3DDevice_SetVertexShaderConstantI(D3DDevice *pDevice, UINT StartRegister, CONST int *pConstantData, DWORD Vector4iCount)
{
    if (!gpu())
        return;
    flow_pending(pDevice, set_ints(pDevice->m_Constants.VertexShaderI, GPU_D3D_VERTEX_CONSTANTI_COUNT, StartRegister, pConstantData, Vector4iCount));
}

void WINAPI D3DDevice_SetPixelShaderConstantI(D3DDevice *pDevice, UINT StartRegister, CONST int *pConstantData, DWORD Vector4iCount)
{
    if (!gpu())
        return;
    flow_pending(pDevice, set_ints(pDevice->m_Constants.PixelShaderI, GPU_D3D_PIXEL_CONSTANTI_COUNT, StartRegister, pConstantData, Vector4iCount));
}

// ---- state the renderer draws with ----

void WINAPI D3DDevice_SetVertexShader(D3DDevice *pDevice, D3DVertexShader *pShader)
{
    s_port_device *pd = port_of(pDevice);
    apply_vertex_shader(pd, pShader);
    if (pd->recording) {
        pd->recording->force_vertex_shader = false;
        if (DWORD *a = record(pd->recording, k_op_vertex_shader, 1))
            a[0] = (DWORD)(size_t)pShader;
    }
}

void WINAPI D3DDevice_SetPixelShader(D3DDevice *pDevice, D3DPixelShader *pShader)
{
    s_port_device *pd = port_of(pDevice);
    apply_pixel_shader(pd, pShader);
    if (pd->recording) {
        pd->recording->force_pixel_shader = false;
        if (DWORD *a = record(pd->recording, k_op_pixel_shader, 1))
            a[0] = (DWORD)(size_t)pShader;
    }
}

void WINAPI D3DDevice_SetVertexDeclaration(D3DDevice *pDevice, D3DVertexDeclaration *pDecl)
{
    s_port_device *pd = port_of(pDevice);
    apply_declaration(pd, pDecl);
    if (pd->recording) {
        if (DWORD *a = record(pd->recording, k_op_declaration, 1))
            a[0] = (DWORD)(size_t)pDecl;
    }
}

void WINAPI D3DDevice_SetStreamSource(D3DDevice *pDevice, UINT StreamNumber, D3DVertexBuffer *pStreamData, UINT OffsetInBytes,
    UINT Stride, UINT64 PendingMask3)
{
    s_port_device *pd = port_of(pDevice);
    if (StreamNumber >= (UINT)k_stream_count)
        return;
    if (!apply_stream(pd, StreamNumber, pStreamData, OffsetInBytes, Stride) && !pd->recording)
        return;
    pDevice->m_Pending.m_Mask[3] |= PendingMask3;
    if (pd->recording) {
        if (DWORD *a = record(pd->recording, k_op_stream, 4)) {
            a[0] = StreamNumber;
            a[1] = (DWORD)(size_t)pStreamData;
            a[2] = OffsetInBytes;
            a[3] = Stride;
        }
    }
}

void WINAPI D3DDevice_SetIndices(D3DDevice *pDevice, D3DIndexBuffer *pIndexData)
{
    s_port_device *pd = port_of(pDevice);
    apply_indices(pd, pIndexData);
    if (pd->recording) {
        if (DWORD *a = record(pd->recording, k_op_indices, 1))
            a[0] = (DWORD)(size_t)pIndexData;
    }
}

// The texture of a sampler is part of its fetch constant: pending until the next draw records it. A NULL texture
// leaves the fetch constant alone, as Xbox D3D does (s_device_state::unbound): the sampler keeps fetching the texture
// it held, here described as the header read when it is unbound, and nothing becomes pending.
void WINAPI D3DDevice_SetTexture(D3DDevice *pDevice, DWORD Sampler, D3DBaseTexture *pTexture, UINT64 PendingMask3)
{
    s_port_device *pd = port_of(pDevice);
    if (Sampler >= (DWORD)k_fetch_count)
        return;
    if (!pTexture && !pd->state.textures[Sampler])
        return;
    if (pTexture && publishes_draw(pd) && pd->state.textures[Sampler] == pTexture &&
        Sampler < PORT_VK_SAMPLER_COUNT && texture_header_unchanged(g_texture_headers[Sampler], pTexture))
        return;
    apply_texture(pd, Sampler, pTexture);
    if (!pTexture)
        return;
    pDevice->m_Pending.m_Mask[3] |= PendingMask3;
    mark_sampler_pending(pd, Sampler);
}

void WINAPI D3DDevice_SetViewport(D3DDevice *pDevice, CONST D3DVIEWPORT9 *pViewport)
{
    s_port_device *pd = port_of(pDevice);
    if (!pViewport)
        return;
    float nv[6];
    nv[0] = (float)pViewport->X;
    nv[1] = (float)pViewport->Y;
    nv[2] = (float)pViewport->Width;
    nv[3] = (float)pViewport->Height;
    nv[4] = pViewport->MinZ;
    nv[5] = pViewport->MaxZ;
    if (!apply_viewport(pd, nv) && !pd->recording)
        return;
    if (pd->recording) {
        if (DWORD *a = record(pd->recording, k_op_viewport, 6))
            for (int i = 0; i < 6; i++)
                a[i] = float_bits(nv[i]);
    }
}

void WINAPI D3DDevice_SetViewportF(D3DDevice *pDevice, CONST D3DVIEWPORTF9 *pViewportF)
{
    s_port_device *pd = port_of(pDevice);
    if (!pViewportF)
        return;
    float nv[6];
    nv[0] = pViewportF->X;
    nv[1] = pViewportF->Y;
    nv[2] = pViewportF->Width;
    nv[3] = pViewportF->Height;
    nv[4] = pViewportF->MinZ;
    nv[5] = pViewportF->MaxZ;
    if (!apply_viewport(pd, nv) && !pd->recording)
        return;
    if (pd->recording) {
        if (DWORD *a = record(pd->recording, k_op_viewport, 6))
            for (int i = 0; i < 6; i++)
                a[i] = float_bits(nv[i]);
    }
}

void WINAPI D3DDevice_GetViewport(D3DDevice *pDevice, D3DVIEWPORT9 *pViewport)
{
    s_port_device *pd = port_of(pDevice);
    if (!pViewport)
        return;
    const float *v = pd->state.viewport;
    pViewport->X = (DWORD)v[0];
    pViewport->Y = (DWORD)v[1];
    pViewport->Width = (DWORD)v[2];
    pViewport->Height = (DWORD)v[3];
    pViewport->MinZ = v[4];
    pViewport->MaxZ = v[5];
}

void WINAPI D3DDevice_SetScissorRect(D3DDevice *pDevice, CONST RECT *pRect)
{
    s_port_device *pd = port_of(pDevice);
    if (!pRect)
        return;
    if (!apply_scissor(pd, *pRect) && !pd->recording)
        return;
    if (pd->recording) {
        if (DWORD *a = record(pd->recording, k_op_scissor, 4)) {
            a[0] = (DWORD)pRect->left;
            a[1] = (DWORD)pRect->top;
            a[2] = (DWORD)pRect->right;
            a[3] = (DWORD)pRect->bottom;
        }
    }
}

// RB_BLENDCONTROLi of one render target (D3DBLENDSTATE has the register's layout); D3DRS_SRCBLEND and the other
// blend states write all four.
void WINAPI D3DDevice_SetBlendState(D3DDevice *pDevice, DWORD RenderTargetIndex, D3DBLENDSTATE BlendState)
{
    s_port_device *pd = port_of(pDevice);
    if (RenderTargetIndex >= 4)
        return;
    DWORD value;
    __builtin_memcpy(&value, &BlendState, 4);
    write_blend_control(pd, RenderTargetIndex, value);
    if (pd->recording) {
        pd->recording->force_blend &= ~(1u << RenderTargetIndex);
        if (DWORD *a = record(pd->recording, k_op_blend_state, 2)) {
            a[0] = RenderTargetIndex;
            a[1] = value;
        }
    }
}

void WINAPI D3DDevice_GetBlendState(D3DDevice *pDevice, DWORD RenderTargetIndex, D3DBLENDSTATE *pBlendState)
{
    s_port_device *pd = port_of(pDevice);
    if (!pBlendState || RenderTargetIndex >= 4)
        return;
    __builtin_memcpy(pBlendState, &pd->state.blend_control[RenderTargetIndex], 4);
}

// User clip planes (clip space), enabled by D3DRS_CLIPPLANEENABLE; the shadow is the device's m_ClipPlanes.
void WINAPI D3DDevice_SetClipPlane(D3DDevice *pDevice, DWORD Index, CONST float *pPlane)
{
    s_port_device *pd = port_of(pDevice);
    if (!pPlane || Index >= D3DMAXUSERCLIPPLANES)
        return;
    apply_clip_plane(pd, Index, pPlane);
    if (pd->recording) {
        if (DWORD *a = record(pd->recording, k_op_clip_plane, 5)) {
            a[0] = Index;
            __builtin_memcpy(a + 1, pPlane, 16);
        }
    }
}

void WINAPI D3DDevice_GetClipPlane(D3DDevice *pDevice, DWORD Index, float *pPlane)
{
    if (!pPlane || Index >= D3DMAXUSERCLIPPLANES)
        return;
    __builtin_memcpy(pPlane, pDevice->m_ClipPlanes[Index], 16);
}

// The tag shaders' D3D objects (scenario_tags_fixup at map load). pPhysicalPart is what the game passes, the object's
// Identifier read in host order: for a big-endian tag object it is byte-swapped, so the object itself is read.
VOID WINAPI XGRegisterVertexShader(IDirect3DVertexShader9 *pShader, VOID *pPhysicalPart)
{
    (void)pPhysicalPart;
    register_tag_object(pShader, PORT_VK_STAGE_VERTEX);
}

VOID WINAPI XGRegisterPixelShader(IDirect3DPixelShader9 *pShader, VOID *pPhysicalPart)
{
    (void)pPhysicalPart;
    register_tag_object(pShader, PORT_VK_STAGE_PIXEL);
}

// ---- render and sampler states set by name (the inline D3DDevice_SetRenderState with a constant state) ----

#define PORT_RENDER_STATES(X) \
    X(D3DRS_ZENABLE, ZEnable) \
    X(D3DRS_ZFUNC, ZFunc) \
    X(D3DRS_ZWRITEENABLE, ZWriteEnable) \
    X(D3DRS_FILLMODE, FillMode) \
    X(D3DRS_CULLMODE, CullMode) \
    X(D3DRS_ALPHABLENDENABLE, AlphaBlendEnable) \
    X(D3DRS_SEPARATEALPHABLENDENABLE, SeparateAlphaBlendEnable) \
    X(D3DRS_BLENDFACTOR, BlendFactor) \
    X(D3DRS_SRCBLEND, SrcBlend) \
    X(D3DRS_DESTBLEND, DestBlend) \
    X(D3DRS_BLENDOP, BlendOp) \
    X(D3DRS_SRCBLENDALPHA, SrcBlendAlpha) \
    X(D3DRS_DESTBLENDALPHA, DestBlendAlpha) \
    X(D3DRS_BLENDOPALPHA, BlendOpAlpha) \
    X(D3DRS_ALPHATESTENABLE, AlphaTestEnable) \
    X(D3DRS_ALPHAREF, AlphaRef) \
    X(D3DRS_ALPHAFUNC, AlphaFunc) \
    X(D3DRS_STENCILENABLE, StencilEnable) \
    X(D3DRS_TWOSIDEDSTENCILMODE, TwoSidedStencilMode) \
    X(D3DRS_STENCILFAIL, StencilFail) \
    X(D3DRS_STENCILZFAIL, StencilZFail) \
    X(D3DRS_STENCILPASS, StencilPass) \
    X(D3DRS_STENCILFUNC, StencilFunc) \
    X(D3DRS_STENCILREF, StencilRef) \
    X(D3DRS_STENCILMASK, StencilMask) \
    X(D3DRS_STENCILWRITEMASK, StencilWriteMask) \
    X(D3DRS_CCW_STENCILFAIL, CCWStencilFail) \
    X(D3DRS_CCW_STENCILZFAIL, CCWStencilZFail) \
    X(D3DRS_CCW_STENCILPASS, CCWStencilPass) \
    X(D3DRS_CCW_STENCILFUNC, CCWStencilFunc) \
    X(D3DRS_CCW_STENCILREF, CCWStencilRef) \
    X(D3DRS_CCW_STENCILMASK, CCWStencilMask) \
    X(D3DRS_CCW_STENCILWRITEMASK, CCWStencilWriteMask) \
    X(D3DRS_CLIPPLANEENABLE, ClipPlaneEnable) \
    X(D3DRS_POINTSIZE, PointSize) \
    X(D3DRS_POINTSIZE_MIN, PointSizeMin) \
    X(D3DRS_POINTSPRITEENABLE, PointSpriteEnable) \
    X(D3DRS_POINTSIZE_MAX, PointSizeMax) \
    X(D3DRS_MULTISAMPLEANTIALIAS, MultiSampleAntiAlias) \
    X(D3DRS_MULTISAMPLEMASK, MultiSampleMask) \
    X(D3DRS_SCISSORTESTENABLE, ScissorTestEnable) \
    X(D3DRS_SLOPESCALEDEPTHBIAS, SlopeScaleDepthBias) \
    X(D3DRS_DEPTHBIAS, DepthBias) \
    X(D3DRS_COLORWRITEENABLE, ColorWriteEnable) \
    X(D3DRS_COLORWRITEENABLE1, ColorWriteEnable1) \
    X(D3DRS_COLORWRITEENABLE2, ColorWriteEnable2) \
    X(D3DRS_COLORWRITEENABLE3, ColorWriteEnable3) \
    X(D3DRS_TESSELLATIONMODE, TessellationMode) \
    X(D3DRS_MINTESSELLATIONLEVEL, MinTessellationLevel) \
    X(D3DRS_MAXTESSELLATIONLEVEL, MaxTessellationLevel) \
    X(D3DRS_WRAP0, Wrap0) \
    X(D3DRS_WRAP1, Wrap1) \
    X(D3DRS_WRAP2, Wrap2) \
    X(D3DRS_WRAP3, Wrap3) \
    X(D3DRS_WRAP4, Wrap4) \
    X(D3DRS_WRAP5, Wrap5) \
    X(D3DRS_WRAP6, Wrap6) \
    X(D3DRS_WRAP7, Wrap7) \
    X(D3DRS_WRAP8, Wrap8) \
    X(D3DRS_WRAP9, Wrap9) \
    X(D3DRS_WRAP10, Wrap10) \
    X(D3DRS_WRAP11, Wrap11) \
    X(D3DRS_WRAP12, Wrap12) \
    X(D3DRS_WRAP13, Wrap13) \
    X(D3DRS_WRAP14, Wrap14) \
    X(D3DRS_WRAP15, Wrap15) \
    X(D3DRS_VIEWPORTENABLE, ViewportEnable) \
    X(D3DRS_HIGHPRECISIONBLENDENABLE, HighPrecisionBlendEnable) \
    X(D3DRS_HIGHPRECISIONBLENDENABLE1, HighPrecisionBlendEnable1) \
    X(D3DRS_HIGHPRECISIONBLENDENABLE2, HighPrecisionBlendEnable2) \
    X(D3DRS_HIGHPRECISIONBLENDENABLE3, HighPrecisionBlendEnable3) \
    X(D3DRS_HALFPIXELOFFSET, HalfPixelOffset) \
    X(D3DRS_PRIMITIVERESETENABLE, PrimitiveResetEnable) \
    X(D3DRS_PRIMITIVERESETINDEX, PrimitiveResetIndex) \
    X(D3DRS_ALPHATOMASKENABLE, AlphaToMaskEnable) \
    X(D3DRS_ALPHATOMASKOFFSETS, AlphaToMaskOffsets) \
    X(D3DRS_GUARDBAND_X, GuardBand_X) \
    X(D3DRS_GUARDBAND_Y, GuardBand_Y) \
    X(D3DRS_DISCARDBAND_X, DiscardBand_X) \
    X(D3DRS_DISCARDBAND_Y, DiscardBand_Y) \
    X(D3DRS_HISTENCILENABLE, HiStencilEnable) \
    X(D3DRS_HISTENCILWRITEENABLE, HiStencilWriteEnable) \
    X(D3DRS_HISTENCILFUNC, HiStencilFunc) \
    X(D3DRS_HISTENCILREF, HiStencilRef) \
    X(D3DRS_PRESENTINTERVAL, PresentInterval) \
    X(D3DRS_PRESENTIMMEDIATETHRESHOLD, PresentImmediateThreshold) \
    X(D3DRS_HIZENABLE, HiZEnable) \
    X(D3DRS_HIZWRITEENABLE, HiZWriteEnable) \
    X(D3DRS_LASTPIXEL, LastPixel) \
    X(D3DRS_LINEWIDTH, LineWidth) \
    X(D3DRS_BUFFER2FRAMES, Buffer2Frames) \

#define PORT_RENDER_STATE_FUNCTIONS(rs, name)                                                                        \
    void WINAPI D3DDevice_SetRenderState_##name(D3DDevice *pDevice, DWORD Value)                                    \
    {                                                                                                               \
        set_render_state(pDevice, rs / 4, Value);                                                                   \
    }                                                                                                               \
    DWORD WINAPI D3DDevice_GetRenderState_##name(D3DDevice *pDevice)                                                \
    {                                                                                                               \
        return port_of(pDevice)->state.render_states[rs / 4];                                                       \
    }
PORT_RENDER_STATES(PORT_RENDER_STATE_FUNCTIONS)

#define PORT_SAMPLER_STATES(X)                                                                                      \
    X(D3DSAMP_ADDRESSU, AddressU)                                                                                   \
    X(D3DSAMP_ADDRESSV, AddressV)                                                                                   \
    X(D3DSAMP_ADDRESSW, AddressW)                                                                                   \
    X(D3DSAMP_BORDERCOLOR, BorderColor)                                                                             \
    X(D3DSAMP_MAGFILTER, MagFilter)                                                                                 \
    X(D3DSAMP_MINFILTER, MinFilter)                                                                                 \
    X(D3DSAMP_MIPFILTER, MipFilter)                                                                                 \
    X(D3DSAMP_MIPMAPLODBIAS, MipMapLodBias)                                                                         \
    X(D3DSAMP_MAXMIPLEVEL, MaxMipLevel)                                                                             \
    X(D3DSAMP_MAXANISOTROPY, MaxAnisotropy)                                                                         \
    X(D3DSAMP_MAGFILTERZ, MagFilterZ)                                                                               \
    X(D3DSAMP_MINFILTERZ, MinFilterZ)                                                                               \
    X(D3DSAMP_SEPARATEZFILTERENABLE, SeparateZFilterEnable)                                                         \
    X(D3DSAMP_MINMIPLEVEL, MinMipLevel)                                                                             \
    X(D3DSAMP_TRILINEARTHRESHOLD, TrilinearThreshold)                                                               \
    X(D3DSAMP_ANISOTROPYBIAS, AnisotropyBias)                                                                       \
    X(D3DSAMP_HGRADIENTEXPBIAS, HGradientExpBias)                                                                   \
    X(D3DSAMP_VGRADIENTEXPBIAS, VGradientExpBias)                                                                   \
    X(D3DSAMP_WHITEBORDERCOLORW, WhiteBorderColorW)                                                                 \
    X(D3DSAMP_POINTBORDERENABLE, PointBorderEnable)

#define PORT_SAMPLER_STATE_FUNCTIONS(state, name)                                                                    \
    void WINAPI D3DDevice_SetSamplerState_##name(D3DDevice *pDevice, DWORD Sampler, DWORD Value)                    \
    {                                                                                                               \
        apply_sampler_state(port_of(pDevice), Sampler, state, Value);                                               \
    }                                                                                                               \
    DWORD WINAPI D3DDevice_GetSamplerState_##name(D3DDevice *pDevice, DWORD Sampler)                                \
    {                                                                                                               \
        return read_sampler_state(port_of(pDevice), Sampler, state);                                                \
    }
PORT_SAMPLER_STATES(PORT_SAMPLER_STATE_FUNCTIONS)

// d3d9 0x5a2948, named by the game for its call in threads.cpp (thread_release_locks_and_d3d_device): marks every
// register pending (D3DTagCollection_SetAll on m_Pending), so the next owner of the device sends all of its state.
void rasterizer_device_prepare_thread_release(D3DDevice *device)
{
    if (device)
        D3DTagCollection_SetAll(&device->m_Pending);
}

// d3d9/state.obj 0x195e88, named by the game (c_rasterizer::set_sampler_filter_mode and the sprite, dynamic geometry,
// breakable surface and render target code): one call per e_sampler_filter_mode, as the retail switch on filter_mode
// sets the sampler (the mip filter it writes into the fetch constant directly, the same as the MipFilter setter):
//   0 trilinear: min / mag / mip linear;  1 point: all point;  2 bilinear: min / mag linear, mip point;
//   3..6 anisotropic: min anisotropic, mag / mip linear, max anisotropy anisotropy_mode - 2 (1..4);
//   7: bilinear with point Z filters (separate Z filtering on, MagFilterZ / MinFilterZ point; volume textures).
// Other values leave the sampler alone (the caller checks <= 7).
void rasterizer_device_set_sampler_filter_modes(D3DDevice *device, long sampler, long anisotropy_mode, long filter_mode)
{
    const DWORD s = (DWORD)sampler;
    switch (filter_mode) {
    case 0:
        D3DDevice_SetSamplerState_MinFilter(device, s, D3DTEXF_LINEAR);
        D3DDevice_SetSamplerState_MagFilter(device, s, D3DTEXF_LINEAR);
        D3DDevice_SetSamplerState_MipFilter(device, s, D3DTEXF_LINEAR);
        break;
    case 1:
        D3DDevice_SetSamplerState_MinFilter(device, s, D3DTEXF_POINT);
        D3DDevice_SetSamplerState_MagFilter(device, s, D3DTEXF_POINT);
        D3DDevice_SetSamplerState_MipFilter(device, s, D3DTEXF_POINT);
        break;
    case 2:
        D3DDevice_SetSamplerState_MinFilter(device, s, D3DTEXF_LINEAR);
        D3DDevice_SetSamplerState_MagFilter(device, s, D3DTEXF_LINEAR);
        D3DDevice_SetSamplerState_MipFilter(device, s, D3DTEXF_POINT);
        break;
    case 3:
    case 4:
    case 5:
    case 6:
        D3DDevice_SetSamplerState_MinFilter(device, s, D3DTEXF_ANISOTROPIC);
        D3DDevice_SetSamplerState_MagFilter(device, s, D3DTEXF_LINEAR);
        D3DDevice_SetSamplerState_MipFilter(device, s, D3DTEXF_LINEAR);
        D3DDevice_SetSamplerState_MaxAnisotropy(device, s, (DWORD)(anisotropy_mode - 2));
        break;
    case 7:
        D3DDevice_SetSamplerState_MinFilter(device, s, D3DTEXF_LINEAR);
        D3DDevice_SetSamplerState_MagFilter(device, s, D3DTEXF_LINEAR);
        D3DDevice_SetSamplerState_MipFilter(device, s, D3DTEXF_POINT);
        D3DDevice_SetSamplerState_SeparateZFilterEnable(device, s, TRUE);
        D3DDevice_SetSamplerState_MagFilterZ(device, s, D3DTEXF_POINT);
        D3DDevice_SetSamplerState_MinFilterZ(device, s, D3DTEXF_POINT);
        break;
    default:
        break;
    }
}

// The same function under the name render_objects.cpp gives it (rasterizer_device_states.h).
void rasterizer_device_set_sampler_filter(D3DDevice *device, long sampler, long value, long mode)
{
    rasterizer_device_set_sampler_filter_modes(device, sampler, value, mode);
}

// d3d9/state.obj 0x16f0a8 is D3DDevice_SetBlendState; the game names it rasterizer_set_packed_blend_state
// (render_mesh.cpp, C linkage, the state as a DWORD) until the units call it by its XDK name.
void rasterizer_set_packed_blend_state(void *device, unsigned long index, unsigned long state)
{
    D3DBLENDSTATE blend;
    __builtin_memcpy(&blend, &state, 4);
    D3DDevice_SetBlendState((D3DDevice *)device, index, blend);
}

} // extern "C"

// ---- the game's C++ names for D3D and XG calls ----

// The C++ form of the same function (render_objects.cpp, render_player_view.cpp: blend_state_value, a DWORD).
void rasterizer_set_packed_blend_state(D3DDevice *device, unsigned long index, blend_state_value state)
{
    D3DBLENDSTATE blend;
    __builtin_memcpy(&blend, &state.value, 4);
    D3DDevice_SetBlendState(device, index, blend);
}

// D3D::CDevice::KickOff (d3d9.lib; d3d9/query.obj calls it when m_pRing passes m_pRingGuarantee): submits the ring
// segment written so far and returns the write pointer of a new one with at least the guaranteed room. Nothing
// consumes the rings here (see g_hal_ring), so the new segment is the ring's start.
unsigned long *D3D::CDevice::KickOff()
{
    reset_ring(port_of(this));
    return (unsigned long *)m_pRing;
}

// Render states the game declares with an int value and C++ linkage (include/rasterizer/xenon/rasterizer_xenon_d3d.h,
// rasterizer_d3d_calls.h): the XDK's setters (above), which keep the device state the renderer reads.
#define PORT_RENDER_STATE_INT_OVERLOAD(name)                                                                       \
    void D3DDevice_SetRenderState_##name(D3DDevice *pDevice, int Value)                                            \
    {                                                                                                               \
        D3DDevice_SetRenderState_##name(pDevice, (DWORD)Value);                                                    \
    }
PORT_RENDER_STATE_INT_OVERLOAD(AlphaBlendEnable)
PORT_RENDER_STATE_INT_OVERLOAD(BlendOp)
PORT_RENDER_STATE_INT_OVERLOAD(ColorWriteEnable)
PORT_RENDER_STATE_INT_OVERLOAD(CullMode)
PORT_RENDER_STATE_INT_OVERLOAD(DestBlend)
PORT_RENDER_STATE_INT_OVERLOAD(SrcBlend)

// XGOffsetResourceAddress as include/rasterizer/xenon/rasterizer_xenon_resource.h declares it (void pointers, C++).
void XGOffsetResourceAddress(void *resource, void *address)
{
    D3DResource *header = (D3DResource *)resource;
    XGOffsetResourceAddress(header, address);
    if (header && address && (header->Common & D3DCOMMON_TYPE_MASK) == D3DCOMMON_TYPE_TEXTURE)
        port_vk_texture_header_changed();
}

// port/sdk/d3d: the GPU backend seam's shared part (gpu_backend.h): which backend a launch gets (HALO3_RENDERER,
// halo3.ini [display] renderer), the registry of backends, and the backend-neutral helpers (format facts, results,
// frame dumps, the default pipeline state). Host code (halo3_gpu).
#include "gpu_backend.h"
#include "gpu_crt.h"
#include "../../shaders/shader_pack.h"
#include "../runtime/port_runtime.h"

namespace {

// ---- the registry ----

// A backend this build knows of: its kind, log name, the data file of its pipeline cache, the variable naming a shader
// pack for it alone (NULL: the shared HALO3_SHADER_PACK), and its creator (NULL: not in this build: Direct3D 12 is
// Windows only, d3d12_backend.cpp).
struct s_backend_entry {
    uint32_t kind;
    const char *name;
    const char *pipeline_cache_file;
    const char *shader_pack_variable;
    gpu_result (*create)(const s_gpu_backend_create_info *info, s_gpu_backend **out_backend);
};

const s_backend_entry k_backends[] = {
    { k_gpu_backend_vulkan, "vulkan", "pipeline_cache.bin", 0, vulkan_gpu_backend_create },
#if defined(_WIN32)
    { k_gpu_backend_d3d12, "d3d12", "pipeline_cache_d3d12.bin", "HALO3_SHADER_PACK_DXIL", d3d12_gpu_backend_create },
#else
    { k_gpu_backend_d3d12, "d3d12", "pipeline_cache_d3d12.bin", "HALO3_SHADER_PACK_DXIL", 0 },
#endif
};

const s_backend_entry *backend_entry(uint32_t kind)
{
    for (const s_backend_entry &entry : k_backends)
        if (entry.kind == kind)
            return &entry;
    return 0;
}

bool equals_ignoring_case(const char *text, const char *word)
{
    for (; *text != '\0' && *word != '\0'; text++, word++) {
        const char c = *text >= 'A' && *text <= 'Z' ? (char)(*text + ('a' - 'A')) : *text;
        if (c != *word)
            return false;
    }
    return *text == '\0' && *word == '\0';
}

// ---- formats ----

struct s_format_info {
    const char *name;
    uint8_t block;   // block width and height in texels (4: BC)
    uint8_t bytes;   // bytes per block; 0: no texel size here (depth / stencil, undefined)
};

const s_format_info k_formats[] = {
    { "undefined", 1, 0 },
    { "r8g8b8a8_unorm", 1, 4 }, { "r8g8b8a8_snorm", 1, 4 }, { "r8g8b8a8_uscaled", 1, 4 }, { "r8g8b8a8_sscaled", 1, 4 },
    { "r8g8b8a8_uint", 1, 4 }, { "r8g8b8a8_sint", 1, 4 }, { "r8g8b8a8_srgb", 1, 4 },
    { "b8g8r8a8_unorm", 1, 4 }, { "b8g8r8a8_snorm", 1, 4 }, { "b8g8r8a8_uscaled", 1, 4 }, { "b8g8r8a8_sscaled", 1, 4 },
    { "b8g8r8a8_uint", 1, 4 }, { "b8g8r8a8_sint", 1, 4 }, { "b8g8r8a8_srgb", 1, 4 },
    { "a2b10g10r10_unorm", 1, 4 }, { "a2b10g10r10_snorm", 1, 4 }, { "a2b10g10r10_uscaled", 1, 4 },
    { "a2b10g10r10_sscaled", 1, 4 }, { "a2b10g10r10_uint", 1, 4 }, { "a2b10g10r10_sint", 1, 4 },
    { "r16g16_unorm", 1, 4 }, { "r16g16_snorm", 1, 4 }, { "r16g16_uscaled", 1, 4 }, { "r16g16_sscaled", 1, 4 },
    { "r16g16_uint", 1, 4 }, { "r16g16_sint", 1, 4 }, { "r16g16_sfloat", 1, 4 },
    { "r16g16b16a16_unorm", 1, 8 }, { "r16g16b16a16_snorm", 1, 8 }, { "r16g16b16a16_uscaled", 1, 8 },
    { "r16g16b16a16_sscaled", 1, 8 }, { "r16g16b16a16_uint", 1, 8 }, { "r16g16b16a16_sint", 1, 8 },
    { "r16g16b16a16_sfloat", 1, 8 },
    { "r32_uint", 1, 4 }, { "r32_sint", 1, 4 }, { "r32_sfloat", 1, 4 },
    { "r32g32_uint", 1, 8 }, { "r32g32_sint", 1, 8 }, { "r32g32_sfloat", 1, 8 },
    { "r32g32b32_uint", 1, 12 }, { "r32g32b32_sint", 1, 12 }, { "r32g32b32_sfloat", 1, 12 },
    { "r32g32b32a32_uint", 1, 16 }, { "r32g32b32a32_sint", 1, 16 }, { "r32g32b32a32_sfloat", 1, 16 },
    { "r8_unorm", 1, 1 }, { "r8_snorm", 1, 1 },
    { "r8g8_unorm", 1, 2 }, { "r8g8_snorm", 1, 2 },
    { "r16_unorm", 1, 2 }, { "r16_snorm", 1, 2 }, { "r16_sfloat", 1, 2 },
    { "b4g4r4a4_unorm_pack16", 1, 2 }, { "b5g6r5_unorm_pack16", 1, 2 }, { "a1r5g5b5_unorm_pack16", 1, 2 },
    { "bc1_rgba_unorm", 4, 8 }, { "bc1_rgba_srgb", 4, 8 },
    { "bc2_unorm", 4, 16 }, { "bc2_srgb", 4, 16 },
    { "bc3_unorm", 4, 16 }, { "bc3_srgb", 4, 16 },
    { "bc4_unorm", 4, 8 }, { "bc5_unorm", 4, 16 }, { "bc5_snorm", 4, 16 },
    { "d24_unorm_s8_uint", 1, 0 },
    { "d32_sfloat_s8_uint", 1, 0 },
};
static_assert(sizeof(k_formats) / sizeof(k_formats[0]) == k_gpu_format_count, "k_formats has a row per e_gpu_format");

size_t format_decimal(char *out, uint32_t value)
{
    char digits[10];
    size_t n = 0;
    do {
        digits[n++] = (char)('0' + value % 10u);
        value /= 10u;
    } while (value != 0);
    for (size_t i = 0; i < n; i++)
        out[i] = digits[n - 1 - i];
    return n;
}

} // namespace

extern "C" {

#ifdef HALO3_64BIT
uint32_t vulkan_gpu_gather_depth(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination,
                                 s_gpu_buffer *parameters, uint32_t offset, uint32_t width, uint32_t height);

uint32_t gpu_cmd_gather_depth(s_gpu_backend *gpu, s_gpu_image *source, s_gpu_image *destination,
                              s_gpu_buffer *parameters, uint32_t offset, uint32_t width, uint32_t height)
{
    return vulkan_gpu_gather_depth(gpu, source, destination, parameters, offset, width, height);
}
#endif

// ---- selection ----

uint32_t gpu_renderer_requested(void)
{
    const char *text = getenv("HALO3_RENDERER");
    if (text == 0 || text[0] == '\0' || equals_ignoring_case(text, "auto"))
        return k_gpu_renderer_auto;
    if (equals_ignoring_case(text, "vulkan"))
        return k_gpu_renderer_vulkan;
    if (equals_ignoring_case(text, "d3d12"))
        return k_gpu_renderer_d3d12;
    port_write_error("renderer: HALO3_RENDERER is not auto, vulkan or d3d12; using auto\n");
    return k_gpu_renderer_auto;
}

int gpu_renderer_fallback_allowed(void)
{
    const char *text = getenv("HALO3_RENDERER_FALLBACK");
    return !(text != 0 && text[0] == '0' && text[1] == '\0');
}

uint32_t gpu_backend_candidates(uint32_t request, uint32_t *out_kinds)
{
    // auto: the platform's default, Vulkan; on Windows Direct3D 12 when Vulkan cannot start (no driver, no device: the
    // other renderer second, unless HALO3_RENDERER_FALLBACK=0)
    const s_backend_entry *d3d12 = backend_entry(k_gpu_backend_d3d12);
    const bool have_d3d12 = d3d12 != 0 && d3d12->create != 0;
    if (request == k_gpu_renderer_auto || request == k_gpu_renderer_vulkan) {
        out_kinds[0] = k_gpu_backend_vulkan;
        if (!have_d3d12 || !gpu_renderer_fallback_allowed())
            return 1;
        out_kinds[1] = k_gpu_backend_d3d12;
        return 2;
    }
#if defined(_WIN32)
    const char *why = "the Direct3D 12 renderer is not in this build";
#else
    const char *why = "Direct3D 12 is Windows only";
#endif
    if (have_d3d12) {
        out_kinds[0] = k_gpu_backend_d3d12;
        if (!gpu_renderer_fallback_allowed())
            return 1;
        out_kinds[1] = k_gpu_backend_vulkan;
        return 2;
    }
    port_write_error("renderer: HALO3_RENDERER=d3d12 asked for, but ");
    port_write_error(why);
    if (!gpu_renderer_fallback_allowed()) {
        port_write_error(" (HALO3_RENDERER_FALLBACK=0: no renderer)\n");
        return 0;
    }
    port_write_error("; using Vulkan\n");
    out_kinds[0] = k_gpu_backend_vulkan;
    return 1;
}

const char *gpu_backend_pipeline_cache_file(uint32_t kind)
{
    const s_backend_entry *entry = backend_entry(kind);
    return entry != 0 ? entry->pipeline_cache_file : "pipeline_cache.bin";
}

const char *gpu_backend_shader_pack(uint32_t kind)
{
    const s_backend_entry *entry = backend_entry(kind);
    const char *path = entry != 0 && entry->shader_pack_variable != 0 ? getenv(entry->shader_pack_variable) : 0;
    return path != 0 && path[0] != '\0' ? path : 0;
}

const char *gpu_backend_name(uint32_t kind)
{
    const s_backend_entry *entry = backend_entry(kind);
    return entry != 0 ? entry->name : "renderer";
}

gpu_result gpu_backend_create(uint32_t kind, const s_gpu_backend_create_info *info, s_gpu_backend **out_backend)
{
    *out_backend = 0;
    const s_backend_entry *entry = backend_entry(kind);
    if (entry == 0 || entry->create == 0)
        return k_gpu_unsupported;
    return entry->create(info, out_backend);
}

// ---- helpers ----

const char *gpu_result_string(gpu_result result)
{
    switch (result) {
    case k_gpu_ok: return "ok";
    case k_gpu_not_ready: return "not ready";
    case k_gpu_incomplete: return "incomplete";
    case k_gpu_suboptimal: return "suboptimal";
    case k_gpu_error: return "error";
    case k_gpu_out_of_memory: return "out of memory";
    case k_gpu_device_lost: return "device lost";
    case k_gpu_out_of_date: return "out of date";
    case k_gpu_surface_lost: return "surface lost";
    case k_gpu_unsupported: return "unsupported";
    default: return "unknown result";
    }
}

int gpu_format_block_info(uint32_t format, uint32_t *out_block_width, uint32_t *out_block_height,
                          uint32_t *out_bytes_per_block)
{
    if (format >= k_gpu_format_count || k_formats[format].bytes == 0)
        return 0;
    *out_block_width = k_formats[format].block;
    *out_block_height = k_formats[format].block;
    *out_bytes_per_block = k_formats[format].bytes;
    return 1;
}

const char *gpu_format_name(uint32_t format)
{
    return format < k_gpu_format_count ? k_formats[format].name : "?";
}

uint32_t gpu_hash_bytes(uint32_t hash, const void *data, size_t size)
{
    const uint8_t *bytes = (const uint8_t *)data;
    for (size_t i = 0; i < size; i++)
        hash = (hash ^ bytes[i]) * 16777619u;
    return hash;
}

int gpu_write_ppm(const char *path, const void *pixels, uint32_t width, uint32_t height, uint32_t row_pitch,
                  uint32_t format)
{
    const bool bgra = format >= k_gpu_format_b8g8r8a8_unorm && format <= k_gpu_format_b8g8r8a8_srgb;
    const bool rgba = format >= k_gpu_format_r8g8b8a8_unorm && format <= k_gpu_format_r8g8b8a8_srgb;
    if (!bgra && !rgba)
        return 0;
    FILE *file = fopen(path, "wb");
    if (file == 0)
        return 0;
    char header[48] = "P6\n";
    size_t length = 3;
    length += format_decimal(header + length, width);
    header[length++] = ' ';
    length += format_decimal(header + length, height);
    memcpy(header + length, "\n255\n", 5);
    length += 5;
    uint8_t *row = (uint8_t *)malloc(width * 3u + 1u);
    bool ok = row != 0 && fwrite(header, 1, length, file) == length;
    for (uint32_t y = 0; ok && y < height; y++) {
        const uint8_t *source = (const uint8_t *)pixels + (size_t)y * row_pitch;
        for (uint32_t x = 0; x < width; x++) {
            row[x * 3 + 0] = source[x * 4 + (bgra ? 2 : 0)];
            row[x * 3 + 1] = source[x * 4 + 1];
            row[x * 3 + 2] = source[x * 4 + (bgra ? 0 : 2)];
        }
        ok = fwrite(row, 1, width * 3u, file) == width * 3u;
    }
    free(row);
    return fclose(file) == 0 && ok;
}

void gpu_graphics_state_default(s_gpu_graphics_state *state, uint32_t color_format, uint32_t depth_stencil_format)
{
    memset(state, 0, sizeof(*state));
    state->topology = k_gpu_topology_triangle_list;
    state->polygon_mode = k_gpu_polygon_fill;
    state->cull_mode = k_gpu_cull_none;
    state->front_face = k_gpu_front_face_clockwise; // D3D: clockwise triangles face the viewer
    state->depth_compare = k_gpu_compare_less_equal;
    const s_gpu_stencil_face keep = { k_gpu_stencil_keep, k_gpu_stencil_keep, k_gpu_stencil_keep, k_gpu_compare_always,
                                      0xff, 0xff, 0 };
    state->stencil_front = keep;
    state->stencil_back = keep;
    state->sample_count = 1;
    if (color_format != k_gpu_format_undefined) {
        state->color_attachment_count = 1;
        state->color_formats[0] = color_format;
    }
    for (uint32_t i = 0; i < GPU_MAX_COLOR_ATTACHMENTS; i++) {
        s_gpu_blend_attachment &blend = state->blend[i];
        blend.src_color = blend.src_alpha = k_gpu_blend_one;
        blend.dst_color = blend.dst_alpha = k_gpu_blend_zero;
        blend.color_op = blend.alpha_op = k_gpu_blend_op_add;
        blend.write_mask = GPU_COLOR_WRITE_ALL;
    }
    state->depth_format = depth_stencil_format;
    // the depth / stencil formats a device picks all have stencil
    if (depth_stencil_format == k_gpu_format_d24_unorm_s8_uint || depth_stencil_format == k_gpu_format_d32_sfloat_s8_uint)
        state->stencil_format = depth_stencil_format;
}

} // extern "C"

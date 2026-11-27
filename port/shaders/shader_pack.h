#ifndef PORT_SHADERS_SHADER_PACK_H
#define PORT_SHADERS_SHADER_PACK_H

/*
 * Halo 3 PC shader pack: SPIR-V for the retail explicit shaders (rasterizer_globals 'rasg') and the render-method
 * template permutations ('rmt2'), built offline by port/tools/shaderpack from Bungie's DX11 HLSL path (DXC), with the
 * reflection the Vulkan backend needs to bind them.
 *
 * Conventions shared with the builder (port/tools/shaderpack/main.odin):
 *   - Register spaces shift as in local/shaders-pc/pgen.sh: b# -> binding #, t# -> 100 + #, s# -> 200 + #,
 *     u# -> 300 + #. MaterialParameters (render-method parameters) is the uniform buffer at b10 (binding 10).
 *   - Descriptor set = stage: vertex shaders use set 0, pixel shaders set 1, compute shaders set 0. A graphics
 *     pipeline layout is therefore {set 0: its vertex shader's bindings, set 1: its pixel shader's bindings}.
 *   - Buffer layouts follow D3D packing (DXC -fvk-use-dx-layout): the device needs scalarBlockLayout.
 *   - Vertex shader inputs are matched to vertex declarations by HLSL semantic ("POSITION", "TEXCOORD1", ...).
 *
 * File layout (little-endian, every table 4-byte aligned; see s_shader_pack_file_header):
 *   header | module table | shader table (sorted by kind, name, entry point, stage, vertex type) | record table |
 *   microcode table | cbuffer table | gather table | sampler table | string table (NUL-terminated) |
 *   blob (one zlib stream per unique SPIR-V module)
 *
 * Version 2 adds the bridge from the game's Xbox D3D calls to the pack (port/tools/shaderpack/microcode.odin,
 * cbuffer.odin):
 *   - microcode table: (stage, FNV-1a 64 of the retail Xenos microcode) -> pack shader, for every compiled shader the
 *     retail maps can bind (explicit shaders, render-method templates and their shared glvs / glps shaders), with
 *     the shader's MaterialParameters gathers (from the tag's Xbox constant table, by parameter name), its sampler
 *     map (pack binding -> Xbox sampler register, by name) and register overrides for cbuffer members the retail
 *     shader keeps elsewhere than the global table;
 *   - cbuffer table: every DX11 cbuffer of the pack's graphics shaders -> its size and the gathers that fill it
 *     from the Xbox register files (the CBUFFER_CONST register declarations of the DX11 tree, resolved through the
 *     DX9 register headers and the retail constant tables).
 *
 * Host C/C++ for the toolchain's target: no exceptions, RTTI, or static constructors; the C library through
 * port/data/data_crt.h; zlib through the port's own z_inflate* (port/sdk/runtime/zlib_shim.cpp).
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SHADER_PACK_MAGIC   0x50533348u /* 'H3SP' */
#define SHADER_PACK_VERSION 2u

enum e_shader_pack_kind {
    k_shader_pack_kind_explicit = 0, /* rasterizer\shaders\<name>, looked up by explicit shader index or name */
    k_shader_pack_kind_template = 1, /* shaders\<type>_templates\_<options>, looked up by tag path */
    k_shader_pack_kind_function = 2, /* a reconstruction of a D3D shader function the executable embeds (by name) */
};

enum e_shader_pack_stage {
    k_shader_pack_stage_vertex  = 0,
    k_shader_pack_stage_pixel   = 1,
    k_shader_pack_stage_compute = 2,
};

/* render_method entry points (c_render_method entry point enum; the rmdf entry point block). */
enum e_shader_pack_entry_point {
    k_shader_pack_entry_default                 = 0,
    k_shader_pack_entry_albedo                  = 1,
    k_shader_pack_entry_static_default          = 2,
    k_shader_pack_entry_static_per_pixel        = 3,
    k_shader_pack_entry_static_per_vertex       = 4,
    k_shader_pack_entry_static_sh               = 5,
    k_shader_pack_entry_static_prt_ambient      = 6,
    k_shader_pack_entry_static_prt_linear       = 7,
    k_shader_pack_entry_static_prt_quadratic    = 8,
    k_shader_pack_entry_dynamic_light           = 9,
    k_shader_pack_entry_shadow_generate         = 10,
    k_shader_pack_entry_shadow_apply            = 11,
    k_shader_pack_entry_active_camo             = 12,
    k_shader_pack_entry_lightmap_debug_mode     = 13,
    k_shader_pack_entry_static_per_vertex_color = 14,
    k_shader_pack_entry_water_tessellation      = 15,
    k_shader_pack_entry_water_shading           = 16,
    k_shader_pack_entry_dynamic_light_cinematic = 17,
    k_shader_pack_entry_count                   = 18,
};

/* Vertex types (e_vertex_type; hlsl_vertex_types.fx order). */
enum e_shader_pack_vertex_type {
    k_shader_pack_vertex_world              = 0,
    k_shader_pack_vertex_rigid              = 1,
    k_shader_pack_vertex_skinned            = 2,
    k_shader_pack_vertex_particle_model     = 3,
    k_shader_pack_vertex_flat_world         = 4,
    k_shader_pack_vertex_flat_rigid         = 5,
    k_shader_pack_vertex_flat_skinned       = 6,
    k_shader_pack_vertex_screen             = 7,
    k_shader_pack_vertex_debug              = 8,
    k_shader_pack_vertex_transparent        = 9,
    k_shader_pack_vertex_particle           = 10,
    k_shader_pack_vertex_contrail           = 11,
    k_shader_pack_vertex_light_volume       = 12,
    k_shader_pack_vertex_chud_simple        = 13,
    k_shader_pack_vertex_chud_fancy         = 14,
    k_shader_pack_vertex_decorator          = 15,
    k_shader_pack_vertex_tiny_position      = 16,
    k_shader_pack_vertex_patchy_fog         = 17,
    k_shader_pack_vertex_water              = 18,
    k_shader_pack_vertex_ripple             = 19,
    k_shader_pack_vertex_implicit           = 20,
    k_shader_pack_vertex_beam               = 21,
    k_shader_pack_vertex_type_count         = 22,
};

/* What a binding is for. */
enum e_shader_pack_binding_role {
    k_shader_pack_role_resource = 0, /* the shader's own resource (cbuffer, LocalTexture_*, LocalSampler_*, UAV) */
    k_shader_pack_role_material_parameters = 1, /* the generated MaterialParameters cbuffer (b10) */
    k_shader_pack_role_material_texture = 2,    /* a render-method texture parameter (MT_<name>) */
    k_shader_pack_role_material_sampler = 3,    /* its sampler (MS_<name>) */
};

/* Image dimensionality of a sampled/storage image binding. */
enum e_shader_pack_image_dim {
    k_shader_pack_image_none = 0,
    k_shader_pack_image_1d = 1,
    k_shader_pack_image_2d = 2,
    k_shader_pack_image_3d = 3,
    k_shader_pack_image_cube = 4,
    k_shader_pack_image_2d_array = 5,
    k_shader_pack_image_buffer = 6,
};

/* Component base type of a vertex input. */
enum e_shader_pack_input_type {
    k_shader_pack_input_float = 0,
    k_shader_pack_input_int = 1,
    k_shader_pack_input_uint = 2,
};

/* ---- on-disk structures ---- */

struct s_shader_pack_file_header {
    uint32_t magic;               /* SHADER_PACK_MAGIC */
    uint32_t version;             /* SHADER_PACK_VERSION */
    uint32_t header_size;         /* sizeof(s_shader_pack_file_header) */
    uint32_t file_size;
    uint32_t module_count;
    uint32_t module_offset;       /* s_shader_pack_file_module[module_count] */
    uint32_t shader_count;
    uint32_t shader_offset;       /* s_shader_pack_file_shader[shader_count], sorted (see shader_pack_compare) */
    uint32_t record_count;
    uint32_t record_offset;       /* s_shader_pack_file_record[record_count] */
    uint32_t string_size;
    uint32_t string_offset;       /* NUL-terminated UTF-8 strings */
    uint32_t blob_size;
    uint32_t blob_offset;         /* zlib (RFC 1950) streams */
    uint32_t explicit_count;      /* explicit shader slots in rasterizer_globals (112 in retail) */
    uint32_t table_crc32;         /* CRC-32 of bytes [header_size, blob_offset) */
    uint32_t microcode_count;
    uint32_t microcode_offset;    /* s_shader_pack_file_microcode[microcode_count], sorted by (hash_hi, hash_lo, stage) */
    uint32_t cbuffer_count;
    uint32_t cbuffer_offset;      /* s_shader_pack_file_cbuffer[cbuffer_count], sorted by name_hash */
    uint32_t gather_count;
    uint32_t gather_offset;       /* s_shader_pack_file_gather[gather_count] */
    uint32_t sampler_count;
    uint32_t sampler_offset;      /* s_shader_pack_file_sampler[sampler_count] */
};

struct s_shader_pack_file_module {
    uint32_t blob_offset;         /* from the blob start */
    uint32_t compressed_size;
    uint32_t spirv_size;          /* bytes of SPIR-V (a multiple of 4) */
    uint32_t spirv_crc32;         /* CRC-32 of the decompressed SPIR-V */
    uint32_t hash_lo;             /* 64-bit FNV-1a of the SPIR-V, the deduplication key */
    uint32_t hash_hi;
};

struct s_shader_pack_file_shader {
    uint8_t  kind;                /* e_shader_pack_kind */
    uint8_t  stage;               /* e_shader_pack_stage */
    uint8_t  entry_point;         /* e_shader_pack_entry_point (0 for explicit shaders) */
    uint8_t  vertex_type;         /* e_shader_pack_vertex_type (the //@generate type for explicit shaders) */
    uint16_t explicit_index;      /* rasterizer_globals explicit shader slot, 0xffff for templates */
    uint16_t flags;
    uint32_t name_offset;         /* string: "rasterizer\shaders\<name>" or "shaders\<type>_templates\_<options>" */
    uint32_t name_hash;           /* shader_pack_hash_name(name) */
    uint32_t module_index;
    uint32_t material_parameters_size; /* bytes of the MaterialParameters cbuffer, 0 if the shader has none */
    uint32_t first_record;
    uint16_t parameter_count;     /* records [first_record, +parameter_count) */
    uint16_t binding_count;       /* then binding records */
    uint16_t input_count;         /* then vertex input records */
    uint16_t reserved;            /* 0 */
};

struct s_shader_pack_file_record {
    uint32_t name_offset;         /* parameter name, binding variable name (MT_/MS_ prefix removed), or semantic */
    uint32_t name_hash;           /* shader_pack_hash_name(name) */
    uint32_t a;                   /* parameter: byte offset | binding: set << 16 | binding | input: location */
    uint32_t b;                   /* parameter: byte size | slot component type << 24 (0 float, 1 int, 2 uint) | binding: descriptor_type | role << 8 | image_dim << 16 |
                                     count << 24 (a storage buffer's image_dim byte holds its element stride in dwords) |
                                     input: component_count | e_shader_pack_input_type << 8 */
};

/* Where a gather reads (the D3D device's register shadows of the shader's stage). */
enum e_shader_pack_gather_source {
    k_shader_pack_gather_float = 0,        /* float4 register c#: `size` bytes from its .x */
    k_shader_pack_gather_bool = 1,         /* bool register b#: one uint32, 0 or 1 */
    k_shader_pack_gather_int = 2,          /* integer register i#: `size` bytes of int32 components */
    k_shader_pack_gather_float_as_int = 3, /* float4 register c# converted to int32 components (truncation) */
    k_shader_pack_gather_int_as_float = 4, /* integer register i# converted to float components (a DX11 float member the Xbox
                                             keeps in an integer register: PARAM(float, layers_of_4), CATEGORY_PARAM) */
};

#define SHADER_PACK_GATHER_MATERIAL_PARAMETERS 0xffffu

struct s_shader_pack_file_microcode {
    uint32_t hash_lo;             /* FNV-1a 64 of the physical microcode bytes in Xbox (big-endian) order */
    uint32_t hash_hi;
    uint32_t shader_index;        /* the pack shader, SHADER_PACK_NOT_FOUND when the pack has none (memexport) */
    uint8_t  stage;               /* e_shader_pack_stage (vertex or pixel) */
    uint8_t  kind;                /* 0 explicit, 1 template, 2 render method shared (glvs / glps), 3 function */
    uint8_t  entry_point;         /* 0xff when the tags do not say */
    uint8_t  vertex_type;         /* 0xff when the tags do not say */
    uint32_t microcode_size;      /* bytes of physical microcode */
    uint32_t first_gather;        /* this shader's gathers: MaterialParameters and cbuffer overrides */
    uint16_t gather_count;
    uint16_t sampler_count;
    uint32_t first_sampler;
    uint16_t unmatched;           /* MaterialParameters members / textures with no Xbox constant of their name */
    uint16_t reserved;
};

struct s_shader_pack_file_cbuffer {
    uint32_t name_offset;         /* the cbuffer's name (the binding name of shader_pack_get_binding) */
    uint32_t name_hash;
    uint32_t size;                /* bytes, a multiple of 16 */
    uint32_t first_gather;
    uint16_t gather_count;
    uint16_t unresolved_count;    /* members no gather fills (left zero): PC-only members and the like */
};

struct s_shader_pack_file_gather {
    uint16_t dst_offset;          /* bytes into the uniform buffer */
    uint8_t  size;                /* bytes written (bool: 4) */
    uint8_t  source;              /* e_shader_pack_gather_source */
    uint16_t register_index;      /* in the source register file */
    uint16_t buffer;              /* microcode gathers: SHADER_PACK_GATHER_MATERIAL_PARAMETERS or a cbuffer table index
                                     (an override applied after that cbuffer's own gathers); cbuffer gathers: their
                                     own table index */
};

struct s_shader_pack_file_sampler {
    uint16_t binding_index;       /* index into the pack shader's bindings (shader_pack_get_binding) */
    uint16_t sampler;             /* Xbox sampler register (D3DDevice_SetTexture / SetSamplerState index) */
};

/* ---- runtime API ---- */

typedef struct s_shader_pack s_shader_pack;

struct s_shader_pack_parameter {
    const char *name;             /* HLSL parameter name: the render-method parameter's string_id text */
    uint32_t    name_hash;
    uint32_t    offset;           /* byte offset in MaterialParameters (16 bytes per float4/int4/bool4 slot) */
    uint32_t    size;             /* bytes (16 x array length) */
};

/* A version 2 binding's descriptor type: the pack records the SPIR-V reflection's Vulkan VkDescriptorType numbers (the
 * pack format's own encoding; the device maps them to the backend's binding types). */
enum e_shader_pack_descriptor_type {
    k_shader_pack_descriptor_sampler        = 0,
    k_shader_pack_descriptor_sampled_image  = 2,
    k_shader_pack_descriptor_storage_image  = 3,
    k_shader_pack_descriptor_uniform_buffer = 6,
    k_shader_pack_descriptor_storage_buffer = 7,
};

struct s_shader_pack_binding {
    const char *name;             /* MT_/MS_ stripped for material textures/samplers */
    uint32_t    name_hash;
    uint32_t    set;
    uint32_t    binding;
    uint32_t    descriptor_type;  /* e_shader_pack_descriptor_type */
    uint32_t    role;             /* e_shader_pack_binding_role */
    uint32_t    image_dim;        /* e_shader_pack_image_dim */
    uint32_t    count;            /* array size (1 for scalars) */
    uint32_t    stride;           /* storage buffers: bytes per element of the (RW)StructuredBuffer (the file record's
                                     image_dim byte, in dwords); 0 for other bindings and for packs built before */
};

struct s_shader_pack_input {
    const char *semantic;         /* "POSITION", "TEXCOORD0", ... (upper case, index appended) */
    uint32_t    location;
    uint32_t    component_count;
    uint32_t    base_type;        /* e_shader_pack_input_type */
};

struct s_shader_pack_shader_info {
    const char *name;
    uint32_t    kind;
    uint32_t    stage;
    uint32_t    entry_point;
    uint32_t    vertex_type;
    uint32_t    explicit_index;   /* 0xffffffff for templates */
    uint32_t    module_index;
    uint32_t    material_parameters_size;
    uint32_t    parameter_count;
    uint32_t    binding_count;
    uint32_t    input_count;
};

#define SHADER_PACK_ANY 0xffffffffu
#define SHADER_PACK_NOT_FOUND 0xffffffffu

/* FNV-1a (32-bit) of a NUL-terminated string; '/' hashes as '\\' so tag paths may use either separator. */
uint32_t shader_pack_hash_name(const char *name);

/* Opens a pack (version 2, or version 3 or 4: see below). path NULL: $HALO3_SHADER_PACK, then shaders/halo3_shaders.pack,
 * then tmp/shaders-work/halo3_shaders_all.pack (shaderpack --all), then tmp/shaders-work/halo3_shaders.pack (relative
 * to the working directory). The file is read whole and validated (sizes, offsets, table CRC); modules are
 * decompressed on first use. Returns NULL on failure. */
s_shader_pack *shader_pack_open(const char *path);
/* Opens a pack already in memory (copied). */
s_shader_pack *shader_pack_open_memory(const void *data, size_t size);
void shader_pack_close(s_shader_pack *pack);
/* The last open/validation error (static text). */
const char *shader_pack_last_error(void);

uint32_t shader_pack_module_count(const s_shader_pack *pack);
uint32_t shader_pack_shader_count(const s_shader_pack *pack);
uint32_t shader_pack_explicit_count(const s_shader_pack *pack);

/* A module's code (SPIR-V words; version 4: a DXBC container, shader_pack_module_format), decompressed and checked
 * (CRC, magic) on first use and cached until close. NULL on failure. */
const uint32_t *shader_pack_module_code(s_shader_pack *pack, uint32_t module_index, size_t *out_size_bytes);

/* Shader lookups; return a shader index or SHADER_PACK_NOT_FOUND. SHADER_PACK_ANY matches any entry point or
 * vertex type (the first match in table order). */
uint32_t shader_pack_find_explicit(const s_shader_pack *pack, uint32_t explicit_index, uint32_t stage);
uint32_t shader_pack_find_explicit_by_name(const s_shader_pack *pack, const char *name, uint32_t stage);
uint32_t shader_pack_find_template(const s_shader_pack *pack, const char *template_path, uint32_t entry_point,
                                   uint32_t stage, uint32_t vertex_type);
/* The generic form: kind + name + (entry point, stage, vertex type). */
uint32_t shader_pack_find(const s_shader_pack *pack, uint32_t kind, const char *name, uint32_t entry_point,
                          uint32_t stage, uint32_t vertex_type);

int shader_pack_get_shader(const s_shader_pack *pack, uint32_t shader_index, struct s_shader_pack_shader_info *out);
int shader_pack_get_parameter(const s_shader_pack *pack, uint32_t shader_index, uint32_t i,
                              struct s_shader_pack_parameter *out);
int shader_pack_get_binding(const s_shader_pack *pack, uint32_t shader_index, uint32_t i,
                            struct s_shader_pack_binding *out);
int shader_pack_get_input(const s_shader_pack *pack, uint32_t shader_index, uint32_t i,
                          struct s_shader_pack_input *out);
/* MaterialParameters byte offset of a parameter by name hash, or SHADER_PACK_NOT_FOUND. */
uint32_t shader_pack_find_parameter(const s_shader_pack *pack, uint32_t shader_index, uint32_t name_hash);
/* Binding of a material texture (role material_texture) by name hash, or SHADER_PACK_NOT_FOUND. */
uint32_t shader_pack_find_texture_binding(const s_shader_pack *pack, uint32_t shader_index, uint32_t name_hash);

/* ---- retail microcode and cbuffer gathers (version 2) ---- */

struct s_shader_pack_microcode {
    uint32_t shader_index;        /* SHADER_PACK_NOT_FOUND: a retail shader the pack has no counterpart for */
    uint32_t stage;
    uint32_t kind;
    uint32_t entry_point;         /* 0xffffffff when unknown */
    uint32_t vertex_type;         /* 0xffffffff when unknown */
    uint32_t microcode_size;
    uint32_t gather_count;
    uint32_t sampler_count;
    uint32_t unmatched;
    const struct s_shader_pack_file_gather *gathers;
    const struct s_shader_pack_file_sampler *samplers;
};

struct s_shader_pack_cbuffer {
    const char *name;
    uint32_t    name_hash;
    uint32_t    table_index;
    uint32_t    size;
    uint32_t    gather_count;
    uint32_t    unresolved_count;
    const struct s_shader_pack_file_gather *gathers;
};

/* FNV-1a 64 of microcode as the pack keys it: the Xbox (big-endian) byte sequence. `words_in_host_order` nonzero:
 * the data is 32-bit words in host order (a CreateVertexShader / CreatePixelShader function the game declared as an
 * array of unsigned long), hashed as their big-endian bytes; zero: the bytes as they lie (microcode in a retail map
 * is big-endian in memory). size_bytes must be a multiple of 4. The result is split as (*out_lo, *out_hi). */
void shader_pack_hash_microcode(const void *microcode, size_t size_bytes, int words_in_host_order, uint32_t *out_lo,
                                uint32_t *out_hi);
/* The microcode table entry of (stage, hash) in O(1) (an index built at open), or SHADER_PACK_NOT_FOUND. */
uint32_t shader_pack_find_microcode(const s_shader_pack *pack, uint32_t stage, uint32_t hash_lo, uint32_t hash_hi,
                                    struct s_shader_pack_microcode *out);
/* The pack shader of retail microcode (Xbox byte order, see shader_pack_hash_microcode), or SHADER_PACK_NOT_FOUND. */
uint32_t shader_pack_find_by_microcode(const s_shader_pack *pack, uint32_t stage, const void *microcode_bytes,
                                       size_t size_bytes);
uint32_t shader_pack_microcode_count(const s_shader_pack *pack);
int shader_pack_get_microcode(const s_shader_pack *pack, uint32_t index, struct s_shader_pack_microcode *out);

/* The gather table of a cbuffer by name hash (shader_pack_hash_name of the binding name). Returns 0 if the pack has
 * none (a compute-only buffer or MaterialParameters). */
int shader_pack_find_cbuffer(const s_shader_pack *pack, uint32_t name_hash, struct s_shader_pack_cbuffer *out);
uint32_t shader_pack_cbuffer_count(const s_shader_pack *pack);
int shader_pack_get_cbuffer(const s_shader_pack *pack, uint32_t index, struct s_shader_pack_cbuffer *out);

/* Applies gathers: writes into `buffer` (a uniform buffer of `buffer_size` bytes) from the register shadows of one
 * stage: float4s (256 x 4 floats), bools (256 bits, bit r of word r / 32), integers (32 dwords of
 * GPU_INTEGER_CONSTANT: count, start, step in bytes 0..2, expanded to int4). Only gathers whose `buffer` field equals
 * `buffer_id` are applied (SHADER_PACK_GATHER_MATERIAL_PARAMETERS or a cbuffer table index); out-of-range gathers
 * are skipped. */
void shader_pack_apply_gathers(const struct s_shader_pack_file_gather *gathers, uint32_t gather_count, uint32_t buffer_id,
                               void *buffer, uint32_t buffer_size, const float *float_registers,
                               const uint32_t *bool_registers, const uint32_t *int_registers);

/* ---- version 3: the retail Xenos microcode translated to SPIR-V (port/tools/shaderpack --xenos) ----
 *
 * The same container (magic, module table, zlib blob) with its own header: no pack shaders, records, gathers or
 * cbuffer tables. Every retail vertex / pixel shader the maps (and the executable) hold has one entry keyed as version
 * 2's microcode table, (stage, FNV-1a 64 of the physical microcode), whose module reads the Xbox register files
 * through the universal pipeline layout of port/shaders/xenos_spirv_abi.h (the device fills set 0 from the D3D
 * device's shadows verbatim and set 1 from the fetch constants' textures; port/tools/xenos_spirv translates). The pack
 * records the ABI version it was translated for. Which version a
 * file is comes from its header, so HALO3_SHADER_PACK selects either kind; the version 2 API sees a version 3 pack
 * as one without shaders (it has modules only), and the version 3 API finds nothing in a version 2 pack.
 *
 * File layout (little-endian, 4-byte aligned): header (s_shader_pack_file_header_v3) | module table | xenos table
 * (sorted by hash_hi, hash_lo, stage) | texture uses | vertex elements | literal constants | memexport stream
 * registers | rectangle-list geometry shaders | ABI 6 metadata, overlays, image keys, sampler keys (ABI 6 only)
 * | string table | blob. */

#define SHADER_PACK_VERSION_XENOS 3u

struct s_shader_pack_file_header_v3 {
    uint32_t magic;               /* SHADER_PACK_MAGIC */
    uint32_t version;             /* SHADER_PACK_VERSION_XENOS */
    uint32_t header_size;         /* sizeof(s_shader_pack_file_header_v3) */
    uint32_t file_size;
    uint32_t abi_version;         /* XE_ABI_VERSION of the translator (xenos_spirv_abi.h) */
    uint32_t module_count;
    uint32_t module_offset;       /* s_shader_pack_file_module[module_count] (as version 2) */
    uint32_t xenos_count;
    uint32_t xenos_offset;        /* s_shader_pack_file_xenos[xenos_count] */
    uint32_t texture_count;
    uint32_t texture_offset;      /* s_shader_pack_file_xenos_texture[texture_count] */
    uint32_t element_count;
    uint32_t element_offset;      /* s_shader_pack_file_xenos_element[element_count] */
    uint32_t def_count;
    uint32_t def_offset;          /* s_shader_pack_file_xenos_def[def_count] */
    uint32_t stream_count;
    uint32_t stream_offset;       /* uint32_t[stream_count]: float registers of memexport stream constants */
    uint32_t string_size;
    uint32_t string_offset;
    uint32_t blob_size;
    uint32_t blob_offset;
    uint32_t table_crc32;         /* CRC-32 of bytes [header_size, blob_offset) */
    uint32_t tess_vs_module;      /* the adaptive tessellation helpers (xenos_spirv helpers.odin): vertex shader */
    uint32_t tess_tcs_module;     /* ... and tessellation control shader */
    uint32_t rect_gs_count;
    uint32_t rect_gs_offset;      /* s_shader_pack_file_xenos_rect_gs[rect_gs_count], sorted by locations */
};

struct s_shader_pack_file_header_v3_abi6 {
    struct s_shader_pack_file_header_v3 base;
    uint32_t module_meta_offset;
    uint32_t entry_overlay_offset;
    uint32_t image_key_count;
    uint32_t image_key_offset;
    uint32_t sampler_key_count;
    uint32_t sampler_key_offset;
};

struct s_shader_pack_xenos_module_meta {
    uint32_t direct_float_mask[8];
    uint16_t relative_lo, relative_hi_exclusive;
    uint32_t bool_mask[8];
    uint32_t loop_mask;
    uint32_t system_rows;
    uint32_t fetch_rows[3];
    uint32_t vertex_fetch_rows[4];
    uint32_t first_image_key, first_sampler_key;
    uint16_t image_count, sampler_count;
    uint32_t reserved_zero;
};

struct s_shader_pack_xenos_entry_overlay {
    uint32_t literal_row_mask[8];
};

#ifdef __cplusplus
static_assert(sizeof(s_shader_pack_file_header_v3) == 104, "ABI 5 header");
static_assert(sizeof(s_shader_pack_file_header_v3_abi6) == 128, "ABI 6 header");
static_assert(offsetof(s_shader_pack_file_header_v3_abi6, module_meta_offset) == 104 &&
              offsetof(s_shader_pack_file_header_v3_abi6, entry_overlay_offset) == 108 &&
              offsetof(s_shader_pack_file_header_v3_abi6, image_key_count) == 112 &&
              offsetof(s_shader_pack_file_header_v3_abi6, image_key_offset) == 116 &&
              offsetof(s_shader_pack_file_header_v3_abi6, sampler_key_count) == 120 &&
              offsetof(s_shader_pack_file_header_v3_abi6, sampler_key_offset) == 124, "ABI 6 header extension");
static_assert(sizeof(s_shader_pack_xenos_module_meta) == 120, "ABI 6 module metadata");
static_assert(offsetof(s_shader_pack_xenos_module_meta, relative_lo) == 32 &&
              offsetof(s_shader_pack_xenos_module_meta, relative_hi_exclusive) == 34 &&
              offsetof(s_shader_pack_xenos_module_meta, bool_mask) == 36 &&
              offsetof(s_shader_pack_xenos_module_meta, loop_mask) == 68 &&
              offsetof(s_shader_pack_xenos_module_meta, system_rows) == 72 &&
              offsetof(s_shader_pack_xenos_module_meta, fetch_rows) == 76 &&
              offsetof(s_shader_pack_xenos_module_meta, vertex_fetch_rows) == 88 &&
              offsetof(s_shader_pack_xenos_module_meta, first_image_key) == 104 &&
              offsetof(s_shader_pack_xenos_module_meta, first_sampler_key) == 108 &&
              offsetof(s_shader_pack_xenos_module_meta, image_count) == 112 &&
              offsetof(s_shader_pack_xenos_module_meta, sampler_count) == 114 &&
              offsetof(s_shader_pack_xenos_module_meta, reserved_zero) == 116, "ABI 6 module metadata offsets");
static_assert(sizeof(s_shader_pack_xenos_entry_overlay) == 32, "ABI 6 entry overlay");
#endif

/* SPIR-V execution model of a translated module (the guest stage it implements is the entry's stage). */
enum e_shader_pack_xenos_model {
    k_shader_pack_xenos_vertex = 0,       /* a vertex shader */
    k_shader_pack_xenos_tess_eval = 2,    /* a vertex shader behind the tessellator (Halo 3 water) */
    k_shader_pack_xenos_fragment = 4,     /* a pixel shader */
    k_shader_pack_xenos_compute = 5,      /* a memexport-only vertex shader: one invocation per guest vertex */
};

enum e_shader_pack_xenos_flags {
    k_shader_pack_xenos_kills = 1,        /* the pixel shader can discard (kill / alpha test) */
    k_shader_pack_xenos_memexport = 2,    /* writes memory (eA / eM#) */
    k_shader_pack_xenos_depth = 4,        /* writes oDepth */
    k_shader_pack_xenos_labels = 8,
    k_shader_pack_xenos_point_size = 16,  /* writes the point size */
    k_shader_pack_xenos_bda = 32,         /* reads or writes memory by device address (vertex fetch, memexport) */
};

struct s_shader_pack_file_xenos {
    uint32_t hash_lo;             /* FNV-1a 64 of the physical microcode (shader_pack_hash_microcode) */
    uint32_t hash_hi;
    uint32_t module_index;
    uint8_t  stage;               /* e_shader_pack_stage of the guest shader: vertex or pixel */
    uint8_t  model;               /* e_shader_pack_xenos_model */
    uint8_t  param_gen;           /* pixel shaders: the register receiving VPOS / VFACE, 0xff none */
    uint8_t  color_outputs;       /* bit i: writes oC i */
    uint32_t flags;               /* e_shader_pack_xenos_flags */
    uint32_t vs_outputs;          /* interface locations written (xenos_spirv_abi.h) */
    uint32_t ps_inputs;           /* interface locations read */
    uint32_t microcode_size;
    uint32_t name_offset;         /* the first tag (or executable offset) the shader came from */
    uint32_t first_texture;
    uint16_t texture_count;
    uint16_t element_count;       /* vertex elements: element k is xe_vertex_fetch.element[k] */
    uint32_t first_element;
    uint32_t first_def;
    uint16_t def_count;
    uint16_t stream_count;
    uint32_t first_stream;
};

/* A texture fetch of the shader: binding XE_BINDING_TEX_* + fetch and sampler binding
 * XE_BINDING_SAMPLER + XE_SAMPLER_VARIANT_STRIDE * sampler + fetch (a 3D fetch uses the stacked binding as well). */
struct s_shader_pack_file_xenos_texture {
    uint8_t fetch;                /* texture fetch constant 0..31 (= D3D sampler) */
    uint8_t dim;                  /* 0 2D, 1 3D, 2 cube, 3 stacked: 1 and 3 are a tfetch3D's two bindings (3D or stacked by
                                     the fetch constant, both bound) */
    uint8_t sampler;              /* XE_SAMPLER_* variant */
    uint8_t reserved;
};

/* The k-th vertex element of a vertex shader's container: which declaration element it reads. */
struct s_shader_pack_file_xenos_element {
    uint8_t usage;                /* D3DDECLUSAGE */
    uint8_t usage_index;
    uint8_t group;
    uint8_t reserved;
};

/* The host geometry shader drawing D3DPT_RECTLIST with vertex shaders of these interface outputs (the fourth corner
 * mirrored across the diagonal, as the Xenos does; xenos_spirv helpers.odin helper_rect_gs). */
struct s_shader_pack_file_xenos_rect_gs {
    uint32_t locations;           /* the vertex shader's vs_outputs */
    uint32_t module_index;
};

/* A literal constant (def): the Xbox runtime writes it into the constant file when it binds the shader. */
struct s_shader_pack_file_xenos_def {
    uint32_t reg;                 /* stage-relative c# */
    uint32_t value[4];            /* float bits */
};

struct s_shader_pack_xenos {
    uint32_t index;               /* in the xenos table */
    uint32_t module_index;
    uint32_t stage, model, flags, param_gen, color_outputs, vs_outputs, ps_inputs, microcode_size;
    const char *name;
    uint32_t texture_count;
    const struct s_shader_pack_file_xenos_texture *textures;
    uint32_t element_count;
    const struct s_shader_pack_file_xenos_element *elements;
    uint32_t def_count;
    const struct s_shader_pack_file_xenos_def *defs;
    uint32_t stream_count;
    const uint32_t *streams;      /* float registers holding the memexport stream constants (eA) */
    const struct s_shader_pack_xenos_module_meta *module_meta;
    const struct s_shader_pack_xenos_entry_overlay *overlay;
    const uint32_t *image_keys, *sampler_keys; /* module-local ordered slices, ABI 6 only */
};

/* 1 when the pack is version 3 or 4 (translated Xenos microcode: SPIR-V or DXIL), 0 for version 2 (DXC) or NULL. */
int shader_pack_is_xenos(const s_shader_pack *pack);
/* The translator ABI version a version 3 pack was built for (XE_ABI_VERSION), 0 otherwise. */
uint32_t shader_pack_xenos_abi_version(const s_shader_pack *pack);
uint32_t shader_pack_table_crc(const s_shader_pack *pack);
const struct s_shader_pack_xenos_module_meta *shader_pack_xenos_get_module_meta(const s_shader_pack *pack,
                                                                               uint32_t module_index);
uint32_t shader_pack_xenos_count(const s_shader_pack *pack);
/* The entry of (stage, microcode hash) in O(1), or SHADER_PACK_NOT_FOUND. */
uint32_t shader_pack_find_xenos(const s_shader_pack *pack, uint32_t stage, uint32_t hash_lo, uint32_t hash_hi,
                                struct s_shader_pack_xenos *out);
int shader_pack_get_xenos(const s_shader_pack *pack, uint32_t index, struct s_shader_pack_xenos *out);
/* The module index of the adaptive tessellation helpers (0 vertex shader, 1 tessellation control shader), or
 * SHADER_PACK_NOT_FOUND. */
uint32_t shader_pack_xenos_helper(const s_shader_pack *pack, uint32_t which);
/* The rectangle-list geometry shader module for a vertex shader's interface outputs, or SHADER_PACK_NOT_FOUND. */
uint32_t shader_pack_xenos_rect_gs(const s_shader_pack *pack, uint32_t vs_outputs);

/* ---- version 4: the same retail Xenos microcode translated to HLSL and compiled to DXIL, for Direct3D 12 ----
 *
 * port/tools/shaderpack --dxil (dxil.odin; port/tools/xenos_spirv hlsl.odin, the D3D12 section of abi.odin). The
 * version 3 layout and tables exactly (the shaders, keys, module grouping, texture uses, vertex elements, literal
 * constants, memexport registers and rectangle-list table are version 3's), with a longer header and DXIL containers
 * (DXBC, validated and hashed by DXC, each embedding the root signature) as modules: s_shader_pack_file_module's
 * spirv_size is the container size. Modules: the translated shaders (vs_6_0 / ps_6_0 / cs_6_0 for the compute model /
 * ds_6_0 for the tessellation evaluation model), the tessellation VS and HS (tess_vs_module / tess_tcs_module), the
 * rectangle-list GS, the point-sprite GS and the root signature alone (rootsig_1_1, for CreateRootSignature).
 * shader_pack_open reads version 3 and 4 alike (shader_pack_is_xenos, the version 3 API); which code its modules hold
 * is shader_pack_module_format, and a renderer backend takes only its own (port/sdk/d3d/gpu_backend.h
 * s_gpu_caps::shader_module_format), so a Vulkan device never takes DXIL for SPIR-V. */

#define SHADER_PACK_VERSION_XENOS_DXIL 4u
#define SHADER_PACK_MODULE_FORMAT_DXIL 0x4c495844u  /* "DXIL": DXBC containers (version 4) */
#define SHADER_PACK_MODULE_FORMAT_SPIRV 0x56525053u /* "SPRV": SPIR-V modules (versions 2 and 3) */

struct s_shader_pack_file_header_v4 {
    struct s_shader_pack_file_header_v3 v3; /* version SHADER_PACK_VERSION_XENOS_DXIL, header_size 116 */
    uint32_t module_format;       /* SHADER_PACK_MODULE_FORMAT_DXIL */
    uint32_t point_gs_module;     /* the point-sprite geometry shader (D3D12 has no point sprites; abi.odin D3D12) */
    uint32_t root_signature_module; /* the root signature (abi.odin D3D12_ROOT_SIGNATURE), a DXBC container with RTS0 */
};

/* The pack's version (SHADER_PACK_VERSION, _XENOS or _XENOS_DXIL), 0 for NULL. */
uint32_t shader_pack_version(const s_shader_pack *pack);
/* The code its modules hold: SHADER_PACK_MODULE_FORMAT_SPIRV (versions 2 and 3) or _DXIL (version 4); 0 for NULL.
 * shader_pack_module_code returns that code (a DXBC container's bytes for DXIL, checked for its magic). */
uint32_t shader_pack_module_format(const s_shader_pack *pack);
/* Version 4: the point-sprite geometry shader module and the root signature module; SHADER_PACK_NOT_FOUND otherwise. */
uint32_t shader_pack_xenos_point_gs(const s_shader_pack *pack);
uint32_t shader_pack_xenos_root_signature(const s_shader_pack *pack);

#ifdef __cplusplus
}
#endif

#endif /* PORT_SHADERS_SHADER_PACK_H */

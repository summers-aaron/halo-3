// port/shaders: the runtime side of the PC shader pack (format and conventions: shader_pack.h; builder:
// port/tools/shaderpack). Host C++ for the toolchain's target: no exceptions, RTTI or static constructors; the C
// library through port/data/data_crt.h; zlib through the port's own z_inflate* (port/sdk/runtime/zlib_shim.cpp).
#include "shader_pack.h"
#include "xenos_spirv_abi.h"

#include "../data/data_crt.h"

// halo3_gpu uses the host compiler flags, without the game and SDK defines.
#if defined(__linux__) && defined(__x86_64__) && !defined(__ILP32__) && !defined(HALO3_64BIT)
#define HALO3_64BIT 1
#endif

#if defined(__x86_64__) && !defined(_WIN32)
// z_inflate* live in halo3_sdk (Win64 ms_abi, LLP64 where uLong is 32-bit); match its z_stream layout and ABI.
extern "C" {
#define ZLIB_VERSION "1.1.4"
#define Z_FINISH     4
#define Z_OK         0
#define Z_STREAM_END 1
#define MAX_WBITS    15
typedef unsigned char Bytef;
typedef unsigned int uInt;
typedef uint32_t uLong;
typedef void *voidpf;
typedef voidpf (PORT_MS_ABI *alloc_func)(voidpf opaque, uInt items, uInt size);
typedef void (PORT_MS_ABI *free_func)(voidpf opaque, voidpf address);
struct internal_state;
typedef struct z_stream_s {
    Bytef *next_in;
    uInt avail_in;
    uLong total_in;
    Bytef *next_out;
    uInt avail_out;
    uLong total_out;
    char *msg;
    struct internal_state *state;
    alloc_func zalloc;
    free_func zfree;
    voidpf opaque;
    int data_type;
    uLong adler;
    uLong reserved;
} z_stream;
typedef z_stream *z_streamp;
typedef char z_stream_win64_size_check[sizeof(z_stream) == 88 ? 1 : -1];
int PORT_MS_ABI z_inflateInit2_(z_streamp strm, int windowBits, const char *version, int stream_size);
int PORT_MS_ABI z_inflate(z_streamp strm, int flush);
int PORT_MS_ABI z_inflateEnd(z_streamp strm);
}
#else
#include "zlib/zlib.h"
#endif

struct s_shader_pack {
    uint8_t *data;
    size_t size;
    const s_shader_pack_file_header *header;
    const s_shader_pack_file_module *modules;
    const s_shader_pack_file_shader *shaders;
    const s_shader_pack_file_record *records;
    const char *strings;
    const uint8_t *blob;
    const s_shader_pack_file_microcode *microcode;
    const s_shader_pack_file_cbuffer *cbuffers;
    const s_shader_pack_file_gather *gathers;
    const s_shader_pack_file_sampler *samplers;
    uint32_t *microcode_slots; // open addressing over the microcode table: entry index + 1, 0 = empty
    uint32_t microcode_slot_mask;
    uint32_t **module_code; // decompressed modules, NULL until first use
    // version 3 (shader_pack_is_xenos): its own header and tables; `header` then points at v2_view, a version 2 header
    // with the module table and blob only, so the version 2 API sees a pack without shaders
    uint32_t version;
    s_shader_pack_file_header v2_view;
    const s_shader_pack_file_header_v3 *xheader;
    uint32_t module_format; // SHADER_PACK_MODULE_FORMAT_SPIRV, or _DXIL (version 4: s_shader_pack_file_header_v4)
    uint32_t point_gs_module, root_signature_module; // version 4's, else SHADER_PACK_NOT_FOUND
    const s_shader_pack_file_xenos *xenos;
    const s_shader_pack_file_xenos_texture *xenos_textures;
    const s_shader_pack_file_xenos_element *xenos_elements;
    const s_shader_pack_file_xenos_def *xenos_defs;
    const uint32_t *xenos_streams;
    const s_shader_pack_file_xenos_rect_gs *xenos_rect_gs;
    const s_shader_pack_xenos_module_meta *module_meta;
    const s_shader_pack_xenos_entry_overlay *entry_overlay;
    const uint32_t *image_keys, *sampler_keys;
    uint32_t *xenos_slots; // open addressing over the xenos table: entry index + 1, 0 = empty
    uint32_t xenos_slot_mask;
};

namespace {

const char *g_last_error = "";

uint32_t g_crc_table[256];
bool g_crc_ready = false;

uint32_t crc32(const uint8_t *data, size_t size)
{
    if (!g_crc_ready) {
        for (uint32_t i = 0; i < 256; ++i) {
            uint32_t c = i;
            for (int k = 0; k < 8; ++k)
                c = (c & 1u) ? 0xedb88320u ^ (c >> 1) : c >> 1;
            g_crc_table[i] = c;
        }
        g_crc_ready = true;
    }
    uint32_t crc = 0xffffffffu;
    for (size_t i = 0; i < size; ++i)
        crc = g_crc_table[(crc ^ data[i]) & 0xffu] ^ (crc >> 8);
    return crc ^ 0xffffffffu;
}

bool fail(const char *error)
{
    g_last_error = error;
    return false;
}

bool range_ok(const s_shader_pack *pack, uint32_t offset, uint32_t count, uint32_t element_size)
{
    const uint64_t end = (uint64_t)offset + (uint64_t)count * element_size;
    return offset >= pack->header->header_size && end <= pack->size && (offset & 3u) == 0;
}

bool range_ok_v3(const s_shader_pack *pack, uint32_t offset, uint32_t count, uint32_t element_size)
{
    const uint64_t end = (uint64_t)offset + (uint64_t)count * element_size;
    return offset >= pack->xheader->header_size && end <= pack->size && (offset & 3u) == 0;
}

bool validate_modules(const s_shader_pack *pack, const s_shader_pack_file_module *modules, uint32_t count,
                      uint32_t blob_size)
{
    for (uint32_t i = 0; i < count; ++i) {
        const s_shader_pack_file_module &m = modules[i];
        if ((uint64_t)m.blob_offset + m.compressed_size > blob_size || m.spirv_size == 0 || (m.spirv_size & 3u))
            return fail("shader pack: bad module entry");
    }
    (void)pack;
    return true;
}

bool validate_keys(const uint32_t *keys, uint32_t total, uint32_t first, uint32_t count, uint32_t limit)
{
    if (first > total || count > total - first)
        return false;
    for (uint32_t i = 0; i < count; ++i)
        if (keys[first + i] >= limit || (i && keys[first + i - 1] >= keys[first + i]))
            return false;
    return true;
}

bool validate_v6_metadata(s_shader_pack *pack, const s_shader_pack_file_header_v3_abi6 &h)
{
    const s_shader_pack_file_header_v3 &b = h.base;
    // Validate the order and bounds of all CRC-covered tables, including the legacy prefix.
    uint64_t end = sizeof(h);
    const auto table = [&end](uint32_t offset, uint32_t count, uint32_t stride) {
        if (offset < end || (offset & 3u))
            return false;
        end = (uint64_t)offset + (uint64_t)count * stride;
        return true;
    };
    if (!table(b.module_offset, b.module_count, sizeof(*pack->modules)) ||
        !table(b.xenos_offset, b.xenos_count, sizeof(*pack->xenos)) ||
        !table(b.texture_offset, b.texture_count, sizeof(*pack->xenos_textures)) ||
        !table(b.element_offset, b.element_count, sizeof(*pack->xenos_elements)) ||
        !table(b.def_offset, b.def_count, sizeof(*pack->xenos_defs)) ||
        !table(b.stream_offset, b.stream_count, sizeof(*pack->xenos_streams)) ||
        !table(b.rect_gs_offset, b.rect_gs_count, sizeof(*pack->xenos_rect_gs)) ||
        !table(h.module_meta_offset, b.module_count, sizeof(*pack->module_meta)) ||
        !table(h.entry_overlay_offset, b.xenos_count, sizeof(*pack->entry_overlay)) ||
        !table(h.image_key_offset, h.image_key_count, sizeof(uint32_t)) ||
        !table(h.sampler_key_offset, h.sampler_key_count, sizeof(uint32_t)) ||
        !table(b.string_offset, b.string_size, 1) || end > b.blob_offset)
        return fail("shader pack: overlapping or out-of-range ABI 6 tables");

    const auto *meta = (const s_shader_pack_xenos_module_meta *)(pack->data + h.module_meta_offset);
    const auto *overlays = (const s_shader_pack_xenos_entry_overlay *)(pack->data + h.entry_overlay_offset);
    const auto *images = (const uint32_t *)(pack->data + h.image_key_offset);
    const auto *samplers = (const uint32_t *)(pack->data + h.sampler_key_offset);
    for (uint32_t i = 0; i < b.module_count; ++i) {
        const auto &m = meta[i];
        if ((m.relative_hi_exclusive == 0 ? m.relative_lo != 0
                                         : m.relative_lo >= m.relative_hi_exclusive || m.relative_hi_exclusive > 256) ||
            (m.system_rows >> 18) || (m.fetch_rows[2] >> 16) || (m.vertex_fetch_rows[3] >> 16) || m.reserved_zero)
            return fail("shader pack: invalid ABI 6 module masks or reserved bits");
        if (!validate_keys(images, h.image_key_count, m.first_image_key, m.image_count, 4u * 32u) ||
            !validate_keys(samplers, h.sampler_key_count, m.first_sampler_key, m.sampler_count, 6u * 32u))
            return fail("shader pack: invalid ABI 6 descriptor key slice");
    }
    for (uint32_t i = 0; i < b.xenos_count; ++i) {
        const auto &x = pack->xenos[i];
        uint32_t literal_mask[8] = {};
        for (uint32_t j = 0; j < x.def_count; ++j) {
            const auto *def = pack->xenos_defs + x.first_def + j;
            if (def->reg >= 256 || (j && def[-1].reg >= def->reg))
                return fail("shader pack: invalid ABI 6 literal register order");
            literal_mask[def->reg >> 5] |= 1u << (def->reg & 31u);
        }
        if (memcmp(literal_mask, overlays[i].literal_row_mask, sizeof(literal_mask)) != 0)
            return fail("shader pack: ABI 6 literal mask does not match definitions");
        for (uint32_t j = 0; j < x.stream_count; ++j)
            if (pack->xenos_streams[x.first_stream + j] >= 256u)
                return fail("shader pack: invalid ABI 6 memexport stream register");
    }
    pack->module_meta = meta;
    pack->entry_overlay = overlays;
    pack->image_keys = images;
    pack->sampler_keys = samplers;
    return true;
}

// Versions 3 and 4 (the translated Xenos microcode as SPIR-V, or as DXIL behind a longer header): their own header and
// tables (the same for both); the version 2 view gets the module table and the blob only.
bool validate_v3(s_shader_pack *pack, uint32_t version)
{
    if (pack->size < sizeof(s_shader_pack_file_header_v3))
        return fail("shader pack: file too small");
    const s_shader_pack_file_header_v3 *h = (const s_shader_pack_file_header_v3 *)pack->data;
    const bool abi6 = version == SHADER_PACK_VERSION_XENOS && h->abi_version == XE_ABI_VERSION_BINDLESS;
    const size_t header_size = abi6 ? sizeof(s_shader_pack_file_header_v3_abi6)
                               : version == SHADER_PACK_VERSION_XENOS_DXIL ? sizeof(s_shader_pack_file_header_v4)
                                                                          : sizeof(*h);
    if (pack->size < header_size)
        return fail("shader pack: file too small");
    pack->xheader = h;
    pack->version = version;
    if (h->header_size != header_size || h->file_size != pack->size)
        return fail("shader pack: header/file size mismatch");
    if (version == SHADER_PACK_VERSION_XENOS_DXIL) {
        const s_shader_pack_file_header_v4 *h4 = (const s_shader_pack_file_header_v4 *)pack->data;
        if (h4->module_format != SHADER_PACK_MODULE_FORMAT_DXIL)
            return fail("shader pack: version 4 with an unknown module format");
        if ((h4->point_gs_module != SHADER_PACK_NOT_FOUND && h4->point_gs_module >= h->module_count) ||
            (h4->root_signature_module != SHADER_PACK_NOT_FOUND && h4->root_signature_module >= h->module_count))
            return fail("shader pack: bad point geometry shader or root signature module");
        pack->module_format = SHADER_PACK_MODULE_FORMAT_DXIL;
        pack->point_gs_module = h4->point_gs_module;
        pack->root_signature_module = h4->root_signature_module;
    }
    if (!abi6 && h->abi_version != XE_ABI_VERSION)
        return fail(version == SHADER_PACK_VERSION_XENOS_DXIL
                        ? "shader pack: translated for another Xenos ABI version (rebuild it: shaderpack --dxil)"
                        : "shader pack: translated for another Xenos SPIR-V ABI version (rebuild it: shaderpack --xenos)");
    if (!range_ok_v3(pack, h->module_offset, h->module_count, sizeof(s_shader_pack_file_module)) ||
        !range_ok_v3(pack, h->xenos_offset, h->xenos_count, sizeof(s_shader_pack_file_xenos)) ||
        !range_ok_v3(pack, h->texture_offset, h->texture_count, sizeof(s_shader_pack_file_xenos_texture)) ||
        !range_ok_v3(pack, h->element_offset, h->element_count, sizeof(s_shader_pack_file_xenos_element)) ||
        !range_ok_v3(pack, h->def_offset, h->def_count, sizeof(s_shader_pack_file_xenos_def)) ||
        !range_ok_v3(pack, h->stream_offset, h->stream_count, sizeof(uint32_t)) ||
        !range_ok_v3(pack, h->rect_gs_offset, h->rect_gs_count, sizeof(s_shader_pack_file_xenos_rect_gs)) ||
        !range_ok_v3(pack, h->string_offset, h->string_size, 1) || h->string_size == 0 ||
        !range_ok_v3(pack, h->blob_offset, h->blob_size, 1) || h->blob_offset < h->header_size)
        return fail("shader pack: table outside the file");
    if (crc32(pack->data + h->header_size, h->blob_offset - h->header_size) != h->table_crc32)
        return fail("shader pack: table checksum mismatch");
    pack->modules = (const s_shader_pack_file_module *)(pack->data + h->module_offset);
    pack->strings = (const char *)(pack->data + h->string_offset);
    pack->blob = pack->data + h->blob_offset;
    pack->xenos = (const s_shader_pack_file_xenos *)(pack->data + h->xenos_offset);
    pack->xenos_textures = (const s_shader_pack_file_xenos_texture *)(pack->data + h->texture_offset);
    pack->xenos_elements = (const s_shader_pack_file_xenos_element *)(pack->data + h->element_offset);
    pack->xenos_defs = (const s_shader_pack_file_xenos_def *)(pack->data + h->def_offset);
    pack->xenos_streams = (const uint32_t *)(pack->data + h->stream_offset);
    pack->xenos_rect_gs = (const s_shader_pack_file_xenos_rect_gs *)(pack->data + h->rect_gs_offset);
    if (pack->strings[h->string_size - 1] != '\0')
        return fail("shader pack: unterminated string table");
    if (!validate_modules(pack, pack->modules, h->module_count, h->blob_size))
        return false;
    if ((h->tess_vs_module != SHADER_PACK_NOT_FOUND && h->tess_vs_module >= h->module_count) ||
        (h->tess_tcs_module != SHADER_PACK_NOT_FOUND && h->tess_tcs_module >= h->module_count))
        return fail("shader pack: bad tessellation helper module");
    for (uint32_t i = 0; i < h->rect_gs_count; ++i) {
        if (pack->xenos_rect_gs[i].module_index >= h->module_count ||
            (i > 0 && pack->xenos_rect_gs[i - 1].locations >= pack->xenos_rect_gs[i].locations))
            return fail("shader pack: bad rectangle-list geometry shader table");
    }
    for (uint32_t i = 0; i < h->xenos_count; ++i) {
        const s_shader_pack_file_xenos &x = pack->xenos[i];
        if (x.module_index >= h->module_count || x.stage > k_shader_pack_stage_pixel || x.name_offset >= h->string_size ||
            (uint64_t)x.first_texture + x.texture_count > h->texture_count ||
            (uint64_t)x.first_element + x.element_count > h->element_count ||
            (uint64_t)x.first_def + x.def_count > h->def_count ||
            (uint64_t)x.first_stream + x.stream_count > h->stream_count ||
            (x.model != k_shader_pack_xenos_vertex && x.model != k_shader_pack_xenos_tess_eval &&
             x.model != k_shader_pack_xenos_fragment && x.model != k_shader_pack_xenos_compute))
            return fail("shader pack: bad xenos entry");
    }
    if (abi6 && !validate_v6_metadata(pack, *(const s_shader_pack_file_header_v3_abi6 *)pack->data))
        return false;
    // the version 2 view: modules and blob, no shaders, records, microcode, cbuffers, gathers or samplers
    s_shader_pack_file_header &v = pack->v2_view;
    memset(&v, 0, sizeof(v));
    v.magic = SHADER_PACK_MAGIC;
    v.version = version;
    v.header_size = h->header_size;
    v.file_size = h->file_size;
    v.module_count = h->module_count;
    v.module_offset = h->module_offset;
    v.string_size = h->string_size;
    v.string_offset = h->string_offset;
    v.blob_size = h->blob_size;
    v.blob_offset = h->blob_offset;
    pack->header = &v;
    return true;
}

bool validate(s_shader_pack *pack)
{
    if (pack->size < sizeof(s_shader_pack_file_header))
        return fail("shader pack: file too small");
    const s_shader_pack_file_header *h = (const s_shader_pack_file_header *)pack->data;
    pack->header = h;
    if (h->magic != SHADER_PACK_MAGIC)
        return fail("shader pack: bad magic");
    pack->module_format = SHADER_PACK_MODULE_FORMAT_SPIRV;
    pack->point_gs_module = pack->root_signature_module = SHADER_PACK_NOT_FOUND;
    if (h->version == SHADER_PACK_VERSION_XENOS || h->version == SHADER_PACK_VERSION_XENOS_DXIL)
        return validate_v3(pack, h->version);
    pack->version = SHADER_PACK_VERSION;
    if (h->version != SHADER_PACK_VERSION)
        return fail("shader pack: unsupported version");
    if (h->header_size != sizeof(s_shader_pack_file_header) || h->file_size != pack->size)
        return fail("shader pack: header/file size mismatch");
    if (!range_ok(pack, h->module_offset, h->module_count, sizeof(s_shader_pack_file_module)) ||
        !range_ok(pack, h->shader_offset, h->shader_count, sizeof(s_shader_pack_file_shader)) ||
        !range_ok(pack, h->record_offset, h->record_count, sizeof(s_shader_pack_file_record)) ||
        !range_ok(pack, h->string_offset, h->string_size, 1) || h->string_size == 0 ||
        !range_ok(pack, h->blob_offset, h->blob_size, 1) ||
        !range_ok(pack, h->microcode_offset, h->microcode_count, sizeof(s_shader_pack_file_microcode)) ||
        !range_ok(pack, h->cbuffer_offset, h->cbuffer_count, sizeof(s_shader_pack_file_cbuffer)) ||
        !range_ok(pack, h->gather_offset, h->gather_count, sizeof(s_shader_pack_file_gather)) ||
        !range_ok(pack, h->sampler_offset, h->sampler_count, sizeof(s_shader_pack_file_sampler)))
        return fail("shader pack: table outside the file");
    if (crc32(pack->data + h->header_size, h->blob_offset - h->header_size) != h->table_crc32)
        return fail("shader pack: table checksum mismatch");
    pack->modules = (const s_shader_pack_file_module *)(pack->data + h->module_offset);
    pack->shaders = (const s_shader_pack_file_shader *)(pack->data + h->shader_offset);
    pack->records = (const s_shader_pack_file_record *)(pack->data + h->record_offset);
    pack->strings = (const char *)(pack->data + h->string_offset);
    pack->blob = pack->data + h->blob_offset;
    pack->microcode = (const s_shader_pack_file_microcode *)(pack->data + h->microcode_offset);
    pack->cbuffers = (const s_shader_pack_file_cbuffer *)(pack->data + h->cbuffer_offset);
    pack->gathers = (const s_shader_pack_file_gather *)(pack->data + h->gather_offset);
    pack->samplers = (const s_shader_pack_file_sampler *)(pack->data + h->sampler_offset);
    if (pack->strings[h->string_size - 1] != '\0')
        return fail("shader pack: unterminated string table");
    for (uint32_t i = 0; i < h->module_count; ++i) {
        const s_shader_pack_file_module &m = pack->modules[i];
        if ((uint64_t)m.blob_offset + m.compressed_size > h->blob_size || m.spirv_size == 0 || (m.spirv_size & 3u))
            return fail("shader pack: bad module entry");
    }
    for (uint32_t i = 0; i < h->shader_count; ++i) {
        const s_shader_pack_file_shader &s = pack->shaders[i];
        const uint64_t records = (uint64_t)s.parameter_count + s.binding_count + s.input_count;
        if (s.module_index >= h->module_count || s.name_offset >= h->string_size ||
            (uint64_t)s.first_record + records > h->record_count || s.stage > k_shader_pack_stage_compute ||
            s.kind > k_shader_pack_kind_function)
            return fail("shader pack: bad shader entry");
    }
    for (uint32_t i = 0; i < h->record_count; ++i) {
        if (pack->records[i].name_offset >= h->string_size)
            return fail("shader pack: bad record");
    }
    for (uint32_t i = 0; i < h->microcode_count; ++i) {
        const s_shader_pack_file_microcode &m = pack->microcode[i];
        if ((m.shader_index != SHADER_PACK_NOT_FOUND && m.shader_index >= h->shader_count) ||
            (uint64_t)m.first_gather + m.gather_count > h->gather_count ||
            (uint64_t)m.first_sampler + m.sampler_count > h->sampler_count || m.stage > k_shader_pack_stage_pixel)
            return fail("shader pack: bad microcode entry");
        if (m.shader_index != SHADER_PACK_NOT_FOUND) {
            const s_shader_pack_file_shader &s = pack->shaders[m.shader_index];
            for (uint32_t k = 0; k < m.sampler_count; ++k) {
                if (pack->samplers[m.first_sampler + k].binding_index >= s.binding_count)
                    return fail("shader pack: bad sampler map");
            }
        }
    }
    for (uint32_t i = 0; i < h->cbuffer_count; ++i) {
        const s_shader_pack_file_cbuffer &c = pack->cbuffers[i];
        if (c.name_offset >= h->string_size || (uint64_t)c.first_gather + c.gather_count > h->gather_count)
            return fail("shader pack: bad cbuffer entry");
    }
    return true;
}

// strcmp with '/' read as '\\' (tag paths may use either separator)
int compare_names(const char *a, const char *b)
{
    for (;; ++a, ++b) {
        unsigned char x = (unsigned char)(*a == '/' ? '\\' : *a);
        unsigned char y = (unsigned char)(*b == '/' ? '\\' : *b);
        if (x != y)
            return x < y ? -1 : 1;
        if (!x)
            return 0;
    }
}

// the table's sort key: kind, name, entry point, stage, vertex type
int compare_key(const s_shader_pack *pack, const s_shader_pack_file_shader &s, uint32_t kind, const char *name)
{
    if (s.kind != kind)
        return s.kind < kind ? -1 : 1;
    return compare_names(pack->strings + s.name_offset, name);
}

// first entry with (kind, name), or shader_count
uint32_t lower_bound(const s_shader_pack *pack, uint32_t kind, const char *name)
{
    uint32_t lo = 0, hi = pack->header->shader_count;
    while (lo < hi) {
        const uint32_t mid = lo + (hi - lo) / 2;
        if (compare_key(pack, pack->shaders[mid], kind, name) < 0)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}

uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *file = fopen(path, "rb");
    if (!file)
        return NULL;
    uint8_t *data = NULL;
    long size = -1;
    if (fseek(file, 0, SEEK_END) == 0)
        size = ftell(file);
    if (size > 0 && fseek(file, 0, SEEK_SET) == 0) {
        data = (uint8_t *)malloc((size_t)size);
        if (data && fread(data, 1, (size_t)size, file) != (size_t)size) {
            free(data);
            data = NULL;
        }
    }
    fclose(file);
    if (data)
        *out_size = (size_t)size;
    return data;
}

voidpf PORT_MS_ABI zlib_alloc(voidpf, uInt items, uInt size)
{
    return calloc(items, size);
}

void PORT_MS_ABI zlib_free(voidpf, voidpf address)
{
    free(address);
}

bool build_microcode_index(s_shader_pack *pack);
bool build_xenos_index(s_shader_pack *pack);

s_shader_pack *open_owned(uint8_t *data, size_t size)
{
    s_shader_pack *pack = (s_shader_pack *)calloc(1, sizeof(s_shader_pack));
    if (!pack) {
        free(data);
        fail("shader pack: out of memory");
        return NULL;
    }
    pack->data = data;
    pack->size = size;
    if (!validate(pack)) {
        free(data);
        free(pack);
        return NULL;
    }
    pack->module_code = (uint32_t **)calloc(pack->header->module_count ? pack->header->module_count : 1, sizeof(uint32_t *));
    if (!pack->module_code || !build_microcode_index(pack) || !build_xenos_index(pack)) {
        free(pack->microcode_slots);
        free(pack->xenos_slots);
        free(pack->module_code);
        free(data);
        free(pack);
        fail("shader pack: out of memory");
        return NULL;
    }
    g_last_error = "";
    return pack;
}

uint32_t microcode_slot(uint32_t stage, uint32_t hash_lo, uint32_t hash_hi)
{
    uint32_t h = hash_lo ^ (hash_hi * 0x9e3779b1u) ^ ((stage + 1u) * 0x85ebca6bu);
    h ^= h >> 16;
    return h * 0x7feb352du;
}

bool build_microcode_index(s_shader_pack *pack)
{
    const uint32_t count = pack->header->microcode_count;
    uint32_t slots = 16;
    while (slots < count * 2u)
        slots <<= 1;
    pack->microcode_slots = (uint32_t *)calloc(slots, sizeof(uint32_t));
    if (!pack->microcode_slots)
        return fail("shader pack: out of memory");
    pack->microcode_slot_mask = slots - 1u;
    for (uint32_t i = 0; i < count; ++i) {
        const s_shader_pack_file_microcode &m = pack->microcode[i];
        uint32_t s = microcode_slot(m.stage, m.hash_lo, m.hash_hi) & pack->microcode_slot_mask;
        while (pack->microcode_slots[s])
            s = (s + 1u) & pack->microcode_slot_mask;
        pack->microcode_slots[s] = i + 1u;
    }
    return true;
}

// Version 3: (stage, hash) -> xenos entry, as build_microcode_index (an empty index for version 2).
bool build_xenos_index(s_shader_pack *pack)
{
    const uint32_t count = pack->xheader != 0 ? pack->xheader->xenos_count : 0u;
    uint32_t slots = 16;
    while (slots < count * 2u)
        slots <<= 1;
    pack->xenos_slots = (uint32_t *)calloc(slots, sizeof(uint32_t));
    if (!pack->xenos_slots)
        return fail("shader pack: out of memory");
    pack->xenos_slot_mask = slots - 1u;
    for (uint32_t i = 0; i < count; ++i) {
        const s_shader_pack_file_xenos &x = pack->xenos[i];
        uint32_t s = microcode_slot(x.stage, x.hash_lo, x.hash_hi) & pack->xenos_slot_mask;
        while (pack->xenos_slots[s])
            s = (s + 1u) & pack->xenos_slot_mask;
        pack->xenos_slots[s] = i + 1u;
    }
    return true;
}

void fill_xenos(const s_shader_pack *pack, uint32_t i, s_shader_pack_xenos *out)
{
    const s_shader_pack_file_xenos &x = pack->xenos[i];
    out->index = i;
    out->module_index = x.module_index;
    out->stage = x.stage;
    out->model = x.model;
    out->flags = x.flags;
    out->param_gen = x.param_gen == 0xffu ? 0xffffffffu : x.param_gen;
    out->color_outputs = x.color_outputs;
    out->vs_outputs = x.vs_outputs;
    out->ps_inputs = x.ps_inputs;
    out->microcode_size = x.microcode_size;
    out->name = pack->strings + x.name_offset;
    out->texture_count = x.texture_count;
    out->textures = pack->xenos_textures + x.first_texture;
    out->element_count = x.element_count;
    out->elements = pack->xenos_elements + x.first_element;
    out->def_count = x.def_count;
    out->defs = pack->xenos_defs + x.first_def;
    out->stream_count = x.stream_count;
    out->streams = pack->xenos_streams + x.first_stream;
    out->module_meta = shader_pack_xenos_get_module_meta(pack, x.module_index);
    out->overlay = pack->entry_overlay ? pack->entry_overlay + i : NULL;
    out->image_keys = out->module_meta ? pack->image_keys + out->module_meta->first_image_key : NULL;
    out->sampler_keys = out->module_meta ? pack->sampler_keys + out->module_meta->first_sampler_key : NULL;
}

void fill_microcode(const s_shader_pack *pack, uint32_t i, s_shader_pack_microcode *out)
{
    const s_shader_pack_file_microcode &m = pack->microcode[i];
    out->shader_index = m.shader_index;
    out->stage = m.stage;
    out->kind = m.kind;
    out->entry_point = m.entry_point == 0xffu ? 0xffffffffu : m.entry_point;
    out->vertex_type = m.vertex_type == 0xffu ? 0xffffffffu : m.vertex_type;
    out->microcode_size = m.microcode_size;
    out->gather_count = m.gather_count;
    out->sampler_count = m.sampler_count;
    out->unmatched = m.unmatched;
    out->gathers = pack->gathers + m.first_gather;
    out->samplers = pack->samplers + m.first_sampler;
}

void fill_cbuffer(const s_shader_pack *pack, uint32_t i, s_shader_pack_cbuffer *out)
{
    const s_shader_pack_file_cbuffer &c = pack->cbuffers[i];
    out->name = pack->strings + c.name_offset;
    out->name_hash = c.name_hash;
    out->table_index = i;
    out->size = c.size;
    out->gather_count = c.gather_count;
    out->unresolved_count = c.unresolved_count;
    out->gathers = pack->gathers + c.first_gather;
}

const s_shader_pack_file_shader *shader_at(const s_shader_pack *pack, uint32_t shader_index)
{
    return pack && shader_index < pack->header->shader_count ? &pack->shaders[shader_index] : NULL;
}

} // namespace

extern "C" {

uint32_t shader_pack_hash_name(const char *name)
{
    uint32_t h = 0x811c9dc5u;
    for (; *name; ++name)
        h = (h ^ (uint8_t)(*name == '/' ? '\\' : *name)) * 0x01000193u;
    return h;
}

const char *shader_pack_last_error(void)
{
    return g_last_error;
}

s_shader_pack *shader_pack_open(const char *path)
{
    // without a path: HALO3_SHADER_PACK, else the installed pack, else the development builds (shaderpack --all, which
    // has every map's shaders, before the default --maps output)
    const char *candidates[4] = { path, NULL, NULL, NULL };
    if (!path) {
        const char *env = getenv("HALO3_SHADER_PACK");
        const bool named = env && *env;
        unsigned next = 0;
#ifdef HALO3_64BIT
        // A probe launched from the checkout can use the translated Xenos shaders in play/.
        if (!named)
            candidates[next++] = "play/halo3_shaders_xenos.pack";
#endif
        candidates[next++] = named ? env : "shaders/halo3_shaders.pack";
        candidates[next++] = named ? NULL : "tmp/shaders-work/halo3_shaders_all.pack";
        candidates[next] = named ? NULL : "tmp/shaders-work/halo3_shaders.pack";
    }
    for (int i = 0; i < 4 && candidates[i]; ++i) {
        size_t size = 0;
        uint8_t *data = read_file(candidates[i], &size);
        if (data)
            return open_owned(data, size);
    }
    fail("shader pack: file not found (HALO3_SHADER_PACK, "
#ifdef HALO3_64BIT
         "play/halo3_shaders_xenos.pack, "
#endif
         "shaders/halo3_shaders.pack, tmp/shaders-work/halo3_shaders_all.pack, "
         "tmp/shaders-work/halo3_shaders.pack)");
    return NULL;
}

s_shader_pack *shader_pack_open_memory(const void *data, size_t size)
{
    uint8_t *copy = (uint8_t *)malloc(size ? size : 1);
    if (!copy) {
        fail("shader pack: out of memory");
        return NULL;
    }
    memcpy(copy, data, size);
    return open_owned(copy, size);
}

void shader_pack_close(s_shader_pack *pack)
{
    if (!pack)
        return;
    for (uint32_t i = 0; i < pack->header->module_count; ++i)
        free(pack->module_code[i]);
    free(pack->module_code);
    free(pack->microcode_slots);
    free(pack->xenos_slots);
    free(pack->data);
    free(pack);
}

uint32_t shader_pack_module_count(const s_shader_pack *pack) { return pack ? pack->header->module_count : 0; }
uint32_t shader_pack_shader_count(const s_shader_pack *pack) { return pack ? pack->header->shader_count : 0; }
uint32_t shader_pack_explicit_count(const s_shader_pack *pack) { return pack ? pack->header->explicit_count : 0; }

const uint32_t *shader_pack_module_code(s_shader_pack *pack, uint32_t module_index, size_t *out_size_bytes)
{
    if (!pack || module_index >= pack->header->module_count)
        return NULL;
    const s_shader_pack_file_module &m = pack->modules[module_index];
    if (!pack->module_code[module_index]) {
        uint32_t *words = (uint32_t *)malloc(m.spirv_size);
        if (!words) {
            fail("shader pack: out of memory");
            return NULL;
        }
        z_stream stream;
        memset(&stream, 0, sizeof(stream));
        stream.zalloc = zlib_alloc;
        stream.zfree = zlib_free;
        if (z_inflateInit2_(&stream, MAX_WBITS, ZLIB_VERSION, (int)sizeof(stream)) != Z_OK) {
            free(words);
            fail("shader pack: inflate init failed");
            return NULL;
        }
        stream.next_in = (Bytef *)(pack->blob + m.blob_offset);
        stream.avail_in = m.compressed_size;
        stream.next_out = (Bytef *)words;
        stream.avail_out = m.spirv_size;
        int rc = Z_OK;
        // The port's inflate may need more than one call to drain its output.
        for (int guard = 0; rc == Z_OK && guard < 64; ++guard)
            rc = z_inflate(&stream, Z_FINISH);
        const bool complete = rc == Z_STREAM_END && stream.total_out == m.spirv_size && stream.avail_out == 0;
        z_inflateEnd(&stream);
        // SPIR-V's magic number, or a DXBC container's "DXBC"
        const uint32_t magic = pack->module_format == SHADER_PACK_MODULE_FORMAT_DXIL ? 0x43425844u : 0x07230203u;
        if (!complete || crc32((const uint8_t *)words, m.spirv_size) != m.spirv_crc32 || words[0] != magic) {
            free(words);
            fail("shader pack: corrupt module");
            return NULL;
        }
        pack->module_code[module_index] = words;
    }
    if (out_size_bytes)
        *out_size_bytes = m.spirv_size;
    return pack->module_code[module_index];
}

uint32_t shader_pack_find(const s_shader_pack *pack, uint32_t kind, const char *name, uint32_t entry_point,
                          uint32_t stage, uint32_t vertex_type)
{
    if (!pack || !name)
        return SHADER_PACK_NOT_FOUND;
    uint32_t any_vertex_type = SHADER_PACK_NOT_FOUND;
    for (uint32_t i = lower_bound(pack, kind, name); i < pack->header->shader_count; ++i) {
        const s_shader_pack_file_shader &s = pack->shaders[i];
        if (compare_key(pack, s, kind, name) != 0)
            break;
        if ((entry_point != SHADER_PACK_ANY && s.entry_point != entry_point) || s.stage != stage)
            continue;
        if (vertex_type == SHADER_PACK_ANY || s.vertex_type == vertex_type)
            return i;
        // pixel shaders are compiled once per entry point (for the render method's first vertex type) and serve
        // every vertex type
        if (stage == k_shader_pack_stage_pixel && any_vertex_type == SHADER_PACK_NOT_FOUND)
            any_vertex_type = i;
    }
    return any_vertex_type;
}

uint32_t shader_pack_find_template(const s_shader_pack *pack, const char *template_path, uint32_t entry_point,
                                   uint32_t stage, uint32_t vertex_type)
{
    return shader_pack_find(pack, k_shader_pack_kind_template, template_path, entry_point, stage, vertex_type);
}

uint32_t shader_pack_find_explicit_by_name(const s_shader_pack *pack, const char *name, uint32_t stage)
{
    // accept the bare name ("copy_surface") as well as the tag path ("rasterizer\shaders\copy_surface")
    static const char prefix[] = "rasterizer\\shaders\\";
    char path[256];
    const char *full = name;
    bool has_separator = false;
    for (const char *c = name; c && *c; ++c)
        has_separator |= *c == '\\' || *c == '/';
    if (name && !has_separator) {
        size_t n = 0;
        for (; prefix[n]; ++n)
            path[n] = prefix[n];
        for (const char *c = name; *c && n + 1 < sizeof(path); ++c)
            path[n++] = *c;
        path[n] = '\0';
        full = path;
    }
    return shader_pack_find(pack, k_shader_pack_kind_explicit, full, 0, stage, SHADER_PACK_ANY);
}

uint32_t shader_pack_find_explicit(const s_shader_pack *pack, uint32_t explicit_index, uint32_t stage)
{
    if (!pack)
        return SHADER_PACK_NOT_FOUND;
    // explicit entries come first in the table (kind 0); a slot index is not part of the sort key, so scan them
    for (uint32_t i = 0; i < pack->header->shader_count; ++i) {
        const s_shader_pack_file_shader &s = pack->shaders[i];
        if (s.kind != k_shader_pack_kind_explicit)
            break;
        if (s.explicit_index == explicit_index && s.stage == stage)
            return i;
    }
    return SHADER_PACK_NOT_FOUND;
}

int shader_pack_get_shader(const s_shader_pack *pack, uint32_t shader_index, s_shader_pack_shader_info *out)
{
    const s_shader_pack_file_shader *s = shader_at(pack, shader_index);
    if (!s || !out)
        return 0;
    out->name = pack->strings + s->name_offset;
    out->kind = s->kind;
    out->stage = s->stage;
    out->entry_point = s->entry_point;
    out->vertex_type = s->vertex_type;
    out->explicit_index = s->kind == k_shader_pack_kind_explicit ? s->explicit_index : 0xffffffffu;
    out->module_index = s->module_index;
    out->material_parameters_size = s->material_parameters_size;
    out->parameter_count = s->parameter_count;
    out->binding_count = s->binding_count;
    out->input_count = s->input_count;
    return 1;
}

int shader_pack_get_parameter(const s_shader_pack *pack, uint32_t shader_index, uint32_t i, s_shader_pack_parameter *out)
{
    const s_shader_pack_file_shader *s = shader_at(pack, shader_index);
    if (!s || !out || i >= s->parameter_count)
        return 0;
    const s_shader_pack_file_record &r = pack->records[s->first_record + i];
    out->name = pack->strings + r.name_offset;
    out->name_hash = r.name_hash;
    out->offset = r.a;
    out->size = r.b & 0xffffffu;
    return 1;
}

int shader_pack_get_binding(const s_shader_pack *pack, uint32_t shader_index, uint32_t i, s_shader_pack_binding *out)
{
    const s_shader_pack_file_shader *s = shader_at(pack, shader_index);
    if (!s || !out || i >= s->binding_count)
        return 0;
    const s_shader_pack_file_record &r = pack->records[s->first_record + s->parameter_count + i];
    out->name = pack->strings + r.name_offset;
    out->name_hash = r.name_hash;
    out->set = r.a >> 16;
    out->binding = r.a & 0xffffu;
    out->descriptor_type = r.b & 0xffu;
    out->role = (r.b >> 8) & 0xffu;
    out->image_dim = (r.b >> 16) & 0xffu;
    out->count = r.b >> 24;
    out->stride = out->descriptor_type == 7u /* VK_DESCRIPTOR_TYPE_STORAGE_BUFFER */ ? out->image_dim * 4u : 0u;
    if (out->stride != 0)
        out->image_dim = 0;
    return 1;
}

int shader_pack_get_input(const s_shader_pack *pack, uint32_t shader_index, uint32_t i, s_shader_pack_input *out)
{
    const s_shader_pack_file_shader *s = shader_at(pack, shader_index);
    if (!s || !out || i >= s->input_count)
        return 0;
    const s_shader_pack_file_record &r = pack->records[s->first_record + s->parameter_count + s->binding_count + i];
    out->semantic = pack->strings + r.name_offset;
    out->location = r.a;
    out->component_count = r.b & 0xffu;
    out->base_type = (r.b >> 8) & 0xffu;
    return 1;
}

uint32_t shader_pack_find_parameter(const s_shader_pack *pack, uint32_t shader_index, uint32_t name_hash)
{
    const s_shader_pack_file_shader *s = shader_at(pack, shader_index);
    if (!s)
        return SHADER_PACK_NOT_FOUND;
    for (uint32_t i = 0; i < s->parameter_count; ++i) {
        const s_shader_pack_file_record &r = pack->records[s->first_record + i];
        if (r.name_hash == name_hash)
            return r.a;
    }
    return SHADER_PACK_NOT_FOUND;
}

uint32_t shader_pack_find_texture_binding(const s_shader_pack *pack, uint32_t shader_index, uint32_t name_hash)
{
    const s_shader_pack_file_shader *s = shader_at(pack, shader_index);
    if (!s)
        return SHADER_PACK_NOT_FOUND;
    for (uint32_t i = 0; i < s->binding_count; ++i) {
        const s_shader_pack_file_record &r = pack->records[s->first_record + s->parameter_count + i];
        if (((r.b >> 8) & 0xffu) == k_shader_pack_role_material_texture && r.name_hash == name_hash)
            return r.a & 0xffffu;
    }
    return SHADER_PACK_NOT_FOUND;
}


void shader_pack_hash_microcode(const void *microcode, size_t size_bytes, int words_in_host_order, uint32_t *out_lo,
                                uint32_t *out_hi)
{
    // FNV-1a 64 (the builder's core:hash fnv64a) over the Xbox byte sequence
    uint64_t h = 0xcbf29ce484222325ull;
    const uint8_t *bytes = (const uint8_t *)microcode;
    for (size_t i = 0; i < size_bytes; ++i) {
        // host-order words: byte i of the big-endian word is byte (3 - i % 4) of the little-endian one
        const uint8_t b = words_in_host_order ? bytes[(i & ~(size_t)3) + (3 - (i & 3))] : bytes[i];
        h = (h ^ b) * 0x100000001b3ull;
    }
    *out_lo = (uint32_t)h;
    *out_hi = (uint32_t)(h >> 32);
}

uint32_t shader_pack_find_microcode(const s_shader_pack *pack, uint32_t stage, uint32_t hash_lo, uint32_t hash_hi,
                                    s_shader_pack_microcode *out)
{
    if (!pack || !pack->microcode_slots)
        return SHADER_PACK_NOT_FOUND;
    uint32_t s = microcode_slot(stage, hash_lo, hash_hi) & pack->microcode_slot_mask;
    for (;;) {
        const uint32_t slot = pack->microcode_slots[s];
        if (!slot)
            return SHADER_PACK_NOT_FOUND;
        const s_shader_pack_file_microcode &m = pack->microcode[slot - 1u];
        if (m.hash_lo == hash_lo && m.hash_hi == hash_hi && m.stage == stage) {
            if (out)
                fill_microcode(pack, slot - 1u, out);
            return slot - 1u;
        }
        s = (s + 1u) & pack->microcode_slot_mask;
    }
}

uint32_t shader_pack_find_by_microcode(const s_shader_pack *pack, uint32_t stage, const void *microcode_bytes,
                                       size_t size_bytes)
{
    uint32_t lo, hi;
    shader_pack_hash_microcode(microcode_bytes, size_bytes, 0, &lo, &hi);
    s_shader_pack_microcode m;
    if (shader_pack_find_microcode(pack, stage, lo, hi, &m) == SHADER_PACK_NOT_FOUND)
        return SHADER_PACK_NOT_FOUND;
    return m.shader_index;
}

uint32_t shader_pack_microcode_count(const s_shader_pack *pack) { return pack ? pack->header->microcode_count : 0; }

int shader_pack_get_microcode(const s_shader_pack *pack, uint32_t index, s_shader_pack_microcode *out)
{
    if (!pack || !out || index >= pack->header->microcode_count)
        return 0;
    fill_microcode(pack, index, out);
    return 1;
}

int shader_pack_find_cbuffer(const s_shader_pack *pack, uint32_t name_hash, s_shader_pack_cbuffer *out)
{
    if (!pack)
        return 0;
    uint32_t lo = 0, hi = pack->header->cbuffer_count;
    while (lo < hi) {
        const uint32_t mid = lo + (hi - lo) / 2;
        if (pack->cbuffers[mid].name_hash < name_hash)
            lo = mid + 1;
        else
            hi = mid;
    }
    if (lo >= pack->header->cbuffer_count || pack->cbuffers[lo].name_hash != name_hash)
        return 0;
    if (out)
        fill_cbuffer(pack, lo, out);
    return 1;
}

uint32_t shader_pack_cbuffer_count(const s_shader_pack *pack) { return pack ? pack->header->cbuffer_count : 0; }

int shader_pack_get_cbuffer(const s_shader_pack *pack, uint32_t index, s_shader_pack_cbuffer *out)
{
    if (!pack || !out || index >= pack->header->cbuffer_count)
        return 0;
    fill_cbuffer(pack, index, out);
    return 1;
}

void shader_pack_apply_gathers(const s_shader_pack_file_gather *gathers, uint32_t gather_count, uint32_t buffer_id,
                               void *buffer, uint32_t buffer_size, const float *float_registers,
                               const uint32_t *bool_registers, const uint32_t *int_registers)
{
    uint8_t *out = (uint8_t *)buffer;
    for (uint32_t i = 0; i < gather_count; ++i) {
        const s_shader_pack_file_gather &g = gathers[i];
        if (g.buffer != buffer_id || (uint32_t)g.dst_offset + g.size > buffer_size || g.size > 16)
            continue;
        switch (g.source) {
        case k_shader_pack_gather_float:
            if (g.register_index < 256)
                memcpy(out + g.dst_offset, float_registers + 4u * g.register_index, g.size);
            break;
        case k_shader_pack_gather_float_as_int:
            if (g.register_index < 256) {
                for (uint32_t c = 0; c < g.size / 4u; ++c) {
                    const int32_t v = (int32_t)float_registers[4u * g.register_index + c];
                    memcpy(out + g.dst_offset + 4u * c, &v, 4);
                }
            }
            break;
        case k_shader_pack_gather_bool:
            if (g.register_index < 256) {
                const uint32_t v = (bool_registers[g.register_index >> 5] >> (g.register_index & 31u)) & 1u;
                memcpy(out + g.dst_offset, &v, 4);
            }
            break;
        case k_shader_pack_gather_int:
            if (g.register_index < 32) {
                const uint32_t packed = int_registers[g.register_index];
                const int32_t v[4] = { (int32_t)(packed & 0xffu), (int32_t)((packed >> 8) & 0xffu),
                                       (int32_t)(int8_t)((packed >> 16) & 0xffu), 0 };
                memcpy(out + g.dst_offset, v, g.size);
            }
            break;
        case k_shader_pack_gather_int_as_float:
            if (g.register_index < 32) {
                // the same components (count, start, step, 0) as k_shader_pack_gather_int, as floats
                const uint32_t packed = int_registers[g.register_index];
                const float v[4] = { (float)(packed & 0xffu), (float)((packed >> 8) & 0xffu),
                                     (float)(int8_t)((packed >> 16) & 0xffu), 0.0f };
                memcpy(out + g.dst_offset, v, g.size);
            }
            break;
        }
    }
}


// ---- version 3 ----

int shader_pack_is_xenos(const s_shader_pack *pack)
{
    return pack != 0 && (pack->version == SHADER_PACK_VERSION_XENOS || pack->version == SHADER_PACK_VERSION_XENOS_DXIL);
}

uint32_t shader_pack_version(const s_shader_pack *pack) { return pack != 0 ? pack->version : 0u; }

uint32_t shader_pack_table_crc(const s_shader_pack *pack)
{
    return !pack ? 0u : pack->xheader ? pack->xheader->table_crc32 : pack->header->table_crc32;
}

const s_shader_pack_xenos_module_meta *shader_pack_xenos_get_module_meta(const s_shader_pack *pack, uint32_t module)
{
    return pack && pack->module_meta && module < pack->header->module_count ? pack->module_meta + module : NULL;
}

uint32_t shader_pack_module_format(const s_shader_pack *pack) { return pack != 0 ? pack->module_format : 0u; }

uint32_t shader_pack_xenos_point_gs(const s_shader_pack *pack)
{
    return pack != 0 ? pack->point_gs_module : SHADER_PACK_NOT_FOUND;
}

uint32_t shader_pack_xenos_root_signature(const s_shader_pack *pack)
{
    return pack != 0 ? pack->root_signature_module : SHADER_PACK_NOT_FOUND;
}

uint32_t shader_pack_xenos_abi_version(const s_shader_pack *pack)
{
    return shader_pack_is_xenos(pack) ? pack->xheader->abi_version : 0u;
}

uint32_t shader_pack_xenos_count(const s_shader_pack *pack)
{
    return shader_pack_is_xenos(pack) ? pack->xheader->xenos_count : 0u;
}

uint32_t shader_pack_find_xenos(const s_shader_pack *pack, uint32_t stage, uint32_t hash_lo, uint32_t hash_hi,
                                s_shader_pack_xenos *out)
{
    if (!shader_pack_is_xenos(pack) || !pack->xenos_slots)
        return SHADER_PACK_NOT_FOUND;
    uint32_t s = microcode_slot(stage, hash_lo, hash_hi) & pack->xenos_slot_mask;
    for (;;) {
        const uint32_t slot = pack->xenos_slots[s];
        if (!slot)
            return SHADER_PACK_NOT_FOUND;
        const s_shader_pack_file_xenos &x = pack->xenos[slot - 1u];
        if (x.hash_lo == hash_lo && x.hash_hi == hash_hi && x.stage == stage) {
            if (out)
                fill_xenos(pack, slot - 1u, out);
            return slot - 1u;
        }
        s = (s + 1u) & pack->xenos_slot_mask;
    }
}

int shader_pack_get_xenos(const s_shader_pack *pack, uint32_t index, s_shader_pack_xenos *out)
{
    if (!shader_pack_is_xenos(pack) || !out || index >= pack->xheader->xenos_count)
        return 0;
    fill_xenos(pack, index, out);
    return 1;
}

uint32_t shader_pack_xenos_helper(const s_shader_pack *pack, uint32_t which)
{
    if (!shader_pack_is_xenos(pack))
        return SHADER_PACK_NOT_FOUND;
    return which == 0 ? pack->xheader->tess_vs_module : which == 1 ? pack->xheader->tess_tcs_module : SHADER_PACK_NOT_FOUND;
}

uint32_t shader_pack_xenos_rect_gs(const s_shader_pack *pack, uint32_t vs_outputs)
{
    if (!shader_pack_is_xenos(pack))
        return SHADER_PACK_NOT_FOUND;
    uint32_t lo = 0, hi = pack->xheader->rect_gs_count;
    while (lo < hi) {
        const uint32_t mid = lo + (hi - lo) / 2;
        if (pack->xenos_rect_gs[mid].locations < vs_outputs)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo < pack->xheader->rect_gs_count && pack->xenos_rect_gs[lo].locations == vs_outputs
               ? pack->xenos_rect_gs[lo].module_index : SHADER_PACK_NOT_FOUND;
}

} // extern "C"

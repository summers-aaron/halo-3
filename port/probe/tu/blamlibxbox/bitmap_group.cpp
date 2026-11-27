// blamlibxbox/bitmap_group.obj: from the records 0x1677a0 0x2a5eb8 0x2a5f40 0x2a6000
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "math/real_math_types.h"
#include "cache/cache_file_tag_resource_runtime_interface.h"
#include "multithreading/thread_tls.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include "bitmaps/bitmap_group.h"
#include "cache/cache_file_tag_resource_runtime.h"
#include "bitmaps/bitmap_data.h"
#include "rasterizer/rasterizer_textures_cache.h"

#include "bitmaps/bitmap_group_definitions.h"
#include "bitmaps/bitmap_hardware_resource.h"
#include "memory/datum_index.h"
#include "cache/cache_file_globals.h"
#include "render/render.h"

// @address 0x74b898 g_geometry_globals

#ifdef HALO3_64BIT
namespace
{
constexpr unsigned int k_bitmap_format_cache_size = 64;

struct s_bitmap_format_cache_entry
{
    unsigned __int64 generation;
    long tag_index;
    long bitmap_index;
    const bitmap_data *bitmaps;
    const bitmap_data *bitmap;
    const void *texture_table;
    const s_bitmap_hardware_resource *reference;
    const s_bitmap_hardware_texture_block *block;
    const char *elements;
    __int64 resident_page;
    long resource_handle;
    // Keep the texture ref's datum in static TLS storage.
    long texture_datum;
    // The hardware-format accessor returns a success byte.
    unsigned char hardware_format;
    bool interleaved;
    signed char interleaved_resource_index;
    signed char interleaved_texture_index;
};

struct s_bitmap_format_cache
{
    const cache_file_tag_instance *tags;
    const c_tag_resource_cache_file_access_cache *render_access;
    unsigned __int64 map_timestamp;
    long checksum;
    long frame;
    unsigned __int64 generation;
    s_bitmap_format_cache_entry entries[k_bitmap_format_cache_size];
};

#ifdef __ELF__
thread_local __attribute__((tls_model("local-exec"))) s_bitmap_format_cache g_bitmap_format_cache;
#else
__declspec(thread) s_bitmap_format_cache g_bitmap_format_cache;
#endif
}
#endif

// @address 0x1677a0 ?bitmap_group_get_xenon_hardware_format@@YAEJJPAVc_rasterizer_texture_ref@@PAPBUbitmap_data@@PA_J@Z
inline unsigned char bitmap_group_get_xenon_hardware_format(long tag_index, long index, class c_rasterizer_texture_ref *out_element,
    // match: 0x1677a0: pointer __restrict qualifier prevents alias reload
    struct bitmap_data const **__restrict out_entry, __int64 *out_datum)
{
    const auto *definition = TAG_GET('bitm', bitmap_group, tag_index);
    const auto *bitmaps = (const bitmap_data *)definition->bitmaps.address;
    const long thread_index = get_current_thread_index();
    const auto &resource_cache = g_geometry_globals.manager->m_resource_cache;
    const auto *render_cache = resource_cache.m_render_access.m_per_thread_access_cache[thread_index];

#ifdef HALO3_64BIT
    auto &cache = g_bitmap_format_cache;
    const long frame = c_render_globals::get_frame_index();
    const auto *header = cache_files_get_header();
    if (cache.frame != frame || cache.tags != global_tag_instances || cache.checksum != header->checksum ||
        cache.map_timestamp != header->timestamp328 || cache.render_access != render_cache)
    {
        cache.frame = frame;
        cache.tags = global_tag_instances;
        cache.checksum = header->checksum;
        cache.map_timestamp = header->timestamp328;
        cache.render_access = render_cache;
        ++cache.generation;
    }
    const auto tag_key = static_cast<unsigned long>(tag_index);
    const auto bucket = (tag_key ^ (tag_key >> 16) ^ (static_cast<unsigned long>(index) * 37)) & (k_bitmap_format_cache_size - 1);
    auto &cached = cache.entries[bucket];

    // Both tag storage and render snapshots can be reused. Check the current
    // tag route and the live resource row before returning the cached outputs.
    if (render_cache && cached.hardware_format && cached.generation == cache.generation && cached.tag_index == tag_index &&
        cached.bitmap_index == index && cached.bitmaps == bitmaps)
    {
        const long absolute_index = datum_index_to_absolute_index(cached.resource_handle);
        const auto &textures = cached.interleaved ? definition->interleaved_hardware_textures : definition->hardware_textures;
        const long resource_index = cached.interleaved ? cached.interleaved_resource_index : index;
        if (absolute_index < render_cache->m_resource_handles.count() &&
            render_cache->m_resource_handles[absolute_index] == cached.resource_handle &&
            render_cache->m_resource_values[absolute_index] == cached.block &&
            render_cache->m_resource_pages[absolute_index] == cached.resident_page && cached.block->elements == cached.elements &&
            cached.interleaved == ((cached.bitmap->runtime_flags & 0x40) != 0) &&
            (!cached.interleaved || (cached.bitmap->interleaved_resource_index == cached.interleaved_resource_index &&
                cached.bitmap->interleaved_texture_index == cached.interleaved_texture_index)) &&
            textures.address == cached.texture_table && resource_index < textures.count && cached.reference->handle == cached.resource_handle)
        {
            if (out_datum)
            {
                const auto *geometry_cache = resource_cache.m_geometry_storage.m_per_thread_access_cache[thread_index];
                *out_datum = geometry_cache->m_resource_pages[absolute_index];
            }
            *out_entry = cached.bitmap;
            *out_element = c_rasterizer_texture_ref(cached.texture_datum);
            return cached.hardware_format;
        }
        cached.hardware_format = false;
    }
#endif

    const bitmap_data *entry = &bitmaps[index];
    const bool interleaved = (entry->runtime_flags & 0x40) != 0;
    const s_tag_block &textures = interleaved ? definition->interleaved_hardware_textures : definition->hardware_textures;
    const long resource_index = interleaved ? entry->interleaved_resource_index : index;
    if (resource_index < 0 || resource_index >= textures.count)
        return false;

    const auto &reference = ((const s_bitmap_hardware_resource *)textures.address)[resource_index];
    const long handle = reference.handle;
    const long absolute_index = datum_index_to_absolute_index(handle);
    const long resident_handle = handle != -1 && render_cache ? render_cache->m_resource_handles[absolute_index] : -1;
    const auto *block = resident_handle != -1
        ? static_cast<const s_bitmap_hardware_texture_block *>(render_cache->m_resource_values[absolute_index]) : nullptr;
    if (out_datum)
    {
        const auto *geometry_cache = resource_cache.m_geometry_storage.m_per_thread_access_cache[thread_index];
        *out_datum = geometry_cache->m_resource_pages[absolute_index];
    }
    if (!block)
        return false;

    *out_entry = entry;
    char *elements = block->elements;
    if (interleaved)
        elements += entry->interleaved_texture_index * 52;
#ifdef HALO3_64BIT
    *out_element = c_rasterizer_texture_ref((D3DBaseTexture *)elements);
    if (resident_handle == handle && block->elements && out_element->valid())
    {
        cached = { cache.generation, tag_index, index, bitmaps, entry, textures.address, &reference, block, block->elements,
            render_cache->m_resource_pages[absolute_index], handle, out_element->get_datum_ref(), true,
            interleaved, entry->interleaved_resource_index, entry->interleaved_texture_index };
    }
#else
    *(char **)out_element = elements;
#endif
    return true;
}


// @address 0x2a5eb8 bitmap_group_get_sprite_bounds_from_sequence
extern "C" real_rectangle2d *bitmap_group_get_sprite_bounds_from_sequence(long bitmap_group_index, short sequence_index, short frame_index)
{
    real_rectangle2d *bounds = 0;
    if (bitmap_group_index != -1)
    {
        bitmap_group *group = (bitmap_group *)tag_get('bitm', bitmap_group_index);
        if (group)
        {
            if (group->sequences.count > 0)
            {
                bitmap_sequence *sequence = &((bitmap_sequence *)group->sequences.address)[sequence_index % group->sequences.count];
                if (sequence->sprites.count)
                {
                    bitmap_sprite *sprite = &((bitmap_sprite *)sequence->sprites.address)[frame_index % sequence->sprites.count];
                    bounds = &sprite->bounds;
                }
            }
        }
    }
    return bounds;
}

long bitmap_group_get_bitmap_index_from_sequence(long bitmap_group_index, short sequence_index, short frame_index)
{
    long bitmap_index = -1;
    if (bitmap_group_index != -1)
    {
        bitmap_group *group = (bitmap_group *)tag_get('bitm', bitmap_group_index);
        if (group)
        {
            if (group->sequences.count > 0)
            {
                bitmap_sequence *sequence = &((bitmap_sequence *)group->sequences.address)[sequence_index % group->sequences.count];
                if (sequence->bitmap_count > 0)
                {
                    bitmap_index = sequence->first_bitmap_index + frame_index % sequence->bitmap_count;
                }
                else if (sequence->sprites.count)
                {
                    bitmap_sprite *sprite = &((bitmap_sprite *)sequence->sprites.address)[frame_index];
                    bitmap_index = sprite->bitmap_index;
                }
                if (bitmap_index == -1)
                    bitmap_index = frame_index;
            }
            else
            {
                bitmap_index = 0;
            }
        }
    }
    return bitmap_index;
}

// @address 0x2a6000 ?bitmap_group_get_bitmap_from_sequence@@YAPAUbitmap_data@@JFF@Z
bitmap_data *bitmap_group_get_bitmap_from_sequence(long bitmap_group_index, short sequence_index, short frame_index)
{
    bitmap_data *bitmap = 0;
    long bitmap_index = bitmap_group_get_bitmap_index_from_sequence(bitmap_group_index, sequence_index, frame_index);
    if (bitmap_index != -1)
    {
        bitmap_group *group = (bitmap_group *)tag_get('bitm', bitmap_group_index);
        if (bitmap_index >= 0 && bitmap_index < group->bitmaps.count)
            bitmap = (bitmap_data *)tag_block_get_element_with_size(&group->bitmaps, bitmap_index, sizeof(bitmap_data));
    }
    return bitmap;
}

// Keep the header-style inline bitmap format accessor available as a COMDAT.
typedef unsigned char (*bitmap_group_hardware_format_proc)(long, long, c_rasterizer_texture_ref *, const bitmap_data **, __int64 *);
bitmap_group_hardware_format_proc bitmap_group_get_hardware_format_accessor()
{
    return &bitmap_group_get_xenon_hardware_format;
}

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
#include "tag_files/tag_resource_types.h"
#include "bitmaps/bitmap_data.h"
#include "rasterizer/rasterizer_textures_cache.h"

#include "bitmaps/bitmap_group_definitions.h"
#include "bitmaps/bitmap_hardware_resource.h"
#include "memory/datum_index.h"

// @address 0x74b898 g_geometry_globals

// @address 0x1677a0 ?bitmap_group_get_xenon_hardware_format@@YAEJJPAVc_rasterizer_texture_ref@@PAPBUbitmap_data@@PA_J@Z
inline unsigned char bitmap_group_get_xenon_hardware_format(long tag_index, long index, class c_rasterizer_texture_ref *out_element,
    // match: 0x1677a0: pointer __restrict qualifier prevents alias reload
    struct bitmap_data const **__restrict out_entry, __int64 *out_datum)
{
    bool result = false;
    bitmap_group *definition = TAG_GET('bitm', bitmap_group, tag_index);
    bitmap_data *entry = &((bitmap_data *)definition->bitmaps.address)[index];
    if (entry->runtime_flags & 0x40)
    {
        if (entry->interleaved_resource_index >= 0 && entry->interleaved_resource_index < definition->interleaved_hardware_textures.count)
        {
            s_bitmap_hardware_resource *reference =
                &((s_bitmap_hardware_resource *)definition->interleaved_hardware_textures.address)[entry->interleaved_resource_index];
            s_bitmap_hardware_texture_block *block = (s_bitmap_hardware_texture_block *)tag_resource_available((const s_tag_resource *)reference);
            if (out_datum)
            {
                const c_tag_resource_cache_file_access_cache *cache =
                    g_geometry_globals.manager->m_resource_cache.m_geometry_storage.m_per_thread_access_cache[get_current_thread_index()];
                *out_datum = cache->m_resource_pages[datum_index_to_absolute_index(reference->handle)];
            }
            if (block)
            {
                *out_entry = (const bitmap_data *)entry;
#ifdef HALO3_64BIT
                *out_element = c_rasterizer_texture_ref((D3DBaseTexture *)(block->elements + entry->interleaved_texture_index * 52));
#else
                *(char **)out_element = block->elements + entry->interleaved_texture_index * 52;
#endif
                result = true;
            }
        }
    }
    else if (index >= 0 && index < definition->hardware_textures.count)
    {
        s_bitmap_hardware_resource *reference = &((s_bitmap_hardware_resource *)definition->hardware_textures.address)[index];
        s_bitmap_hardware_texture_block *block = (s_bitmap_hardware_texture_block *)tag_resource_available((const s_tag_resource *)reference);
        if (out_datum)
        {
            const c_tag_resource_cache_file_access_cache *cache =
                g_geometry_globals.manager->m_resource_cache.m_geometry_storage.m_per_thread_access_cache[get_current_thread_index()];
            *out_datum = cache->m_resource_pages[datum_index_to_absolute_index(reference->handle)];
        }
        if (block)
        {
            *out_entry = (const bitmap_data *)entry;
#ifdef HALO3_64BIT
            *out_element = c_rasterizer_texture_ref((D3DBaseTexture *)block->elements);
#else
            *(char **)out_element = block->elements;
#endif
            result = true;
        }
    }
    return result;
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

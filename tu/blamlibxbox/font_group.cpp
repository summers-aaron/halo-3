#include "multithreading/threads.h"
#include "cseries/async_block.h"
// blamlibxbox/font_group.obj: from the records 0x1a0390 0x1a0498 0x1a07c8 0x1a0890 0x1a0ae0 0x1a0be0 0x1a0c58 0x1efca8 0x1f26a0 0x1f28e8 0x1f2d58 0x7076b0 0x70b648
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "cseries/async_helpers.h"
#include "main/thread_globals_lock.h"
#include "memory/static_arrays.h"
#include <string.h>
#include <xtl.h>
#include "multithreading/thread_tls.h"
#include "cseries/crt_xdk.h"
#include "main/game_preferences.h"
#include "tag_files/files.h"
#include "tag_files/files_windows.h"
#include "tag_files/files_windows_types.h"
#include "text/font_group.h"
#include "memory/byte_swapping.h"
#include "cseries/cseries.h"
#include "cseries/async.h"
#include "memory/byte_swapping_types.h"
#include <ppcintrinsics.h>
#include "multithreading/synchronized_value.h"
#include "text/font_cache_file_types.h"
#include "tag_files/file_reference.h"
#include "cseries/language.h"
#include "text/font_group_globals.h"
#include "shell/utility_drive.h"
#include "main/game_preferences_defaults.h"

struct s_file_last_modification_date;
enum e_language;


#include "text/font_package_types.h"

#include "text/font_cache_internal_types.h"

// match: 0x1a0c58: adjacent swap definitions share the package-entry data anchor.
struct s_font_byte_swap_storage
{
    font_cache_swap_definition font_header;
    const char *sound_class_names[58];
    font_cache_swap_definition kerning_pair;
    font_cache_swap_definition package_entry;
    font_cache_swap_definition package_header;
};

#include "main/game_preferences_types.h"




struct named_object;


// @address 0x1a0390 ?get_font_modification_date@@YA_NPAUs_file_last_modification_date@@@Z
bool get_font_modification_date(struct s_file_last_modification_date * file_last_modification_date)
{
    bool result;
    char font_table_name[256];
    char full_path[256];
    s_file_reference base_reference;
    s_file_reference reference;
    WIN32_FILE_ATTRIBUTE_DATA file_attributes;

    memset(file_last_modification_date, 0, sizeof(file_attributes.ftLastWriteTime.dwLowDateTime));
    file_reference_create_from_path(&base_reference, "d:\\maps\\fonts\\", true);
    memcpy(&reference, &base_reference, offsetof(s_file_reference, handle));
    font_group_get_package_name((e_language)g_font_globals.language, font_table_name, sizeof(font_table_name));
    file_reference_set_name(&reference, font_table_name);
    full_path[0] = '\0';
    memset(full_path + 1, 0, sizeof(full_path) - 1);
    result = false;
    memset(file_last_modification_date, 0, sizeof(*file_last_modification_date));
    file_location_get_full_path(reference.location, reference.path, full_path, 256);
    if (GetFileAttributesExA(full_path, GetFileExInfoStandard, &file_attributes) != 0)
    {
        result = true;
        memcpy(file_last_modification_date, &file_attributes.ftLastWriteTime, sizeof(*file_last_modification_date));
    }
    if (!result)
    {
        file_error("file_get_last_modification_date", (named_object *)&reference, 0, false);
    }
    return result;
}

// @address 0x1a07c8 font_group_initialize_language
extern "C" void font_group_initialize_language(void)
{
    e_language language = get_current_language();
    if (language != _language_english)
    {
        char font_package_name[256];
        s_file_reference base_reference;
        s_file_reference reference;

        font_group_get_package_name(language, font_package_name, sizeof(font_package_name));
        file_reference_create_from_path(&base_reference, "d:\\maps\\fonts\\", true);
        memcpy(&reference, &base_reference, offsetof(s_file_reference, handle));
        file_reference_set_name(&reference, font_package_name);
        if (!file_exists(&reference))
        {
            language = _language_english;
        }
    }
    g_font_globals.language = language;
}

// @address 0x1a0890 font_group_get_package_name
extern "C" void font_group_get_package_name(e_language language, char *name, unsigned int size)
{
    strncpy(name, "font_package", 256);
    name[255] = '\0';
    if (language != _language_english)
    {
        const char *suffix = get_language_suffix(language, true);
        if (*suffix != '\0')
        {
            csnzappendf(name, 256, "_");
            csnzappendf(name, 256, suffix);
        }
    }
    csnzappendf(name, 256, ".bin");
}

template<> const char *c_static_string<256>::append(const char *source)
{
    csstrnzcat(m_string, source);
    return m_string;
}



// @address 0x84f90 ?simple_yield_function@@YA_NPAVc_synchronized_long@@@Z


// @address 0x1a0be0 font_cache_file_block_until_finished
extern "C" void font_cache_file_block_until_finished(s_font_cache_file *file)
{
    if (file->started.peek() != 0 && file->done.peek() == 0)
    {
        async_task_change_priority(file->task_id, _async_priority_blocking_below_sound);
        async_block_until_done(&file->done, simple_yield_function, 0, 0, 0, 4);
    }
}


// @address 0x1a0ae0 font_cache_file_load
extern "C" void font_cache_file_load(s_font_cache_file *file, long font_index, const char *name, bool block)
{
    e_async_priority priority = (block ? _async_priority_blocking_below_sound : _async_priority_blocking_generic);
    strncpy(file->name, name, sizeof(file->name));
    file->name[sizeof(file->name) - 1] = '\0';
    file->font_index = -1;
    if (g_font_globals.use_cached_fonts)
        file_reference_create_from_path(&file->reference, "cache0:\\maps\\cached_fonts\\", true);
    else
        file_reference_create_from_path(&file->reference, "d:\\maps\\fonts\\", true);
    file_reference_set_name(&file->reference, name);
    file->started.set(1);
    s_async_task task;
    memset(&task, 0, sizeof(task));
    task.font_loading_task.file = file;
    file->task_id = async_task_add_ex(priority, &task, _async_category_fonts,
        font_group_handle_load_task, &file->done, true);
    if (block)
        font_cache_file_block_until_finished(file);
}

static __forceinline bool font_group_in_range(long value, long minimum, long maximum)
{
    return value >= minimum ? value <= maximum : false;
}

// @address 0x1f28e8 font_header_is_valid
extern "C" bool font_header_is_valid(const s_font_header *header)
{
    bool valid = true;
    valid &= header->version == 0xF0000005;
    valid &= font_group_in_range(header->ascending_height, 0, 64);
    valid &= font_group_in_range(header->descending_height, 0, 64);
    valid &= font_group_in_range(header->leading_height, 0, 64);
    valid &= font_group_in_range(header->maximum_kerning, 0, 256);
    valid &= (header->kerning_pairs_offset >= sizeof(s_font_header)) &
        (header->ascending_height + header->descending_height > 0);
    valid &= font_group_in_range(header->kerning_pair_count, 0, 255);
    long previous_index = 0;
    for (long index = 0; index < 256; ++index)
    {
        long kerning_index = header->character_first_kerning_pair_index[index];
        valid &= kerning_index >= 0 && kerning_index <= header->kerning_pair_count;
        valid &= kerning_index >= previous_index;
        previous_index = kerning_index;
    }
    valid &= header->character_data_offset >= header->kerning_pairs_offset + 2 * header->kerning_pair_count;
    valid &= font_group_in_range(header->character_count, 0, 65536);
    valid &= header->character_data_count > 0 && header->character_data_count <= 65536 &&
        header->character_data_count <= header->character_count;
    if (valid && header->fallback_character != 0xFFFFFFFF)
    {
        long count = header->fallback_character >> 21;
        long first = header->fallback_character & 0x1FFFFF;
        valid &= count >= 1 && count <= 1026;
        valid &= first >= 0 && first + count <= header->packed_data_size / 8;
    }
    valid &= header->maximum_character_width > 0 && header->maximum_character_width <= 8192;
    valid &= header->maximum_character_height > 0 && (unsigned long)header->maximum_character_height <= 32768;
    valid &= header->maximum_character_data_size > 0 && header->maximum_character_data_size <= 0x8000000;
    return valid;
}

static __forceinline bool font_group_above_and_at_most(long value, long minimum, long maximum)
{
    return value > minimum ? value <= maximum : false;
}
// @address 0x1f2d58 font_package_is_valid
extern "C" bool font_package_is_valid(const s_font_package_header *package)
{
    long count = package->font_count;
    bool valid = true;
    valid &= package->version == 0xc0000003;
    valid &= font_group_above_and_at_most(count, 1, 16);
    unsigned long offset = package->data_offset;
    long data_end = offset + package->data_size;
    valid &= offset >= sizeof(s_font_package_header);
    valid &= data_end <= package->package_size;
    long character_count = package->character_count;
    valid &= font_group_above_and_at_most(character_count, 0, 65536);
    valid &= package->package_size >= data_end;
    for (long index = 0; valid && index < count; ++index)
    {
        const s_font_package_entry *entry = &package->fonts[index];
        valid &= entry->header_size >= sizeof(s_font_header);
        valid &= entry->header_offset >= (long)offset && entry->header_offset + entry->header_size <= data_end;
        valid &= entry->first_character >= 0 && entry->first_character + entry->character_count <= character_count;
        for (long previous = 0; previous < index; ++previous)
        {
            const s_font_package_entry *other = &package->fonts[previous];
            valid &= other->header_offset + other->header_size <= entry->header_offset;
            valid &= other->first_character + other->character_count - 1 <= entry->first_character;
        }
    }
    for (long index = 0; index < 16; ++index)
    {
        long font_index = package->font_indices[index];
        valid &= font_index == -1 || (font_index >= 0 && font_index < count);
    }
    return valid;
}



// @address 0x39350 k_font_header_swap_codes
// @rdata 0x82039350..0x82039438
static const long k_font_header_swap_codes[] =
{
    _byte_swap_array_start, 1,
        _byte_swap_4byte,
        _byte_swap_array_start, 32,
            1,
        _byte_swap_array_end,
        _byte_swap_2byte, _byte_swap_2byte, _byte_swap_2byte, _byte_swap_2byte,
        _byte_swap_4byte, _byte_swap_4byte,
        _byte_swap_array_start, 256,
            1,
        _byte_swap_array_end,
        _byte_swap_4byte, _byte_swap_4byte, _byte_swap_4byte, _byte_swap_4byte, _byte_swap_4byte,
        _byte_swap_4byte, _byte_swap_4byte, _byte_swap_4byte, _byte_swap_4byte, _byte_swap_4byte,
    _byte_swap_array_end
};

// @address 0x393c0 k_kerning_pair_swap_codes
static const long k_kerning_pair_swap_codes[] =
{
    _byte_swap_array_start, 1,
        1, 1,
    _byte_swap_array_end
};

// @address 0x393d4 k_font_package_entry_swap_codes
static const long k_font_package_entry_swap_codes[] =
{
    _byte_swap_array_start, 1,
        _byte_swap_4byte, _byte_swap_4byte,
    _byte_swap_array_end
};

// @address 0x393e8 k_font_package_header_swap_codes
static const long k_font_package_header_swap_codes[] =
{
    _byte_swap_array_start, 1,
        _byte_swap_4byte, _byte_swap_4byte,
        _byte_swap_array_start, 16,
            _byte_swap_4byte, _byte_swap_4byte, _byte_swap_2byte, _byte_swap_2byte,
        _byte_swap_array_end,
        _byte_swap_array_start, 16,
            _byte_swap_4byte,
        _byte_swap_array_end,
        _byte_swap_4byte, _byte_swap_4byte, _byte_swap_4byte, _byte_swap_4byte,
    _byte_swap_array_end
};
// @rdata-end

#if defined(_XBOX) && !defined(__clang__)
// @address 0x746054 g_font_package_entry_swap_definition
static font_cache_swap_definition g_font_package_entry_swap_definition =
{
    { (void *)"s_font_package_entry", 8, (void *)k_font_package_entry_swap_codes },
    0x62797377, 0
};
// @address 0x746068 g_font_package_header_swap_definition
static font_cache_swap_definition g_font_package_header_swap_definition =
{
    { (void *)"s_font_package_file_header", sizeof(s_font_package_header), (void *)k_font_package_header_swap_codes },
    0x62797377, 0
};
#endif

static __forceinline s_font_byte_swap_storage *font_group_get_swap_storage()
{
#if !defined(_XBOX) || defined(__clang__)
    static s_font_byte_swap_storage storage =
    {
        { { (void *)"s_font_header", sizeof(s_font_header), (void *)k_font_header_swap_codes }, 'bysw', 0 },
        { 0 },
        { { (void *)"s_kerning_pair", sizeof(s_kerning_pair), (void *)k_kerning_pair_swap_codes }, 'bysw', 0 },
        { { (void *)"s_font_package_entry", 8, (void *)k_font_package_entry_swap_codes }, 'bysw', 0 },
        { { (void *)"s_font_package_file_header", sizeof(s_font_package_header), (void *)k_font_package_header_swap_codes }, 'bysw', 0 }
    };
    return &storage;
#else
    return CONTAINING_RECORD(&g_font_package_entry_swap_definition, s_font_byte_swap_storage, package_entry);
#endif
}

static __forceinline font_cache_swap_definition *font_group_kerning_swap_definition()
{
    return &font_group_get_swap_storage()->kerning_pair;
}

static __forceinline font_cache_swap_definition *font_group_entry_swap_definition()
{
#if !defined(_XBOX) || defined(__clang__)
    return &font_group_get_swap_storage()->package_entry;
#else
    return &g_font_package_entry_swap_definition;
#endif
}


// @address 0x1a0c58 font_group_handle_load_task
extern "C" e_async_completion font_group_handle_load_task(s_async_task *task)
{
    s_font_cache_file *file = task->font_loading_task.file;
    bool finished = false;
    if (!file->open)
    {
        unsigned long error;
        if (file_open(&file->reference, 1, &error))
            file->open = true;
        else
        {
            file->reserved_140.set(1);
            finished = true;
        }
    }
    else
    {
        s_font_globals *font_globals = &g_font_globals;
        bool success = false;
        if (file_read(&file->reference, sizeof(font_globals->package_buffer), false, font_globals->package_buffer))
        {
            s_font_package_header &package = font_globals->package_header;
#if !defined(_XBOX) || defined(__clang__)
            element_type_t *type = &font_group_get_swap_storage()->package_header.type;
            byte_swap_element(type, (char *)&package, type->definition, 0, 0);
#else
            byte_swap_element(&g_font_package_header_swap_definition.type, (char *)&package,
                g_font_package_header_swap_definition.type.definition, 0, 0);
#endif
            if (font_package_is_valid(&package))
            {
                bool invalid = false;
                for (long index = 0; index < package.font_count && !invalid; ++index)
                {
                    unsigned long offset = package.fonts[index].header_offset;
                    if (offset >= sizeof(s_font_package_header) &&
                        offset + sizeof(s_font_header) <= package.data_offset + package.data_size)
                    {
                        s_font_header *header = (s_font_header *)(font_globals->package_buffer + offset);
                        if (header)
                        {
                            element_type_t *const &header_type = &font_group_get_swap_storage()->font_header.type;
                            byte_swap_element(header_type, (char *)header,
                                font_group_get_swap_storage()->font_header.type.definition, 0, 0);
                        }
                        if (font_header_is_valid(header))
                        {
                            if (header->kerning_pair_count > 0)
                                byte_swap_array(&font_group_kerning_swap_definition()->type, (char *)header + header->kerning_pairs_offset,
                                    header->kerning_pair_count);
                        }
                        else
                            invalid = true;
                    }
                    else
                        invalid = true;
                }
                byte_swap_array(&font_group_entry_swap_definition()->type,
                    font_globals->package_buffer + package.package_size, package.character_count);
                if (!invalid)
                {
                    success = true;
                    font_globals->package = &package;
                }
            }
        }
        if (!success)
            file->reserved_140.set(1);
        finished = true;
    }
    return (e_async_completion)(finished != false);
}

static __forceinline unsigned short font_make_pixel(unsigned short color, unsigned char alpha)
{
    unsigned long alpha_bits = alpha;
    return (unsigned short)((alpha_bits << 13) | ((alpha_bits & 1) << 12) | color);
}

// @address 0x1f26a0 ?font_unpack_character_pixels@@YAJJPBXJPAG@Z
long font_unpack_character_pixels(long packed_size, const void *packed_pixels, long maximum_destination_pixel_count, unsigned short *destination_pixels)
{
    long remaining = packed_size;
    long unpacked_size = 0;
    const unsigned char *src = (const unsigned char *)packed_pixels;
    unsigned short *dest = destination_pixels;
    unsigned short color = 0xfff;
    while (remaining > 0)
    {
        long count = 0;
        unsigned short pixel = 0;
        long consumed = 0;
        long type = (*src >> 6) & 3;
        switch (type)
        {
        case 0:
        case 1:
            count = *src & 0x3f;
            if (count == 0 && type == 0)
            {
                unsigned short high = (unsigned short)(src[1] << 8);
                unsigned short low = src[2];
                color = (unsigned short)((high | low) & 0xfff);
                unpacked_size++;
                if (dest)
                    *dest++ = high | low;
                consumed = 3;
            }
            else
            {
                unpacked_size += count;
                if (dest)
                {
                    pixel = font_make_pixel(color, 0 == type ? 0 : 7);
                    for (long index = 0; index < count; index++)
                        dest[index] = pixel;
                    dest += count;
                }
                consumed = 1;
            }
            break;
        case 2:
            unpacked_size++;
            if (dest)
                *dest++ = font_make_pixel(color, (*src >> 3) & 7);
            switch (*src & 7)
            {
            case 0:
                count = 0;
                break;
            case 4:
                count = 5;
                pixel = font_make_pixel(color, 0);
                break;
            case 1:
                count = 4;
                pixel = font_make_pixel(color, 7);
                break;
            case 7:
                count = 2;
                pixel = font_make_pixel(color, 0);
                break;
            case 3:
                count = 2;
                pixel = font_make_pixel(color, 7);
                break;
            case 6:
                count = 3;
                pixel = font_make_pixel(color, 0);
                break;
            case 2:
                count = 3;
                pixel = font_make_pixel(color, 7);
                break;
            case 5:
                count = 4;
                pixel = font_make_pixel(color, 0);
                break;
            default:
                // match: 0x1f26a0: unreachable hint omits dead branch check
                __assume(0);
            }
            unpacked_size += count;
            if (count != 0 && dest)
            {
                for (long index = 0; index < count; index++)
                    dest[index] = pixel;
                dest += count;
            }
            consumed = 1;
            break;
        case 3:
            unpacked_size += 2;
            if (dest)
            {
                *dest++ = font_make_pixel(color, (*src >> 3) & 7);
                *dest++ = font_make_pixel(color, *src & 7);
            }
            consumed = 1;
            break;
        default:
            // match: 0x1f26a0: unreachable hint omits dead branch check
            __assume(0);
        }
        remaining -= consumed;
        src += consumed;
    }
    return unpacked_size;
}


// @address 0x8a1ead g_utility_drive_mounted
class c_interlocked_long;
// @address 0x1a0498 ?cache_files_copy_fonts@@YAXXZ
void cache_files_copy_fonts()
{
    s_font_globals *font_globals = &g_font_globals;
    font_globals->use_cached_fonts = 0;
    if (!g_utility_drive_mounted)
        return;
    s_file_reference source_directory, destination_directory;
    s_file_reference found, source, destination;
    s_find_file_data search, next_search;
    char name[256];
    s_file_last_modification_date previous_date;
    file_reference_create_from_path(&source_directory, "d:\\maps\\fonts\\", true);
    file_reference_create_from_path(&destination_directory, "cache0:\\maps\\cached_fonts\\", true);
    if (!global_preferences_should_use_defaults()) {
        c_thread_globals_lock lock;
        previous_date = thread_global_preferences->primary_file.data.font_modification_date;
    }
    s_file_last_modification_date source_date;
    long previous_language = global_preferences_get_font_language();
    bool changed = !file_exists(&destination_directory);
    changed |= !get_font_modification_date(&source_date);
    changed |= CompareFileTime((const FILETIME *)&previous_date, (const FILETIME *)&source_date) != 0;
    changed |= previous_language != font_globals->language;
    if (changed) {
        if (global_preferences_get_font_language() != -1) {
            global_preferences_set_font_language(-1);
            global_preferences_flush();
        }
        global_preferences_flush();
        bool success = file_create_parent_directories_if_not_present(&destination_directory);
        if (success) {
            if (file_exists(&destination_directory)) {
                long count = 0;
                find_files_start(&search, 0, &destination_directory, "*.*");
                s_file_reference *found_file = &found;
                do {
                    if (!find_files_next(&search, found_file, 0))
                        break;
                    ++count;
                    ++found_file;
                } while (count < 1);
                find_files_end(&search);
                if (count == 1) {
                    do {
                        file_delete(&found);
                        count = 0;
                        find_files_start(&next_search, 0, &destination_directory, "*.*");
                        found_file = &found;
                        do {
                            if (!find_files_next(&next_search, found_file, 0))
                                break;
                            ++count;
                            ++found_file;
                        } while (count < 1);
                        find_files_end(&next_search);
                    } while (count == 1);
                }
            } else {
                success = file_create(&destination_directory);
            }
        }
        if (success) {
            font_group_get_package_name((e_language)font_globals->language, name, sizeof(name));
            memcpy(&source, &source_directory, offsetof(s_file_reference, handle));
            file_reference_set_name(&source, name);
            memcpy(&destination, &destination_directory, offsetof(s_file_reference, handle));
            file_reference_set_name(&destination, name);
            success = file_copy_to(&source, &destination, true);
        }
        if (!success)
            return;
        global_preferences_set_font_language(font_globals->language);
        if (!global_preferences_should_use_defaults()) {
            c_thread_globals_lock lock;
            // match: 0x1a0498: retain one preferences pointer for both stores after TLS unification.
            s_global_preferences *preferences = thread_global_preferences;
            preferences->primary_file.data.font_modification_date = source_date;
            preferences->dirty = true;
        }
    }
    font_globals->use_cached_fonts = 1;
}

s_font_globals g_font_globals;


#ifndef TEXT_FONT_GROUP_H
#define TEXT_FONT_GROUP_H

#include "cseries/async_types.h"
#include "text/font_id_types.h"

// Functions defined in tu/blamlibxbox/font_group.cpp.

long font_unpack_character_pixels(long packed_size, const void *packed_pixels, long maximum_destination_pixel_count, unsigned short *destination_pixels);
void cache_files_copy_fonts();

struct s_font_cache_file;

extern "C" void font_group_initialize_language(void);
extern "C" void font_cache_file_block_until_finished(s_font_cache_file *file);
extern "C" void font_cache_file_load(s_font_cache_file *file, long font_index, const char *name, bool block);

struct s_file_last_modification_date;

bool get_font_modification_date(struct s_file_last_modification_date * file_last_modification_date);

struct s_async_task;

extern "C" e_async_completion font_group_handle_load_task(s_async_task *task);

enum e_language;

extern "C" void font_group_get_package_name(e_language language, char *name, unsigned int size);
#endif // TEXT_FONT_GROUP_H

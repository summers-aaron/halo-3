#ifndef CACHE_CACHE_FILE_RUNTIME_GLOBALS_H
#define CACHE_CACHE_FILE_RUNTIME_GLOBALS_H

#include "tag_files/tag_resource_internals_types.h"
#include "cache/cache_file_memory_region.h"
#include "cache/cache_files_globals_types.h"
struct s_exposed_variable;

extern "C" {

#if defined(_XBOX) && !defined(__clang__)
extern cache_files_progress_globals g_cache_files_globals;
#endif
extern long g_cache_tag_memory_base;
extern long g_cache_tag_memory_end;
}

extern s_exposed_variable *g_exposed_variable_list;

class c_tag_resource_page_table;
extern "C" {
float tag_resource_page_table_get_loading_progress(c_tag_resource_page_table *table);
}

#endif // CACHE_CACHE_FILE_RUNTIME_GLOBALS_H

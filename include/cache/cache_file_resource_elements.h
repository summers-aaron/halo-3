#ifndef CACHE_CACHE_FILE_RESOURCE_ELEMENTS_H
#define CACHE_CACHE_FILE_RESOURCE_ELEMENTS_H

#include "cache/cache_file_tag_resource_element_types.h"

struct s_cache_file_resource_element_set;
extern "C" s_cache_file_tag_resource_element *cache_file_resource_get_element_by_datum(const s_cache_file_resource_element_set *set, long datum_index);

#endif // CACHE_CACHE_FILE_RESOURCE_ELEMENTS_H

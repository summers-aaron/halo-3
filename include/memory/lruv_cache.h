#ifndef MEMORY_LRUV_CACHE_H
#define MEMORY_LRUV_CACHE_H

// Functions defined in tu/blamlibxbox/lruv_cache.cpp.

class c_allocation_base;
class c_font_cache;
struct s_lruv_cache;

void lruv_idle(s_lruv_cache *cache);
s_lruv_cache *lruv_new(const char *name, long maximum_size, long page_size_bits, long maximum_block_count,
	void *client_data, void (*delete_proc)(void *, long, bool), bool (*locked_proc)(void *, long),
	unsigned char (*usage_proc)(void *, long), c_allocation_base *allocator, long unknown);
void lruv_delete(s_lruv_cache *cache);
void lruv_flush(s_lruv_cache *cache);
extern "C" void lruv_block_delete(s_lruv_cache *cache, long block_index, bool purge);
extern "C" void lruv_block_delete_internal(c_font_cache *glyph_font_cache, int hardware_block_index);
extern "C" long lruv_block_new(s_lruv_cache *cache, long datum_index, unsigned long size_in_bytes, long minimum_age);
extern "C" long lruv_block_new_at_page(s_lruv_cache *cache, long datum_index, long page_index, unsigned long size_in_bytes);

struct s_lruv_cache_hole;

void lruv_setup(s_lruv_cache *cache, const char *name, long page_size_bits, void *client_data,
	void (*delete_proc)(void *, long, bool), bool (*locked_proc)(void *, long),
	unsigned char (*usage_proc)(void *, long), c_allocation_base *allocator, long unknown);
extern "C" void lruv_block_initialize(s_lruv_cache *cache, const s_lruv_cache_hole *hole, long count, long block_index);

#endif // MEMORY_LRUV_CACHE_H

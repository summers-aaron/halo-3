#ifndef TEXT_FONT_CACHE_H
#define TEXT_FONT_CACHE_H

// Functions defined in tu/blamlibxbox/font_cache.cpp.

void internal_critical_section_leave(long index);
void font_cache_delete();

struct s_font_cache_thread_state;

extern "C" s_font_cache_thread_state *font_cache_lock(s_font_cache_thread_state *state);
extern "C" void font_cache_initialize_pixels(void);
extern "C" void font_cache_initialize(void);
extern "C" bool font_cache_open_file(bool block);
extern "C" void font_cache_close_file(void);
extern "C" void font_cache_update(void);
extern "C" void font_cache_initialize_channels(void);
extern "C" void font_cache_reset_channels(void);

struct s_font_cache_channel;

extern "C" void font_cache_reset(void);
void character_data_initialize(void);
extern "C" unsigned char font_cache_lru_pixel_is_locked(long unused, unsigned long handle);
extern "C" unsigned char font_cache_pixel_block_is_locked(unsigned long handle);
extern "C" void character_data_free_callback(void *client_data, long character_data_index);
extern "C" void font_cache_flush(void);
extern "C" unsigned long font_cache_compute_hash(void *character_key);
extern "C" unsigned char font_cache_test_keys_equal(void *first_key, void *second_key);
extern "C" void font_cache_update_channel(unsigned char flag, s_font_cache_channel *channel);
extern "C" void font_cache_handle_exit(void);
extern "C" unsigned char font_cache_is_page_valid(void *data);
void font_cache_idle();

// Data defined in tu/blamlibxbox/font_cache.cpp that other units use.

struct s_font_cache_globals;

extern s_font_cache_globals g_internal_font_cache_globals;

class c_font_cache;
class c_synchronized_long;
template<typename, typename, long> class c_flags;
enum e_character_data_index;
enum e_font_index;
struct s_font_cache_character_metrics;
struct s_font_cache_channel_block;
struct font_cache_swap_definition;

e_character_data_index character_data_allocate(c_font_cache *, e_font_index, s_font_cache_character_metrics *, long, const void *);
void character_data_free(c_font_cache *font_cache, e_character_data_index character_data_index);
extern "C" long font_cache_try_and_get_character(void *, long, long, c_flags<long, unsigned long, 2>, void *, void *, void *);
extern c_synchronized_long g_font_cache_initialized;
extern s_font_cache_channel_block g_font_cache_channels;
extern "C" {
extern font_cache_swap_definition k_font_cache_metrics_swap_definition;
}

#endif // TEXT_FONT_CACHE_H

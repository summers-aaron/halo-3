#ifndef CACHE_XBOX_SOUND_CACHE_H
#define CACHE_XBOX_SOUND_CACHE_H

// Functions defined in tu/blamlibxbox/xbox_sound_cache.cpp.

struct s_sound_permutation_chunk;

unsigned long _sound_cache_sound_request(long definition_index, s_sound_permutation_chunk *chunk, unsigned long request);
void sound_cache_new(void);
void sound_cache_flush_all_sound_guaranteed(void);
extern "C" void sound_cache_sound_release(s_sound_permutation_chunk *sound_permutation_chunk);
extern "C" bool sound_cache_is_block_locked(void *client_data, long block_index);
void sound_cache_delete_block_proc(void *client_data, long block_index, bool purge);
extern "C" void sound_cache_allocate_and_read_block(long sound_index, s_sound_permutation_chunk *chunk, bool *out_failed);

#endif // CACHE_XBOX_SOUND_CACHE_H

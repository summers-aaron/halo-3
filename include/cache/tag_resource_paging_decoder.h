#ifndef CACHE_TAG_RESOURCE_PAGING_DECODER_H
#define CACHE_TAG_RESOURCE_PAGING_DECODER_H

#include "cache/tag_resource_page_datum.h"

struct s_tag_resource_paging_fiber
{
    unsigned long state;
    long result;
    void *fiber;
    void *stack;
};
#pragma pack(push, 4)
struct tag_resource_cache_paging_decoder_state
{
    union { unsigned __int64 bit_buffer; unsigned long bits[2]; };
    int bit_count;
    int error_count;
    unsigned char literal_lengths[288];
    unsigned char distance_lengths[32];
    signed char code_lengths[19];
    int literal_lookup[256];
    int distance_lookup[256];
    int code_table[256];
    int literal_long_lookup[576];
    int distance_long_lookup[64];
    int code_nodes[38];
    union { int *position_a; unsigned long *processed; };
    union { int *position_b; unsigned long *available; };
    union { unsigned char *read_pointer; unsigned int *read_words; const char *input; };
    union { int bytes_left; unsigned long input_size; };
    union { unsigned char *output_base; char *begin; };
    union { unsigned char *output; char *cursor; };
    union { int consumed_b; unsigned long capacity; };
    union
    {
        s_tag_resource_paging_fiber fiber;
        struct
        {
            unsigned long fiber_state;
            volatile int error;
            void *suspended_fiber;
            void *volatile jump_buffer;
        };
    };
    unsigned char window[0x9828 - 0x1828];
    int window_count;
    union { unsigned char end_of_stream; unsigned char flags; };
    long status;
};

#pragma pack(pop)
typedef tag_resource_cache_paging_decoder_state s_tag_resource_paging_codec_stream;

struct s_saved_film_stream_compressor;

struct s_tag_resource_paging_owner
{
    // Same placement storage and active-object pointer as the saved-film owner.
#ifdef HALO3_64BIT
    char storage[0x9900];
#else
    char storage[0x9850];
#endif
    s_saved_film_stream_compressor *decoder;
};
typedef char paging_owner_decoder_offset[(sizeof(void *) != 4 || offsetof(s_tag_resource_paging_owner, decoder) == 0x9850) ? 1 : -1];


#endif

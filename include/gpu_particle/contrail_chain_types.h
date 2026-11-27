#ifndef GPU_PARTICLE_CONTRAIL_CHAIN_TYPES_H
#define GPU_PARTICLE_CONTRAIL_CHAIN_TYPES_H

#include <stddef.h>

struct s_data_array;
extern "C" __declspec(thread) s_data_array *g_contrail_chain_data;

// Shared by particle_block's allocation path and contrail_gpu's destruction path.
// Both traverse the same TLS data arrays (0x831df7e4 and g_contrail_gpu_link_data).
struct s_contrail_chain_element
{
    short identifier;
    long instance_index;
    long first_link;
    unsigned short link_count;
};

struct s_contrail_chain_link
{
    short identifier;
    unsigned short index;
    long previous;
    long next;
};

typedef char contrail_chain_element_size[(sizeof(s_contrail_chain_element) == 0x10) ? 1 : -1];
typedef char contrail_chain_element_link_offset[(offsetof(s_contrail_chain_element, first_link) == 8) ? 1 : -1];
typedef char contrail_chain_element_count_offset[(offsetof(s_contrail_chain_element, link_count) == 12) ? 1 : -1];
typedef char contrail_chain_link_size[(sizeof(s_contrail_chain_link) == 12) ? 1 : -1];
typedef char contrail_chain_link_next_offset[(offsetof(s_contrail_chain_link, next) == 8) ? 1 : -1];

#endif

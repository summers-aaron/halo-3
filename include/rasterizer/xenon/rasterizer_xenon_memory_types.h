#ifndef RASTERIZER_XENON_RASTERIZER_XENON_MEMORY_TYPES_H
#define RASTERIZER_XENON_RASTERIZER_XENON_MEMORY_TYPES_H

typedef struct _D3DRECT D3DRECT;
#include <xtl.h>
#include <stddef.h>
#include "math/real_math_types.h"
#include "cseries/cseries.h"
#include "rasterizer/xenon/rasterizer_xenon_memory_request_types.h"
#include "rasterizer/xenon/rasterizer_xenon_main.h"

// Member names and offsets reconciled from rasterizer_xenon_main and render_method_submit.
// The March debug image has no class records for this manager.
struct D3DCommandBuffer;

struct s_rasterizer_entry {
    unsigned int key_low;
    unsigned int key_high;
    union {
        unsigned int unused;
        D3DCommandBuffer *buffer;
    };
    unsigned int flags;
    union
    {
        unsigned int key_range;
        struct { unsigned short key_index; unsigned short key_count; };
    };
};

struct s_rasterizer_xenon_mask
{
    union
    {
        unsigned __int64 words[5];
        struct { unsigned __int64 groups; unsigned __int64 bits[4]; };
    };
};

struct s_rasterizer_xenon_memory_entry
{
    unsigned __int64 key;
};

class c_rasterizer_xenon_memory
{
public:
    friend void ::rasterizer_xenon_memory_initialize_masks(c_rasterizer_xenon_memory *);
    c_rasterizer_xenon_memory();
    bool initialize();
    void reset();
    bool new_request(s_rasterizer_xenon_memory_request **out_request, unsigned __int64 data, long type);
    s_rasterizer_xenon_memory_entry *new_entry(s_rasterizer_xenon_memory_request *request);
    s_rasterizer_xenon_memory_request m_requests[3072];
    long m_request_count;
    s_rasterizer_entry *hash_table[12287];
    union { unsigned int stage_data[4096][4]; real_vector4d stage_constants[4096]; };
    union { unsigned int stage_keys[4096]; long stage_registers[4096]; };
    long m_stage;
    s_rasterizer_xenon_memory_entry m_entries[12288];
    long m_entry_count;
    void *m_physical_buffer;
    unsigned int physical_offset;
    void *m_runtime_buffer;
    unsigned int runtime_offset;
    void *m_command_buffer;
    long submit_count;
    long m_shared_entry_count;
    bool alternate_entry_point;
    long m_tiling_configuration;
    long m_command_buffer_flags;
    long m_tile_count;
    const D3DRECT *m_tile_rects;
    s_rasterizer_xenon_mask pixel_constant_masks[2];
    s_rasterizer_xenon_mask vertex_constant_mask;
    long transparent;
    D3DTAGCOLLECTION state_a;
    D3DTAGCOLLECTION transparent_state;
    D3DTAGCOLLECTION opaque_state;
    long collision_overflow_count;
    long collision_count;
    long command_buffer_build_count;
    long command_buffer_invalidation_count;
    long command_buffer_clone_failure_count;
    long texture_entry_count;
};

STATIC_ASSERT_ABI32(offsetof(c_rasterizer_xenon_memory, stage_registers) == 0x2b000, rasterizer_xenon_registers_offset);
STATIC_ASSERT(sizeof(s_rasterizer_xenon_mask) == 40, rasterizer_xenon_memory_layout_1);
STATIC_ASSERT(sizeof(s_rasterizer_xenon_memory_entry) == 8, rasterizer_xenon_memory_layout_2);
STATIC_ASSERT_ABI32(offsetof(c_rasterizer_xenon_memory, hash_table) == 0xf004, rasterizer_xenon_memory_layout_3);
STATIC_ASSERT_ABI32(offsetof(c_rasterizer_xenon_memory, stage_data) == 0x1b000, rasterizer_xenon_memory_layout_4);
STATIC_ASSERT_ABI32(offsetof(c_rasterizer_xenon_memory, m_entries) == 0x2f008, rasterizer_xenon_memory_layout_5);
STATIC_ASSERT_ABI32(offsetof(c_rasterizer_xenon_memory, pixel_constant_masks) == 0x47040, rasterizer_xenon_memory_layout_6);
STATIC_ASSERT_ABI32(offsetof(c_rasterizer_xenon_memory, state_a) == 0x470c0, rasterizer_xenon_memory_layout_7);
STATIC_ASSERT_ABI32(offsetof(c_rasterizer_xenon_memory, transparent) == 0x470b8, rasterizer_xenon_memory_layout_9);
STATIC_ASSERT_ABI32(sizeof(c_rasterizer_xenon_memory) == 0x47150, rasterizer_xenon_memory_layout_8);

#endif

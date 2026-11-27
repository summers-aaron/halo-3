#ifndef TAG_FILES_TAG_RESOURCE_INTERNALS_TYPES_H
#define TAG_FILES_TAG_RESOURCE_INTERNALS_TYPES_H

#include "memory/basic_buffer.h"
#include "memory/wrapped_arrays.h"
#include "cache/cache_file_resource_gestalt.h"
#include "tag_files/tag_api_interop.h"
#include "tag_files/tag_resource_interop_location_types.h"
#include "tag_files/tag_resource_fixup_location_types.h"

class c_tag_interop_verifier;

struct s_tag_resource_fixup_setup
{
    c_basic_buffer<void> data_to_fixup;
    c_basic_buffer<void> cache_data;
    c_wrapped_array<const s_tag_resource_fixup_location> fixups;
};

enum e_tag_resource_fixup_type
{
    _tag_resource_fixup_type_value = 0,
    _tag_resource_fixup_type_data,
    _tag_resource_fixup_type_cache_data,
    _tag_resource_fixup_type_optional_data,
};

class c_tag_resource_interop_verifier
{
public:
    inline c_tag_resource_interop_verifier(...);

    c_basic_buffer<void> m_control_data;
    c_basic_buffer<void> m_cache_data;
    c_basic_buffer<void> m_optional_data;
};

class c_aligned_allocator
{
public:
    virtual void *allocate(unsigned long size, unsigned long alignment_bits) = 0;
    virtual void deallocate(void *pointer, unsigned long size) = 0;
};

class c_tag_resource_fixup_aligned_allocator : public c_aligned_allocator
{
public:
    __forceinline c_tag_resource_fixup_aligned_allocator(c_basic_buffer<void> allocation_buffer)
        : m_allocation_buffer(allocation_buffer), m_allocation_offset(0) {}

    virtual void *allocate(unsigned long size, unsigned long alignment_bits);
    virtual void deallocate(void *pointer, unsigned long size);

    c_basic_buffer<void> m_allocation_buffer;
    unsigned long m_allocation_offset;
};

struct s_tag_resource_interop_fixup_setup
{
    c_basic_buffer<void> control_data;
    c_basic_buffer<void> cache_data;
    c_basic_buffer<void> optional_data;
    c_basic_buffer<void> interop_data;
    c_wrapped_array<const s_tag_resource_interop_location> interops;
};

struct s_tag_resource_interop_type
{
    s_tag_persistent_identifier identifier;
    s_tag_persistent_identifier alternate_identifier;
    unsigned long size;
    unsigned long alignment_bits;
    bool (*fixup)(s_tag_interop *, c_tag_interop_verifier *);
    bool (*teardown)(s_tag_interop *);
};

typedef char tag_resource_interop_type_size[(sizeof(void *) != 4 || ((sizeof(s_tag_resource_interop_type) == 0x30))) ? 1 : -1];
typedef char tag_resource_interop_type_size_offset[(offsetof(s_tag_resource_interop_type, size) == 0x20) ? 1 : -1];
typedef char tag_resource_interop_verifier_size[(sizeof(void *) != 4 || ((sizeof(c_tag_resource_interop_verifier) == 0x18))) ? 1 : -1];
typedef char tag_resource_fixup_aligned_allocator_size[(sizeof(void *) != 4 || ((sizeof(c_tag_resource_fixup_aligned_allocator) == 0x10))) ? 1 : -1];

// Fixup/teardown routines owned by tag_vertex_buffers.obj and bitmap_group_xenon_interop.obj (March).
bool tag_d3d_texture_fixup(s_tag_interop *, c_tag_interop_verifier *);
extern "C" bool tag_d3d_texture_interleaved_fixup(s_tag_interop *, c_tag_interop_verifier *);
bool tag_d3d_vertex_buffer_teardown(s_tag_interop *);
bool tag_d3d_index_buffer_teardown(s_tag_interop *);
bool tag_d3d_texture_teardown(s_tag_interop *);
bool tag_d3d_texture_interleaved_teardown(s_tag_interop *);

extern "C" {
extern const s_tag_resource_interop_type *k_tag_resource_interop_types[];
}

#endif

// blamlibxbox/tag_resource_internals.obj: from the records 0x86990 0x86b48 0x86ca0 0x86ce0 0x287878
// @flags /TP /O2 /Oc
#include "cseries/pointer_utilities.h"
#include "phys/phys_shape_container.h"
#include "memory/wrapped_arrays.h"
#include "tag_files/tag_resource_internals.h"
#include "tag_files/tag_resource_interop_location_types.h"
#include "tag_files/tag_resource_fixup_location_types.h"

#include "memory/basic_buffer.h"
#include "tag_files/tag_api_interop.h"
#include "tag_files/tag_resource_internals_types.h"

static uintptr_t get_value_from_fixup(unsigned long fixup, c_basic_buffer<void> data, c_basic_buffer<void> *cache_data, c_basic_buffer<void> *optional_data)
{
    uintptr_t result = 0;
    uintptr_t value = fixup & 0x1fffffff;
    switch (fixup >> 29)
    {
    case _tag_resource_fixup_type_value:
        result = value;
        break;
    case _tag_resource_fixup_type_data:
        result = (uintptr_t)data.get_range(value, 0);
        break;
    case _tag_resource_fixup_type_cache_data:
        result = cache_data ? (uintptr_t)cache_data->get_range(value, 0) : 0;
        break;
    case _tag_resource_fixup_type_optional_data:
        result = optional_data ? (uintptr_t)optional_data->get_range(value, 0) : 0;
        break;
    default:
        // match: 0x86990: without it the jump table gains a fixup>>29 > 3 bounds check (cmplwi/bgt) and a li 0 default result
        __assume(0);
    }
    return result;
}

static void tag_resource_fixup_data_references(c_basic_buffer<void> data_to_fixup, c_basic_buffer<void> data, c_basic_buffer<void> *cache_data,
    c_basic_buffer<void> *optional_data, c_wrapped_array<const s_tag_resource_fixup_location> fixups)
{
    for (long index = 0; index < fixups.count(); index++)
    {
        const s_tag_resource_fixup_location &fixup = fixups[index];
        uintptr_t *destination = data_to_fixup.get_as_type_from_offset<uintptr_t>(fixup.offset);
        *destination = get_value_from_fixup(fixup.value, data, cache_data, optional_data);
    }
}

void tag_resource_fixup_cache_data_references(s_tag_resource_fixup_setup *setup, c_basic_buffer<void> *optional_data)
{
    if (!setup->data_to_fixup.empty())
        tag_resource_fixup_data_references(setup->data_to_fixup, setup->data_to_fixup, &setup->cache_data, optional_data, setup->fixups);
}

// @address 0x86ca0 ??0c_tag_resource_interop_verifier@@QAA@ZZ
inline c_tag_resource_interop_verifier::c_tag_resource_interop_verifier(...)
{
}

// Defined here (not in hkcollide): the records attribute 0x86ce0 to tag_resource_internals.obj, and the image
// pins this unit as its provider (a definition in hkConvexTransformShape.cpp leaves 0x86ce0 UNPLACED).
// @address 0x86ce0 ?get_child_shape@phys_single_shape_container@@UBAPBVphys_shape@@IAAY0CAA@D@Z
const phys_shape *phys_single_shape_container::get_child_shape(unsigned int key, char (&buffer)[512]) const
{
    return m_single_child_shape;
}

// @address 0x1f90 ??_7c_tag_resource_fixup_aligned_allocator@@6B@
// @address 0x86ce8 ?allocate@c_tag_resource_fixup_aligned_allocator@@UAAPAXKK@Z
void *c_tag_resource_fixup_aligned_allocator::allocate(unsigned long size, unsigned long alignment_bits)
{
    void *base = m_allocation_buffer.get_range(m_allocation_offset, 0);
    void *aligned = align_pointer(base, alignment_bits);
    unsigned long offset = m_allocation_offset + pointer_difference(base, aligned);
    void *result = m_allocation_buffer.get_range(offset, size);
    m_allocation_offset = offset + size;
    return result;
}
// @address 0x5bec98 ?deallocate@c_tag_resource_fixup_aligned_allocator@@UAAXPAXK@Z
void c_tag_resource_fixup_aligned_allocator::deallocate(void *pointer, unsigned long size)
{
    // The fixup bump allocator never frees individual allocations.
}


// @address 0x746030 k_tag_resource_interop_types


static __forceinline bool tag_resource_internals_fixup_manually_for_cache(s_tag_interop *interop, long type_index,
    c_aligned_allocator *allocator, c_tag_interop_verifier *verifier)
{
    struct tag_resource_internals_alloc_params { long type_index; unsigned long size; unsigned long alignment_bits; } alloc_params;
    bool valid_type = type_index >= 0 && (unsigned long)type_index < 4;
    if (valid_type)
    {
        const s_tag_resource_interop_type *type = k_tag_resource_interop_types[type_index];
        alloc_params.alignment_bits = type->alignment_bits;
        alloc_params.size = type->size;
    }
    interop->address = ((c_aligned_allocator *)allocator)->allocate(alloc_params.size, alloc_params.alignment_bits);
    const s_tag_resource_interop_type *type = k_tag_resource_interop_types[type_index];
    bool result = type->fixup == 0;
    if (type->fixup)
        result = type->fixup(interop, verifier);
    return result;
}

// @address 0x86b48 ?tag_resource_fixup_interops@@YAXPAUs_tag_resource_interop_fixup_setup@@@Z
void tag_resource_fixup_interops(s_tag_resource_interop_fixup_setup *setup)
{
    c_tag_resource_fixup_aligned_allocator allocator(setup->interop_data);
    c_tag_resource_interop_verifier verifier(setup->control_data, setup->cache_data, setup->optional_data);
    long interop_count = setup->interops.count();
    c_basic_buffer<void> control_data;
    for (long index = 0; index < interop_count; index++)
    {
        control_data = setup->control_data;
        const s_tag_resource_interop_location *location = &setup->interops[index];
        unsigned long fixup = location->interop_fixup;
        uintptr_t value = fixup & 0x1fffffff;
        s_tag_interop *interop;
        switch (fixup >> 29)
        {
        case _tag_resource_fixup_type_value:
            interop = (s_tag_interop *)value;
            break;
        case _tag_resource_fixup_type_data:
            interop = (s_tag_interop *)control_data.get_range(value, 0);
            break;
        case _tag_resource_fixup_type_cache_data:
            interop = 0;
            break;
        case _tag_resource_fixup_type_optional_data:
            interop = 0;
            break;
        default:
            // match: 0x86b48: without it the jump table gains a fixup>>29 > 3 bounds check (cmplwi/bgt)
            __assume(0);
        }
        tag_resource_internals_fixup_manually_for_cache(interop, location->interop_type_index, (c_aligned_allocator *)&allocator,
            (c_tag_interop_verifier *)&verifier);
    }
}

static __forceinline void *tag_resource_internals_get_address_from_fixup(unsigned long fixup, const c_basic_buffer<void> &data,
    c_basic_buffer<void> *cache_data, c_basic_buffer<void> *optional_data)
{
    void *result = 0;
    uintptr_t value = fixup & 0x1fffffff;
    switch (fixup >> 29)
    {
    case _tag_resource_fixup_type_value:
        result = (void *)value;
        break;
    case _tag_resource_fixup_type_data:
        result = data.get_range(value, 0);
        break;
    case _tag_resource_fixup_type_cache_data:
        result = cache_data ? cache_data->get_range(value, 0) : 0;
        break;
    case _tag_resource_fixup_type_optional_data:
        result = optional_data ? optional_data->get_range(value, 0) : 0;
        break;
    default:
        // match: 0x287878: without it the jump table gains a fixup>>29 > 3 bounds check (cmplwi/bgt) and a li 0 default result
        __assume(0);
    }
    return result;
}

static __forceinline void tag_resource_internals_teardown_manually_for_cache(s_tag_interop *interop, long type_index,
    c_aligned_allocator *allocator, unsigned long &size)
{
    bool valid_type = type_index >= 0 && (unsigned long)type_index < 4;
    if (valid_type)
    {
        const s_tag_resource_interop_type *type = k_tag_resource_interop_types[type_index];
        size = type->alignment_bits;
    }
    const s_tag_resource_interop_type *type = k_tag_resource_interop_types[type_index];
    if (type->teardown)
        type->teardown(interop);
    allocator->deallocate(interop->address, size);
    interop->address = 0;
}

// @address 0x287878 ?tag_resource_teardown_interops@@YAXPAUs_tag_resource_interop_fixup_setup@@@Z
void tag_resource_teardown_interops(s_tag_resource_interop_fixup_setup *setup)
{ 
    struct tag_resource_internals_teardown_context
    {
        c_basic_buffer<void> control_data;
        unsigned long size;
        tag_resource_internals_teardown_context() : control_data(c_basic_buffer<void>::_uninitialized) { }
    } context;
    c_tag_resource_fixup_aligned_allocator allocator(setup->interop_data);
    long interop_count = setup->interops.count();
    for (long index = 0; index < interop_count; index++)
    {
        context.control_data = setup->control_data;
        const s_tag_resource_interop_location *location = &setup->interops[index];
        tag_resource_internals_teardown_manually_for_cache(
            (s_tag_interop *)tag_resource_internals_get_address_from_fixup(location->interop_fixup, context.control_data, 0, 0),
            location->interop_type_index, (c_aligned_allocator *)&allocator, context.size);
    }
}


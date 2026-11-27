// blamlibxbox/runtime_state.obj: from the records 0x19c820 0x19c838 0x19c848 0x19c8d0
// @flags /TP /O1
// @address 0x19c820 ?allocate@c_runtime_state_allocation@@UAAPAXIPBD@Z
// @address 0x19c848 ?runtime_state_malloc@@YAPAXPBD0J@Z
// @address 0x19c8d0 ?runtime_state_data_new@@YAPAUs_data_array@@PBDJJ@Z
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "cseries/runtime_state.h"
#include "cseries/pointer_utilities.h"
#include "memory/crc.h"
#include "memory/data.h"
#include "memory/no_allocation.h"
#include "memory/runtime_state_allocation.h"
#include "memory/runtime_state.h"
#include "memory/data_types.h"

enum
{
#ifdef HALO3_64BIT
    k_runtime_state_size = 0x600000
#else
    k_runtime_state_size = 0x2f0000
#endif
};

// @address 0x74b7ec g_no_allocation_storage
c_no_allocation g_no_allocation_storage;
// @address 0x74b80c g_runtime_state_allocation
c_runtime_state_allocation g_runtime_state_allocation;

// ICF-folds into 0x42d298 (`li r3,0; blr`), so it takes no @address and must stay a bare `return 0`.
void *c_no_allocation::allocate(unsigned int size, const char *name)
{
    return 0;
}

// ICF-folds into 0x5bec98 (`blr`), so it takes no @address and must stay empty.
void c_no_allocation::deallocate(void *allocation)
{
}

void *c_runtime_state_allocation::allocate(unsigned int size, const char *name)
{
    return runtime_state_malloc("", "", size);
}

// @address 0x19c838 ?runtime_state_initialize_for_new_map@@YAXXZ
void runtime_state_initialize_for_new_map(void)
{
    g_runtime_state_globals.valid = true;
}

// @address 0x5bec98 ?runtime_state_initialize@@YAXXZ
void runtime_state_initialize(void)
{
}

// @address 0x5bec98 ?runtime_state_dispose@@YAXXZ
void runtime_state_dispose(void)
{
}

// @address 0x5bec98 ?runtime_state_dispose_from_old_map@@YAXXZ
void runtime_state_dispose_from_old_map(void)
{
}

void *runtime_state_malloc(const char *name, const char *type, long requested_size)
{
    void *result = 0;
    void *unaligned_address = offset_pointer(g_runtime_state_globals.buffer, g_runtime_state_globals.size);
    void *aligned_address = align_pointer(unaligned_address, 2);
    long true_size = (long)((char *)aligned_address - (char *)unaligned_address) + requested_size;
    long allocated_size = g_runtime_state_globals.size + true_size;
    if ((unsigned long)allocated_size <= k_runtime_state_size) {
        g_runtime_state_globals.size = allocated_size;
        g_runtime_state_globals.checksum = crc_checksum_buffer(g_runtime_state_globals.checksum, &true_size, sizeof(true_size));
        result = aligned_address;
    }
    return result;
}

s_data_array *runtime_state_data_new(const char *name, long maximum_count, long datum_size)
{
    s_data_array *data_array = (s_data_array *)runtime_state_malloc(name, "data array", data_allocation_size(maximum_count, datum_size, 0));
    data_initialize(data_array, name, maximum_count, datum_size, 0, &g_no_allocation_storage);
    return data_array;
}

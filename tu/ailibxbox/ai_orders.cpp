// ailibxbox/ai_orders.obj: from the records 0x20ed00 0x210be0 0x210c80 0x210c98
// @flags /TP /O2
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0x20ed00 ?__tls_set_g_trigger_activations_allocator@@YAXPAX@Z
#include "cache/restricted_memory_regions.h"
#include "multithreading/thread_tls.h"
#include "ai/ai_orders.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory.h"
#include "cache/restricted_memory_registration.h"

// @address 0x11df860 g_bink_prediction_frames
__declspec(thread) int *g_bink_prediction_frames;

void __tls_set_g_trigger_activations_allocator(void *allocator)
{
    g_bink_prediction_frames = static_cast<int *>(allocator);
}


// Registers a member of field layout 1 (see c_restricted_memory::add_member) and publishes its
// per-thread address. One of a family of identical allocators that differ
// only in the layout index, the update callback and the thread-local pointer.






// @address 0x210be0 ?reserve_memory@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_trigger_activations_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_trigger_activations_allocator>::reserve_memory(const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = restricted_memory_register_member(&g_restricted_regions[3], name, type, size, 0, __tls_set_g_trigger_activations_allocator, 0, 0);
  unsigned int offset = restricted_region_get_member_offset(3, member_index);
  void *address = restricted_region_get_base_address(3) + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  g_bink_prediction_frames = static_cast<int *>(address);
  return restricted_region_get_base_address(3) + restricted_region_get_member_offset(3, member_index);
}
// @address 0x210c80 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_trigger_activations_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_trigger_activations_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}



// 0x210c98
// @address 0x210c98 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_trigger_activations_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_trigger_activations_allocator>::deallocate(void *address)
{
    const long &member_index = +m_member_index;
    g_bink_prediction_frames = 0;
    restricted_region_free_member(3, member_index);
}

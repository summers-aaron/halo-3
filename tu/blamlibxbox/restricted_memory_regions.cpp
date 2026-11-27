// @address 0x11df8a8 g_chud_widget_data
// blamlibxbox/restricted_memory_regions.obj: from the records 0x9cd60 0x9cdc8 0x1d6b10 0x1d6bb0 0x1d6c70 0x1d6ce0 0x1d6da8 0x1d6e40 0x44dc90 0x44dd30 0x44dd48 0x44de80 0x44de90 0x44df30 0x44dfd0 0x44dfe8 0x472d98
// @flags /TP /O2 /Oc
#include "cache/restricted_memory_regions_externs.h"
#include "interface/chud/chud_widget_data.h"
#include "interface/chud/chud.h"
#include "cseries/crt_xdk.h"
#include "multithreading/thread_tls.h"
#include <ppcintrinsics.h>
#include "multithreading/synchronization.h"
#include "cache/restricted_memory_regions.h"
#include "render/camera_fx_settings.h"
typedef unsigned int uint;
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory.h"
#include "multithreading/synchronization_inline.h"
#include "cache/restricted_memory_registration.h"
#include "cache/restricted_memory_regions_inline.h"

// @address 0x1d54e8 ??0c_restricted_memory@@QAA@XZ
// @address 0x1d5708 ?uninitialize@c_restricted_memory@@AAAXXZ
// @address 0x5bec98 ??1s_mirror_slot@c_restricted_memory@@QAA@XZ
// @address 0x5bec98 ??1s_registered_member@c_restricted_memory@@QAA@XZ

__forceinline c_restricted_memory::~c_restricted_memory()
{
    uninitialize();
}

// March 829691f0: destruction enters the region lock before uninitialization.
__forceinline void c_restricted_memory::destroy()
{
    if (m_critical_section != -1)
        internal_critical_section_enter(m_critical_section);
    uninitialize();
    internal_critical_section_leave(m_critical_section);
}

// @address 0x11df630 g_restricted_address
__declspec(thread) char *g_restricted_address[5];
// @address 0x11df644 g_restricted_alias
__declspec(thread) bool g_restricted_alias[5];

/* natural 0x9cd60: no March name or pairing is available. */
// @address 0x9cd60 restricted_region_is_available
extern "C" bool restricted_region_is_available(int index)
{
    return g_restricted_address[index] != 0 && !restricted_region_mirror_locked_for_current_thread(index);
}

// @address 0x9cdc8 ?mirror_locked_for_current_thread@c_restricted_memory@@QAA_NXZ
bool c_restricted_memory::mirror_locked_for_current_thread()
{
    bool result = false;
    if (m_mirror_count > 0)
    {
        long mirror_index = m_mirror_readers.peek() % m_mirror_count;
        result = m_mirrors[mirror_index].section->locked_for_current_thread();
    }
    return result;
}

bool restricted_region_mirror_locked_for_current_thread(long index)
{
    return g_restricted_regions[index].mirror_locked_for_current_thread();
}
void restricted_region_destroy(long index)
{
    g_restricted_regions[index].destroy();
}
void restricted_region_free_member(long index, long member_index)
{
    g_restricted_regions[index].free_member(member_index);
}
void restricted_region_lock_primary(long index)
{
    void *base_address = g_restricted_regions[index].lock_primary();
    g_restricted_address[index] = reinterpret_cast<char *>(base_address);
}
// @address 0x1d6ce0 ?restricted_region_try_and_lock_primary@@YA_NJ@Z
bool restricted_region_try_and_lock_primary(long index)
{
    void *address = g_restricted_regions[index].try_and_lock_primary();
    g_restricted_address[index] = static_cast<char *>(address);
    return address != 0;
}

void restricted_region_add_alias(long index)
{
    g_restricted_address[index] = reinterpret_cast<char *>(g_restricted_regions[index].add_alias());
    g_restricted_alias[index] = true;
}
void restricted_region_remove_alias(long index)
{
    g_restricted_regions[index].remove_alias();
    g_restricted_address[index] = 0;
    g_restricted_alias[index] = false;
}

namespace {
static __forceinline void *restricted_memory_regions_get_member_address(long region, long member)
{
    char *base = restricted_region_get_base_address(region);
    uint offset = g_restricted_regions[region].get_member_offset(member);
    return base + offset;
}
static __forceinline long restricted_memory_regions_add_member_region3(const char *name, const char *type, uint size, long alignment_bits)
{
    long member = restricted_memory_register_member(&g_restricted_regions[3], name, type, size, alignment_bits,
        camera_fx_set_globals_address, 0, 0);
    restricted_camera_fx_set_address(restricted_memory_regions_get_member_address(3, member));
    return member;
}
}
// @address 0x44dc90 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?camera_fx_set_globals_address@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0), &camera_fx_set_globals_address>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x44dc90: value range assumption guides branch lowering
    __assume(alignment_bits == 0);
    m_member_index = restricted_memory_regions_add_member_region3(name, type, size, alignment_bits);
    m_thread_id = get_current_thread_index();
    return restricted_memory_regions_get_member_address(3, m_member_index);
}
// @address 0x44dd30 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?camera_fx_set_globals_address@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &camera_fx_set_globals_address>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}
// @address 0x44dd48 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?camera_fx_set_globals_address@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &camera_fx_set_globals_address>::deallocate(void *address)
{
    restricted_camera_fx_set_address(0);
    restricted_region_free_member(3, m_member_index);
}
// @address 0x44df30 ?reserve_memory@?$t_restricted_allocation_manager@$01$0A@$0A@$1?restricted_region_set_region2_address@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &restricted_region_set_region2_address>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x44df30: value range assumption guides branch lowering
    __assume(alignment_bits == 0);
    long member = restricted_memory_register_member(&g_restricted_regions[2], name, type, size, alignment_bits,
        restricted_region_set_region2_address, 0, 0);
    restricted_region2_set_address(restricted_memory_regions_get_member_address(2, member));
    m_member_index = member;
    m_thread_id = get_current_thread_index();
    return restricted_memory_regions_get_member_address(2, m_member_index);
}
/* natural 0x44de80: sets thread-local address for region 2. */
// @address 0x44de80 restricted_region_set_region2_address
extern "C" void restricted_region_set_region2_address(void *value)
{
    restricted_region2_set_address(value);
}
/* natural 0x44de90: reserves the 0x910-byte block for the 2-region manager. */
// @address 0x44de90 restricted_region_reserve_region2_storage
extern "C" void restricted_region_reserve_region2_storage(void)
{
    restricted_region2_set_address(g_restricted_region2_allocator.reserve_memory("render game globals", 0, 0x910, 0));
}
// @address 0x44dfd0 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?restricted_region_set_region2_address@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &restricted_region_set_region2_address>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}
// @address 0x44dfe8 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?restricted_region_set_region2_address@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &restricted_region_set_region2_address>::deallocate(void *address)
{
    restricted_region2_set_address(0);
    restricted_region_free_member(2, m_member_index);
}
/* natural 0x472d98: clears the thread's slot, then frees region 3 member. */
// @address 0x472d98 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_chud_user_data_allocator_0@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_chud_user_data_allocator_0>::deallocate(void *allocation)
{
    g_chud_widget_data[0] = 0;
    restricted_region_free_member(3, m_member_index);
}

c_restricted_memory g_restricted_regions[5];
restricted_region2_allocation_manager g_restricted_region2_allocator;



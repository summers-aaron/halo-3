// hkbase/hkMemory.obj: from the records 0x60c368 0x60c370 0x60c380 0x60c388 0x60c390 0x60c418 0x60c438 0x60c448
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x60c368 ?PHYS_ASSERT_OBJECT_SIZE_OK_FUNC@@YAXI@Z
// @address 0x60c370 ??1phys_memory@@UAA@XZ
// @address 0x60c380 ?phys_windows_malloc@@YAPAXHH@Z
// @address 0x60c388 ?phys_windows_free@@YAXPAX@Z
// @address 0x60c390 ?replace_singleton_instance@phys_memory@@SAXPAV1@@Z
// @address 0x60c418 ?get_sector_allocated_size@phys_memory@@UAAHH@Z
// @address 0x60c438 ?is_stream_ok@phys_memory@@UBA?AVphys_bool@@XZ
// @address 0x60c448 ??0phys_memory@@QAA@XZ
#include "phys/phys_memory.h"
#include "physics/havok_shapes.h"

phys_memory *phys_memory::s_instance = 0;

void PHYS_ASSERT_OBJECT_SIZE_OK_FUNC(unsigned int nbytes)
{
}

phys_memory::~phys_memory()
{
}

void *phys_windows_malloc(int size, int alignment)
{
    return havok_aligned_malloc(size, alignment);
}

void phys_windows_free(void *block)
{
    havok_aligned_free(block);
}

void phys_memory::replace_singleton_instance(phys_memory *memory)
{
    if (memory)
        memory->add_object_reference();
    if (s_instance)
        s_instance->object_remove_ref();
    s_instance = memory;
}

int phys_memory::get_sector_allocated_size(int size)
{
    if (size > 16) {
        size = (size + 15) & ~15;
        return size + 16;
    } else {
        return size + 8;
    }
}

phys_bool phys_memory::is_stream_ok() const
{
    return true;
}

// The linker folds these base-class trivial virtuals: the two null-returning
// bodies onto 0x42d298, the two empty bodies onto 0x5bec98.
// @address 0x42d298 ?is_debug_memory@phys_memory@@UAA_NXZ
bool phys_memory::is_debug_memory()
{
    return false;
}

// @address 0x42d298 ?find_base_address@phys_memory@@UAAPAXPAXH@Z
void *phys_memory::find_base_address(void *, int)
{
    return 0;
}

// @address 0x5bec98 ?lock_mem_block@phys_memory@@UAAXPAX@Z
void phys_memory::lock_mem_block(void *)
{
}

// @address 0x5bec98 ?unlock_mem_block@phys_memory@@UAAXPAX@Z
void phys_memory::unlock_mem_block(void *)
{
}

phys_memory::phys_memory()
{
    m_memory_ref_count = 1;
    m_sys_alloc_count = 0;
    m_sys_free_count = 0;
    m_total_sys_memory = 0;
    m_current_sys_memory = 0;
    m_peak_sys_memory = 0;
    m_runtime_alloc_count = 0;
    m_runtime_free_count = 0;
    m_memory_state = MEMORY_STATE_OK;
    m_memory_limit = 0x7fffffff;
}

// @address 0x827244e0
void *(*phys_system_malloc)(int, int) = phys_windows_malloc;
void (*phys_system_free)(void *) = phys_windows_free;


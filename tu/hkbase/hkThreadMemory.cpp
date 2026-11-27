// hkbase/hkThreadMemory.obj: from the records 0x609d48 0x609d50 0x609d78 0x609d88 0x609dd0 0x60a0e8 0x60a1b0 0x60a298 0x60a380 0x60a488 0x60a500 0x60a530 0x60a5b8 0x60a5c8 0x60a620 0x60a670 0x60a680 0x60a6d8 0x60a708 0x60a768 0x60a808
// @flags /TP /O2 /Oc
// @address 0x609d48 ?verify_cached_memory@phys_thread_memory@@QAAXXZ
// @address 0x609d50 ?object_remove_ref@phys_thread_memory@@QAAXXZ
// @address 0x609d78 ?add_object_reference@phys_thread_memory@@QAAXXZ
// @address 0x609d88 ?set_stack_area@phys_thread_memory@@UAAXPAXH@Z
// @address 0x609dd0 ??0phys_thread_memory@@QAA@PAVphys_memory@@H@Z
// @address 0x60a0e8 ?on_row_empty@phys_thread_memory@@IAAPAXH@Z
// @address 0x60a1b0 ?on_row_full@phys_thread_memory@@IAAXHH@Z
// @address 0x60a298 ?allocate_chunk@phys_thread_memory@@QAAPAXHW4phys_memory_class@@@Z
// @address 0x60a380 ?free_chunk@phys_thread_memory@@QAAXPAXHW4phys_memory_class@@@Z
// @address 0x60a500 ?on_stack_underflow@phys_thread_memory@@MAAXPAX@Z
// @address 0x60a530 ?replace_singleton_instance@phys_thread_memory@@SAXPAV1@@Z
// @address 0x60a5b8 ?init@phys_thread_memory@@SAXXZ
// @address 0x60a5c8 ?quit_system@phys_thread_memory@@SAXXZ
// @address 0x60a620 ?release_cached_memory@phys_thread_memory@@UAAXXZ
// @address 0x60a670 ??1phys_thread_memory@@UAA@XZ
// @address 0x60a680 ?allocate@phys_thread_memory@@QAAPAXHW4phys_memory_class@@@Z
// @address 0x60a6d8 ?deallocate@phys_thread_memory@@QAAXPAX@Z
// @address 0x60a708 ?aligned_allocate@phys_thread_memory@@UAAPAXHHW4phys_memory_class@@@Z
// @address 0x60a768 ?aligned_deallocate@phys_thread_memory@@UAAXPAX@Z

#include "physics/havok_memory.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_memory.h"

phys_thread_local_data<phys_thread_memory *> phys_thread_memory::s_threadMemoryInstance;



namespace {
struct memory_header {
    unsigned int m_magic_tag;
    int m_size;
    phys_memory_class m_class_desc;
    int m_offset;
};
static __forceinline int phys_thread_memory_const_row(int size)
{
    if (size <= 8) return 1;
    else if (size <= 16) return 2;
    else if (size <= 32) return 3;
    else if (size <= 48) return 4;
    else if (size <= 64) return 5;
    else if (size <= 96) return 6;
    else if (size <= 128) return 7;
    else if (size <= 160) return 8;
    else if (size <= 192) return 9;
    else if (size <= 256) return 10;
    else if (size <= 320) return 11;
    else if (size <= 512) return 12;
    else if (size <= 1024) return 13;
    else if (size <= 2048) return 14;
    else if (size <= 4096) return 15;
    else if (size <= 8192) return 16;
    else { halo_external_breakpoint(); return -1; }
}

static __forceinline int phys_thread_memory_row_for_size(phys_thread_memory *memory, int size)
{
    if (size <= 512)
        return memory->m_size_to_row[size];
    return memory->m_large_size_row[(size - 1) >> 10];
}
static __forceinline void *phys_thread_memory_free_list_get(phys_thread_memory::free_block_list *list)
{
    phys_thread_memory::free_block_list::free_elem *element = list->m_head;
    if (element != 0) {
        list->m_elem_count--;
        list->m_head = element->m_next;
        return element;
    } else {
        return 0;
    }
}
static __forceinline void phys_thread_memory_free_list_put(phys_thread_memory::free_block_list *list, void *block)
{
    list->m_elem_count++;
    phys_thread_memory::free_block_list::free_elem *element = (phys_thread_memory::free_block_list::free_elem *)block;
    element->m_next = list->m_head;
    list->m_head = element;
}
}

void phys_thread_memory::verify_cached_memory(void)
{
    havok_override_thread_memory_allocations();
}

#if defined(_MSC_VER) && !defined(__clang__)
// match: 0x60a530: replaceInstance keeps the 136B inline expansion of this call; without forceinline the Xbox build
// emits a call there and shrinks it by 32B, while the PC link needs the out-of-line body (plain signature below).
__forceinline void phys_thread_memory::object_remove_ref()
#else
void phys_thread_memory::object_remove_ref()
#endif
{
    --m_object_ref_count;
    if (m_object_ref_count == 0) {
        delete this;
    }
}


__forceinline void phys_thread_memory::add_object_reference()
{
    ++m_object_ref_count;
}

void phys_thread_memory::set_stack_area(void *p, int nbytes)
{
    size_t alignmentOffset = (size_t)p & 15;
    m_mem_stack.m_base = (char *)-1;
    m_stack_size = nbytes;
    if (alignmentOffset != 0) {
        m_mem_stack.m_current = (char *)p + 16 - alignmentOffset;
        m_mem_stack.m_end = m_mem_stack.m_current + (nbytes - alignmentOffset);
    } else {
        m_mem_stack.m_current = (char *)p;
        m_mem_stack.m_end = m_mem_stack.m_current + nbytes;
    }
}

phys_thread_memory::phys_thread_memory(phys_memory *owner, int max_free_list_size)
    : m_object_ref_count(1)
{
    if (owner->is_debug_memory())
        max_free_list_size = 0;
    m_memory_pool = owner;
    m_max_free_list_elems = max_free_list_size < 0x0fffffff ? max_free_list_size : 0x0fffffff;
    for (int size = 0; size <= 512; ++size) {
        int row = phys_thread_memory_const_row(size);
        m_size_to_row[size] = (char)row;
        m_row_size[row] = size;
    }
    for (int index = 0; index < 8; ++index) {
        int size = (index + 1) << 10;
        int row = phys_thread_memory_const_row(size);
        m_large_size_row[index] = row;
        m_row_size[row] = size;
    }
}

void *phys_thread_memory::on_row_empty(int row)
{
    void *chunks[4];
    m_memory_pool->allocate_chunk_batch(chunks, 4, m_row_size[row]);
    free_block_list &row_free_list = m_block_free_list[row];
    for (int index = 1; index < 4; ++index) {
        phys_thread_memory_free_list_put(&row_free_list, chunks[index]);
    }
    return chunks[0];
}

void phys_thread_memory::on_row_full(int row, int release_all)
{
    int row_chunk_size = m_row_size[row];
    free_block_list &row_free_list = m_block_free_list[row];
    int count = row_free_list.m_elem_count;
    int target_free_count = release_all ? 0 : m_max_free_list_elems / 2;
    void *chunks[4];
    while (count > target_free_count) {
        int release_batch = count - target_free_count > 4 ? 4 : count - target_free_count;
        for (int index = 0; index < release_batch; ++index) {
            chunks[index] = phys_thread_memory_free_list_get(&row_free_list);
        }
        count -= release_batch;
        m_memory_pool->free_chunk_batch(chunks, release_batch, row_chunk_size);
    }
}


void *phys_thread_memory::allocate_chunk(int nbytes, phys_memory_class cls)
{
    if (havok_override_thread_memory_allocations())
        return havok_thread_memory_allocate_chunk(nbytes, cls);
    if (nbytes <= 8192 && m_max_free_list_elems != 0) {
        int row = phys_thread_memory_row_for_size(this, nbytes);
        void *block = phys_thread_memory_free_list_get(&m_block_free_list[row]);
        if (block != 0)
            return block;
        return on_row_empty(row);
    }
    return m_memory_pool->allocate_chunk(nbytes, cls);
}

void phys_thread_memory::free_chunk(void *block, int nbytes, phys_memory_class cls)
{
    if (havok_override_thread_memory_allocations()) {
        havok_thread_memory_deallocate_chunk(block, nbytes, cls);
        return;
    }
    if (block == 0)
        return;
    if (nbytes <= 8192 && m_max_free_list_elems != 0) {
        int row = phys_thread_memory_row_for_size(this, nbytes);
        if (m_block_free_list[row].m_elem_count >= m_max_free_list_elems)
            on_row_full(row, 0);
        phys_thread_memory_free_list_put(&m_block_free_list[row], block);
        return;
    }
    m_memory_pool->free_chunk(block, nbytes, cls);
}

void phys_thread_memory::on_stack_underflow(void *p)
{
    char *chunk = m_mem_stack.m_base - 16;
    int row_chunk_size = m_mem_stack.m_end - m_mem_stack.m_base + 16;
    m_mem_stack = *(Stack *)chunk;
    free_chunk(chunk, row_chunk_size, PHYS_MEM_ARRAY);
}

void phys_thread_memory::replace_singleton_instance(phys_thread_memory *memory)
{
    if (memory) {
        memory->add_object_reference();
    }
    phys_thread_memory *prev_thread_mem = s_threadMemoryInstance.get_chain_data();
    if (prev_thread_mem) {
        prev_thread_mem->object_remove_ref();
    }
    s_threadMemoryInstance.lock_set_data(memory);
}

void phys_thread_memory::init()
{
    s_threadMemoryInstance.lock_set_data(0);
}

void phys_thread_memory::quit_system()
{
    phys_thread_memory *memory = s_threadMemoryInstance.get_chain_data();
    delete memory;
    s_threadMemoryInstance.lock_set_data(0);
}

void phys_thread_memory::release_cached_memory()
{
    for (int row = 16; row >= 0; --row) {
        if (m_block_free_list[row].m_elem_count != 0) {
            on_row_full(row, 1);
        }
    }
}

phys_thread_memory::~phys_thread_memory()
{
    release_cached_memory();
}

void *phys_thread_memory::allocate(int nbytes, phys_memory_class cls)
{
    memory_header *header = (memory_header *)allocate_chunk(nbytes + 16, cls);
    header->m_magic_tag = 0x2345656;
    header->m_size = nbytes;
    header->m_class_desc = cls;
    return header + 1;
}

void phys_thread_memory::deallocate(void *p)
{
    if (p) {
        memory_header *header = (memory_header *)p - 1;
        header->m_magic_tag = 0xdeadbeef;
        free_chunk(header, header->m_size + 16, header->m_class_desc);
    }
}

void *phys_thread_memory::aligned_allocate(int alignment, int nbytes, phys_memory_class cls)
{
    char *chunk = (char *)allocate_chunk(alignment + nbytes + 16, cls);
    char *aligned_addr = (char *)((size_t)(chunk + 16 + (alignment - 1)) & ~(size_t)(alignment - 1));
    memory_header *header = (memory_header *)aligned_addr - 1;
    header->m_magic_tag = 0x2345656;
    header->m_size = nbytes + alignment;
    header->m_class_desc = cls;
    header->m_offset = aligned_addr - chunk;
    return (void *)aligned_addr;
}

void phys_thread_memory::aligned_deallocate(void *p)
{
    if (p) {
        memory_header *header = (memory_header *)p - 1;
        header->m_magic_tag = 0xdeadbeef;
        char *chunk = (char *)p - header->m_offset;
        free_chunk(chunk, header->m_size + 16, header->m_class_desc);
    }
}

// @address 0x60a808 ??3phys_thread_memory@@SAXPAX@Z
void phys_thread_memory::operator delete(void *p)
{
}

// @address 0x60a488 ?on_stack_overflow@phys_thread_memory@@MAAPAXH@Z
void *phys_thread_memory::on_stack_overflow(int p1)
{
    int v1 = p1 + 0x400;
    if (v1 <= 0x1000)
        v1 = 0x1000;
    phys_thread_memory *instance = s_threadMemoryInstance.get_chain_data();
    char *new_chunk_mem = (char *)instance->allocate_chunk(v1 + 16, PHYS_MEM_ARRAY);
    *(Stack *)new_chunk_mem = m_mem_stack;
    m_mem_stack.m_base = new_chunk_mem + 16;
    m_mem_stack.m_current = m_mem_stack.m_base + p1;
    m_mem_stack.m_end = m_mem_stack.m_base + v1;
    m_mem_stack.m_prev_link = (Stack *)new_chunk_mem;
    return m_mem_stack.m_base;
}

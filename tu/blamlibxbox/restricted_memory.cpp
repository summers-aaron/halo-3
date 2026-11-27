// blamlibxbox/restricted_memory.obj: from the records 0x142210 0x142338 0x1d5488 0x1d54e8 0x1d5600 0x1d5708 0x1d5888 0x1d5a08 0x1d5b30 0x1d5be0 0x1d5cf0 0x1d5d50 0x1d6d48
// @flags /TP /O2 /J /Oc
#include "multithreading/synchronization_types.h"
#include "cseries/crt_xdk.h"
#include "multithreading/synchronization.h"

#include "cache/restricted_memory_registration.h"
#include <ppcintrinsics.h>

#include "multithreading/synchronized_value.h"
#include "cache/restricted_memory.h"
#include "multithreading/thread_tls.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_copy.h"
#include "cseries/pointer_utilities.h"
#include "cache/restricted_memory_regions.h"

// @address 0x846a8 ?internal_critical_section_enter@@YAXJ@Z

// If the index is valid, decrement its lock count and call the native
// critical-section release (read_write_lock.cpp has the same inline expansion).
static __forceinline void restricted_memory_critical_section_leave(long index)
{
    if (index != -1)
    {
        _InterlockedDecrement(&g_synch_globals.critical_sections[index].lock_count.value);
        RtlLeaveCriticalSection(&g_synch_globals.critical_sections[index].critical_section);
    }
}

// @address 0x142278 ?internal_critical_section_try_and_enter@@YA_NJ@Z
// @address 0x5a18c0 restricted_memory_copy_aligned_blocks
// @address 0x5a1c50 restricted_memory_copy_unaligned_blocks
// @address 0x142210 ?try_and_lock@c_restricted_section@@AAAPAXXZ
void *c_restricted_section::try_and_lock()
{
    void *result = 0;
    if (internal_critical_section_try_and_enter(m_critical_section)) {
        m_owner_thread = get_current_thread_index();
        result = m_address;
    }
    return result;
}

// @address 0x1d5488 ?unlock@c_restricted_section@@AAAXXZ
void c_restricted_section::unlock()
{
    m_owner_thread = -1;
    restricted_memory_critical_section_leave(m_critical_section);
}

// @address 0x1d54e8 ??0c_restricted_memory@@QAA@XZ
c_restricted_memory::c_restricted_memory()
    : m_primary_locked(0), m_mirror_locked(0),
      m_primary_readers(0), m_mirror_readers(0),
      m_primary_valid(0), m_mirror_valid(0),
      m_primary_section(0), m_internal_flags(0), m_subsection_size(0), m_reserved_size(0),
      m_mirror_count(0), m_critical_section(-1), m_registered_member_count(0),
      m_callbacks(0), m_primary_allocation(0), m_mirror_allocation(0)
{
    for (long mirror_index = 0; mirror_index < 4; mirror_index++) {
        m_mirrors[mirror_index].readers.set(0);
        m_mirrors[mirror_index].valid.set(0);
        m_mirrors[mirror_index].locked.set(0);
        m_mirrors[mirror_index].section = 0;
    }
}

// @address 0x1d5600 ?add_mirror@c_restricted_memory@@QAAXPAVc_restricted_section@@@Z
void c_restricted_memory::add_mirror(c_restricted_section *mirror_section)
{
    if (m_critical_section != -1)
        internal_critical_section_enter(m_critical_section);
    m_mirrors[m_mirror_count].section = mirror_section;
    m_mirrors[m_mirror_count].locked.set(1);
    m_mirrors[m_mirror_count].readers.set(1);
    m_mirror_count++;
    restricted_memory_critical_section_leave(m_critical_section);
}

// @address 0x1d5708 ?uninitialize@c_restricted_memory@@AAAXXZ
void c_restricted_memory::uninitialize()
{
  if ((m_internal_flags & 1) != 0) {
    m_primary_section = 0;
    if (m_primary_allocation) {
      VirtualFree(m_primary_allocation, m_subsection_size, 0x8000);
    }
    m_primary_allocation = 0;
    if (m_mirror_allocation) {
      VirtualFree(m_mirror_allocation, m_subsection_size, 0x8000);
    }
    m_mirror_allocation = 0;
    m_primary_readers.set(0);
    m_mirror_readers.set(0);
    m_primary_locked.set(0);
    m_mirror_locked.set(0);
    for (long mirror_index = 0; mirror_index < 4; mirror_index++) {
      m_mirrors[mirror_index].readers.set(0);
      m_mirrors[mirror_index].valid.set(0);
      m_mirrors[mirror_index].locked.set(0);
      m_mirrors[mirror_index].section = 0;
    }
    m_subsection_size = 0;
    m_mirror_count = 0;
    m_reserved_size = 0;
    m_callbacks = 0;
    m_internal_flags = 0;
  }
}

// @address 0x1d5888 ?mirror_contents@c_restricted_memory@@QAA_NXZ
bool c_restricted_memory::mirror_contents()
{
    bool result = false;
    m_mirror_locked.set(1);
    long mirror_index = m_primary_readers.peek() % m_mirror_count;
    if (mirror_index != -1) {
        if (m_mirrors[mirror_index].section->try_and_lock() != 0) {
            unsigned char *writable;
            if (m_mirrors[mirror_index].valid.peek() != 0) {
                writable = (unsigned char *)m_mirrors[mirror_index].section->m_address;
                for (unsigned long member_index = 0; member_index < m_registered_member_count; member_index++) {
                    if (m_registered_members[member_index].pre_overwrite)
                        m_registered_members[member_index].pre_overwrite(writable + m_registered_members[member_index].offset);
                }
            }
#if defined(HALO3_64BIT)
            unsigned int copy_size = m_subsection_size;
            if (m_reserved_size > 0) {
                copy_size = (m_reserved_size + 127u) & ~127u;
                if (copy_size < m_reserved_size || copy_size > m_subsection_size)
                    copy_size = m_subsection_size;
            }
            restricted_memory_copy(m_mirrors[mirror_index].section->m_address, m_primary_section->m_address, copy_size);
#else
            restricted_memory_copy(m_mirrors[mirror_index].section->m_address, m_primary_section->m_address, m_subsection_size);
#endif
            writable = (unsigned char *)m_mirrors[mirror_index].section->m_address;
            for (unsigned long member_index = 0; member_index < m_registered_member_count; member_index++) {
                if (m_registered_members[member_index].post_copy)
                    m_registered_members[member_index].post_copy(writable + m_registered_members[member_index].offset);
            }
            m_mirrors[mirror_index].valid.set(1);
            m_mirrors[mirror_index].section->unlock();
            result = true;
        }
    }
    m_mirror_locked.set(0);
    return result;
}

// @address 0x1d5a08 ?internal_lock_mirror@c_restricted_memory@@AAAPAX_N@Z
void *c_restricted_memory::internal_lock_mirror(bool wait)
{
    long mirror_index = m_mirror_readers.peek() % m_mirror_count;
    void *result = m_mirrors[mirror_index].section->lock();
    if (result) {
        m_primary_locked.set(1);
        long previous_valid = m_mirrors[mirror_index].valid.set(0);
        if (previous_valid != 0) {
            m_primary_readers.increment();
            m_mirrors[mirror_index].valid.set(0);
            set_user_addresses_internal((unsigned char *)result);
        } else {
            m_primary_locked.set(0);
            m_mirrors[mirror_index].section->unlock();
            result = 0;
        }
    }
    return result;
}

// @address 0x1d5b30 ?internal_unlock_mirror@c_restricted_memory@@AAAX_N@Z
void c_restricted_memory::internal_unlock_mirror(bool clear)
{
    m_primary_locked.peek();
    long mirror_index = m_mirror_readers.peek() % m_mirror_count;
    if (clear)
        clear_user_addresses_internal();
    m_mirrors[mirror_index].valid.set(0);
    m_mirror_readers.increment();
    m_primary_locked.set(0);
    m_mirrors[mirror_index].section->unlock();
}

// Appends a field aligned to 1 << alignment_bits bytes and returns its index.
// @address 0x1d5be0 ?add_member@c_restricted_memory@@QAAJPBD0IJP6AXPAX@Z22@Z
long c_restricted_memory::add_member(const char *name, const char *type, unsigned int size, long alignment_bits,
                 field_proc_t construct, field_proc_t pre_overwrite, field_proc_t post_copy)
{
  unsigned int allocated_size = size;
  if (m_callbacks) {
    allocated_size = m_callbacks->filter_size_request(size);
  }
  int index = m_registered_member_count;
  unsigned int unaligned_offset = m_reserved_size;
  unsigned int mask = (unsigned int)((1i64 << alignment_bits) - 1);
  unsigned int offset = (unaligned_offset + mask) & ~mask;
  unsigned int padding = offset - unaligned_offset;
  m_reserved_size += padding + allocated_size;
  m_registered_members[index].offset = offset;
  m_registered_members[index].size = size;
  m_registered_members[index].construct = construct;
  m_registered_members[index].post_copy = post_copy;
  m_registered_members[index].pre_overwrite = pre_overwrite;
  m_registered_members[index].unaligned_offset = unaligned_offset;
  m_registered_member_count++;
  if (m_callbacks) {
    m_callbacks->handle_allocation(this, name, type, index, (unsigned char *)m_primary_section->get_address() + offset, size);
  }
  return index;
}

// Updates each registered member's user address. March 8296c938 has one index local.
// @address 0x1d5cf0 ?set_user_addresses_internal@c_restricted_memory@@AAAXPAE@Z
void c_restricted_memory::set_user_addresses_internal(unsigned char *address)
{
  for (unsigned long member_index = 0; member_index < m_registered_member_count; member_index++) {
    if (m_registered_members[member_index].construct) {
      m_registered_members[member_index].construct(address + m_registered_members[member_index].offset);
    }
  }
}

// @address 0x1d5d50 ?clear_user_addresses_internal@c_restricted_memory@@AAAXXZ
void c_restricted_memory::clear_user_addresses_internal()
{
    for (unsigned long member_index = 0; member_index < m_registered_member_count; member_index++)
    {
        s_registered_member *member = &m_registered_members[member_index];
        if (member->construct)
            member->construct(0);
    }
}


// @address 0x1d6d48 ?restricted_region_unlock_primary@@YAXJ@Z
void restricted_region_unlock_primary(long index)
{
    c_restricted_memory *region = &g_restricted_regions[index];
    g_restricted_address[index] = 0;
    region->clear_user_addresses_internal();
    region->m_primary_section->unlock();
}
// @address 0x142338 restricted_memory_copy
extern "C" void *restricted_memory_copy(void *destination, const void *source, unsigned int size)
{
    unsigned int offset = (0 - address_from_pointer(destination)) & 127;
    char *dest = (char *)destination;
    const char *src = (const char *)source;
    unsigned int bulk;
    unsigned int tail;
    if (size < 0x100)
        return memcpy(destination, source, size);
    if (offset != 0) {
        memcpy(dest, src, offset);
        dest += offset;
        src += offset;
        size -= offset;
    }
    bulk = size & ~127u;
    tail = size & 127u;
    if ((address_from_pointer(src) & 15) != 0) {
        restricted_memory_copy_unaligned_blocks(dest, src, bulk);
        dest += bulk;
    } else {
        restricted_memory_copy_aligned_blocks(dest, src, bulk);
        dest += bulk;
    }
    const char *&source_cursor = src;
    source_cursor += bulk;
    if (tail != 0)
        memcpy(dest, src, tail);
    return destination;
}


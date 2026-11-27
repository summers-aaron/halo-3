#ifndef CACHE_RESTRICTED_MEMORY_H
#define CACHE_RESTRICTED_MEMORY_H

#include "multithreading/synchronized_value.h"
#include "multithreading/registered_thread_index.h"
#include "cache/restricted_memory_layout.h"
#include "multithreading/synchronization.h"

class c_restricted_memory;
extern "C" void threads_unlock_restricted_region(s_field_layout *region);
void game_state_call_before_load_procs();
class c_restricted_memory_callbacks
{
public:
    virtual unsigned int filter_size_request(unsigned int size);
    virtual long filter_base_offset(long offset);
    virtual void handle_allocation(const c_restricted_memory *memory, const char *name,
        const char *type, long member_index, void *address, unsigned int size);
    virtual void handle_release(const c_restricted_memory *memory, long member_index,
        void *address, unsigned int size);
};
void restricted_region_unlock_primary(long index);
extern "C" unsigned long __stdcall GetTickCount();

class c_restricted_section
{
public:
    __forceinline c_restricted_section()
        : m_internal_flags(0), m_protection_scheme(-1), m_critical_section(-1),
          m_address(0), m_size(0), m_owner_thread(-1) {}
    __forceinline ~c_restricted_section() {}

    void initialize(void *base_address, unsigned int size, long critical_section_id)
    {
        m_critical_section = critical_section_id;
        m_internal_flags |= 1;
        m_address = base_address;
        m_size = size;
        m_protection_scheme = -1;
        m_owner_thread = -1;
    }
    bool initialized() const { return (m_internal_flags & 1) != 0; }
    bool locked_for_current_thread() const
    {
        bool result = initialized() && m_owner_thread == g_registered_thread_index;
        return result;
    }

private:
    friend class c_restricted_memory;
    friend bool threads_section_owned_by_current_thread(c_restricted_section *section);
    friend void game_state_call_before_load_procs();
    friend void restricted_region_unlock_primary(long index);
    friend void threads_unlock_restricted_region(s_field_layout *region);
    void *try_and_lock();
    void unlock();
    __forceinline void *lock()
    {
        if (m_critical_section != -1)
            internal_critical_section_enter(m_critical_section);
        m_owner_thread = g_registered_thread_index;
        void *address = m_address;
        return address;
    }
    const void *get_address() const { return m_address; }
    unsigned long get_size() const { return m_size; }
    void *get_writable_address() { return m_address; }

    unsigned long m_internal_flags;
    long m_protection_scheme;
    long m_critical_section;
    void *m_address;
    unsigned long m_size;
    long m_owner_thread;
};

typedef char restricted_section_size[(sizeof(void *) != 4 || sizeof(c_restricted_section) == 0x18) ? 1 : -1];

// March's owner methods assert cache\restricted_memory.cpp. The plain s_* layouts in
// restricted_memory_layout.h describe this storage without constructing its members.
class c_restricted_memory
{
public:
    c_restricted_memory();
    ~c_restricted_memory();
    void initialize(long region_index, c_restricted_section *primary_section,
        long critical_section_index, c_restricted_memory_callbacks *callbacks)
    {
        m_region_index = region_index;
        m_primary_section = primary_section;
        m_subsection_size = m_primary_section->get_size();
        m_mirror_count = 0;
        m_reserved_size = 0;
        m_registered_member_count = 0;
        m_callbacks = callbacks;
        m_critical_section = critical_section_index;
        m_primary_valid.set(0);
        m_mirror_valid.set(0);
        m_internal_flags |= 1;
    }
    bool initialized() const { return (m_internal_flags & 1) != 0; }
    unsigned long get_member_offset(long member_index)
    {
        unsigned long result = 0;
        result = m_registered_members[member_index].offset;
        return result;
    }
    void add_mirror(c_restricted_section *mirror_section);
    bool mirror_contents();
    void destroy();
    void free_member(long member_index)
    {
        if (m_callbacks)
        {
            unsigned char *address = (unsigned char *)m_primary_section->get_writable_address();
            m_callbacks->handle_release(this, member_index,
                address + m_registered_members[member_index].offset, m_registered_members[member_index].size);
        }
        m_registered_members[member_index].unaligned_offset = 0;
        m_registered_members[member_index].offset = 0;
        m_registered_members[member_index].size = 0;
        m_registered_members[member_index].construct = 0;
    }
    void *lock_primary()
    {
        void *result = m_primary_section->lock();
        set_user_addresses_internal((unsigned char *)result);
        return result;
    }
    void *try_and_lock_primary()
    {
        void *result = m_primary_section->try_and_lock();
        if (result)
            set_user_addresses_internal((unsigned char *)result);
        return result;
    }
    void *add_alias()
    {
        m_mirror_valid.increment();
        void *result = m_primary_section->get_writable_address();
        set_user_addresses_internal((unsigned char *)result);
        return result;
    }
    void remove_alias()
    {
        m_mirror_valid.decrement();
        clear_user_addresses_internal();
    }
    bool mirror_locked_for_current_thread();
    void *lock_mirror() { return internal_lock_mirror(true); }
    void *try_and_lock_mirror() { return internal_lock_mirror(false); }
    void unlock_mirror() { internal_unlock_mirror(true); }
    bool wait_for_available_mirror(unsigned long timeout) const
    {
        unsigned long start = GetTickCount();
        long mirror_index = m_mirror_readers.peek() % m_mirror_count;
        unsigned long elapsed = 0;
        while (!m_mirrors[mirror_index].valid.peek() && elapsed < timeout)
        {
            mirror_index = m_mirror_readers.peek() % m_mirror_count;
            elapsed = GetTickCount() - start;
        }
        return m_mirrors[mirror_index].valid.peek() != 0;
    }

    long add_member(const char *name, const char *type, unsigned int size, long alignment_bits,
        field_proc_t construct, field_proc_t pre_overwrite, field_proc_t post_copy);

private:
    friend void restricted_region_unlock_primary(long index);
    friend void threads_unlock_restricted_region(s_field_layout *region);
    friend struct s_restricted_memory_layout_asserts;
    friend unsigned long _internal_halt_render_thread_and_lock_resources(const char *file, long line);
    friend void game_state_call_before_load_procs();
    void uninitialize();
    void *internal_lock_mirror(bool wait);
    void internal_unlock_mirror(bool clear);
    void clear_user_addresses_internal();
    void set_user_addresses_internal(unsigned char *address);

    struct s_mirror_slot
    {
        c_interlocked_long readers;
        c_synchronized_long valid;
        c_synchronized_long locked;
        c_restricted_section *section;
        s_mirror_slot() : readers(0), valid(0), locked(0) {}
        ~s_mirror_slot() {}
    };
    struct s_registered_member
    {
        unsigned int unaligned_offset;
        unsigned int offset;
        unsigned int size;
        field_proc_t construct;
        field_proc_t post_copy;
        field_proc_t pre_overwrite;
        s_registered_member() : unaligned_offset(0), offset(0), size(0), construct(0), post_copy(0), pre_overwrite(0) {}
        ~s_registered_member() {}
    };
    c_interlocked_long m_primary_locked;
    c_interlocked_long m_mirror_locked;
    s_mirror_slot m_mirrors[4];
    c_interlocked_long m_primary_readers;
    c_interlocked_long m_mirror_readers;
    c_synchronized_long m_primary_valid;
    c_synchronized_long m_mirror_valid;
    c_restricted_section *m_primary_section;
    unsigned long m_internal_flags;
    // Padding gap: no March accessor touches this word (all 48 March methods swept)
    // and no shipped-code reference or raw offset uses it.
    unsigned long m_pad;
    unsigned int m_subsection_size;
    unsigned int m_reserved_size;
    long m_mirror_count;
    long m_critical_section;
    unsigned long m_registered_member_count;
    s_registered_member m_registered_members[128];
    c_restricted_memory_callbacks *m_callbacks;
    void *m_primary_allocation;
    void *m_mirror_allocation;
    long m_region_index;
};

// Friendship only permits compile-time checks of the private storage; no object is emitted.
struct s_restricted_memory_layout_asserts
{
    typedef char mirror_slot_size[(sizeof(void *) != 4 || sizeof(c_restricted_memory::s_mirror_slot) == 0x10) ? 1 : -1];
    typedef char registered_member_size[(sizeof(void *) != 4 || sizeof(c_restricted_memory::s_registered_member) == 0x18) ? 1 : -1];
    typedef char post_copy[(sizeof(void *) != 4 || offsetof(c_restricted_memory::s_registered_member, post_copy) == 0x10) ? 1 : -1];
    typedef char pre_overwrite[(sizeof(void *) != 4 || offsetof(c_restricted_memory::s_registered_member, pre_overwrite) == 0x14) ? 1 : -1];
    typedef char size[(sizeof(void *) != 4 || sizeof(c_restricted_memory) == 0xc88) ? 1 : -1];
    typedef char primary_section[(sizeof(void *) != 4 || offsetof(c_restricted_memory, m_primary_section) == 0x58) ? 1 : -1];
    typedef char pad[(sizeof(void *) != 4 || offsetof(c_restricted_memory, m_pad) == 0x60) ? 1 : -1];
    typedef char subsection_size[(sizeof(void *) != 4 || offsetof(c_restricted_memory, m_subsection_size) == 0x64) ? 1 : -1];
    typedef char reserved_size[(sizeof(void *) != 4 || offsetof(c_restricted_memory, m_reserved_size) == 0x68) ? 1 : -1];
    typedef char member_count[(sizeof(void *) != 4 || offsetof(c_restricted_memory, m_registered_member_count) == 0x74) ? 1 : -1];
    typedef char members[(sizeof(void *) != 4 || offsetof(c_restricted_memory, m_registered_members) == 0x78) ? 1 : -1];
    typedef char callbacks[(sizeof(void *) != 4 || offsetof(c_restricted_memory, m_callbacks) == 0xc78) ? 1 : -1];
    typedef char region_index[(sizeof(void *) != 4 || offsetof(c_restricted_memory, m_region_index) == 0xc84) ? 1 : -1];
};


// Functions defined in tu/blamlibxbox/restricted_memory.cpp.

extern "C" void *restricted_memory_copy(void *destination, const void *source, unsigned int size);

#endif // CACHE_RESTRICTED_MEMORY_H

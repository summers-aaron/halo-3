#ifndef CACHE_RESTRICTED_MEMORY_REGIONS_H
#define CACHE_RESTRICTED_MEMORY_REGIONS_H

// Functions defined in tu/blamlibxbox/restricted_memory_regions.cpp.

class c_restricted_memory;
extern c_restricted_memory g_restricted_regions[];

bool restricted_region_mirror_locked_for_current_thread(long index);
void restricted_region_destroy(long index);
void restricted_region_free_member(long index, long member_index);
void restricted_region_lock_primary(long index);
void restricted_region_unlock_primary(long index);
bool restricted_region_try_and_lock_primary(long index);
void restricted_region_add_alias(long index);
void restricted_region_remove_alias(long index);

// An allocator that hands out one member of a restricted memory region.
// It is configured by a region index plus adopt, post-copy and allocator callbacks.
// mangled names: the second and third arguments are function pointers (`$1?adopt_old_render_message_queue...`,
// `$1?object_broadphase_post_copy_fixup...`), `$0A@` when null; they and the fourth are passed to
// restricted_region_add_member.
// March derives it from c_allocation_base (its constructor calls c_allocation_base::c_allocation_base()). The base
// is left out until c_allocation_base has one definition: memory/data.h holds it, and including that here collides
// with private copies of c_data_iterator, s_data_iterator and datum_get in about 20 of these units. Layout and
// mangled names are the same either way (the vtable pointer is at 0 in both).
// allocate takes a size and a name; deallocate takes the allocation.
template<long k_region_index, void (*k_adopt_function)(void *), void (*k_post_copy_function)(void *),
    void (*k_set_allocator_function)(void *)>
class t_restricted_allocation_manager
{
public:
    t_restricted_allocation_manager() :
        m_member_index(-1),
        m_thread_id(-1)
    {
    }

    ~t_restricted_allocation_manager()
    {
    }

    // Reserves memory with default alignment; each instantiation supplies its own type string.
    // Each instantiation's body lives in the unit that owns it.
    virtual void *allocate(unsigned int size, const char *name);

    // March: free_memory();
    virtual void deallocate(void *allocation);

    // March (restricted_memory_regions.h:146):
    //     ASSERT(!valid());
    //     m_member_index = restricted_region_add_member(k_region_index, name, type, size, alignment_bits,
    //         k_set_allocator_function, k_adopt_function, k_post_copy_function);
    //     m_thread_id = get_current_thread_index();
    //     return restricted_region_get_member_address(k_region_index, m_member_index);
    // Stays a prototype until restricted_region_add_member and get_current_thread_index have true definitions;
    // the units that instantiate it define it for now.
    void *reserve_memory(const char *name, const char *type, unsigned int size, long alignment_bits);

    // March (restricted_memory_regions.h:155, 156) also asserts valid() and m_thread_id == get_current_thread_index().
    void free_memory()
    {
        k_set_allocator_function(NULL);
        restricted_region_free_member(k_region_index, m_member_index);
    }

    bool valid() const
    {
        return m_member_index != -1 && m_thread_id != -1;
    }

// Access is not attested (March has no accessor); public while hex-named free functions (havok_shapes.cpp
// ) still write them.
public:
    long m_member_index;
    long m_thread_id;
};
extern "C" bool restricted_region_is_available(int index);
extern "C" void restricted_region_set_region2_address(void *value);

extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &restricted_region_set_region2_address> g_restricted_region2_allocator;

extern "C" void restricted_region_reserve_region2_storage(void);
#endif // CACHE_RESTRICTED_MEMORY_REGIONS_H

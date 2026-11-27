#ifndef INCLUDE_PHYS_SINGLETON_H
#define INCLUDE_PHYS_SINGLETON_H

#include "phys/phys_object.h"

#ifndef PHYS_CALL
#define PHYS_CALL
#endif

template <typename T>
class phys_singleton : public phys_ref_object {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_SINGLETON);
    static T* s_instance;
    static __forceinline T& PHYS_CALL get_singleton_instance() { return *s_instance; }
    static void PHYS_CALL replace_singleton_instance(T* t) {
        if (s_instance) { s_instance->object_remove_ref(); }
        s_instance = t;
    }
    static phys_bool PHYS_CALL has_instance() { return s_instance != 0; }
protected:
    phys_singleton() {}
    phys_singleton(const phys_singleton&);
    phys_singleton& operator=(const phys_singleton&);
};

extern struct phys_singleton_init_node* phys_singleton_init_list;
struct phys_singleton_init_node {
    typedef phys_ref_object* (PHYS_CALL *singleton_create_fn)();
    phys_singleton_init_node(singleton_create_fn func, void** ptr) : m_singleton_create_fn(func), m_value(ptr) {
        m_next = phys_singleton_init_list;
        phys_singleton_init_list = this;
    }
    singleton_create_fn m_singleton_create_fn;
    phys_singleton_init_node* m_next;
    void** m_value;
private:
    PHYS_DECLARE_SYSTEM_ALLOCATOR();
};

#endif // INCLUDE_HAVOK_HK_SINGLETON_H

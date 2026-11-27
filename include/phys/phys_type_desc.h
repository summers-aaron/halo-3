#ifndef INCLUDE_PHYS_TYPE_DESC_H
#define INCLUDE_PHYS_TYPE_DESC_H

#include "phys/phys_fixup_tag.h"

// Reflection records hold the name, load/cleanup callbacks, and constructed vtable.
class phys_type_desc {
public:
    // Reflection reads the identity of a real object constructed without finishing fields.
    template <typename T>
    static __forceinline const void *get_vtable()
    {
        union __declspec(align(16)) type_desc_storage {
            void *type_identity;
            char object[sizeof(T)];
        } storage;
        phys_fixup_tag flag;
        new (storage.object) T(flag);
        return storage.type_identity;
    }

    typedef void (*loaded_object_fixup_fn)(void *);
    typedef void (*loaded_object_cleanup_fn)(void *);

    phys_type_desc(const char *name, loaded_object_fixup_fn finish,
               loaded_object_cleanup_fn cleanup, const void *vtable)
        : m_class_name(name), m_fixup_fn(finish),
          m_cleanup_loaded_fn(cleanup), m_type_vtable(vtable) {}

private:
    const char *m_class_name;
    loaded_object_fixup_fn m_fixup_fn;
    loaded_object_cleanup_fn m_cleanup_loaded_fn;
    const void *m_type_vtable;
};

#endif // INCLUDE_HAVOK_HK_TYPE_INFO_H

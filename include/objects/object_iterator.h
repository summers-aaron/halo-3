#ifndef OBJECTS_OBJECT_ITERATOR_H
#define OBJECTS_OBJECT_ITERATOR_H

#include "cseries/cseries_types.h"

// Methods are the March 2007 debug build's (`marchbody class c_object_iterator_base`, `class
// c_object_iterator`): get_index (public, non-const), get_datum_internal, object_iterator_begin_internal,
// object_iterator_next_internal and object_iterator_next_with_match_flags_internal (protected).
// The out-of-line members live in objects.cpp (assert objects.cpp:5677). Field names are not in the map.
class c_object_iterator_base {
public:
    // March 0x824b93e8 (non-const).
    __forceinline long get_index() { return m_index; }

protected:
    // March 0x827550a8.
    __forceinline struct object_datum *get_datum_internal() { return m_datum; }

    // Reset the iterator state to start a new search.
    __forceinline void object_iterator_begin_internal(unsigned long type_mask, unsigned long header_mask, unsigned long match_flags) {
        m_datum = 0;
        m_signature = 0x86868686;
        m_type_mask = type_mask ? type_mask : 0xffffffffUL;
        m_header_mask = header_mask;
        m_match_flags = match_flags;
        m_absolute_index = 0;
        m_index = -1;
    }
    bool object_iterator_next_internal();
    bool object_iterator_next_with_match_flags_internal();

public:
    // Public like c_data_iterator's state: impacts_create_orphaned_impacts resumes a saved
    // absolute index by writing m_absolute_index alongside the other initial field stores.
    struct object_datum *m_datum;
    unsigned long m_type_mask;
    unsigned long m_header_mask;
    unsigned long m_match_flags;
    long m_absolute_index;
    long m_index;
    unsigned long m_signature;
};

template<typename t_object_type>
class c_object_iterator : public c_object_iterator_base {
public:
    // March 0x824ba9a0.
    __forceinline void begin(unsigned long type_mask, unsigned char header_mask) {
        object_iterator_begin_internal(type_mask, header_mask, 0);
    }
    // March 0x824ba9e8.
    __forceinline bool next() {
        return object_iterator_next_internal();
    }
    // March 0x82755068.
    __forceinline t_object_type *get_datum() {
        return (t_object_type *)get_datum_internal();
    }
};

typedef char obj_iter_size_iterator[(sizeof(void *) != 4 || ((sizeof(c_object_iterator_base) == 0x1c) )) ? 1 : -1];

#endif // OBJECTS_OBJECT_ITERATOR_H

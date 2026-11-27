#ifndef INCLUDE_PHYS_TYPED_BROADPHASE_HANDLE_PAIR_H
#define INCLUDE_PHYS_TYPED_BROADPHASE_HANDLE_PAIR_H

class phys_broadphase_handle;
class phys_typed_broadphase_handle;

class phys_broadphase_handle_pair {
public:
    phys_broadphase_handle *m_a;
    phys_broadphase_handle *m_b;
};

class phys_typed_broadphase_pair : public phys_broadphase_handle_pair {
public:
    // March 0x834fc5e0 / 0x834fc5f0 return the two handle pointers without locals.
    __forceinline phys_typed_broadphase_handle *get_element_a() const { return reinterpret_cast<phys_typed_broadphase_handle *>(m_a); }
    __forceinline phys_typed_broadphase_handle *get_element_b() const { return reinterpret_cast<phys_typed_broadphase_handle *>(m_b); }
};

typedef char phys_broadphase_handle_pair_size[(sizeof(void *) != 4 || ((sizeof(phys_typed_broadphase_pair) == 8) )) ? 1 : -1];

#endif

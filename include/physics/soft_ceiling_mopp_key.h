#ifndef PHYSICS_SOFT_CEILING_MOPP_KEY_H
#define PHYSICS_SOFT_CEILING_MOPP_KEY_H

// Soft-ceiling MOPP primitives identify a group and a triangle within that group.
class c_soft_ceiling_mopp_key
{
public:
    __forceinline explicit c_soft_ceiling_mopp_key(unsigned long key) : m_key(key) {}
    __forceinline unsigned long group_index() const { return m_key >> 16; }
    __forceinline unsigned long triangle_index() const { return m_key & 0xffff; }

private:
    unsigned long m_key;
};

#endif // PHYSICS_SOFT_CEILING_MOPP_KEY_H

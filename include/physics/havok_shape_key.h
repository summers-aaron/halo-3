#ifndef PHYSICS_HAVOK_SHAPE_KEY_H
#define PHYSICS_HAVOK_SHAPE_KEY_H

static __forceinline void values_from_shape_key(long *index, long *surface_index, long *type, const unsigned int &key)
{
    *index = key & 0xffff;
    *surface_index = (key >> 16) & 0x1fff;
    *type = (key >> 29) & 7;
}

static __forceinline unsigned int shape_key_from_values(long index, long surface_index, long type)
{
    return index | (surface_index << 16) | (type << 29);
}

#endif // PHYSICS_HAVOK_SHAPE_KEY_H

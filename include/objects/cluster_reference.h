#ifndef OBJECTS_CLUSTER_REFERENCE_H
#define OBJECTS_CLUSTER_REFERENCE_H

struct s_cluster_reference {
    signed char bsp_index;
    unsigned char cluster_index;

    // Packed 2-byte form (s_portal_reference's .value equivalent) for whole-ref compares.
    __forceinline short value() const { return *(const short *)this; }
    __forceinline short &value() { return *(short *)this; }
};

typedef char obj_ref_size_cluster_ref[(sizeof(s_cluster_reference) == 2) ? 1 : -1];

#endif // OBJECTS_CLUSTER_REFERENCE_H

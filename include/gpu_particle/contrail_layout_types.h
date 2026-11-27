#ifndef GPU_PARTICLE_CONTRAIL_LAYOUT_TYPES_H
#define GPU_PARTICLE_CONTRAIL_LAYOUT_TYPES_H

struct s_contrail_layout {
#if !defined(_XBOX) || defined(__clang__)
    volatile int &fence;
    long &gpr_allocation;
#else
    long fence;
    char pad4[0x2bf8 - 4];
    long gpr_allocation;
#endif
};

#endif // GPU_PARTICLE_CONTRAIL_LAYOUT_TYPES_H

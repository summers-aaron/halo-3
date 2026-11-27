#ifndef GPU_PARTICLE_CONSTANT_TABLE_TYPES_H
#define GPU_PARTICLE_CONSTANT_TABLE_TYPES_H

#include "gpu_particle/gpu_constant_table_types.h"

template<class ConstantList>
struct s_gpu_constant_table : s_gpu_constant_table_base
{
    c_rasterizer_constant_desc slots[ConstantList::slot_count];

#if !defined(_XBOX) || defined(__clang__)
private:
    template<int... Indices> struct s_indices {};
    template<int Count, int... Indices> struct s_slot_indices : s_slot_indices<Count - 1, Count - 1, Indices...> {};
    template<int... Indices> struct s_slot_indices<0, Indices...> : s_indices<Indices...> {};

    template<int... Indices>
    constexpr s_gpu_constant_table(const s_gpu_constant_table_base &base,
        const c_rasterizer_constant_desc (&initial_slots)[ConstantList::slot_count], s_indices<Indices...>)
        : s_gpu_constant_table_base(base), slots{initial_slots[Indices]...} {}

public:
    s_gpu_constant_table() = default;
    constexpr s_gpu_constant_table(s_entry_table *shader, c_rasterizer_constant_desc *descs,
        unsigned long desc_count, unsigned char valid,
        const c_rasterizer_constant_desc (&initial_slots)[ConstantList::slot_count])
        : s_gpu_constant_table({shader, descs, desc_count, valid}, initial_slots,
            s_slot_indices<ConstantList::slot_count>()) {}
#endif
};

#endif // GPU_PARTICLE_CONSTANT_TABLE_TYPES_H

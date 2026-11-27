#ifndef INCLUDE_PHYS_VEL_ACCUM_OFFSET_H
#define INCLUDE_PHYS_VEL_ACCUM_OFFSET_H

class phys_vel_accum;

class phys_vel_accum_offset {
public:
    unsigned int m_offset;
    __forceinline phys_vel_accum_offset() {}
    __forceinline phys_vel_accum_offset(const phys_vel_accum *base, const phys_vel_accum *accumulator) {
        m_offset = reinterpret_cast<const char *>(accumulator) - reinterpret_cast<const char *>(base);
    }
    __forceinline phys_vel_accum &get_accumulator(phys_vel_accum *base) {
        return *reinterpret_cast<phys_vel_accum *>(reinterpret_cast<char *>(base) + m_offset);
    }
    __forceinline const phys_vel_accum &get_accumulator(const phys_vel_accum *base) const {
        return *reinterpret_cast<const phys_vel_accum *>(reinterpret_cast<const char *>(base) + m_offset);
    }
    __forceinline const phys_vel_accum &get_accumulator(const phys_vel_accum *base) {
        // match: 0x665bb8: retain the byte-base local from the chain builder's original accessor.
        const char *bytes = reinterpret_cast<const char *>(base);
        return *reinterpret_cast<const phys_vel_accum *>(bytes + m_offset);
    }
};

typedef char phys_vel_accum_offset_size[(sizeof(phys_vel_accum_offset) == 4) ? 1 : -1];

#endif

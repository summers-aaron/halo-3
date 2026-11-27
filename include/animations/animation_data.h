#ifndef ANIMATIONS_ANIMATION_DATA_H
#define ANIMATIONS_ANIMATION_DATA_H

#include <stddef.h>
#include "math/real_math_types.h"

class c_animation_data_sizes
{
public:
    __forceinline long total_size() const
    {
        return m_default_data_size + m_compressed_data_size + m_static_node_flags_data_size
            + m_animated_node_flags_data_size + m_movement_data_size + m_pill_offset_data_size
            + m_uncompressed_data_size;
    }
    __forceinline bool contains_movement_data() const { return m_movement_data_size != 0; }
    __forceinline bool contains_pill_offset_data() const { return m_pill_offset_data_size != 0; }
    __forceinline long get_pill_offset_data_size() const { return m_pill_offset_data_size; }
    // March 827f9ad8/827f9b18 and 827f9dd0/827f9e10.
    __forceinline long get_animated_node_flags_data_size() const { return m_animated_node_flags_data_size; }
    __forceinline long get_animated_node_flags_data_offset() const
    {
        return m_default_data_size + m_compressed_data_size + m_static_node_flags_data_size;
    }
    __forceinline long get_static_node_flags_data_size() const { return m_static_node_flags_data_size; }
    __forceinline long get_static_node_flags_data_offset() const
    {
        return m_default_data_size + m_compressed_data_size;
    }
    __forceinline long get_pill_offset_data_offset() const
    {
        return m_default_data_size + m_compressed_data_size + m_static_node_flags_data_size
            + m_animated_node_flags_data_size + m_movement_data_size;
    }

    signed char m_static_node_flags_data_size;
    signed char m_animated_node_flags_data_size;
    short m_movement_data_size;
    short m_pill_offset_data_size;
    short m_default_data_size;
    long m_uncompressed_data_size;
    long m_compressed_data_size;
};

class c_animation_data
{
public:
    c_animation_data() : m_data(0), m_sizes(0), m_node_count(0), m_movement_data_type(0) {}
    ~c_animation_data() {}
    bool valid() const;
    __forceinline long total_size() const { return m_sizes->total_size(); }
    __forceinline void setup(const unsigned char *data, long node_count, long movement_data_type,
        long frame_count, const c_animation_data_sizes *sizes)
    {
        m_data = data;
        m_sizes = sizes;
        m_movement_data_type = (unsigned char)movement_data_type;
        m_node_count = (unsigned char)node_count;
        m_frame_count = (short)frame_count;
    }
    void get_pill_offset_for_frame(long frame_index, real_point3d *position, real_vector3d *position_delta) const;
    __forceinline bool contains_pill_offset_data() const { return m_sizes->contains_pill_offset_data(); }
    __forceinline const unsigned char *get_data(long offset, long size) const { return m_data + offset; }
    // March animation_data.h:307 (827f9b78): the two component sizes are real locals.
    __forceinline const unsigned char *get_flag_data(long offset, long size, long component) const
    {
        long aligned_component_size = ((m_node_count + 31) >> 5) * 4;
        offset += aligned_component_size * component;
        long component_size = size / 3;
        // March asserts component_size * 3 == size.
        return get_data(offset, component_size);
    }
    __forceinline const unsigned long *get_animated_rotation_flags() const
    {
        return (const unsigned long *)get_flag_data(m_sizes->get_animated_node_flags_data_offset(),
            m_sizes->get_animated_node_flags_data_size(), 0);
    }
    __forceinline const unsigned long *get_animated_translation_flags() const
    {
        return (const unsigned long *)get_flag_data(m_sizes->get_animated_node_flags_data_offset(),
            m_sizes->get_animated_node_flags_data_size(), 1);
    }
    __forceinline const unsigned long *get_animated_scale_flags() const
    {
        return (const unsigned long *)get_flag_data(m_sizes->get_animated_node_flags_data_offset(),
            m_sizes->get_animated_node_flags_data_size(), 2);
    }
    __forceinline const unsigned long *get_static_rotation_flags() const
    {
        return (const unsigned long *)get_flag_data(m_sizes->get_static_node_flags_data_offset(),
            m_sizes->get_static_node_flags_data_size(), 0);
    }
    __forceinline const unsigned long *get_static_translation_flags() const
    {
        return (const unsigned long *)get_flag_data(m_sizes->get_static_node_flags_data_offset(),
            m_sizes->get_static_node_flags_data_size(), 1);
    }
    __forceinline const unsigned long *get_static_scale_flags() const
    {
        return (const unsigned long *)get_flag_data(m_sizes->get_static_node_flags_data_offset(),
            m_sizes->get_static_node_flags_data_size(), 2);
    }
    // March 82870988; the flag words are indexed by node, least-significant bit first.
    __forceinline bool node_contains_animated_translation_data(short node_index) const
    {
        return (get_animated_translation_flags()[node_index >> 5] & (1 << (node_index & 31))) != 0;
    }
    __forceinline const unsigned char *get_pill_offset_data() const
    {
        return get_data(m_sizes->get_pill_offset_data_offset(), m_sizes->get_pill_offset_data_size());
    }

    const unsigned char *m_data;
    const c_animation_data_sizes *m_sizes;
    unsigned char m_node_count;
    unsigned char m_movement_data_type;
    short m_frame_count;
};

#if !defined(_WIN64) && !defined(__LP64__)
typedef char animation_data_sizes_size[(sizeof(c_animation_data_sizes) == 0x10) ? 1 : -1];
typedef char animation_data_sizes_default_offset[(offsetof(c_animation_data_sizes, m_default_data_size) == 6) ? 1 : -1];
typedef char animation_data_sizes_compressed_offset[(offsetof(c_animation_data_sizes, m_compressed_data_size) == 12) ? 1 : -1];
typedef char animation_data_size[(sizeof(c_animation_data) == 12) ? 1 : -1];
typedef char animation_data_frame_count_offset[(offsetof(c_animation_data, m_frame_count) == 10) ? 1 : -1];
#endif

// Functions defined in tu/blamlibxbox/animation_data.cpp.

extern "C" void animation_data_merge_flags(const c_animation_data *buffer, unsigned char *destination);

#endif // ANIMATIONS_ANIMATION_DATA_H

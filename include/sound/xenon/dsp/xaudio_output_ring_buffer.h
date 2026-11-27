#ifndef SOUND_XENON_DSP_XAUDIO_OUTPUT_RING_BUFFER_H
#define SOUND_XENON_DSP_XAUDIO_OUTPUT_RING_BUFFER_H

#include <stddef.h>
#include <string.h>
#include "memory/static_arrays.h"
#include "memory/wrapped_arrays.h"

enum { k_xaudio_frame_size = 256, k_xaudio_ring_count = 2 };

class c_xaudio_output_ring_buffer
{
public:
    void initialize(const char *name, long ring_count);
    void accumulate_current_frame_output(long channel, c_wrapped_array<const float> samples) const;
    __forceinline long get_ring_count() const { return m_ring_count; }
    __forceinline const float *get_last_frame_output(long channel) const
    {
        if (m_current_ring && m_current_ring->valid_index(channel))
            return (*m_current_ring)[channel].get_elements();
        return 0;
    }
    __forceinline void update_on_frame_start()
    {
        long next_ring_index = (m_current_ring_index + 1) % m_rings.get_count();
        m_current_ring = &m_rings[m_current_ring_index];
        m_next_ring = &m_rings[next_ring_index];
        memset(m_next_ring->get_elements(), 0, m_next_ring->get_total_element_size());
        m_current_ring_index = next_ring_index;
    }
    __forceinline void reset();
    __forceinline void register_engine_callbacks();

    c_static_array<c_wrapped_array<c_static_array<float, k_xaudio_frame_size> >, k_xaudio_ring_count> m_rings;
    long m_ring_count;
    long m_current_ring_index;
    c_wrapped_array<c_static_array<float, k_xaudio_frame_size> > *m_current_ring;
    c_wrapped_array<c_static_array<float, k_xaudio_frame_size> > *m_next_ring;
    void *m_callback_handle;

private:
    static void xaudio_frame_start_callback(void *context);
};

typedef char xaudio_output_ring_buffer_size[(sizeof(void *) != 4 || ((sizeof(c_xaudio_output_ring_buffer) == 0x24) )) ? 1 : -1];
typedef char xaudio_output_ring_count_offset[(sizeof(void *) != 4 || ((offsetof(c_xaudio_output_ring_buffer, m_ring_count) == 0x10) )) ? 1 : -1];
typedef char xaudio_output_current_ring_index_offset[(sizeof(void *) != 4 || ((offsetof(c_xaudio_output_ring_buffer,
    m_current_ring_index) == 0x14) )) ? 1 : -1];
typedef char xaudio_output_current_ring_offset[(sizeof(void *) != 4 || ((offsetof(c_xaudio_output_ring_buffer, m_current_ring) == 0x18) )) ? 1 : -1];
typedef char xaudio_output_next_ring_offset[(sizeof(void *) != 4 || ((offsetof(c_xaudio_output_ring_buffer, m_next_ring) == 0x1c) )) ? 1 : -1];
typedef char xaudio_output_callback_handle_offset[(sizeof(void *) != 4 || ((offsetof(c_xaudio_output_ring_buffer, m_callback_handle) == 0x20) )) ? 1 : -1];

#endif // SOUND_XENON_DSP_XAUDIO_OUTPUT_RING_BUFFER_H

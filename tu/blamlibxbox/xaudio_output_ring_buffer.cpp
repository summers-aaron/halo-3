// blamlibxbox/xaudio_output_ring_buffer.obj: from the records 0x2f8a40 0x2f8ae8 0x2f8bc0
// @flags /TP /O2
#include <xtl.h>
#include <xaudio.h>
#include "sound/xenon/dsp/xaudio_output_ring_buffer.h"
#include "cseries/runtime_state.h"

__forceinline void c_xaudio_output_ring_buffer::reset()
{
    for (unsigned long ring_index = 0; ring_index < k_xaudio_ring_count; ring_index++)
        memset(m_rings[ring_index].get_elements(), 0, m_rings[ring_index].get_total_element_size());
}

__forceinline void c_xaudio_output_ring_buffer::register_engine_callbacks()
{
    XAudioRegisterEngineCallback(XAUDIOENGINECALLBACKTYPE_FRAMESTART, xaudio_frame_start_callback, this, &m_callback_handle);
}

// @address 0x2f8a40 ?initialize@c_xaudio_output_ring_buffer@@QAAXPBDJ@Z
void c_xaudio_output_ring_buffer::initialize(const char *name, long ring_count)
{
    m_ring_count = ring_count;
    c_static_array<float, k_xaudio_frame_size> *ring_buffer_raw_data = (c_static_array<float, k_xaudio_frame_size> *)::runtime_state_malloc(
        "", "", ring_count * sizeof(c_static_array<float, k_xaudio_frame_size>) * k_xaudio_ring_count);
    m_current_ring_index = 0;
    for (unsigned long ring_index = 0; ring_index < k_xaudio_ring_count; ring_index++)
        m_rings[ring_index] = make_wrapped_array(ring_buffer_raw_data + ring_index * ring_count, ring_count);
    reset();
    register_engine_callbacks();
}

// @address 0x2f8ae8 ?accumulate_current_frame_output@c_xaudio_output_ring_buffer@@QBAXJV?$c_wrapped_array@$$CBM@@@Z
void c_xaudio_output_ring_buffer::accumulate_current_frame_output(long channel, c_wrapped_array<const float> samples) const
{
    if (m_next_ring && m_next_ring->valid_index(channel))
    {
        float *dest = (*m_next_ring)[channel].get_elements();
        const float *src = samples.begin();
        for (long i = 0; i < k_xaudio_frame_size; ++i)
            dest[i] += src[i];
    }
}

// @address 0x2f8bc0 ?xaudio_frame_start_callback@c_xaudio_output_ring_buffer@@CAXPAX@Z
void c_xaudio_output_ring_buffer::xaudio_frame_start_callback(void *context)
{
    c_xaudio_output_ring_buffer *ring_buffer = (c_xaudio_output_ring_buffer *)context;
    ring_buffer->update_on_frame_start();
}

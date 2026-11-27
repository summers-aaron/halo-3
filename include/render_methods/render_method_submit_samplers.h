#ifndef RENDER_METHODS_RENDER_METHOD_SUBMIT_SAMPLERS_H
#define RENDER_METHODS_RENDER_METHOD_SUBMIT_SAMPLERS_H

struct D3DDevice;
struct s_rasterizer_xenon_memory_entry;
extern "C" void render_method_submit_set_extern_sampler(D3DDevice *device, s_rasterizer_xenon_memory_entry *memory_entry, long index, long sampler);

#endif

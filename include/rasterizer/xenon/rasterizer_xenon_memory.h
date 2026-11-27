#ifndef RASTERIZER_XENON_RASTERIZER_XENON_MEMORY_H
#define RASTERIZER_XENON_RASTERIZER_XENON_MEMORY_H

// The rasterizer's EDRAM/command memory manager (rasterizer\xenon\rasterizer_xenon_memory.cpp in the March 2007
// build). Its layout is not settled yet; units that read its fields still use their own views.
class c_rasterizer_xenon_memory;

// @address 0x10f5d70 g_rasterizer_xenon_memory
extern "C++" c_rasterizer_xenon_memory g_rasterizer_xenon_memory;

#endif // RASTERIZER_XENON_RASTERIZER_XENON_MEMORY_H

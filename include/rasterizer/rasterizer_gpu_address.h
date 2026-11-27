#ifndef RASTERIZER_GPU_ADDRESS_H
#define RASTERIZER_GPU_ADDRESS_H

#include <xtl.h>

// Xenon GPU address mappings: physical aperture and cached read-only CPU alias.
// The bodies are the XDK converters from d3d9gpu.h (via <xtl.h>); Bungie's
// read_lock calls GPU_CONVERT_CPU_TO_CPU_CACHED_READONLY_ADDRESS directly
// (March rasterizer\rasterizer_geometry.cpp:101 assert). The DWORD-domain
// parameter is carried through UINT_PTR, the XDK's own pointer-sized carrier,
// so no address arithmetic happens in integers here.
static __forceinline unsigned long rasterizer_gpu_cpu_to_physical(unsigned long address)
{
    return GPU_CONVERT_CPU_TO_GPU_ADDRESS((const void *)(UINT_PTR)address);
}
static __forceinline const void *rasterizer_gpu_physical_to_cached_readonly(unsigned long address)
{
    return GPU_CONVERT_GPU_TO_CPU_CACHED_READONLY_ADDRESS(address);
}
static __forceinline const void *rasterizer_gpu_cpu_to_cached_readonly(unsigned long address)
{
    return rasterizer_gpu_physical_to_cached_readonly(rasterizer_gpu_cpu_to_physical(address));
}
#endif

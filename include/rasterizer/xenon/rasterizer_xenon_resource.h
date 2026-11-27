#ifndef RASTERIZER_XENON_RASTERIZER_XENON_RESOURCE_H
#define RASTERIZER_XENON_RASTERIZER_XENON_RESOURCE_H

extern "C" unsigned long D3DResource_Release(void *resource);

extern "C" void XGOffsetResourceAddress(void *resource, void *base_address);

#endif // RASTERIZER_XENON_RASTERIZER_XENON_RESOURCE_H

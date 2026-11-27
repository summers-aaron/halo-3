#ifndef RASTERIZER_XENON_XG_TEXTURE_H
#define RASTERIZER_XENON_XG_TEXTURE_H

extern "C" unsigned int XGSetTextureHeader(unsigned int width, unsigned int height, unsigned int levels, unsigned long usage,
    int format, int pool, unsigned int base_offset, unsigned int mip_offset, unsigned int pitch,
    void *texture, unsigned int *base_size, unsigned int *mip_size);

#endif // RASTERIZER_XENON_XG_TEXTURE_H

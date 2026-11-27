// port/sdk/d3d: the image file decoders behind the D3DX file calls (d3dx_image.cpp): baseline and progressive JPEG
// (JFIF / Adobe, the IJG libjpeg 6b defaults d3dx9.lib decodes with: the ISLOW integer IDCT, fancy 2x upsampling and
// libjpeg's fixed-point YCbCr conversion) and PNG (every colour type and bit depth, Adam7, through the zlib interface
// the game links, port/sdk/runtime/zlib_shim.cpp). Self-contained C++ with C-style data: no C library beyond
// malloc / free / memcpy / memset. Built with the SDK code (halo3_sdk) and by the tmp/ tests.
#ifndef PORT_SDK_D3D_D3DX_DECODE_H
#define PORT_SDK_D3D_D3DX_DECODE_H

#include <stddef.h>
#if defined(__has_include)
#if __has_include(<stdint.h>)
#define PORT_D3DX_HAVE_STDINT 1
#endif
#endif
#if defined(PORT_D3DX_HAVE_STDINT)
#include <stdint.h>
#else
// compiled against the XDK headers (halo3_sdk: no <stdint.h>): the compiler's own exact-width types
typedef __UINT8_TYPE__ uint8_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __INT16_TYPE__ int16_t;
typedef __INT32_TYPE__ int32_t;
typedef __UINT64_TYPE__ uint64_t;
#endif

enum e_port_image_file {
    k_port_image_file_unknown = 0,
    k_port_image_file_jpeg,
    k_port_image_file_png,
};

struct s_port_image_info {
    uint32_t file;       // e_port_image_file
    uint32_t width, height;
    uint32_t channels;   // 1 grey, 3 colour, 4 colour with alpha (PNG alpha or tRNS)
};

// Reads the image's header: returns 0 if the data is not a JPEG or PNG this decoder can read.
int port_image_get_info(const void *data, uint32_t size, struct s_port_image_info *out);

// Decodes the whole image into *out_pixels (width * height words, row-major, each a D3DCOLOR 0xAARRGGBB as a host
// integer; alpha 0xff where the image has none), allocated with malloc (free() it). Returns 0 on any error.
int port_image_decode(const void *data, uint32_t size, struct s_port_image_info *out_info, uint32_t **out_pixels);

#endif

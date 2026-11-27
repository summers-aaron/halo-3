// port/sdk/d3d: the image file encoders behind D3DXSaveSurfaceToFileInMemory (d3dx_image.cpp), the counterpart of
// d3dx_decode.h. JPEG is written as d3dx9.lib's IJG libjpeg 6b writes it: baseline JFIF 1.01, YCbCr 4:2:0, the
// jpeg_set_defaults quality (75) with the Annex K tables, the standard Huffman tables and libjpeg's arithmetic
// (fixed-point RGB to YCbCr, h2v2 averaging with replicated edges, the ISLOW integer FDCT). PNG is 8-bit RGB or RGBA,
// not interlaced, one adaptive filter per row, in a zlib stream this file deflates itself (LZ77 with dynamic
// Huffman blocks). Self-contained C++ with C-style data: no C library beyond malloc / free / memcpy / memset. Built
// with the SDK code (halo3_sdk) and by the tmp/ tests.
#ifndef PORT_SDK_D3D_D3DX_ENCODE_H
#define PORT_SDK_D3D_D3DX_ENCODE_H

#include "d3dx_decode.h" // the exact-width types, e_port_image_file

// d3dx9.lib's JPEG writer keeps the quality jpeg_set_defaults gives (retail: jpeg_add_quant_table's scale factor 50).
#define PORT_IMAGE_JPEG_DEFAULT_QUALITY 75u

// Encodes width x height pixels (row-major, `pitch` words from one row to the next, each a D3DCOLOR 0xAARRGGBB as a
// host integer) as a `file` (k_port_image_file_jpeg or k_port_image_file_png) into *out_data, allocated with malloc
// (free() it), *out_size bytes long. channels: 3 colour, 4 colour with alpha (a PNG keeps it; a JPEG has none).
// quality: the JPEG's, 1..100 (0: PORT_IMAGE_JPEG_DEFAULT_QUALITY); a PNG ignores it. Returns 0 on any error (bad
// arguments, an image larger than the format allows, out of memory).
int port_image_encode(uint32_t file, const uint32_t *pixels, uint32_t width, uint32_t height, uint32_t pitch,
                      uint32_t channels, uint32_t quality, uint8_t **out_data, uint32_t *out_size);

#endif

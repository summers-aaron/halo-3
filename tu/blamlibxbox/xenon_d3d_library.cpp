// blamlibxbox/xenon_d3d_library.obj: from the records 0x161910
// @flags /TP /O2 /Oc
#include <ppcintrinsics.h>
namespace {
static __forceinline unsigned int XGLog2LE16(unsigned int texel_pitch) {
    return 31 - _CountLeadingZeros(texel_pitch);
}
}
extern "C" unsigned int __stdcall XGAddress2DTiledOffset(unsigned int x, unsigned int y, unsigned int width, unsigned int texel_pitch) {
    unsigned int aligned_width;
    unsigned int log_bpp;
    unsigned int macro;
    unsigned int micro;
    unsigned int offset;
    aligned_width = (width + 31) & ~31;
    log_bpp = XGLog2LE16(texel_pitch);
    macro = ((x >> 5) + (y >> 5) * (aligned_width >> 5)) << (log_bpp + 7);
    micro = ((x & 7) + ((y & 6) << 2)) << log_bpp;
    offset = macro + ((micro & ~15) << 1) + (micro & 15) + ((y & 8) << (3 + log_bpp)) + ((y & 1) << 4);
    return (((offset & ~511) << 3) + ((offset & 448) << 2) + (offset & 63) +
        ((y & 16) << 7) + (((((y & 8) >> 2) + (x >> 3)) & 3) << 6)) >> log_bpp;
}

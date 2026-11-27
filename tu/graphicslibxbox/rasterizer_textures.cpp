// graphicslibxbox/rasterizer_textures.obj: from the records 0x45ed90 0x45ee28
// @flags /TP /O2
#include <stddef.h>
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_textures.h"
#include "rasterizer/xenon/rasterizer_xenon_device_caps.h"
#include "rasterizer/xenon/rasterizer_xenon_device_info_types.h"
#include "rasterizer/rasterizer_render_targets.h"
// @address 0x45ee28 ?bitmap_format_to_hardware_format@c_rasterizer@@SAKF_N00@Z
// @address 0x16b798 D3DTexture_LockRect
// @address 0x81dad0 g_rasterizer_xenon_device_info.caps
// @address 0x81dac8 g_rasterizer_xenon_device_info.display_mode.Format

// @address 0x4dbc0 g_bitmap_format_to_hardware_format_table
// @rdata 0x8204dbc0..0x8204dc60
extern "C" {
const unsigned int g_bitmap_format_to_hardware_format_table[] = {
    0x04900102, 0x28000102, 0x1a200102, 0x0800014a,
    D3DFMT_UNKNOWN, D3DFMT_UNKNOWN, 0x28280144, D3DFMT_UNKNOWN,
    0x18280143, 0x1828014f, 0x28280186, 0x18280186,
    D3DFMT_UNKNOWN, D3DFMT_UNKNOWN, 0x1a200152, 0x1a200153,
    0x1a200154, 0x1829ff4f, D3DFMT_UNKNOWN, D3DFMT_UNKNOWN,
    D3DFMT_UNKNOWN, D3DFMT_UNKNOWN, 0x2d20ab4a, D3DFMT_UNKNOWN,
    0x1a22aba6, 0x1a22ab5d, 0x1a20ab86, 0x182801b6,
    0x1a20015a, 0x2d20ab99, 0x1a20017a, 0x1a20017b,
    0x1a20017d, 0x1a215571, 0x1a21557c, 0x1c90017a,
    0x2a20017a, 0x1c90017b, 0x2a20017b, 0x08000171
};
}
// @rdata-end

// Locks level 0 of the texture in *texture_slot and returns its bits, or NULL when the slot holds no texture
// (0 or -1). The row pitch goes to *pitch. A zero overwrite locks with D3DLOCK_NOOVERWRITE.
// @address 0x45ed90 rasterizer_texture_lock_bits
void *rasterizer_texture_lock_bits(D3DTexture **texture_slot, int overwrite, long *pitch)
{
  void *bits = 0;
  D3DTexture *texture = 0;
#ifdef HALO3_64BIT
  long slot = *(const long *)texture_slot;
  bool valid = (slot == 0 || slot == -1) ? false : true;
  _D3DLOCKED_RECT locked;
  if (valid) {
    texture = (D3DTexture *)(unsigned __int64)(unsigned long)slot;
  }
#else
  ptrdiff_t slot = (ptrdiff_t)*texture_slot;
  bool valid = (slot == 0 || slot == -1) ? false : true;
  _D3DLOCKED_RECT locked;
  if (valid) {
    texture = (D3DTexture *)slot;
  }
#endif
  *pitch = 0;
  if (texture != 0) {
    unsigned long flags = 0;
    if (overwrite == 0) {
      flags = 0x1000;
    }
    D3DTexture_LockRect(texture, 0, &locked, 0, flags);
    *pitch = locked.Pitch;
    bits = locked.pBits;
  }
  return bits;
}

// The D3D format for a bitmap format, or D3DFMT_UNKNOWN when the device
// cannot use it as a texture.
unsigned long c_rasterizer::bitmap_format_to_hardware_format(short bitmap_format, bool tiled, bool gamma, bool alpha_gamma)
{
  unsigned int format = g_bitmap_format_to_hardware_format_table[bitmap_format];
  // match: 0x45ee28: reloading the adapter ordinal through a volatile pointer preserves the separate load from the caps anchor.
  volatile unsigned int *adapter_ordinal = &g_rasterizer_device_caps.AdapterOrdinal;
  if (format != D3DFMT_UNKNOWN) {
    if (!tiled) {
      format &= ~0x100;
    }
    if (gamma) {
      format = g_gputextureformat_to_srgb_gputextureformat_table[format & 0x3f] |
          (format & ~0x3f) | 0x7e00;
    }
    if (format != D3DFMT_UNKNOWN &&
        Direct3D_CheckDeviceFormat(*adapter_ordinal,
            g_rasterizer_device_caps.DeviceType, rasterizer_get_display_mode_format(),
            0, D3DRTYPE_TEXTURE, (D3DFORMAT)format) < 0)
        {
      return D3DFMT_UNKNOWN;
    }
  }
  return format;
}

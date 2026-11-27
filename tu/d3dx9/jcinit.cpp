// d3dx9/jcinit.obj: from the records 0x5d8018
// @flags /TP /O2

/* Derived from the Independent JPEG Group's libjpeg 6b (jcinit.c, jerror.h, jmorecfg.h, jpegint.h, jpeglib.h),
   as the XDK's D3DX library embeds it.
   jcinit.c: Copyright (C) 1991-1997, Thomas G. Lane.
   jerror.h: Copyright (C) 1994-1997, Thomas G. Lane.
   jmorecfg.h: Copyright (C) 1991-1997, Thomas G. Lane.
   jpegint.h: Copyright (C) 1991-1997, Thomas G. Lane.
   jpeglib.h: Copyright (C) 1991-1998, Thomas G. Lane.
   This file is part of the Independent JPEG Group's software.
   For conditions of distribution and use, see IJG's README file (its LEGAL ISSUES section);
   docs/THIRD_PARTY.md records what a distribution of this source would need.

   Altered: one function of the library per file for byte-matching the Xbox 360 build (compiled as C++ in
   D3DX's namespace, with D3DX's int boolean; the function and the static ones it calls are GLOBAL here,
   the file's functions it inlines are LOCAL; only the declarations the function uses are kept;
   D3DX's jpeg_compress_struct has two more booleans, after write_Adobe_marker). */

// libjpeg 6b jcinit.c jinit_compress_master
#include <stddef.h>

#include "d3dx9/jpeg_types.h"
#include "d3dx9/jcmarker.h"
// @address 0x5d7f78 ?jinit_marker_writer@D3DX@@YAXPAUjpeg_compress_struct@1@@Z
namespace D3DX {
EXTERN(void) jinit_compress_master JPP((j_compress_ptr cinfo));
EXTERN(void) jinit_c_master_control JPP((j_compress_ptr cinfo,
                     boolean transcode_only));
EXTERN(void) jinit_c_main_controller JPP((j_compress_ptr cinfo,
                      boolean need_full_buffer));
EXTERN(void) jinit_c_prep_controller JPP((j_compress_ptr cinfo,
                      boolean need_full_buffer));
EXTERN(void) jinit_c_coef_controller JPP((j_compress_ptr cinfo,
                      boolean need_full_buffer));
EXTERN(void) jinit_color_converter JPP((j_compress_ptr cinfo));
EXTERN(void) jinit_downsampler JPP((j_compress_ptr cinfo));
EXTERN(void) jinit_forward_dct JPP((j_compress_ptr cinfo));
EXTERN(void) jinit_huff_encoder JPP((j_compress_ptr cinfo));
EXTERN(void) jinit_phuff_encoder JPP((j_compress_ptr cinfo));
#define ERREXIT(cinfo,code) ((cinfo)->err->msg_code = (code), (*(cinfo)->err->error_exit) ((j_common_ptr) (cinfo)))

/*
 * Master selection of compression modules.
 * This is done once at the start of processing an image.  We determine
 * which modules will be used and give them appropriate initialization calls.
 */

// @address 0x5d8018 ?jinit_compress_master@D3DX@@YAXPAUjpeg_compress_struct@1@@Z
GLOBAL(void)
jinit_compress_master (j_compress_ptr cinfo)
{
  /* Initialize master control (includes parameter checking/processing) */
  jinit_c_master_control(cinfo, FALSE /* full compression */);

  /* Preprocessing */
  if (! cinfo->raw_data_in) {
    jinit_color_converter(cinfo);
    jinit_downsampler(cinfo);
    jinit_c_prep_controller(cinfo, FALSE /* never need full buffer here */);
  }
  /* Forward DCT */
  jinit_forward_dct(cinfo);
  /* Entropy encoding: either Huffman or arithmetic coding. */
  if (cinfo->arith_code) {
    ERREXIT(cinfo, JERR_ARITH_NOTIMPL);
  } else {
    if (cinfo->progressive_mode) {
#ifdef C_PROGRESSIVE_SUPPORTED
      jinit_phuff_encoder(cinfo);
#else
      ERREXIT(cinfo, JERR_NOT_COMPILED);
#endif
    } else
      jinit_huff_encoder(cinfo);
  }

  /* Need a full-image coefficient buffer in any multi-pass mode. */
  jinit_c_coef_controller(cinfo,
        (boolean) (cinfo->num_scans > 1 || cinfo->optimize_coding));
  jinit_c_main_controller(cinfo, FALSE /* never need full buffer here */);

  jinit_marker_writer(cinfo);

  /* We can now tell the memory manager to allocate virtual arrays. */
  (*cinfo->mem->realize_virt_arrays) ((j_common_ptr) cinfo);

  /* Write the datastream header (SOI) immediately.
   * Frame and scan headers are postponed till later.
   * This lets application insert special markers after the SOI.
   */
  (*cinfo->marker->write_file_header) (cinfo);
}

}  // namespace D3DX

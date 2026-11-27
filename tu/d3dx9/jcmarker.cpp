// d3dx9/jcmarker.obj: from the records 0x5d6c00 0x5d6c88 0x5d7828 0x5d7940 0x5d7a48 0x5d7ac8 0x5d7ad0 0x5d7b48 0x5d7db8 0x5d7df8
// @flags /TP /O2
/* Derived from the Independent JPEG Group's libjpeg 6b (jcmarker.c, jerror.h, jmorecfg.h, jpeglib.h),
   as the XDK's D3DX library embeds it.
   jcmarker.c: Copyright (C) 1991-1998, Thomas G. Lane.
   jerror.h: Copyright (C) 1994-1997, Thomas G. Lane.
   jmorecfg.h: Copyright (C) 1991-1997, Thomas G. Lane.
   jpeglib.h: Copyright (C) 1991-1998, Thomas G. Lane.
   This file is part of the Independent JPEG Group's software.
   For conditions of distribution and use, see IJG's README file (its LEGAL ISSUES section);
   docs/THIRD_PARTY.md records what a distribution of this source would need.

   Altered: one function of the library per file for byte-matching the Xbox 360 build (compiled as C++ in
   D3DX's namespace, with D3DX's int boolean; the function and the static ones it calls are GLOBAL here,
   the file's functions it inlines are LOCAL; only the declarations the function uses are kept;
   D3DX's jpeg_compress_struct has two more booleans, after write_Adobe_marker). */

#include <stddef.h>

#include "d3dx9/jpeg_types.h"
#include "d3dx9/jcmarker.h"
#include "d3dx9/jutils.h"
namespace D3DX {
#define SIZEOF(object) ((size_t) sizeof(object))
#define ERREXIT(cinfo,code) ((cinfo)->err->msg_code = (code), (*(cinfo)->err->error_exit) ((j_common_ptr) (cinfo)))
typedef struct {
  struct jpeg_marker_writer pub; /* public fields */
  unsigned int last_restart_interval; /* last DRI value emitted; 0 after SOI */
} my_marker_writer;
typedef my_marker_writer * my_marker_ptr;
typedef enum {			/* JPEG marker codes */
  M_SOF0  = 0xc0,
  M_SOF1  = 0xc1,
  M_SOF2  = 0xc2,
  M_SOF3  = 0xc3,
  M_SOF5  = 0xc5,
  M_SOF6  = 0xc6,
  M_SOF7  = 0xc7,
  M_JPG   = 0xc8,
  M_SOF9  = 0xc9,
  M_SOF10 = 0xca,
  M_SOF11 = 0xcb,
  M_SOF13 = 0xcd,
  M_SOF14 = 0xce,
  M_SOF15 = 0xcf,
  M_DHT   = 0xc4,
  M_DAC   = 0xcc,
  M_RST0  = 0xd0,
  M_RST1  = 0xd1,
  M_RST2  = 0xd2,
  M_RST3  = 0xd3,
  M_RST4  = 0xd4,
  M_RST5  = 0xd5,
  M_RST6  = 0xd6,
  M_RST7  = 0xd7,
  M_SOI   = 0xd8,
  M_EOI   = 0xd9,
  M_SOS   = 0xda,
  M_DQT   = 0xdb,
  M_DNL   = 0xdc,
  M_DRI   = 0xdd,
  M_DHP   = 0xde,
  M_EXP   = 0xdf,
  M_APP0  = 0xe0,
  M_APP1  = 0xe1,
  M_APP2  = 0xe2,
  M_APP3  = 0xe3,
  M_APP4  = 0xe4,
  M_APP5  = 0xe5,
  M_APP6  = 0xe6,
  M_APP7  = 0xe7,
  M_APP8  = 0xe8,
  M_APP9  = 0xe9,
  M_APP10 = 0xea,
  M_APP11 = 0xeb,
  M_APP12 = 0xec,
  M_APP13 = 0xed,
  M_APP14 = 0xee,
  M_APP15 = 0xef,
  M_JPG0  = 0xf0,
  M_JPG13 = 0xfd,
  M_COM   = 0xfe,
  M_TEM   = 0x01,
  M_ERROR = 0x100
} JPEG_MARKER;
#define TRUE 1
#define ERREXIT1(cinfo,code,p1) ((cinfo)->err->msg_code = (code), (cinfo)->err->msg_parm.i[0] = (p1), (*(cinfo)->err->error_exit) ((j_common_ptr) (cinfo)))
#define TRACEMS(cinfo,lvl,code) ((cinfo)->err->msg_code = (code), (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), (lvl)))

/*
 * Basic output routines.
 *
 * Note that we do not support suspension while writing a marker.
 * Therefore, an application using suspension must ensure that there is
 * enough buffer space for the initial markers (typ. 600-700 bytes) before
 * calling jpeg_start_compress, and enough space to write the trailing EOI
 * (a few bytes) before calling jpeg_finish_compress.  Multipass compression
 * modes are not supported at all with suspension, so those two are the only
 * points where markers will be written.
 */

GLOBAL(void)
emit_byte (j_compress_ptr cinfo, int val)
{
  struct jpeg_destination_mgr * dest = cinfo->dest;

  *(dest->next_output_byte)++ = (JOCTET) val;
  if (--dest->free_in_buffer == 0) {
    if (! (*dest->empty_output_buffer) (cinfo))
      ERREXIT(cinfo, JERR_CANT_SUSPEND);
  }
}

LOCAL(void)
emit_marker (j_compress_ptr cinfo, JPEG_MARKER mark)
{
  emit_byte(cinfo, 0xFF);
  emit_byte(cinfo, (int) mark);
}
LOCAL(void)
emit_2bytes (j_compress_ptr cinfo, int value)
{
  emit_byte(cinfo, (value >> 8) & 0xFF);
  emit_byte(cinfo, value & 0xFF);
}

static __forceinline void jcmarker_emit_byte (j_compress_ptr cinfo, int val)
{
  struct jpeg_destination_mgr * dest = cinfo->dest;

  *(dest->next_output_byte)++ = (JOCTET) val;
  if (--dest->free_in_buffer == 0) {
    if (! (*dest->empty_output_buffer) (cinfo))
      ERREXIT(cinfo, JERR_CANT_SUSPEND);
  }
}
static __forceinline void jcmarker_emit_marker (j_compress_ptr cinfo, JPEG_MARKER mark)
{
  jcmarker_emit_byte(cinfo, 0xFF);
  jcmarker_emit_byte(cinfo, (int) mark);
}
static __forceinline void jcmarker_emit_2bytes (j_compress_ptr cinfo, int value)
{
  jcmarker_emit_byte(cinfo, (value >> 8) & 0xFF);
  jcmarker_emit_byte(cinfo, value & 0xFF);
}

GLOBAL(int)
emit_dqt (j_compress_ptr cinfo, int index)
{
  JQUANT_TBL * qtbl = cinfo->quant_tbl_ptrs[index];
  int prec;
  int i;

  if (qtbl == NULL)
    ERREXIT1(cinfo, JERR_NO_QUANT_TABLE, index);

  prec = 0;
  for (i = 0; i < DCTSIZE2; i++) {
    if (qtbl->quantval[i] > 255)
      prec = 1;
  }

  if (! qtbl->sent_table) {
    jcmarker_emit_marker(cinfo, M_DQT);

    jcmarker_emit_2bytes(cinfo, prec ? DCTSIZE2*2 + 1 + 2 : DCTSIZE2 + 1 + 2);

    jcmarker_emit_byte(cinfo, index + (prec<<4));

    for (i = 0; i < DCTSIZE2; i++) {
      unsigned int qval = qtbl->quantval[jpeg_natural_order[i]];
      if (prec)
    jcmarker_emit_byte(cinfo, (int) (qval >> 8));
      jcmarker_emit_byte(cinfo, (int) (qval & 0xFF));
    }

    qtbl->sent_table = TRUE;
  }

  return prec;
}










GLOBAL(void)
emit_jfif_app0 (j_compress_ptr cinfo)
{









  
  emit_marker(cinfo, M_APP0);
  
  emit_2bytes(cinfo, 2 + 4 + 1 + 2 + 1 + 2 + 2 + 1 + 1); 

  emit_byte(cinfo, 0x4A);	
  emit_byte(cinfo, 0x46);
  emit_byte(cinfo, 0x49);
  emit_byte(cinfo, 0x46);
  emit_byte(cinfo, 0);
  emit_byte(cinfo, cinfo->JFIF_major_version); 
  emit_byte(cinfo, cinfo->JFIF_minor_version);
  emit_byte(cinfo, cinfo->density_unit); 
  emit_2bytes(cinfo, (int) cinfo->X_density);
  emit_2bytes(cinfo, (int) cinfo->Y_density);
  emit_byte(cinfo, 0);		
  emit_byte(cinfo, 0);
}


GLOBAL(void)
emit_adobe_app14 (j_compress_ptr cinfo)
{













  
  emit_marker(cinfo, M_APP14);
  
  emit_2bytes(cinfo, 2 + 5 + 2 + 2 + 2 + 1); 

  emit_byte(cinfo, 0x41);	
  emit_byte(cinfo, 0x64);
  emit_byte(cinfo, 0x6F);
  emit_byte(cinfo, 0x62);
  emit_byte(cinfo, 0x65);
  emit_2bytes(cinfo, 100);	
  emit_2bytes(cinfo, 0);	
  emit_2bytes(cinfo, 0);	
  switch (cinfo->jpeg_color_space) {
  case JCS_YCbCr:
    emit_byte(cinfo, 1);	
    break;
  case JCS_YCCK:
    emit_byte(cinfo, 2);	
    break;
  default:
    emit_byte(cinfo, 0);	
    break;
  }
}







GLOBAL(void)
write_marker_header (j_compress_ptr cinfo, int marker, unsigned int datalen)
{
  if (datalen > (unsigned int) 65533)		
    ERREXIT(cinfo, JERR_BAD_LENGTH);

  emit_marker(cinfo, (JPEG_MARKER) marker);

  emit_2bytes(cinfo, (int) (datalen + 2));	
}

// @address 0x5d7ac8 ?write_marker_byte@D3DX@@YAXPAUjpeg_compress_struct@1@H@Z
GLOBAL(void)
write_marker_byte (j_compress_ptr cinfo, int val)
{
  emit_byte(cinfo, val);
}










GLOBAL(void)
write_file_header (j_compress_ptr cinfo)
{
  my_marker_ptr marker = (my_marker_ptr) cinfo->marker;

  emit_marker(cinfo, M_SOI);	

  marker->last_restart_interval = 0;

  if (cinfo->write_JFIF_header)	
    emit_jfif_app0(cinfo);
  if (cinfo->write_Adobe_marker) 
    emit_adobe_app14(cinfo);
}













GLOBAL(void)
emit_sof (j_compress_ptr cinfo, JPEG_MARKER code);

GLOBAL(void)
write_frame_header (j_compress_ptr cinfo)
{
  int ci, prec;
  boolean is_baseline;
  jpeg_component_info *compptr;

  prec = 0;
  for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
       ci++, compptr++) {
    prec += emit_dqt(cinfo, compptr->quant_tbl_no);
  }

  if (cinfo->arith_code || cinfo->progressive_mode ||
      cinfo->data_precision != 8) {
    is_baseline = FALSE;
  } else {
    is_baseline = TRUE;
    for (ci = 0, compptr = cinfo->comp_info; ci < cinfo->num_components;
     ci++, compptr++) {
      if (compptr->dc_tbl_no > 1 || compptr->ac_tbl_no > 1)
    is_baseline = FALSE;
    }
    if (prec && is_baseline) {
      is_baseline = FALSE;
      TRACEMS(cinfo, 0, JTRC_16BIT_TABLES);
    }
  }

  if (cinfo->arith_code) {
    emit_sof(cinfo, M_SOF9);
  } else {
    if (cinfo->progressive_mode)
      emit_sof(cinfo, M_SOF2);
    else if (is_baseline)
      emit_sof(cinfo, M_SOF0);
    else
      emit_sof(cinfo, M_SOF1);
  }
}

GLOBAL(void)
write_file_trailer (j_compress_ptr cinfo)
{
  emit_marker(cinfo, M_EOI);
}

GLOBAL(void)
emit_dht (j_compress_ptr cinfo, int index, boolean is_ac);

GLOBAL(void)
write_tables_only (j_compress_ptr cinfo)
{
  int i;

  emit_marker(cinfo, M_SOI);

  for (i = 0; i < NUM_QUANT_TBLS; i++) {
    if (cinfo->quant_tbl_ptrs[i] != NULL)
      (void) emit_dqt(cinfo, i);
  }

  if (! cinfo->arith_code) {
    for (i = 0; i < NUM_HUFF_TBLS; i++) {
      if (cinfo->dc_huff_tbl_ptrs[i] != NULL)
    emit_dht(cinfo, i, FALSE);
      if (cinfo->ac_huff_tbl_ptrs[i] != NULL)
    emit_dht(cinfo, i, TRUE);
    }
  }

  jcmarker_emit_marker(cinfo, M_EOI);
}





}  // namespace D3DX

// d3dx9/pngwutil.obj: from the records 0x5d8128 0x5d81b0 0x5d81f8 0x5d8210 0x5d82f0 0x5d8478 0x5d86d8 0x5d8738 0x5d8a90 0x5d8ae0 0x5d8d40
// @flags /TP /O2 /Os
// @address 0x104e8 png_signature
// @address 0x10508 png_PLTE
// @address 0x10478 k_png_pass_tables
// @address 0x104f0 png_IHDR
// @address 0x104f8 png_IDAT
// @address 0x10500 png_IEND
// @address 0x5ca578 ?png_zfree@D3DX@@YAXPAX0@Z
// @address 0x5d8b30 ?png_write_finish_row@D3DX@@YAXPAUpng_struct_def@1@@Z

/* Derived from libpng 1.2.8 (pngwutil.c, png.h, pngconf.h, zconf.h, zlib.h), as the XDK's D3DX library embeds it.
   Copyright (c) 1998-2004 Glenn Randers-Pehrson
   (Version 0.96 Copyright (c) 1996, 1997 Andreas Dilger)
   (Version 0.88 Copyright (c) 1995, 1996 Guy Eric Schalnat, Group 42, Inc.)
   For conditions of distribution and use, see the copyright notice in libpng's png.h;
   zlib.h and zconf.h: zlib 1.1.4, Copyright (C) 1995-2002 Jean-loup Gailly and Mark Adler, under
   the zlib license (its full notice is in this project's zlib-derived sources);
   docs/THIRD_PARTY.md records what a distribution of this source would need.

   Altered: one function of the library per file for byte-matching the Xbox 360 build (compiled as C++ in
   D3DX's namespace with D3DX's configuration, see its PNG_NO_ options below; static functions are external
   here so the compiler keeps them;
   only the declarations the function uses are kept;
   D3DX's png_struct ends at int_gamma (none of the members libpng 1.0.13 and later appended)). */

#include <setjmp.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "d3dx9/png.h"
namespace D3DX {

/* D3DX's libpng configuration (pngusr.h) */
#define PNG_NO_STDIO
#define PNG_NO_CONSOLE_IO
#define PNG_NO_PROGRESSIVE_READ
#define PNG_NO_INFO_IMAGE
#define PNG_NO_MNG_FEATURES
#define PNG_NO_WRITE_TRANSFORMS
#define PNG_NO_READ_INVERT
#define PNG_NO_READ_PACKSWAP
#define PNG_NO_READ_SWAP_ALPHA
#define PNG_NO_READ_INVERT_ALPHA
#define PNG_WRITE_EMPTY_PLTE_SUPPORTED
#define PNG_NO_ERROR_NUMBERS
#define PNG_NO_WRITE_gAMA
#define PNG_NO_WRITE_sRGB
#define PNG_NO_WRITE_tRNS
#define PNG_WRITE_INTERLACING_SUPPORTED
#define PNG_WRITE_FLUSH_SUPPORTED
#define PNG_INTERLACE_ADAM7 1
#define PNG_NO_READ_STRIP_ALPHA
#define PNG_NO_READ_RGB_TO_GRAY
#define PNG_NO_READ_GRAY_TO_RGB
#define PNG_NO_READ_BACKGROUND
#define PNG_NO_READ_USER_TRANSFORM
#define PNG_NO_READ_bKGD
#define PNG_NO_READ_cHRM
#define PNG_NO_READ_hIST
#define PNG_NO_READ_iCCP
#define PNG_NO_READ_iTXt
#define PNG_NO_READ_oFFs
#define PNG_NO_READ_pCAL
#define PNG_NO_READ_sCAL
#define PNG_NO_READ_pHYs
#define PNG_NO_READ_sBIT
#define PNG_NO_READ_sPLT
#define PNG_NO_READ_tEXt
#define PNG_NO_READ_tIME
#define PNG_NO_READ_zTXt
#define PNG_NO_READ_UNKNOWN_CHUNKS
#define PNG_NO_READ_USER_CHUNKS
#define PNG_NO_WRITE_bKGD
#define PNG_NO_WRITE_cHRM
#define PNG_NO_WRITE_hIST
#define PNG_NO_WRITE_iCCP
#define PNG_NO_WRITE_iTXt
#define PNG_NO_WRITE_oFFs
#define PNG_NO_WRITE_pCAL
#define PNG_NO_WRITE_sCAL
#define PNG_NO_WRITE_pHYs
#define PNG_NO_WRITE_sBIT
#define PNG_NO_WRITE_sPLT
#define PNG_NO_WRITE_tEXt
#define PNG_NO_WRITE_tIME
#define PNG_NO_WRITE_zTXt
#define PNG_NO_WRITE_UNKNOWN_CHUNKS
#define PNG_NO_HANDLE_AS_UNKNOWN
#define PNG_NO_USER_MEM
#define PNG_NO_TIME_RFC1123
#define PNG_NO_ASSEMBLER_CODE
#define PNG_NO_MMX_CODE
#define PNG_NO_FREE_ME
#define PNG_NO_FIXED_POINT_SUPPORTED
#define PNG_NO_SET_USER_LIMITS

#define png_debug2(l,m,p1,p2) 

static void /* PRIVATE */
pngwutil_save_uint_32(png_bytep buf, png_uint_32 i)
{
   buf[0] = (png_byte)((i >> 24) & 0xff);
   buf[1] = (png_byte)((i >> 16) & 0xff);
   buf[2] = (png_byte)((i >> 8) & 0xff);
   buf[3] = (png_byte)(i & 0xff);
}

/* Write the start of a PNG chunk.  The type is the chunk type.
 * The total_length is the sum of the lengths of all the data you will be
 * passing in pngwutil_write_chunk_data().
 */
// @address 0x5d8128 ?png_write_chunk_start@D3DX@@YAXPAUpng_struct_def@1@PAEK@Z
void 
png_write_chunk_start(png_structp png_ptr, png_bytep chunk_name,
   png_uint_32 length)
{
   png_byte buf[4];
   png_debug2(0, "Writing %s chunk (%lu bytes)\n", chunk_name, length);

   /* write the length */
   pngwutil_save_uint_32(buf, length);
   png_write_data(png_ptr, buf, (png_size_t)4);

   /* write the chunk name */
   png_write_data(png_ptr, chunk_name, (png_size_t)4);
   /* reset the crc and run it over the chunk name */
   png_reset_crc(png_ptr);
   png_calculate_crc(png_ptr, chunk_name, (png_size_t)4);
}


/* Finish a chunk started with png_write_chunk_start(). */
// @address 0x5d81b0 ?png_write_chunk_end@D3DX@@YAXPAUpng_struct_def@1@@Z
void 
png_write_chunk_end(png_structp png_ptr)
{
   png_byte buf[4];

   /* write the crc */
   pngwutil_save_uint_32(buf, png_ptr->crc);

   png_write_data(png_ptr, buf, (png_size_t)4);
}


/* png_signature, by its address */

/* D3DX: png_write_sig as libpng 1.0 had it, without the signature mode flag
 * (written from the machine code).
 */
// @address 0x5d81f8 ?png_write_sig@D3DX@@YAXPAUpng_struct_def@1@@Z
void /* PRIVATE */
png_write_sig(png_structp png_ptr)
{
   /* write the rest of the 8 byte signature */
   png_write_data(png_ptr, &png_signature[png_ptr->sig_bytes],
      (png_size_t)8 - png_ptr->sig_bytes);
}

static void 
pngwutil_write_chunk_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
   /* write the data, and run the CRC over it */
   if (data != NULL && length > 0)
   {
      png_calculate_crc(png_ptr, data, length);
      png_write_data(png_ptr, data, length);
   }
}
#define PNG_COLOR_MASK_PALETTE 1
#define PNG_COLOR_MASK_COLOR 2
#define PNG_COLOR_MASK_ALPHA 4
#define PNG_COLOR_TYPE_GRAY 0
#define PNG_COLOR_TYPE_PALETTE (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_PALETTE)
#define PNG_COLOR_TYPE_RGB (PNG_COLOR_MASK_COLOR)
#define PNG_COLOR_TYPE_RGB_ALPHA (PNG_COLOR_MASK_COLOR | PNG_COLOR_MASK_ALPHA)
#define PNG_COLOR_TYPE_GRAY_ALPHA (PNG_COLOR_MASK_ALPHA)
#define PNG_COMPRESSION_TYPE_BASE 0
#define PNG_FILTER_TYPE_BASE 0
#define PNG_INTERLACE_NONE 0
#define PNG_FILTER_NONE 0x08
#define PNG_FILTER_SUB 0x10
#define PNG_FILTER_UP 0x20
#define PNG_FILTER_AVG 0x40
#define PNG_FILTER_PAETH 0x80
#define PNG_ALL_FILTERS (PNG_FILTER_NONE | PNG_FILTER_SUB | PNG_FILTER_UP | PNG_FILTER_AVG | PNG_FILTER_PAETH)
#define PNG_FILTER_AVG 0x40
#define PNG_FILTER_PAETH 0x80
#define PNG_FILTER_VALUE_NONE 0
#define PNG_FILTER_VALUE_SUB 1
#define PNG_FILTER_VALUE_UP 2
#define PNG_FILTER_VALUE_AVG 3
#define PNG_FILTER_VALUE_PAETH 4
#define png_debug(l,m)
#define png_debug1(l,m,p1)
#define PNG_HAVE_PLTE 0x02
/* png_PLTE, by its address */
/* write the palette.  We are careful not to trust png_color to be in the
 * correct order for PNG, so people can redefine it to any convenient
 * structure.
 */
// @address 0x5d8210 ?png_write_PLTE@D3DX@@YAXPAUpng_struct_def@1@PAUpng_color_struct@1@K@Z
void /* PRIVATE */
png_write_PLTE(png_structp png_ptr, png_colorp palette, png_uint_32 num_pal)
{
#ifdef PNG_USE_LOCAL_ARRAYS
   PNG_PLTE;
#endif
   png_uint_32 i;
   png_colorp pal_ptr;
   png_byte buf[3];

   png_debug(1, "in png_write_PLTE\n");
   if ((
#ifdef PNG_WRITE_EMPTY_PLTE_SUPPORTED
        !png_ptr->empty_plte_permitted &&
#endif
        num_pal == 0) || num_pal > 256)
     {
       if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE)
         {
           png_error(png_ptr, "Invalid number of colors in palette");
         }
       else
         {
           png_warning(png_ptr, "Invalid number of colors in palette");
           return;
         }
   }

   png_ptr->num_palette = (png_uint_16)num_pal;
   png_debug1(3, "num_palette = %d\n", png_ptr->num_palette);

   png_write_chunk_start(png_ptr, (png_bytep)png_PLTE, num_pal * 3);
#ifndef PNG_NO_POINTER_INDEXING
   for (i = 0, pal_ptr = palette; i < num_pal; i++, pal_ptr++)
   {
      buf[0] = pal_ptr->red;
      buf[1] = pal_ptr->green;
      buf[2] = pal_ptr->blue;
      pngwutil_write_chunk_data(png_ptr, buf, (png_size_t)3);
   }
#else
   /* This is a little slower but some buggy compilers need to do this instead */
   pal_ptr=palette;
   for (i = 0; i < num_pal; i++)
   {
      buf[0] = pal_ptr[i].red;
      buf[1] = pal_ptr[i].green;
      buf[2] = pal_ptr[i].blue;
      pngwutil_write_chunk_data(png_ptr, buf, (png_size_t)3);
   }
#endif
   png_write_chunk_end(png_ptr);
   png_ptr->mode |= PNG_HAVE_PLTE;
}

#define PNG_INTERLACE 0x0002

#define PNG_USE_LOCAL_ARRAYS
/* initializes the row writing capability of libpng */
// @address 0x5d82f0 ?png_write_start_row@D3DX@@YAXPAUpng_struct_def@1@@Z
void /* PRIVATE */
png_write_start_row(png_structp png_ptr)
{
#ifdef PNG_USE_LOCAL_ARRAYS
   /* arrays to facilitate easy interlacing - use pass (0 - 6) as index */

   /* start of interlace block */
   int png_pass_start[7] = {0, 4, 0, 2, 0, 1, 0};

   /* offset to next interlace block */
   int png_pass_inc[7] = {8, 8, 4, 4, 2, 2, 1};

   /* start of interlace block in the y direction */
   int png_pass_ystart[7] = {0, 0, 4, 0, 2, 0, 1};

   /* offset to next interlace block in the y direction */
   int png_pass_yinc[7] = {8, 8, 8, 4, 4, 2, 2};
#endif

   png_size_t buf_size;

   png_debug(1, "in png_write_start_row\n");
   buf_size = (png_size_t)(((png_ptr->width * png_ptr->usr_channels *
                            png_ptr->usr_bit_depth + 7) >> 3) + 1);

   /* set up row buffer */
   png_ptr->row_buf = (png_bytep)png_malloc(png_ptr, (png_uint_32)buf_size);
   png_ptr->row_buf[0] = PNG_FILTER_VALUE_NONE;

   /* set up filtering buffer, if using this filter */
   if (png_ptr->do_filter & PNG_FILTER_SUB)
   {
      png_ptr->sub_row = (png_bytep)png_malloc(png_ptr,
         (png_ptr->rowbytes + 1));
      png_ptr->sub_row[0] = PNG_FILTER_VALUE_SUB;
   }

   /* We only need to keep the previous row if we are using one of these. */
   if (png_ptr->do_filter & (PNG_FILTER_AVG | PNG_FILTER_UP | PNG_FILTER_PAETH))
   {
     /* set up previous row buffer */
      png_ptr->prev_row = (png_bytep)png_malloc(png_ptr, (png_uint_32)buf_size);
      memset(png_ptr->prev_row, 0, buf_size);

      if (png_ptr->do_filter & PNG_FILTER_UP)
      {
         png_ptr->up_row = (png_bytep )png_malloc(png_ptr,
            (png_ptr->rowbytes + 1));
         png_ptr->up_row[0] = PNG_FILTER_VALUE_UP;
      }

      if (png_ptr->do_filter & PNG_FILTER_AVG)
      {
         png_ptr->avg_row = (png_bytep)png_malloc(png_ptr,
            (png_ptr->rowbytes + 1));
         png_ptr->avg_row[0] = PNG_FILTER_VALUE_AVG;
      }

      if (png_ptr->do_filter & PNG_FILTER_PAETH)
      {
         png_ptr->paeth_row = (png_bytep )png_malloc(png_ptr,
            (png_ptr->rowbytes + 1));
         png_ptr->paeth_row[0] = PNG_FILTER_VALUE_PAETH;
      }
   }

#ifdef PNG_WRITE_INTERLACING_SUPPORTED
   /* if interlaced, we need to set up width and height of pass */
   if (png_ptr->interlaced)
   {
      if (!(png_ptr->transformations & PNG_INTERLACE))
      {
         png_ptr->num_rows = (png_ptr->height + png_pass_yinc[0] - 1 -
            png_pass_ystart[0]) / png_pass_yinc[0];
         png_ptr->usr_width = (png_ptr->width + png_pass_inc[0] - 1 -
            png_pass_start[0]) / png_pass_inc[0];
      }
      else
      {
         png_ptr->num_rows = png_ptr->height;
         png_ptr->usr_width = png_ptr->width;
      }
   }
   else
#endif
   {
      png_ptr->num_rows = png_ptr->height;
      png_ptr->usr_width = png_ptr->width;
   }
   png_ptr->zstream.avail_out = (uInt)png_ptr->zbuf_size;
   png_ptr->zstream.next_out = png_ptr->zbuf;
}

/* Pick out the correct pixels for the interlace pass.
 * The basic idea here is to go through the row with a source
 * pointer and a destination pointer (sp and dp), and copy the
 * correct pixels for the pass.  As the row gets compacted,
 * sp will always be >= dp, so we should never overwrite anything.
 * See the default: case for the easiest code to understand.
 */
/* D3DX's copy: its interlace arrays at namespace scope (constants: internal linkage) */
#define png_pass_start (k_png_pass_tables)
#define png_pass_inc (k_png_pass_tables+7)
#define png_pass_ystart (k_png_pass_tables+14)
#define png_pass_yinc (k_png_pass_tables+21)

typedef png_row_info * png_row_infop;
// @address 0x5d8478 ?png_do_write_interlace@D3DX@@YAXPAUpng_row_info_struct@1@PAEH@Z
void /* PRIVATE */
png_do_write_interlace(png_row_infop row_info, png_bytep row, int pass)
{
   /* D3DX's copy: its interlace arrays are at namespace scope, above */

   png_debug(1, "in png_do_write_interlace\n");
   /* we don't have to do anything on the last pass (6) */
#if defined(PNG_USELESS_TESTS_SUPPORTED)
   if (row != NULL && row_info != NULL && pass < 6)
#else
   if (pass < 6)
#endif
   {
      /* each pixel depth is handled separately */
      switch (row_info->pixel_depth)
      {
         case 1:
         {
            png_bytep sp;
            png_bytep dp;
            int shift;
            int d;
            int value;
            png_uint_32 i;
            png_uint_32 row_width = row_info->width;

            dp = row;
            d = 0;
            shift = 7;
            for (i = png_pass_start[pass]; i < row_width;
               i += png_pass_inc[pass])
            {
               sp = row + (png_size_t)(i >> 3);
               value = (int)(*sp >> (7 - (int)(i & 0x07))) & 0x01;
               d |= (value << shift);

               if (shift == 0)
               {
                  shift = 7;
                  *dp++ = (png_byte)d;
                  d = 0;
               }
               else
                  shift--;

            }
            if (shift != 7)
               *dp = (png_byte)d;
            break;
         }
         case 2:
         {
            png_bytep sp;
            png_bytep dp;
            int shift;
            int d;
            int value;
            png_uint_32 i;
            png_uint_32 row_width = row_info->width;

            dp = row;
            shift = 6;
            d = 0;
            for (i = png_pass_start[pass]; i < row_width;
               i += png_pass_inc[pass])
            {
               sp = row + (png_size_t)(i >> 2);
               value = (*sp >> ((3 - (int)(i & 0x03)) << 1)) & 0x03;
               d |= (value << shift);

               if (shift == 0)
               {
                  shift = 6;
                  *dp++ = (png_byte)d;
                  d = 0;
               }
               else
                  shift -= 2;
            }
            if (shift != 6)
                   *dp = (png_byte)d;
            break;
         }
         case 4:
         {
            png_bytep sp;
            png_bytep dp;
            int shift;
            int d;
            int value;
            png_uint_32 i;
            png_uint_32 row_width = row_info->width;

            dp = row;
            shift = 4;
            d = 0;
            for (i = png_pass_start[pass]; i < row_width;
               i += png_pass_inc[pass])
            {
               sp = row + (png_size_t)(i >> 1);
               value = (*sp >> ((1 - (int)(i & 0x01)) << 2)) & 0x0f;
               d |= (value << shift);

               if (shift == 0)
               {
                  shift = 4;
                  *dp++ = (png_byte)d;
                  d = 0;
               }
               else
                  shift -= 4;
            }
            if (shift != 4)
               *dp = (png_byte)d;
            break;
         }
         default:
         {
            png_bytep sp;
            png_bytep dp;
            png_uint_32 i;
            png_uint_32 row_width = row_info->width;
            png_size_t pixel_bytes;

            /* start at the beginning */
            dp = row;
            /* find out how many bytes each pixel takes up */
            pixel_bytes = (row_info->pixel_depth >> 3);
            /* loop through the row, only looking at the pixels that
               matter */
            for (i = png_pass_start[pass]; i < row_width;
               i += png_pass_inc[pass])
            {
               /* find out where the original pixel is */
               sp = row + (png_size_t)i * pixel_bytes;
               /* move the pixel */
               if (dp != sp)
                  memcpy(dp, sp, pixel_bytes);
               /* next pixel */
               dp += pixel_bytes;
            }
            break;
         }
      }
      /* set new row width */
      row_info->width = (row_info->width +
         png_pass_inc[pass] - 1 -
         png_pass_start[pass]) /
         png_pass_inc[pass];
         row_info->rowbytes = ((row_info->width *
            row_info->pixel_depth + 7) >> 3);
   }
}


#undef PNG_USE_LOCAL_ARRAYS

/* Write a PNG chunk all at once.  The type is an array of ASCII characters
 * representing the chunk name.  The array must be at least 4 bytes in
 * length, and does not need to be null terminated.  To be safe, pass the
 * pre-defined chunk names here, and if you need a new one, define it
 * where the others are defined.  The length is the length of the data.
 * All the data must be present.  If that is not possible, use the
 * png_write_chunk_start(), pngwutil_write_chunk_data(), and png_write_chunk_end()
 * functions instead.
 */
// @address 0x5d86d8 ?png_write_chunk@D3DX@@YAXPAUpng_struct_def@1@PAE1I@Z
void 
png_write_chunk(png_structp png_ptr, png_bytep chunk_name,
   png_bytep data, png_size_t length)
{
   png_write_chunk_start(png_ptr, chunk_name, (png_uint_32)length);
   pngwutil_write_chunk_data(png_ptr, data, length);
   png_write_chunk_end(png_ptr);
}
/* zlib version string "1.1.4", by its address (owned by data_compress.obj) */
#define deflateInit2(strm,level,method,windowBits,memLevel,strategy) \
   deflateInit2_((strm),(level),(method),(windowBits),(memLevel), \
      (strategy), "1.1.4", sizeof(z_stream))
#define png_debug(l,m) 
#define PNG_HAVE_IHDR 0x01
#define PNG_FLAG_ZLIB_CUSTOM_STRATEGY 0x0001
#define PNG_FLAG_ZLIB_CUSTOM_LEVEL 0x0002
#define PNG_FLAG_ZLIB_CUSTOM_MEM_LEVEL 0x0004
#define PNG_FLAG_ZLIB_CUSTOM_WINDOW_BITS 0x0008
#define PNG_FLAG_ZLIB_CUSTOM_METHOD 0x0010
#define PNG_IHDR
/* png_zfree, by its address */
#define PNG_USE_LOCAL_ARRAYS
/* Write the IHDR chunk, and update the png_struct with the necessary
 * information.  Note that the rest of this code depends upon this
 * information being correct.
 */
/* png_IHDR, by its address */
// @address 0x5d8738 ?png_write_IHDR@D3DX@@YAXPAUpng_struct_def@1@KKHHHHH@Z
void /* PRIVATE */
png_write_IHDR(png_structp png_ptr, png_uint_32 width, png_uint_32 height,
   int bit_depth, int color_type, int compression_type, int filter_type,
   int interlace_type)
{
#ifdef PNG_USE_LOCAL_ARRAYS
   PNG_IHDR;
#endif
   png_byte buf[13]; /* buffer to store the IHDR info */

   png_debug(1, "in png_write_IHDR\n");
   /* Check that we have valid input data from the application info */
   switch (color_type)
   {
      case PNG_COLOR_TYPE_GRAY:
         switch (bit_depth)
         {
            case 1:
            case 2:
            case 4:
            case 8:
            case PNG_MAX_BIT_DEPTH: png_ptr->channels = 1; break;
            default: png_error(png_ptr,"Invalid bit depth for grayscale image");
         }
         break;
      case PNG_COLOR_TYPE_RGB:
         if (bit_depth != 8 && bit_depth != PNG_MAX_BIT_DEPTH)
            png_error(png_ptr, "Invalid bit depth for RGB image");
         png_ptr->channels = 3;
         break;
      case PNG_COLOR_TYPE_PALETTE:
         switch (bit_depth)
         {
            case 1:
            case 2:
            case 4:
            case 8: png_ptr->channels = 1; break;
            default: png_error(png_ptr, "Invalid bit depth for paletted image");
         }
         break;
      case PNG_COLOR_TYPE_GRAY_ALPHA:
         if (bit_depth != 8 && bit_depth != PNG_MAX_BIT_DEPTH)
            png_error(png_ptr, "Invalid bit depth for grayscale+alpha image");
         png_ptr->channels = 2;
         break;
      case PNG_COLOR_TYPE_RGB_ALPHA:
         if (bit_depth != 8 && bit_depth != PNG_MAX_BIT_DEPTH)
            png_error(png_ptr, "Invalid bit depth for RGBA image");
         png_ptr->channels = 4;
         break;
      default:
         png_error(png_ptr, "Invalid image color type specified");
   }

   if (compression_type != PNG_COMPRESSION_TYPE_BASE)
   {
      png_warning(png_ptr, "Invalid compression type specified");
      compression_type = PNG_COMPRESSION_TYPE_BASE;
   }

   if (filter_type != PNG_FILTER_TYPE_BASE)
   {
      png_warning(png_ptr, "Invalid filter type specified");
      filter_type = PNG_FILTER_TYPE_BASE;
   }

#ifdef PNG_WRITE_INTERLACING_SUPPORTED
   if (interlace_type != PNG_INTERLACE_NONE &&
      interlace_type != PNG_INTERLACE_ADAM7)
   {
      png_warning(png_ptr, "Invalid interlace type specified");
      interlace_type = PNG_INTERLACE_ADAM7;
   }
#else
   interlace_type=PNG_INTERLACE_NONE;
#endif

   /* save off the relevent information */
   png_ptr->bit_depth = (png_byte)bit_depth;
   png_ptr->color_type = (png_byte)color_type;
   png_ptr->interlaced = (png_byte)interlace_type;
   png_ptr->width = width;
   png_ptr->height = height;

   png_ptr->pixel_depth = (png_byte)(bit_depth * png_ptr->channels);
   png_ptr->rowbytes = ((width * ((png_size_t)png_ptr->pixel_depth + 7)) >> 3);	/* D3DX: width * (pixel_depth + 7) */
   /* set the usr info, so any transformations can modify it */
   png_ptr->usr_width = png_ptr->width;
   png_ptr->usr_bit_depth = png_ptr->bit_depth;
   png_ptr->usr_channels = png_ptr->channels;

   /* pack the header information into the buffer */
   pngwutil_save_uint_32(buf, width);
   pngwutil_save_uint_32(buf + 4, height);
   buf[8] = (png_byte)bit_depth;
   buf[9] = (png_byte)color_type;
   buf[10] = (png_byte)compression_type;
   buf[11] = (png_byte)filter_type;
   buf[12] = (png_byte)interlace_type;

   /* write the chunk */
   png_write_chunk(png_ptr, (png_bytep)png_IHDR, buf, (png_size_t)13);

   /* initialize zlib with PNG info */
   png_ptr->zstream.zalloc = png_zalloc;
   png_ptr->zstream.zfree = png_zfree;
   png_ptr->zstream.opaque = (voidpf)png_ptr;
   if (!(png_ptr->do_filter))
   {
      if (png_ptr->color_type == PNG_COLOR_TYPE_PALETTE ||
         png_ptr->bit_depth < 8)
         png_ptr->do_filter = PNG_FILTER_NONE;
      else
         png_ptr->do_filter = PNG_ALL_FILTERS;
   }
   if (!(png_ptr->flags & PNG_FLAG_ZLIB_CUSTOM_STRATEGY))
   {
      if (png_ptr->do_filter != PNG_FILTER_NONE)
         png_ptr->zlib_strategy = Z_FILTERED;
      else
         png_ptr->zlib_strategy = Z_DEFAULT_STRATEGY;
   }
   if (!(png_ptr->flags & PNG_FLAG_ZLIB_CUSTOM_LEVEL))
      png_ptr->zlib_level = Z_DEFAULT_COMPRESSION;
   if (!(png_ptr->flags & PNG_FLAG_ZLIB_CUSTOM_MEM_LEVEL))
      png_ptr->zlib_mem_level = 8;
   if (!(png_ptr->flags & PNG_FLAG_ZLIB_CUSTOM_WINDOW_BITS))
      png_ptr->zlib_window_bits = 15;
   if (!(png_ptr->flags & PNG_FLAG_ZLIB_CUSTOM_METHOD))
      png_ptr->zlib_method = 8;
   deflateInit2(&png_ptr->zstream, png_ptr->zlib_level,
      png_ptr->zlib_method, png_ptr->zlib_window_bits,
      png_ptr->zlib_mem_level, png_ptr->zlib_strategy);
   png_ptr->zstream.next_out = png_ptr->zbuf;
   png_ptr->zstream.avail_out = (uInt)png_ptr->zbuf_size;

   png_ptr->mode = PNG_HAVE_IHDR;
}


#undef PNG_USE_LOCAL_ARRAYS

#define FARDATA
#define PNG_EXPORT_VAR(type) extern PNG_IMPEXP type
#define png_debug(l,m) 
#define PNG_HAVE_IDAT 0x04
#define PNG_IDAT const png_byte png_IDAT[5] = { 73, 68, 65, 84, '\0'}
/* png_IDAT, by its address */

/* write an IDAT chunk */
// @address 0x5d8a90 ?png_write_IDAT@D3DX@@YAXPAUpng_struct_def@1@PAEI@Z
void /* PRIVATE */
png_write_IDAT(png_structp png_ptr, png_bytep data, png_size_t length)
{
#ifdef PNG_USE_LOCAL_ARRAYS
   PNG_IDAT;
#endif
   png_debug(1, "in png_write_IDAT\n");
   png_write_chunk(png_ptr, (png_bytep)png_IDAT, data, length);
   png_ptr->mode |= PNG_HAVE_IDAT;
}

#define PNG_HAVE_IEND 0x10
#define PNG_IEND const png_byte png_IEND[5] = { 73, 69, 78, 68, '\0'}
/* png_IEND, by its address */

static void 
pngwutil_write_chunk_iend(png_structp png_ptr, png_bytep chunk_name,
   png_bytep data, png_size_t length)
{
   png_write_chunk_start(png_ptr, chunk_name, (png_uint_32)length);
   pngwutil_write_chunk_data(png_ptr, data, length);
   png_write_chunk_end(png_ptr);
}

/* write an IEND chunk */
// @address 0x5d8ae0 ?png_write_IEND@D3DX@@YAXPAUpng_struct_def@1@@Z
void /* PRIVATE */
png_write_IEND(png_structp png_ptr)
{
#ifdef PNG_USE_LOCAL_ARRAYS
   PNG_IEND;
#endif
   png_debug(1, "in png_write_IEND\n");
   pngwutil_write_chunk_iend(png_ptr, (png_bytep)png_IEND, NULL, (png_size_t)0);
   png_ptr->mode |= PNG_HAVE_IEND;
}

/* png_write_finish_row, by its address (not accepted yet) */

/* Do the actual writing of a previously filtered row. */
// @address 0x5d8d40 ?png_write_filtered_row@D3DX@@YAXPAUpng_struct_def@1@PAE@Z
void /* PRIVATE */
png_write_filtered_row(png_structp png_ptr, png_bytep filtered_row)
{
   png_debug(1, "in png_write_filtered_row\n");
   png_debug1(2, "filter = %d\n", filtered_row[0]);
   /* set up the zlib input buffer */
   png_ptr->zstream.next_in = filtered_row;
   png_ptr->zstream.avail_in = (uInt)png_ptr->row_info.rowbytes + 1;
   /* repeat until we have compressed all the data */
   do
   {
      int ret; /* return of zlib */

      /* compress the data */
      ret = deflate(&png_ptr->zstream, Z_NO_FLUSH);
      /* check for compression errors */
      if (ret != Z_OK)
      {
         if (png_ptr->zstream.msg != NULL)
            png_error(png_ptr, png_ptr->zstream.msg);
         else
            png_error(png_ptr, "zlib error");
      }

      /* see if it is time to write another IDAT */
      if (!(png_ptr->zstream.avail_out))
      {
         /* write the IDAT and reset the zlib output buffer */
         png_write_IDAT(png_ptr, png_ptr->zbuf, png_ptr->zbuf_size);
         png_ptr->zstream.next_out = png_ptr->zbuf;
         png_ptr->zstream.avail_out = (uInt)png_ptr->zbuf_size;
      }
   /* repeat until all data has been compressed */
   } while (png_ptr->zstream.avail_in);

   /* swap the current and previous rows */
   if (png_ptr->prev_row != NULL)
   {
      png_bytep tptr;

      tptr = png_ptr->prev_row;
      png_ptr->prev_row = png_ptr->row_buf;
      png_ptr->row_buf = tptr;
   }

   /* finish row - updates counters and flushes zlib if last row */
   png_write_finish_row(png_ptr);

#if defined(PNG_WRITE_FLUSH_SUPPORTED)
   png_ptr->flush_rows++;

   if (png_ptr->flush_dist > 0 &&
       png_ptr->flush_rows >= png_ptr->flush_dist)
   {
      png_write_flush(png_ptr);
   }
#endif
}

}  // namespace D3DX

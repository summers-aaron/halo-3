#ifndef D3DX9_PNG_H
#define D3DX9_PNG_H

#include <setjmp.h>
#include <stddef.h>
#include "d3dx9/deflate.h"

// The D3DX PNG configuration ends the PNG state at int_gamma.
namespace D3DX {
enum { PNG_MAX_BIT_DEPTH = 16 };

typedef unsigned long png_uint_32;
typedef long png_int_32;
typedef unsigned short png_uint_16;
typedef unsigned char png_byte;
   typedef size_t png_size_t;
typedef png_int_32 png_fixed_point;
typedef void             * png_voidp;
typedef png_byte         * png_bytep;
typedef png_uint_16      * png_uint_16p;
typedef const char   * png_const_charp;
typedef png_uint_16      *  * png_uint_16pp;
typedef struct png_color_struct
{
   png_byte red;
   png_byte green;
   png_byte blue;
} png_color;
typedef png_color  * png_colorp;
typedef struct png_color_16_struct
{
   png_byte index;    /* used for palette files */
   png_uint_16 red;   /* for use in red green blue files */
   png_uint_16 green;
   png_uint_16 blue;
   png_uint_16 gray;  /* for use in grayscale files */
} png_color_16;
typedef struct png_color_8_struct
{
   png_byte red;   /* for use in red green blue files */
   png_byte green;
   png_byte blue;
   png_byte gray;  /* for use in grayscale files */
   png_byte alpha; /* for alpha channel files */
} png_color_8;
typedef struct png_row_info_struct
{
   png_uint_32 width; /* width of row */
   png_uint_32 rowbytes; /* number of bytes in row */
   png_byte color_type; /* color type of row */
   png_byte bit_depth; /* bit depth of row */
   png_byte channels; /* number of channels (1, 2, 3, or 4) */
   png_byte pixel_depth; /* bits per pixel (depth * channels) */
} png_row_info;
typedef struct png_struct_def png_struct;
typedef png_struct  * png_structp;
typedef void ( *png_error_ptr) (png_structp, png_const_charp);
typedef void ( *png_rw_ptr) (png_structp, png_bytep, png_size_t);
typedef void ( *png_flush_ptr) (png_structp);
typedef void ( *png_read_status_ptr) (png_structp, png_uint_32,
   int);
typedef void ( *png_write_status_ptr) (png_structp, png_uint_32,
   int);
struct png_struct_def
{
   jmp_buf jmpbuf;
   png_error_ptr error_fn;
   png_error_ptr warning_fn;
   png_voidp error_ptr;
   png_rw_ptr write_data_fn;
   png_rw_ptr read_data_fn;
   png_voidp io_ptr;
   png_uint_32 mode;
   png_uint_32 flags;
   png_uint_32 transformations;
   z_stream zstream;
   png_bytep zbuf;
   png_size_t zbuf_size;
   int zlib_level;
   int zlib_method;
   int zlib_window_bits;
   int zlib_mem_level;
   int zlib_strategy;
   png_uint_32 width;
   png_uint_32 height;
   png_uint_32 num_rows;
   png_uint_32 usr_width;
   png_uint_32 rowbytes;
   png_uint_32 irowbytes;
   png_uint_32 iwidth;
   png_uint_32 row_number;
   png_bytep prev_row;
   png_bytep row_buf;
   png_bytep sub_row;
   png_bytep up_row;
   png_bytep avg_row;
   png_bytep paeth_row;
   png_row_info row_info;
   png_uint_32 idat_size;
   png_uint_32 crc;
   png_colorp palette;
   png_uint_16 num_palette;
   png_uint_16 num_trans;
   png_byte chunk_name[5];
   png_byte compression;
   png_byte filter;
   png_byte interlaced;
   png_byte pass;
   png_byte do_filter;
   png_byte color_type;
   png_byte bit_depth;
   png_byte usr_bit_depth;
   png_byte pixel_depth;
   png_byte channels;
   png_byte usr_channels;
   png_byte sig_bytes;
   png_uint_16 filler;
   png_flush_ptr output_flush_fn;
   png_uint_32 flush_dist;
   png_uint_32 flush_rows;
   int gamma_shift;
   float gamma;
   float screen_gamma;
   png_bytep gamma_table;
   png_bytep gamma_from_1;
   png_bytep gamma_to_1;
   png_uint_16pp gamma_16_table;
   png_uint_16pp gamma_16_from_1;
   png_uint_16pp gamma_16_to_1;
   png_color_8 sig_bit;
   png_color_8 shift;
   png_bytep trans;
   png_color_16 trans_values;
   png_read_status_ptr read_row_fn;
   png_write_status_ptr write_row_fn;
   png_bytep palette_lookup;
   png_bytep dither_index;
   png_uint_16p hist;
   png_byte heuristic_method;
   png_byte num_prev_filters;
   png_bytep prev_filters;
   png_uint_16p filter_weights;
   png_uint_16p inv_filter_weights;
   png_uint_16p filter_costs;
   png_uint_16p inv_filter_costs;
   png_byte empty_plte_permitted;
   png_uint_32 mng_features_permitted;
   png_fixed_point int_gamma;
};

void png_write_chunk_start(png_structp png_ptr, png_bytep chunk_name, png_uint_32 length);
void png_write_chunk_end(png_structp png_ptr);
void png_reset_crc(png_structp png_ptr);
void png_write_data(png_structp png_ptr, png_bytep data, png_size_t length);
void png_calculate_crc(png_structp png_ptr, png_bytep data, png_size_t length);
void png_write_sig(png_structp png_ptr);
void png_error(png_structp png_ptr, png_const_charp error);
void png_warning(png_structp png_ptr, png_const_charp message);
png_voidp png_malloc(png_structp png_ptr, png_uint_32 size);
voidpf png_zalloc(voidpf png_ptr, uInt items, uInt size);
void png_zfree(voidpf png_ptr, voidpf pointer);
void png_write_IDAT(png_structp png_ptr, png_bytep data, png_size_t length);
void png_write_IEND(png_structp png_ptr);
void png_write_flush(png_structp png_ptr);
void png_write_finish_row(png_structp png_ptr);

extern "C" {
extern png_byte png_signature[8];
extern const png_byte png_PLTE[5];
extern const png_byte png_IHDR[5];
extern const png_byte png_IDAT[5];
extern const png_byte png_IEND[5];
extern const int k_png_pass_tables[28];
}
}

#endif // D3DX9_PNG_H

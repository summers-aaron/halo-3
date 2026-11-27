#ifndef D3DX9_JPEG_TYPES_H
#define D3DX9_JPEG_TYPES_H

#include <stddef.h>

// Compressor layouts used by the XDK D3DX JPEG implementation (libjpeg 6b).
// D3DX adds two booleans after write_Adobe_marker.
namespace D3DX {
#define METHODDEF(type) static type
#define LOCAL(type) static type
typedef unsigned char JOCTET;
typedef unsigned char JSAMPLE;
typedef short JCOEF;
typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int JDIMENSION;
#define GLOBAL(type) type
#define EXTERN(type) extern type
#define JMETHOD(type,methodname,arglist) type (*methodname) arglist
#define FAR 
typedef int boolean;
#define FALSE 0
#define C_PROGRESSIVE_SUPPORTED 
#define DCTSIZE2 64
#define NUM_QUANT_TBLS 4
#define NUM_HUFF_TBLS 4
#define NUM_ARITH_TBLS 16
#define MAX_COMPS_IN_SCAN 4
#define C_MAX_BLOCKS_IN_MCU 10
typedef JSAMPLE FAR *JSAMPROW;	/* ptr to one image row of pixel samples. */
typedef JSAMPROW *JSAMPARRAY;	/* ptr to some rows (a 2-D sample array) */
typedef JCOEF JBLOCK[DCTSIZE2];	/* one block of coefficients */
typedef JBLOCK FAR *JBLOCKROW;	/* pointer to one row of coefficient blocks */
typedef JBLOCKROW *JBLOCKARRAY;		/* a 2-D array of coefficient blocks */
typedef struct {
  UINT16 quantval[DCTSIZE2];	/* quantization step for each coefficient */
  boolean sent_table;		/* TRUE when table has been output */
} JQUANT_TBL;
typedef struct {
  UINT8 bits[17];		/* bits[k] = # of symbols with codes of */
  UINT8 huffval[256];		/* The symbols, in order of incr code length */
  boolean sent_table;		/* TRUE when table has been output */
} JHUFF_TBL;
typedef struct {
  int component_id;		/* identifier for this component (0..255) */
  int component_index;		/* its index in SOF or cinfo->comp_info[] */
  int h_samp_factor;		/* horizontal sampling factor (1..4) */
  int v_samp_factor;		/* vertical sampling factor (1..4) */
  int quant_tbl_no;		/* quantization table selector (0..3) */
  int dc_tbl_no;		/* DC entropy table selector (0..3) */
  int ac_tbl_no;		/* AC entropy table selector (0..3) */
  JDIMENSION width_in_blocks;
  JDIMENSION height_in_blocks;
  int DCT_scaled_size;
  JDIMENSION downsampled_width;	 /* actual width in samples */
  JDIMENSION downsampled_height; /* actual height in samples */
  boolean component_needed;	/* do we need the value of this component? */
  int MCU_width;		/* number of blocks per MCU, horizontally */
  int MCU_height;		/* number of blocks per MCU, vertically */
  int MCU_blocks;		/* MCU_width * MCU_height */
  int MCU_sample_width;		/* MCU width in samples, MCU_width*DCT_scaled_size */
  int last_col_width;		/* # of non-dummy blocks across in last MCU */
  int last_row_height;		/* # of non-dummy blocks down in last MCU */
  JQUANT_TBL * quant_table;
  void * dct_table;
} jpeg_component_info;
typedef struct {
  int comps_in_scan;		/* number of components encoded in this scan */
  int component_index[MAX_COMPS_IN_SCAN]; /* their SOF/comp_info[] indexes */
  int Ss, Se;			/* progressive JPEG spectral selection parms */
  int Ah, Al;			/* progressive JPEG successive approx. parms */
} jpeg_scan_info;
typedef enum {
    JCS_UNKNOWN,		/* error/unspecified */
    JCS_GRAYSCALE,		/* monochrome */
    JCS_RGB,		/* red/green/blue */
    JCS_YCbCr,		/* Y/Cb/Cr (also known as YUV) */
    JCS_CMYK,		/* C/M/Y/K */
    JCS_YCCK		/* Y/Cb/Cr/K */
} J_COLOR_SPACE;
typedef enum {
    JDCT_ISLOW,		/* slow but accurate integer algorithm */
    JDCT_IFAST,		/* faster, less accurate integer method */
    JDCT_FLOAT		/* floating-point: accurate, fast on fast HW */
} J_DCT_METHOD;
#define jpeg_common_fields struct jpeg_error_mgr * err; struct jpeg_memory_mgr * mem; \
	struct jpeg_progress_mgr * progress; void * client_data; boolean is_decompressor; int global_state
struct jpeg_common_struct {
  jpeg_common_fields;		/* Fields common to both master struct types */
};
typedef struct jpeg_common_struct * j_common_ptr;
typedef struct jpeg_compress_struct * j_compress_ptr;
struct jpeg_compress_struct {
  jpeg_common_fields;		/* Fields shared with jpeg_decompress_struct */
  struct jpeg_destination_mgr * dest;
  JDIMENSION image_width;	/* input image width */
  JDIMENSION image_height;	/* input image height */
  int input_components;		/* # of color components in input image */
  J_COLOR_SPACE in_color_space;	/* colorspace of input image */
  double input_gamma;		/* image gamma of input image */
  int data_precision;		/* bits of precision in image data */
  int num_components;		/* # of color components in JPEG image */
  J_COLOR_SPACE jpeg_color_space; /* colorspace of JPEG image */
  jpeg_component_info * comp_info;
  JQUANT_TBL * quant_tbl_ptrs[NUM_QUANT_TBLS];
  JHUFF_TBL * dc_huff_tbl_ptrs[NUM_HUFF_TBLS];
  JHUFF_TBL * ac_huff_tbl_ptrs[NUM_HUFF_TBLS];
  UINT8 arith_dc_L[NUM_ARITH_TBLS]; /* L values for DC arith-coding tables */
  UINT8 arith_dc_U[NUM_ARITH_TBLS]; /* U values for DC arith-coding tables */
  UINT8 arith_ac_K[NUM_ARITH_TBLS]; /* Kx values for AC arith-coding tables */
  int num_scans;		/* # of entries in scan_info array */
  const jpeg_scan_info * scan_info; /* script for multi-scan file, or NULL */
  boolean raw_data_in;		/* TRUE=caller supplies downsampled data */
  boolean arith_code;		/* TRUE=arithmetic coding, FALSE=Huffman */
  boolean optimize_coding;	/* TRUE=optimize entropy encoding parms */
  boolean CCIR601_sampling;	/* TRUE=first samples are cosited */
  int smoothing_factor;		/* 1..100, or 0 for no input smoothing */
  J_DCT_METHOD dct_method;	/* DCT algorithm selector */
  unsigned int restart_interval; /* MCUs per restart, or 0 for no restart */
  int restart_in_rows;		/* if > 0, MCU rows per restart interval */
  boolean write_JFIF_header;	/* should a JFIF marker be written? */
  UINT8 JFIF_major_version;	/* What to write for the JFIF version number */
  UINT8 JFIF_minor_version;
  UINT8 density_unit;		/* JFIF code for pixel size units */
  UINT16 X_density;		/* Horizontal pixel density */
  UINT16 Y_density;		/* Vertical pixel density */
  boolean write_Adobe_marker;	/* should an Adobe marker be written? */
  boolean d3dx_unknown1;	/* D3DX: two booleans IJG's struct does not have; */
  boolean d3dx_unknown2;	/* jpeg_set_colorspace sets both TRUE */
  JDIMENSION next_scanline;	/* 0 .. image_height-1  */
  boolean progressive_mode;	/* TRUE if scan script uses progressive mode */
  int max_h_samp_factor;	/* largest h_samp_factor */
  int max_v_samp_factor;	/* largest v_samp_factor */
  JDIMENSION total_iMCU_rows;	/* # of iMCU rows to be input to coef ctlr */
  int comps_in_scan;		/* # of JPEG components in this scan */
  jpeg_component_info * cur_comp_info[MAX_COMPS_IN_SCAN];
  JDIMENSION MCUs_per_row;	/* # of MCUs across the image */
  JDIMENSION MCU_rows_in_scan;	/* # of MCU rows in the image */
  int blocks_in_MCU;		/* # of DCT blocks per MCU */
  int MCU_membership[C_MAX_BLOCKS_IN_MCU];
  int Ss, Se, Ah, Al;		/* progressive JPEG parameters for scan */
  struct jpeg_comp_master * master;
  struct jpeg_c_main_controller * main;
  struct jpeg_c_prep_controller * prep;
  struct jpeg_c_coef_controller * coef;
  struct jpeg_marker_writer * marker;
  struct jpeg_color_converter * cconvert;
  struct jpeg_downsampler * downsample;
  struct jpeg_forward_dct * fdct;
  struct jpeg_entropy_encoder * entropy;
  jpeg_scan_info * script_space; /* workspace for jpeg_simple_progression */
  int script_space_size;
};
struct jpeg_error_mgr {
  void (*error_exit) (j_common_ptr cinfo);
  void (*emit_message) (j_common_ptr cinfo, int msg_level);
  void (*output_message) (j_common_ptr cinfo);
  void (*format_message) (j_common_ptr cinfo, char * buffer);
  void (*reset_error_mgr) (j_common_ptr cinfo);
  int msg_code;
  union {
    int i[8];
    char s[80];
  } msg_parm;
  int trace_level;
  long num_warnings;
  const char * const * jpeg_message_table;
  int last_jpeg_message;
  const char * const * addon_message_table;
  int first_addon_message;
  int last_addon_message;
};
typedef struct jvirt_sarray_control * jvirt_sarray_ptr;
typedef struct jvirt_barray_control * jvirt_barray_ptr;
struct jpeg_memory_mgr {
  JMETHOD(void *, alloc_small, (j_common_ptr cinfo, int pool_id,
                size_t sizeofobject));
  JMETHOD(void FAR *, alloc_large, (j_common_ptr cinfo, int pool_id,
                     size_t sizeofobject));
  JMETHOD(JSAMPARRAY, alloc_sarray, (j_common_ptr cinfo, int pool_id,
                     JDIMENSION samplesperrow,
                     JDIMENSION numrows));
  JMETHOD(JBLOCKARRAY, alloc_barray, (j_common_ptr cinfo, int pool_id,
                      JDIMENSION blocksperrow,
                      JDIMENSION numrows));
  JMETHOD(jvirt_sarray_ptr, request_virt_sarray, (j_common_ptr cinfo,
                          int pool_id,
                          boolean pre_zero,
                          JDIMENSION samplesperrow,
                          JDIMENSION numrows,
                          JDIMENSION maxaccess));
  JMETHOD(jvirt_barray_ptr, request_virt_barray, (j_common_ptr cinfo,
                          int pool_id,
                          boolean pre_zero,
                          JDIMENSION blocksperrow,
                          JDIMENSION numrows,
                          JDIMENSION maxaccess));
  JMETHOD(void, realize_virt_arrays, (j_common_ptr cinfo));
  JMETHOD(JSAMPARRAY, access_virt_sarray, (j_common_ptr cinfo,
                       jvirt_sarray_ptr ptr,
                       JDIMENSION start_row,
                       JDIMENSION num_rows,
                       boolean writable));
  JMETHOD(JBLOCKARRAY, access_virt_barray, (j_common_ptr cinfo,
                        jvirt_barray_ptr ptr,
                        JDIMENSION start_row,
                        JDIMENSION num_rows,
                        boolean writable));
  JMETHOD(void, free_pool, (j_common_ptr cinfo, int pool_id));
  JMETHOD(void, self_destruct, (j_common_ptr cinfo));
  long max_memory_to_use;
  long max_alloc_chunk;
};
#define JPP(arglist) arglist
struct jpeg_marker_writer {
  JMETHOD(void, write_file_header, (j_compress_ptr cinfo));
  JMETHOD(void, write_frame_header, (j_compress_ptr cinfo));
  JMETHOD(void, write_scan_header, (j_compress_ptr cinfo));
  JMETHOD(void, write_file_trailer, (j_compress_ptr cinfo));
  JMETHOD(void, write_tables_only, (j_compress_ptr cinfo));
  JMETHOD(void, write_marker_header, (j_compress_ptr cinfo, int marker,
                      unsigned int datalen));
  JMETHOD(void, write_marker_byte, (j_compress_ptr cinfo, int val));
};
struct jpeg_destination_mgr {
  JOCTET * next_output_byte;	/* => next byte to write in buffer */
  size_t free_in_buffer;	/* # of byte spaces remaining in buffer */
  JMETHOD(void, init_destination, (j_compress_ptr cinfo));
  JMETHOD(boolean, empty_output_buffer, (j_compress_ptr cinfo));
  JMETHOD(void, term_destination, (j_compress_ptr cinfo));
};
typedef enum {
JMSG_NOMESSAGE ,
JERR_ARITH_NOTIMPL ,
JERR_BAD_ALIGN_TYPE ,
JERR_BAD_ALLOC_CHUNK ,
JERR_BAD_BUFFER_MODE ,
JERR_BAD_COMPONENT_ID ,
JERR_BAD_DCT_COEF ,
JERR_BAD_DCTSIZE ,
JERR_BAD_HUFF_TABLE ,
JERR_BAD_IN_COLORSPACE ,
JERR_BAD_J_COLORSPACE ,
JERR_BAD_LENGTH ,
JERR_BAD_LIB_VERSION ,
JERR_BAD_MCU_SIZE ,
JERR_BAD_POOL_ID ,
JERR_BAD_PRECISION ,
JERR_BAD_PROGRESSION ,
JERR_BAD_PROG_SCRIPT ,
JERR_BAD_SAMPLING ,
JERR_BAD_SCAN_SCRIPT ,
JERR_BAD_STATE ,
JERR_BAD_STRUCT_SIZE ,
JERR_BAD_VIRTUAL_ACCESS ,
JERR_BUFFER_SIZE ,
JERR_CANT_SUSPEND ,
JERR_CCIR601_NOTIMPL ,
JERR_COMPONENT_COUNT ,
JERR_CONVERSION_NOTIMPL ,
JERR_DAC_INDEX ,
JERR_DAC_VALUE ,
JERR_DHT_INDEX ,
JERR_DQT_INDEX ,
JERR_EMPTY_IMAGE ,
JERR_EMS_READ ,
JERR_EMS_WRITE ,
JERR_EOI_EXPECTED ,
JERR_FILE_READ ,
JERR_FILE_WRITE ,
JERR_FRACT_SAMPLE_NOTIMPL ,
JERR_HUFF_CLEN_OVERFLOW ,
JERR_HUFF_MISSING_CODE ,
JERR_IMAGE_TOO_BIG ,
JERR_INPUT_EMPTY ,
JERR_INPUT_EOF ,
JERR_MISMATCHED_QUANT_TABLE ,
JERR_MISSING_DATA ,
JERR_MODE_CHANGE ,
JERR_NOTIMPL ,
JERR_NOT_COMPILED ,
JERR_NO_BACKING_STORE ,
JERR_NO_HUFF_TABLE ,
JERR_NO_IMAGE ,
JERR_NO_QUANT_TABLE ,
JERR_NO_SOI ,
JERR_OUT_OF_MEMORY ,
JERR_QUANT_COMPONENTS ,
JERR_QUANT_FEW_COLORS ,
JERR_QUANT_MANY_COLORS ,
JERR_SOF_DUPLICATE ,
JERR_SOF_NO_SOS ,
JERR_SOF_UNSUPPORTED ,
JERR_SOI_DUPLICATE ,
JERR_SOS_NO_SOF ,
JERR_TFILE_CREATE ,
JERR_TFILE_READ ,
JERR_TFILE_SEEK ,
JERR_TFILE_WRITE ,
JERR_TOO_LITTLE_DATA ,
JERR_UNKNOWN_MARKER ,
JERR_VIRTUAL_BUG ,
JERR_WIDTH_OVERFLOW ,
JERR_XMS_READ ,
JERR_XMS_WRITE ,
JMSG_COPYRIGHT ,
JMSG_VERSION ,
JTRC_16BIT_TABLES ,
JTRC_ADOBE ,
JTRC_APP0 ,
JTRC_APP14 ,
JTRC_DAC ,
JTRC_DHT ,
JTRC_DQT ,
JTRC_DRI ,
JTRC_EMS_CLOSE ,
JTRC_EMS_OPEN ,
JTRC_EOI ,
JTRC_HUFFBITS ,
JTRC_JFIF ,
JTRC_JFIF_BADTHUMBNAILSIZE ,
JTRC_JFIF_EXTENSION ,
JTRC_JFIF_THUMBNAIL ,
JTRC_MISC_MARKER ,
JTRC_PARMLESS_MARKER ,
JTRC_QUANTVALS ,
JTRC_QUANT_3_NCOLORS ,
JTRC_QUANT_NCOLORS ,
JTRC_QUANT_SELECTED ,
JTRC_RECOVERY_ACTION ,
JTRC_RST ,
JTRC_SMOOTH_NOTIMPL ,
JTRC_SOF ,
JTRC_SOF_COMPONENT ,
JTRC_SOI ,
JTRC_SOS ,
JTRC_SOS_COMPONENT ,
JTRC_SOS_PARAMS ,
JTRC_TFILE_CLOSE ,
JTRC_TFILE_OPEN ,
JTRC_THUMB_JPEG ,
JTRC_THUMB_PALETTE ,
JTRC_THUMB_RGB ,
JTRC_UNKNOWN_IDS ,
JTRC_XMS_CLOSE ,
JTRC_XMS_OPEN ,
JWRN_ADOBE_XFORM ,
JWRN_BOGUS_PROGRESSION ,
JWRN_EXTRANEOUS_DATA ,
JWRN_HIT_MARKER ,
JWRN_HUFF_BAD_CODE ,
JWRN_JFIF_MAJOR ,
JWRN_JPEG_EOF ,
JWRN_MUST_RESYNC ,
JWRN_NOT_SEQUENTIAL ,
JWRN_TOO_MUCH_DATA ,
  JMSG_LASTMSGCODE
} J_MESSAGE_CODE;
}

#endif // D3DX9_JPEG_TYPES_H

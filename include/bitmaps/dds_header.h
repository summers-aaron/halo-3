#ifndef BITMAPS_DDS_HEADER_H
#define BITMAPS_DDS_HEADER_H

// DDS file header, following the four-byte magic; reserved words are required by the format.
struct s_dds_header {
    unsigned long size;
    unsigned long flags;
    unsigned long height;
    unsigned long width;
    unsigned long pitch_or_linear_size;
    unsigned long depth;
    unsigned long mip_map_count;
    unsigned long dwReserved1[11];
    unsigned long pixel_format_size;
    unsigned long pixel_format_flags;
    unsigned long pixel_format_four_cc;
    unsigned long pixel_format_bit_count;
    unsigned long red_mask;
    unsigned long green_mask;
    unsigned long blue_mask;
    unsigned long alpha_mask;
    unsigned long caps;
    unsigned long caps2;
    unsigned long caps3;
    unsigned long caps4;
    unsigned long dwReserved2;
};
typedef char dds_header_size[(sizeof(s_dds_header) == 124) ? 1 : -1];

#endif

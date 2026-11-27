#ifndef RASTERIZER_XENON_RASTERIZER_XENON_MEMORY_REQUEST_TYPES_H
#define RASTERIZER_XENON_RASTERIZER_XENON_MEMORY_REQUEST_TYPES_H

struct s_rasterizer_xenon_memory_request
{
    unsigned long data_low;
    unsigned long data_high;
    long pad; // unreferenced; only the whole-array memset touches offset 8, and March has no class records
    // the console allocates these from the most significant bit (type in
    // bits 31..26, shared_type 25..23: render_method_submit.cpp reads it as (flags >> 23) & 7 through
    // s_rasterizer_entry); x86 allocates from the least, so clang declares them in reverse.
#if defined(_XBOX) && !defined(__clang__)
    unsigned long type : 6;
    unsigned long shared_type : 3;
    unsigned long unknown_flag0 : 1;
    unsigned long unknown_flag1 : 1;
    unsigned long stage : 13;
    unsigned long unknown_bits : 8;
#else
    unsigned long unknown_bits : 8;
    unsigned long stage : 13;
    unsigned long unknown_flag1 : 1;
    unsigned long unknown_flag0 : 1;
    unsigned long shared_type : 3;
    unsigned long type : 6;
#endif
    unsigned short first_entry;
    unsigned short entry_count;
};

#endif // RASTERIZER_XENON_RASTERIZER_XENON_MEMORY_REQUEST_TYPES_H

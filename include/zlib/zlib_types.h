// zlib 1.1.4 basic types (zconf.h) and stream structure (zlib.h).
//
// No include guard: the game links two zlib instances built from this one source. zlib/zlib.h includes it at
// global scope (the zlibxbox units, C linkage) and d3dx9/zlib.h includes it inside namespace D3DX, so the D3DX
// functions take D3DX::z_stream_s. Each includer guards itself.

typedef unsigned char Byte;
typedef unsigned int uInt;
#if defined(__LP64__)
// Keep the Xbox/Windows stream layout in native LP64 builds.
typedef unsigned int uLong;
#else
typedef unsigned long uLong;
#endif
typedef Byte Bytef;
typedef char charf;
typedef int intf;
typedef uInt uIntf;
typedef uLong uLongf;
typedef void *voidpf;
typedef void *voidp;

typedef voidpf (*alloc_func)(voidpf opaque, uInt items, uInt size);
typedef void (*free_func)(voidpf opaque, voidpf address);

struct internal_state;

typedef struct z_stream_s {
    Bytef *next_in;
    uInt avail_in;
    uLong total_in;
    Bytef *next_out;
    uInt avail_out;
    uLong total_out;
    char *msg;
    struct internal_state *state;
    alloc_func zalloc;
    free_func zfree;
    voidpf opaque;
    int data_type;
    uLong adler;
    uLong reserved;
} z_stream;
typedef char z_stream_size_check[(sizeof(void *) != 4 || ((sizeof(z_stream) == 0x38) )) ? 1 : -1];

typedef z_stream *z_streamp;

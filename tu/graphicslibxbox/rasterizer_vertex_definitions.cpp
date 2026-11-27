// graphicslibxbox/rasterizer_vertex_definitions.obj: from the records 0x46af10
// @flags /TP /O2
// @address 0x46af10 ?rasterizer_vertex_get_stride_from_declaration@@YAEPBUrasterizer_vertex_declaration@@@Z

/* The byte size of a vertex declaration: the sum of its register mappings' sizes up to the terminator (-1); a
   mapping with attribute -2 is padding whose size is its format byte. */

#include "rasterizer/rasterizer_vertex_definitions.h"

// @address 0x24a0c k_vertex_semantic_usages
// @rdata 0x82024a0c..0x82024a44
const unsigned long k_vertex_semantic_usages[14] = { 0, 2, 1, 5, 3, 7, 6, 5, 10, 10, 0, 3, 2, 1 };
// @rdata-end

// @address 0x730580 g_rasterizer_vertex_buffer_declarations
rasterizer_vertex_declaration g_rasterizer_vertex_buffer_declarations[30] = {
    { 0, { { -1 } } },
    { 1, { { 0, 2, 0, 0 }, { 3, 1, 5, 0 }, { 4, 14, 3, 0 }, { 6, 14, 6, 0 }, { 5, 14, 7, 0 }, { -1 } } },
    { 2, { { 0, 12, 0, 0 }, { 3, 11, 5, 0 }, { 4, 14, 3, 0 }, { 6, 14, 6, 0 }, { 5, 14, 7, 0 }, { -1 } } },
    { 3, { { 0, 12, 0, 0 }, { 3, 11, 5, 0 }, { 4, 14, 3, 0 }, { 6, 14, 6, 0 }, { 5, 14, 7, 0 }, { 1, 4, 2, 0 }, { 2, 5, 1, 0 }, { -1 } } },
    { 4, { { 7, 11, 5, 1 }, { -1 } } },
    { 5, { { 8, 6, 10, 0 }, { -1 } } },
    { 6, { { 13, 6, 5, 3 }, { 13, 6, 5, 4 }, { 13, 6, 5, 5 }, { 13, 6, 5, 6 }, { 13, 6, 5, 7 }, { -1 } } },
    { 7, { { 12, 0, 11, 0 }, { 13, 3, 5, 2 }, { -1 } } },
    { 8, { { -1 } } },
    { 9, { { 13, 5, 1, 1 }, { -1 } } },
    { 10, { { 13, 5, 1, 1 }, { -1 } } },
    { 11, { { 13, 17, 1, 1 }, { 13, 17, 1, 2 }, { 13, 17, 1, 3 }, { -1 } } },
    { 12, { { 0, 1, 0, 0 }, { 3, 1, 5, 0 }, { 8, 6, 10, 0 }, { -1 } } },
    { 13, { { 0, 2, 0, 0 }, { 8, 6, 10, 0 }, { -1 } } },
    { 14, { { 9, 2, 5, 2 }, { -1 } } },
    { 15, { { 0, 2, 0, 0 }, { 3, 1, 5, 0 }, { 8, 6, 10, 0 }, { -1 } } },
    { 0 },
    { 0 },
    { 0 },
    { 0 },
    { 20, { { 0, 12, 0, 1 }, { 0, 4, 3, 1 }, { 8, 6, 10, 1 }, { -1 } } },
    { 21, { { 0, 7, 0, 2 }, { -1 } } },
    { 22, { { 0, 7, 0, 2 }, { -1 } } },
    { 23, { { 0, 7, 0, 2 }, { -1 } } },
    { 24, { { 0, 12, 0, 0 }, { 3, 11, 5, 0 }, { 4, 14, 3, 0 }, { -1 } } },
    { 25, { { 0, 12, 0, 0 }, { 3, 11, 5, 0 }, { 4, 14, 3, 0 }, { -1 } } },
    { 26, { { 0, 7, 0, 0 }, { 0, 7, 0, 0 }, { 0, 7, 0, 0 }, { -1 } } },
    { 27, { { 0, 2, 0, 0 }, { 0, 2, 0, 0 }, { 0, 2, 0, 0 }, { -1 } } },
    { 28, { { 0, 3, 0, 0 }, { 0, 3, 0, 0 }, { 0, 3, 0, 0 }, { 0, 3, 0, 0 }, { 0, 3, 0, 0 }, { 0, 3, 0, 0 }, { 0, 3, 0, 0 }, { 0, 3, 0, 0 }, { -1 } } },
    { 0 },
};

static __forceinline unsigned char vertex_type_size(signed char type)
{
  unsigned char size;
  switch (type) {
  case 2:
    size = 12;
    break;
  case 3:
    size = 16;
    break;
  case 0:
  case 4:
  case 5:
  case 6:
  case 7:
  case 9:
  case _rasterizer_vertex_format_ushort2_normalized:
  case _rasterizer_vertex_format_udec3:
  case _rasterizer_vertex_format_dhenn3:
  case _rasterizer_vertex_format_half2:
  case _rasterizer_vertex_format_dec3n:
    size = 4;
    break;
  case 1:
  case 8:
  case _rasterizer_vertex_format_short4_normalized:
  case _rasterizer_vertex_format_ushort4_normalized:
  case _rasterizer_vertex_format_half4:
    size = 8;
    break;
  default:
    // match: 0x46af10: unreachable hint omits dead branch check
    __assume(0);
  }
  return size;
}

unsigned char rasterizer_vertex_get_stride_from_declaration(const rasterizer_vertex_declaration *declaration)
{
  long total = 0;
  for (unsigned long i = 0; i < 13; i++) {
    const rasterizer_vertex_register_mapping *register_mapping = &declaration->register_mappings[i];
    long size;
    if (register_mapping->attribute >= 0)
      size = vertex_type_size(register_mapping->format);
    else if (register_mapping->attribute == -2)
      size = register_mapping->format;
    else
      size = 0;
    total += size;
    if (register_mapping->attribute == -1)
      break;
  }
  return (unsigned char)total;
}

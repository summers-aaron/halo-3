// graphicslibxbox/rasterizer_constants.obj: from the records 0x14e7f0
// @flags /TP /O2
// @address 0x14e7f0 ?set_vertex_shader_constant@c_rasterizer@@SAXJJPBTreal_vector4d@@@Z
#include <xtl.h>
#include "rasterizer/rasterizer.h"

void c_rasterizer::set_vertex_shader_constant(long start_register, long vector4f_count, const real_vector4d *constant_data)
{
  g_device->SetVertexShaderConstantF(((UINT)start_register), ((const float *)constant_data), ((DWORD)vector4f_count));
}

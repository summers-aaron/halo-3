// graphicslibxbox/rasterizer_resource_definitions.obj: from the records 0x14d200 0x45a380 0x45a4d8 0x45a6e8 0x45a828
// @flags /TP /O2 /Oc /fp:strict
// @address 0x14d200 rasterizer_get_vertex_declaration
// @address 0x45a380 ?concat_decl@c_vertex_declaration_table@@CAXQAU_D3DVERTEXELEMENT9@@QBU2@11@Z
// @address 0x45a4d8 ?initialize_vertex_declarations@c_vertex_declaration_table@@SA_NXZ
// @address 0x45a6e8 rasterizer_pack_vector_dhenn3
#define XM_NO_MISALIGNED_VECTOR_ACCESS
#include <string.h>
#include <xtl.h>
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_resource_definitions.h"
#include "rasterizer/rasterizer_d3d_allocations.h"
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "render_methods/render_method_definitions.h"

// @address 0x1f80 k_dhenn3_packing_constant
const XMVECTORI k_dhenn3_packing_constant = { 0x447FC000, 0x0003FF0A, 0x1FFFFF15, 0x43FF8000 };

// @address 0x4b994 k_vertex_declaration_terminator
// @rdata 0x8204b994..0x8204b9a0
const _D3DVERTEXELEMENT9 k_vertex_declaration_terminator = D3DDECL_END();
// @rdata-end

// @address 0x7316f8 ?base_vertex_decls@c_vertex_declaration_table@@2PAY0EA@U_D3DVERTEXELEMENT9@@A
_D3DVERTEXELEMENT9 c_vertex_declaration_table::base_vertex_decls[22][64] = {
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 20, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 24, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
        { 0, 28, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
        D3DDECL_END()
    },
    {
        { 0, 0, D3DDECLTYPE_USHORT4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 8, D3DDECLTYPE_USHORT2N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 12, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 16, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
        { 0, 20, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
        D3DDECL_END()
    },
    {
        { 0, 0, D3DDECLTYPE_USHORT4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 8, D3DDECLTYPE_USHORT2N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 12, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 16, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
        { 0, 20, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
        { 0, 24, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },
        { 0, 28, D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
        D3DDECL_END()
    },
    {
        { 0, 0, D3DDECLTYPE_USHORT4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 8, D3DDECLTYPE_USHORT2N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 12, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        D3DDECL_END()
    },
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 20, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 24, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
        { 0, 28, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
        D3DDECL_END()
    },
    {
        { 0, 0, D3DDECLTYPE_USHORT4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 8, D3DDECLTYPE_USHORT2N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 12, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 16, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
        { 0, 20, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
        D3DDECL_END()
    },
    {
        { 0, 0, D3DDECLTYPE_USHORT4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 8, D3DDECLTYPE_USHORT2N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 12, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 16, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
        { 0, 20, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
        { 0, 24, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },
        { 0, 28, D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
        D3DDECL_END()
    },
    {
        { 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        D3DDECL_END()
    },
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        D3DDECL_END()
    },
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        D3DDECL_END()
    },
    {
        D3DDECL_END()
    },
    {
        D3DDECL_END()
    },
    {
        D3DDECL_END()
    },
    {
        { 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    },
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        { 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    },
    {
        { 0, 0, D3DDECLTYPE_USHORT4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 8, D3DDECLTYPE_USHORT2N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 12, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 1, 0, D3DDECLTYPE_USHORT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1 },
        { 1, 8, D3DDECLTYPE_BYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 1 },
        { 1, 12, D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1 },
        { 2, 0, D3DDECLTYPE_USHORT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 2 },
        D3DDECL_END()
    },
    {
        { 0, 0, D3DDECLTYPE_SHORT4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        D3DDECL_END()
    },
    {
        { 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        D3DDECL_END()
    },
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 20, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 24, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
        { 0, 28, D3DDECLTYPE_DHEN3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
        { 4, 0, D3DDECLTYPE_USHORT2N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
        { 2, 0, D3DDECLTYPE_USHORT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        { 2, 4, D3DDECLTYPE_USHORT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1 },
        { 2, 8, D3DDECLTYPE_USHORT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 2 },
        { 3, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 1 },
        { 3, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 2 },
        { 3, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 3 },
        D3DDECL_END()
    },
    {
        { 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        { 0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
        { 0, 48, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2 },
        { 0, 64, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
        { 0, 80, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },
        { 0, 96, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        { 0, 112, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1 },
        D3DDECL_END()
    },
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END()
    },
    {
        D3DDECL_END()
    },
};

// @address 0x7358f8 ?lighting_vertex_decls@c_vertex_declaration_table@@2PAY0EA@U_D3DVERTEXELEMENT9@@A
_D3DVERTEXELEMENT9 c_vertex_declaration_table::lighting_vertex_decls[4][64] = {
    {
        D3DDECL_END()
    },
    {
        { 4, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
        D3DDECL_END()
    },
    {
        { 4, 0, D3DDECLTYPE_USHORT2N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
        D3DDECL_END()
    },
    {
        { 4, 0, D3DDECLTYPE_BYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 },
        { 4, 4, D3DDECLTYPE_BYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 4 },
        { 4, 8, D3DDECLTYPE_BYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 5 },
        { 4, 12, D3DDECLTYPE_BYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 6 },
        { 4, 16, D3DDECLTYPE_BYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 7 },
        D3DDECL_END()
    },
};

// @address 0x7364f8 ?transfer_vertex_decls@c_vertex_declaration_table@@2PAY0EA@U_D3DVERTEXELEMENT9@@A
_D3DVERTEXELEMENT9 c_vertex_declaration_table::transfer_vertex_decls[4][64] = {
    {
        D3DDECL_END()
    },
    {
        { 2, 0, D3DDECLTYPE_UBYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 1 },
        D3DDECL_END()
    },
    {
        { 2, 0, D3DDECLTYPE_BYTE4N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 1 },
        D3DDECL_END()
    },
    {
        { 2, 0, D3DDECLTYPE_DEC3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 1 },
        { 2, 4, D3DDECLTYPE_DEC3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 2 },
        { 2, 8, D3DDECLTYPE_DEC3N, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 3 },
        D3DDECL_END()
    },
};

// @address 0x836948 g_rasterizer_vertex_declarations
#include "rasterizer/rasterizer_vertex_declaration_table.h"
// @address 0x8c485c g_d3d_resource_allocator_device
__forceinline long create_vertex_declaration(D3DVERTEXELEMENT9 *elements,D3DVertexDeclaration **out) {
 long result=0;
 if(g_d3d_resource_allocator_device) {
  *out=D3DDevice_CreateVertexDeclaration(elements);
  if(!*out) result=-1;
 }
 return result;
}

extern "C" D3DVertexDeclaration *rasterizer_get_vertex_declaration(
    e_vertex_type vertex_type, e_transfer_vector_vertex_types transfer_type, e_entry_point entry)
{
    switch (entry)
    {
    case _entry_point_static_per_pixel:
    case _entry_point_lightmap_debug_mode:
        return g_rasterizer_vertex_declarations[vertex_type][2][0];
    case _entry_point_static_per_vertex:
        return g_rasterizer_vertex_declarations[vertex_type][3][0];
    case _entry_point_static_per_vertex_color:
        return g_rasterizer_vertex_declarations[vertex_type][1][0];
    case _entry_point_static_prt_ambient:
    case _entry_point_static_prt_linear:
    case _entry_point_static_prt_quadratic:
    case _entry_point_active_camo:
        return g_rasterizer_vertex_declarations[vertex_type][0][transfer_type];
    default:
        return g_rasterizer_vertex_declarations[vertex_type][0][0];
    }
}

void c_vertex_declaration_table::concat_decl(D3DVERTEXELEMENT9 *const result,
    const D3DVERTEXELEMENT9 *const a, const D3DVERTEXELEMENT9 *const b, const D3DVERTEXELEMENT9 *const c)
{
  long count = 0;
  long i;
  for (i = 0; a[i].Stream != 255; i++, count++)
    result[count] = a[i];
  for (i = 0; b[i].Stream != 255; i++, count++)
    result[count] = b[i];
  for (i = 0; c[i].Stream != 255; i++, count++)
    result[count] = c[i];
  result[count].Stream = 255;
  result[count].Offset = 0;
  result[count].Type = -1;
  result[count].Method = 0;
  result[count].Usage = 0;
  result[count].UsageIndex = 0;
}

bool c_vertex_declaration_table::initialize_vertex_declarations() {
 bool success=true;
 if(c_rasterizer::get_device()) {
  for(long type=0;type<22;type++) {
   for(long lighting=0;lighting<4;lighting++) {
    for(long transfer=0;transfer<4;transfer++) {
     if(lighting!=0 && transfer==3) {g_rasterizer_vertex_declarations[type][lighting][transfer]=0;continue;}
     D3DVERTEXELEMENT9 elements[64]; memset(elements,0,sizeof(elements));
     concat_decl(elements, base_vertex_decls[type],
         lighting_vertex_decls[lighting], transfer_vertex_decls[transfer]);
     long result=create_vertex_declaration(elements,&g_rasterizer_vertex_declarations[type][lighting][transfer]);
     if(result<0) success=false; else success &= true;
    }
   }
  }
 }
 return success;
}

static __forceinline void rasterizer_resource_definitions_store_dhenn3(XMDHENN3 *pDestination, XMVECTOR V)
{

    XMVECTOR               Convert;
    XMVECTOR               Select;
    XMVECTOR               Scale;
    XMVECTOR               X, Y, Z, XY;
    XMVECTOR               S;


    Scale = __vpermwi(*(XMVECTOR*)k_dhenn3_packing_constant, 0xC0);

    Convert = __vmulfp(V, Scale);
    Select = __vspltisw(8);
    Convert = __vrfin(Convert);
    Select = __vsrw(*(XMVECTOR*)k_dhenn3_packing_constant, Select);
    Convert = __vctsxs(Convert, 0);
    XY = __vspltw(Select, 1);
    S = __vspltw(Select, 2);
    Convert = __vslw(Convert, *(XMVECTOR*)k_dhenn3_packing_constant);
    X = __vspltw(Convert, 0);
    Y = __vspltw(Convert, 1);
    Z = __vspltw(Convert, 2);
    XY = __vsel(Y, X, XY);
    S = __vsel(Z, XY, S);

    __stvewx(S, pDestination, 0);

}

static __forceinline float rasterizer_resource_definitions_clamp(float value)
{
    value = (float)__fsel(value - -1.0f, value, -1.0f);
    return (float)__fsel(value - 1.0f, 1.0f, value);
}

// 0x45a6e8
extern "C" unsigned long rasterizer_pack_vector_dhenn3(const float *vector)
{
    XMVECTOR clamped;
    clamped.v[0] = rasterizer_resource_definitions_clamp(vector[0]);
    clamped.v[1] = rasterizer_resource_definitions_clamp(vector[1]);
    clamped.v[2] = rasterizer_resource_definitions_clamp(vector[2]);
    clamped.v[3] = 1.0f;
    XMDHENN3 packed;
    rasterizer_resource_definitions_store_dhenn3(&packed, clamped);
    return packed.v;
}

// @address 0x45a828 rasterizer_get_declaration_size
extern "C" long rasterizer_get_declaration_size(long declaration_type, long declaration_index)
{
    const D3DVERTEXELEMENT9 *elements;
    if (declaration_type == 0) {
        elements = c_vertex_declaration_table::base_vertex_decls[declaration_index];
    } else if (declaration_type == 1) {
        elements = c_vertex_declaration_table::lighting_vertex_decls[declaration_index];
    } else {
        elements = c_vertex_declaration_table::transfer_vertex_decls[declaration_index];
    }

    long size = 0;
    for (long i = 0; elements[i].Stream != 255; i++) {
        switch (elements[i].Type) {
        case D3DDECLTYPE_FLOAT1:
            size += 4;
            break;
        case D3DDECLTYPE_FLOAT2:
            size += 8;
            break;
        case D3DDECLTYPE_FLOAT3:
            size += 12;
            break;
        case D3DDECLTYPE_FLOAT4:
            size += 16;
            break;
        case D3DDECLTYPE_DEC3N:
            size += 4;
            break;
        case D3DDECLTYPE_D3DCOLOR:
        case D3DDECLTYPE_UBYTE4:
        case D3DDECLTYPE_UBYTE4N:
        case D3DDECLTYPE_BYTE4N:
        case D3DDECLTYPE_SHORT2:
        case D3DDECLTYPE_SHORT2N:
        case D3DDECLTYPE_USHORT2N:
        case D3DDECLTYPE_UDEC3:
        case D3DDECLTYPE_UHEND3:
        case D3DDECLTYPE_HEND3:
        case D3DDECLTYPE_UHEND3N:
        case D3DDECLTYPE_HEND3N:
        case D3DDECLTYPE_UDHEN3:
        case D3DDECLTYPE_DHEN3:
        case D3DDECLTYPE_UDHEN3N:
        case D3DDECLTYPE_DHEN3N:
        case D3DDECLTYPE_FLOAT16_2:
            size += 4;
            break;
        case D3DDECLTYPE_SHORT4:
        case D3DDECLTYPE_SHORT4N:
        case D3DDECLTYPE_USHORT4N:
        case D3DDECLTYPE_FLOAT16_4:
            size += 8;
            break;
        }
    }
    return size;
}

#ifndef RASTERIZER_RASTERIZER_RESOURCE_DEFINITIONS_H
#define RASTERIZER_RASTERIZER_RESOURCE_DEFINITIONS_H

// Functions and data defined in tu/graphicslibxbox/rasterizer_resource_definitions.cpp that other units use.

extern "C" unsigned long rasterizer_pack_vector_dhenn3(const float *vector);
extern "C" long rasterizer_get_declaration_size(long declaration_type, long declaration_index);

struct _D3DVERTEXELEMENT9;
struct D3DVertexDeclaration;
enum e_vertex_type;
enum e_transfer_vector_vertex_types;
enum e_entry_point;

extern "C" D3DVertexDeclaration *rasterizer_get_vertex_declaration(
    e_vertex_type vertex_type, e_transfer_vector_vertex_types transfer_type, e_entry_point entry);

// The D3DDECL_END() element that ends every vertex declaration.
extern "C" const _D3DVERTEXELEMENT9 k_vertex_declaration_terminator;

class c_vertex_declaration_table
{
public:
    static bool initialize_vertex_declarations();
    static bool set(e_vertex_type vertex_type, e_transfer_vector_vertex_types transfer_type, e_entry_point entry);

    // Elements of each vertex type, lighting and transfer vector: concatenated into one declaration per
    // combination. The GPU effect units append their streams to their vertex types at startup.
    static _D3DVERTEXELEMENT9 base_vertex_decls[22][64];
    static _D3DVERTEXELEMENT9 lighting_vertex_decls[4][64];
    static _D3DVERTEXELEMENT9 transfer_vertex_decls[4][64];

private:
    static void concat_decl(_D3DVERTEXELEMENT9 *const result,
        const _D3DVERTEXELEMENT9 *const a, const _D3DVERTEXELEMENT9 *const b,
        const _D3DVERTEXELEMENT9 *const c);
};

#endif // RASTERIZER_RASTERIZER_RESOURCE_DEFINITIONS_H

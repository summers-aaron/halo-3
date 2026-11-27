#ifndef RASTERIZER_RASTERIZER_VERTEX_TYPES_H
#define RASTERIZER_RASTERIZER_VERTEX_TYPES_H

// The vertex declaration (register mappings up to a terminator whose attribute is -1).

// Vertex types in the shaders' VERTEX_TYPE_* order.
enum e_vertex_type
{
    _vertex_type_world = 0,
    _vertex_type_rigid,
    _vertex_type_skinned,
    _vertex_type_particle_model,
    _vertex_type_flat_world,
    _vertex_type_flat_rigid,
    _vertex_type_flat_skinned,
    _vertex_type_screen,
    _vertex_type_debug,
    _vertex_type_transparent,
    _vertex_type_particle,
    _vertex_type_contrail,
    _vertex_type_light_volume,
    _vertex_type_chud_simple,
    _vertex_type_chud_fancy,
    _vertex_type_decorator,
    _vertex_type_tiny_position,
    _vertex_type_patchy_fog,
    _vertex_type_water,
    _vertex_type_ripple,
    _vertex_type_implicit,
    k_number_of_vertex_types
};

// Shader declarations without transfer vectors.
enum e_transfer_vector_vertex_types
{
    _transfer_vector_vertex_none = 0,
};

enum e_rasterizer_vertex_format
{
    _rasterizer_vertex_format_short4_normalized = 10,
    _rasterizer_vertex_format_ushort2_normalized = 11,
    _rasterizer_vertex_format_ushort4_normalized = 12,
    _rasterizer_vertex_format_udec3 = 13,
    _rasterizer_vertex_format_dhenn3 = 14,
    _rasterizer_vertex_format_half2 = 15,
    _rasterizer_vertex_format_half4 = 16,
    _rasterizer_vertex_format_dec3n = 17
};

// One vertex attribute of a declaration. An attribute of -1 ends the declaration; -2 is padding whose
// size in bytes is `format`.
struct rasterizer_vertex_register_mapping {
    signed char attribute;
    signed char format;
    unsigned char usage;
    unsigned char usage_index;
};
typedef char rasterizer_vertex_register_mapping_size_check[(sizeof(rasterizer_vertex_register_mapping) == 4) ? 1 : -1];

struct rasterizer_vertex_declaration {
    unsigned char index;
    rasterizer_vertex_register_mapping register_mappings[13];
};
typedef char rasterizer_vertex_declaration_size_check[(sizeof(rasterizer_vertex_declaration) == 53) ? 1 : -1];

#endif // RASTERIZER_RASTERIZER_VERTEX_TYPES_H

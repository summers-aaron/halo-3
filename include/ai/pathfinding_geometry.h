#ifndef AI_PATHFINDING_GEOMETRY_H
#define AI_PATHFINDING_GEOMETRY_H

#include <stddef.h>

// Pathfinding links joining sector edges, walked forward and backward.
struct sector_link
{
    union
    {
        struct { unsigned short start_vertex, end_vertex; };
        unsigned short vertices[2];
    };
    short flags;
    short hint_index;
    union
    {
        struct { unsigned short forward_link, reverse_link; };
        unsigned short links[2];
        unsigned short next_links[2];
    };
    union
    {
        struct { unsigned short left_sector, right_sector; };
        unsigned short sectors[2];
    };
};

typedef char sector_link_size[(sizeof(sector_link) == 0x10) ? 1 : -1];
typedef char sector_link_flags_offset[(offsetof(sector_link, flags) == 4) ? 1 : -1];
typedef char sector_link_hint_offset[(offsetof(sector_link, hint_index) == 6) ? 1 : -1];
typedef char sector_link_next_offset[(offsetof(sector_link, forward_link) == 8) ? 1 : -1];
typedef char sector_link_sector_offset[(offsetof(sector_link, left_sector) == 0xc) ? 1 : -1];

// A hint giving a link or surface index followed by type-specific data.
//
// The tag stores the hint data as four longs (pathfinding_hints_block) and the port converts them as longs, so every
// long read (data[i], flags, (unsigned short)data[i], data[i] >> 16) sees the Xbox value. The halfword and bit-field
// views below are the Xbox code's reads of those longs: on the console a halfword at +0 of a long is its high half and
// bit-fields are allocated from the high bit. Little-endian hosts declare each long's halves and bit-fields low to
// high so every named field keeps the Xbox value (F12 / F33 class; Xbox code: ai_hint_get_jump_destination_rail 0xcdf30
// reads the sector as lhz 8(jump), build_path_links_for_sector 0x552180 reads index1 as lhz 6 and heights as bits 15..8).
struct hint_jump_data
{
    union
    {
        long data[4];
#if defined(_XBOX) && !defined(__clang__)
        struct
        {
            unsigned long jump_end_vertex : 16;
            unsigned long jump_start_vertex : 16;
        };
        struct
        {
            unsigned short vertex0, vertex1, vertex2, vertex3;
            union
            {
                long flags;
                struct { unsigned short sector_index, destination_sector_index; };
            };
            long geometry;
        };
#else
        struct
        {
            unsigned long jump_start_vertex : 16;
            unsigned long jump_end_vertex : 16;
        };
        struct
        {
            unsigned short vertex1, vertex0, vertex3, vertex2;
            union
            {
                long flags;
                struct { unsigned short destination_sector_index, sector_index; };
            };
            long geometry;
        };
#endif
    };
};
typedef char hint_jump_data_size[(sizeof(hint_jump_data) == 0x10) ? 1 : -1];
#if defined(_XBOX) && !defined(__clang__)
typedef char hint_jump_data_sector_offset[(offsetof(hint_jump_data, sector_index) == 8) ? 1 : -1];
#else
typedef char hint_jump_data_sector_offset[(offsetof(hint_jump_data, sector_index) == 0xa) ? 1 : -1];
#endif
struct pathfinding_hint             /* 0x14 bytes */
{
    short type;
    short next_hint_index;
    union
    {
        long raw_data[4];
        hint_jump_data data;
        hint_jump_data jump;
#if defined(_XBOX) && !defined(__clang__)
        struct
        {
            long geometry;
            unsigned short alignment_vertex;
            unsigned short position_vertex;
            long : 16;
            long jump_height : 8;
            long : 8;
            long jump_geometry_start : 16;
            long jump_geometry_end : 16;
        };
#else // little-endian hosts: each long's halves and bit-fields low to high (see hint_jump_data)
        struct
        {
            long geometry;
            unsigned short position_vertex;
            unsigned short alignment_vertex;
            long : 8;
            long jump_height : 8;
            long : 16;
            long jump_geometry_end : 16;
            long jump_geometry_start : 16;
        };
#endif
        struct { long link_index; long parameters[3]; };
        struct { long surface_index; long variant; long surface_parameters[2]; };
        struct
        {
#if defined(_XBOX) && !defined(__clang__)
            unsigned short index0;
            unsigned short index1;
            unsigned short index2;
            unsigned short index3;
            union
            {
                unsigned short index4;
                struct
                {
                    long : 16;
                    long heights : 8;
                    long : 8;
                };
                long flags;
            };
#else
            unsigned short index1;
            unsigned short index0;
            unsigned short index3;
            unsigned short index2;
            union
            {
                struct
                {
                    unsigned short : 16;
                    unsigned short index4;
                };
                struct
                {
                    long : 8;
                    long heights : 8;
                    long : 16;
                };
                long flags;
            };
#endif
            long reference_frame;
        } fields;
    };
};
typedef char pathfinding_hint_size[(sizeof(pathfinding_hint) == 0x14) ? 1 : -1];

#endif // AI_PATHFINDING_GEOMETRY_H

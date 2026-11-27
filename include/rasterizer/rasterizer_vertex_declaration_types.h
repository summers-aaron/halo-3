#ifndef RASTERIZER_VERTEX_DECLARATION_TYPES_H
#define RASTERIZER_VERTEX_DECLARATION_TYPES_H

#ifndef _d3d9TYPES_H_
struct _D3DVERTEXELEMENT9 {
    unsigned short Stream;
    unsigned short Offset;
    unsigned long Type;
    unsigned char Method;
    unsigned char Usage;
    unsigned char UsageIndex;
};
#endif

typedef _D3DVERTEXELEMENT9 s_declaration_element;

typedef char contrail_declaration_element_size[(sizeof(s_declaration_element) == 12) ? 1 : -1];

#endif

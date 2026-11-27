#ifndef RENDER_DYNAMIC_VERTEX_BUFFER_TYPES_H
#define RENDER_DYNAMIC_VERTEX_BUFFER_TYPES_H

struct D3DVertexBuffer;

struct s_dynamic_vertex_buffer {
    char pad0;
    unsigned char stride;
    unsigned short count;
    D3DVertexBuffer *buffer;
};

#endif // RENDER_DYNAMIC_VERTEX_BUFFER_TYPES_H

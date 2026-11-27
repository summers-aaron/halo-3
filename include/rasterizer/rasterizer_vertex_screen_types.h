#ifndef RASTERIZER_RASTERIZER_VERTEX_SCREEN_TYPES_H
#define RASTERIZER_RASTERIZER_VERTEX_SCREEN_TYPES_H

struct rasterizer_vertex_screen
{
    float x;
    float y;
    float u;
    float v;
    unsigned long color;
};

typedef char rasterizer_vertex_screen_size[(sizeof(void *) != 4 || sizeof(rasterizer_vertex_screen) == 0x14) ? 1 : -1];

#endif // RASTERIZER_RASTERIZER_VERTEX_SCREEN_TYPES_H

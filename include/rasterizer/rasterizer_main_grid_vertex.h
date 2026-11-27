#pragma once

struct rasterizer_main_packed_state
{
    unsigned long value;
    unsigned long mask;
};

extern "C" inline void rasterizer_main_pack_grid_vertex_state(rasterizer_main_packed_state *state,
    unsigned char red, unsigned char green, unsigned char blue, unsigned int channel,
    bool value_flag, bool mask_flag);

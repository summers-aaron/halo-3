#ifndef RASTERIZER_RASTERIZER_TILING_GLOBALS_H
#define RASTERIZER_RASTERIZER_TILING_GLOBALS_H
struct s_rasterizer_tiling_state;
extern "C" {
volatile s_rasterizer_tiling_state *rasterizer_tiling_get_state();
}
#endif

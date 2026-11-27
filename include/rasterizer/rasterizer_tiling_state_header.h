#ifndef RASTERIZER_RASTERIZER_TILING_STATE_HEADER_H
#define RASTERIZER_RASTERIZER_TILING_STATE_HEADER_H

// Device-independent state shared by tiling setup and render submission.
struct s_rasterizer_tiling_state_header
{
    bool disabled;
    long width, height;
    long pass_count;
    char spare_before_enabled[4];
    bool enabled;
    bool spare_after_enabled;
    bool include_ui;
    bool screenshot_active;
};

typedef char rasterizer_tiling_state_header_size[(sizeof(s_rasterizer_tiling_state_header) == 0x18) ? 1 : -1];

const volatile s_rasterizer_tiling_state_header *rasterizer_tiling_get_state_header();

#endif

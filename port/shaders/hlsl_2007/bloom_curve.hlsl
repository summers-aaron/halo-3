#line 2 "source/rasterizer/hlsl/bloom_curve.hlsl"
// Halo 3 PC port: clean-room reconstruction of the retail 2007 explicit shader rasterizer\shaders\bloom_curve.
// Its vertex shader is the common screen vertex shader and its pixel shader microcode in mainmenu.map is
// instruction-for-instruction identical to downsample_4x4_block_bloom_new (same constants: g_exposure c0,
// ps_postprocess_pixel_size c1, ps_postprocess_scale c2, intensity_vector c3; same sampler dark_source_sampler),
// so it is the same shader: see downsample_4x4_block_bloom_new.hlsl for the reconstruction notes.
//@generate screen

#include "downsample_4x4_block_bloom_new.hlsl"

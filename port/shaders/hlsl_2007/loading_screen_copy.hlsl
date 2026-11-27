#line 2 "source/rasterizer/hlsl/loading_screen_copy.hlsl"
// Halo 3 PC port: clean-room reconstruction of the retail 2007 rasterizer's shared shader pair 7, which the loading
// screen binds to copy its surface (pixel function 7, vertex function 7). Both are D3D shader functions embedded in
// the executable (rasterizer_main_initialize_shared_shaders), not tags; written from their Xenos microcode:
//   vertex (2 vertex fetches, screen vertex): oPos = (position.xy, 1, 1), o0 = texcoord.xy;
//   pixel (1 texture fetch): oC0 = tex(source_sampler s0, texcoord * scale.xy (c2)).
// That is the MCC tree's copy.hlsl with the scale under its own name.
//@function vs 9bec91c1e59a117e
//@function ps 8a1afafa0ba50b15

#include "global.fx"
#include "hlsl_vertex_types.fx"
#include "utilities.fx"
#include "postprocess.fx"
//@generate screen

LOCAL_SAMPLER_2D(source_sampler, 0);
PARAM(float4, scale);

float4 default_ps(screen_output IN) : SV_Target
{
	return sample2D(source_sampler, IN.texcoord * scale.xy);
}

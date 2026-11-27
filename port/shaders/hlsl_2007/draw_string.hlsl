#line 2 "source/rasterizer/hlsl/draw_string.hlsl"
// Halo 3 PC port: clean-room reconstruction of the retail 2007 rasterizer's shared shader pair 0, the one
// c_rasterizer_draw_string binds for every glyph run (pixel function 0, vertex function 1). Both are D3D shader
// functions embedded in the executable (rasterizer_main_initialize_shared_shaders creates them with
// CreatePixelShader / CreateVertexShader), not tags; written from their Xenos microcode:
//   vertex (3 vertex fetches, screen vertex): oPos = (position.xy, 1, 1), o0 = texcoord.xy, o1 = color;
//   pixel (1 texture fetch): oC0 = tex(source_sampler s0, texcoord) * color * scale (c1).
// That is the MCC tree's copy_surface with its color (POSTPROCESS_COLOR) and the scale under its own name; the game
// sets scale = (1, 1, 1, render target alpha multiplier).
//@function vs c521eee230411523
//@function ps 284ba4299042f08a

#define POSTPROCESS_COLOR

#include "global.fx"
#include "hlsl_vertex_types.fx"
#include "utilities.fx"
#include "postprocess.fx"
//@generate screen

LOCAL_SAMPLER_2D(source_sampler, 0);
PARAM(float4, scale);

float4 default_ps(screen_output IN) : SV_Target
{
	return sample2D(source_sampler, IN.texcoord) * IN.color * scale;
}

#line 2 "source/rasterizer/hlsl/downsample_4x4_block_bloom_new.hlsl"
// Halo 3 PC port: clean-room reconstruction of the retail 2007 explicit shader
// rasterizer\shaders\downsample_4x4_block_bloom_new, written from its Xenos microcode in mainmenu.map
// (rasg explicit shader; pixl tag microcode disassembled with Xenia's microcode analysis). The shader is absent from
// the public MCC HLSL tree, which only has downsample_4x4_block_bloom (the 2007 "old" variant).
//
// Retail microcode (pixel shader, 4 texture fetches, c255 literals = {0.7, -0.7, 0.25, 0}):
//   - four bilinear fetches of dark_source_sampler at texcoord +- 0.7 * ps_postprocess_pixel_size (c1) in x and y,
//     each covering a 2x2 texel footprint, together a 4x4 block;
//   - each sample squared (the Xenon HDR target stores sqrt-encoded color) and scaled by g_exposure.y (c0.y,
//     DARK_COLOR_MULTIPLIER);
//   - per sample: intensity = dot(color, intensity_vector) (c3); bloom = max(intensity * scale.y,
//     intensity - scale.x) (c2 = ps_postprocess_scale); color *= bloom / intensity;
//   - output rgb = the average of the four bloomed samples, alpha = the average intensity.
// Unlike downsample_4x4_block_bloom, which applies the bloom curve once to the averaged color, this variant applies
// it to each 2x2 sample before averaging. The retail bloom_curve explicit shader has the identical microcode.
//
// The pc path (DX_VERSION 11) squares too: the port keeps the Xbox's sqrt-encoded HDR target (the game's hdr_gamma2,
// b15, which the pack gathers). A tiny bias keeps the divide by intensity defined for black samples.

#include "global.fx"
#include "hlsl_vertex_types.fx"
#include "utilities.fx"
#include "postprocess.fx"
#include "downsample_registers.fx"
//@generate screen

LOCAL_SAMPLER_2D_IN_VIEWPORT_MAYBE(dark_source_sampler, 0);

#define BLOOM_TAP_OFFSET 0.7f

// One 2x2 tap of the 4x4 block: exposed color, bloomed by its own intensity.
float3 bloom_tap(float2 texcoord, out float intensity)
{
	float3 color= sample2D(dark_source_sampler, texcoord).rgb;
	color *= color;
#ifdef pc
	color += 0.00000001f;							// keeps the divide by intensity defined for black samples
#endif
	color *= DARK_COLOR_MULTIPLIER;

	intensity= dot(color, intensity_vector.rgb);
	float bloom_intensity= max(intensity * ps_postprocess_scale.y, intensity - ps_postprocess_scale.x);
	return color * (bloom_intensity / intensity);
}

float4 default_ps(screen_output IN) : SV_Target
{
	float2 offset= BLOOM_TAP_OFFSET * ps_postprocess_pixel_size.xy;

	float intensity0, intensity1, intensity2, intensity3;
	float3 color= bloom_tap(IN.texcoord + float2(-offset.x, +offset.y), intensity0);
	color += bloom_tap(IN.texcoord + float2(+offset.x, -offset.y), intensity1);
	color += bloom_tap(IN.texcoord + float2(-offset.x, -offset.y), intensity2);
	color += bloom_tap(IN.texcoord + float2(+offset.x, +offset.y), intensity3);

	float intensity= (intensity0 + intensity1 + intensity2 + intensity3) * 0.25f;
	return float4(color * 0.25f, intensity);
}

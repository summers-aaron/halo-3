// port/sdk/d3d/pwl_convert.hlsl: the Xenos 8_8_8_8 <-> 8_8_8_8_GAMMA conversion, for one EDRAM read through the other
// color space and for a resolve fetched with the other signs. EDRAM (and a resolve's memory) holds 8-bit codes; a
// gamma render target stores PWL gamma codes of the linear colors the shader wrote (the port keeps those linear, in
// 16-bit UNORM, as xenia's gamma_render_target_as_unorm16 does), and a GPUSIGN_GAMMA fetch returns the PWL degamma
// of the codes. So reading linear storage as codes encodes (PWL_ENCODE=1), reading codes as linear decodes. RGB only;
// alpha is never gamma.
//
// The two curves are xenia's (src/xenia/gpu/xenos.cc PWLGammaToLinear / LinearToPWLGamma, the Source Engine's
// X360GammaToLinear / X360LinearToGamma with the Direct3D 9 truncation). The encode adds 1/64 of a code before its
// truncation: the linear values it reads came from a decode, and 16-bit UNORM or float rounding leaves some a hair
// under their exact value; with it the encode inverts the decode on all 256 codes (without it 64 come back one
// lower through R16 UNORM).
//
// Built into pwl_convert_spirv.h (do not edit that file by hand), once per direction:
//   dxc -spirv -T cs_6_0 -E main -fspv-target-env=vulkan1.3 -O3 -D PWL_ENCODE=0 port/sdk/d3d/pwl_convert.hlsl -Fo <decode>.spv
//   dxc -spirv -T cs_6_0 -E main -fspv-target-env=vulkan1.3 -O3 -D PWL_ENCODE=1 port/sdk/d3d/pwl_convert.hlsl -Fo <encode>.spv
//   spirv-opt --target-env=vulkan1.3 -O <out>.spv -o <optimized>.spv
//   spirv-val --target-env vulkan1.3 <optimized>.spv
//   od -An -tx4 -v <optimized>.spv  (one 0x...u word per entry)
// DXIL for Direct3D 12 (registers and root signature: helper_binding.hlsli), each build with its -D as above:
//   dxc -T cs_6_0 -E main -HV 2021 -O3 -WX port/sdk/d3d/pwl_convert.hlsl -Fo <out>.dxil
// The destination's format is the image's own (R16G16B16A16_UNORM, R8G8B8A8_UNORM or B8G8R8A8_UNORM): the storage
// write is untyped (shaderStorageImageWriteWithoutFormat).

#include "helper_binding.hlsli"

XE_VK([[vk::binding(0, 0)]]) Texture2D<float4> source : register(t0);
XE_VK([[vk::binding(1, 0)]] [[vk::image_format("unknown")]]) RWTexture2D<float4> destination : register(u0);

float pwl_to_linear(float gamma)
{
    gamma = saturate(gamma);
    float scale, offset;
    if (gamma >= 96.0 / 255.0) {
        if (gamma >= 192.0 / 255.0) { scale = 8.0 / 1024.0; offset = -1024.0; }
        else                        { scale = 4.0 / 1024.0; offset = -256.0; }
    } else {
        if (gamma >= 64.0 / 255.0)  { scale = 2.0 / 1024.0; offset = -64.0; }
        else                        { scale = 1.0 / 1024.0; offset = 0.0; }
    }
    float value = gamma * ((255.0 * 1024.0) * scale) + offset;
    value += trunc(value * scale);
    return value * (1.0 / 1023.0);
}

float linear_to_pwl(float value)
{
    value = saturate(value);
    float scale, offset;
    if (value >= 128.0 / 1023.0) {
        if (value >= 512.0 / 1023.0) { scale = 1023.0 / 8.0; offset = 128.0 / 255.0; }
        else                          { scale = 1023.0 / 4.0; offset = 64.0 / 255.0; }
    } else {
        if (value >= 64.0 / 1023.0)  { scale = 1023.0 / 2.0; offset = 32.0 / 255.0; }
        else                          { scale = 1023.0; offset = 0.0; }
    }
    return trunc(value * scale + 1.0 / 64.0) * (1.0 / 255.0) + offset;
}

float4 convert_pwl(float4 color)
{
#if PWL_ENCODE
    color.rgb = float3(linear_to_pwl(color.r), linear_to_pwl(color.g), linear_to_pwl(color.b));
#else
    color.rgb = float3(pwl_to_linear(color.r), pwl_to_linear(color.g), pwl_to_linear(color.b));
#endif
    return color;
}

[RootSignature(XE_HELPER_ROOT_SIGNATURE)]
#ifdef __spirv__
[numthreads(16, 8, 1)]
#else
[numthreads(8, 8, 1)]
#endif
void main(uint3 id : SV_DispatchThreadID)
{
    uint width, height;
    destination.GetDimensions(width, height);
    uint source_width, source_height;
    source.GetDimensions(source_width, source_height);
#ifdef __spirv__
    const uint2 extent = min(uint2(width, height), uint2(source_width, source_height));
    if (any(id.xy >= extent))
#else
    if (id.x >= width || id.y >= height || id.x >= source_width || id.y >= source_height)
#endif
        return;
    destination[id.xy] = convert_pwl(source.Load(int3(id.xy, 0)));
}

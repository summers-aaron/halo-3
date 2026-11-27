// port/sdk/d3d/edram_7e3.hlsl: the EDRAM alias of a 2_10_10_10_FLOAT (7e3) render target read through a 2_10_10_10
// header. EDRAM holds the 7e3 encoding of each color (3-bit exponent, 7-bit mantissa per channel, 2-bit alpha); a
// surface of format 2_10_10_10 over the same EDRAM reads those bits as 10-bit unsigned normalized values. The 7e3
// target is emulated by a float image, so this re-encodes its texels as 7e3 and writes the bits as unorm.
//
// The encoding is DirectXTex's / xenia's (PreClampedFloat32To7e3): clamp to [0, 31.875]; below 0.25 the denormal
// form; round to nearest even.
//
// Built into edram_7e3_spirv.h (do not edit that file by hand):
//   dxc -spirv -T cs_6_0 -E main -fspv-target-env=vulkan1.3 -O3 port/sdk/d3d/edram_7e3.hlsl -Fo <out>.spv
//   od -An -tx4 -v <out>.spv  (one 0x...u word per entry)
// DXIL for Direct3D 12 (registers and root signature: helper_binding.hlsli), each build with its -D as above:
//   dxc -T cs_6_0 -E main -HV 2021 -O3 -WX port/sdk/d3d/edram_7e3.hlsl -Fo <out>.dxil

#include "helper_binding.hlsli"

XE_VK([[vk::binding(0, 0)]]) Texture2D<float4> source : register(t0);
XE_VK([[vk::binding(1, 0)]] [[vk::image_format("rgb10a2")]]) RWTexture2D<float4> destination : register(u0);

uint float_to_7e3(float value)
{
    const float clamped = min(max(value, 0.0f), 31.875f);
    uint bits = asuint(clamped);
    if (bits < 0x3e800000u)
    {
        // denormal 7e3: the mantissa with its implicit 1, shifted by how far the exponent is below -2
        const uint shift = min(125u - (bits >> 23), 24u);
        bits = ((bits & 0x7fffffu) | 0x800000u) >> shift;
    }
    else
    {
        bits -= 124u << 23; // rebias the exponent
    }
    return ((bits + 0x7fffu + ((bits >> 16) & 1u)) >> 16) & 0x3ffu;
}

[RootSignature(XE_HELPER_ROOT_SIGNATURE)]
[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint width, height;
    destination.GetDimensions(width, height);
    if (id.x >= width || id.y >= height)
        return;
    const float4 color = source.Load(int3(id.xy, 0));
    const uint3 encoded = uint3(float_to_7e3(color.r), float_to_7e3(color.g), float_to_7e3(color.b));
    destination[id.xy] = float4(float3(encoded) / 1023.0f, saturate(color.a));
}

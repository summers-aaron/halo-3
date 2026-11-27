// port/sdk/d3d/display_gamma.hlsl: the Xenos display controller's gamma, applied as the console scans the front
// buffer out (D3DDevice_SetPWLGamma / D3DDevice_SetGammaRamp program it; the GPU's render targets never see it).
//
// The front buffer holds each color as a 10-bit (2_10_10_10) or 8-bit (8_8_8_8) unsigned value. For Halo 3's 7e3
// final composite those 10 bits are the 7e3 codewords themselves (edram_7e3.hlsl), and the PWL ramp the game builds
// (c_rasterizer_LUT_control::compute_LUT) decodes 7e3 to linear and applies the display gamma in one table.
//
//   PWL (D3DPWLGAMMA): 128 segments per channel, {Base, Delta} 16-bit words. The display controller keeps the top
//     10 bits of each (DC_LUT_PWL_DATA); a 10-bit input selects segment input >> 3 and interpolates over its low
//     3 bits: output = Base + Delta * (input & 7) / 8 (xenia's apply_gamma_pwl, AMD's RRG-216M56).
//   Table (D3DGAMMARAMP): 256 16-bit words per channel indexed by the 8-bit input (the top 10 bits are kept).
//
// table[256]: PWL, .xyz = red / green / blue segment (Base | Delta << 16) for segments 0..127; ramp, .xyz = the
// red / green / blue words for entries 0..255. header.x: 1 ramp, 2 PWL; header.y: 1 when source is a float 7e3
// render target (encode float_to_7e3 inline before applying the ramp, fusing edram_7e3.hlsl into this pass).
// Output: [0, 1] for an R8G8B8A8_UNORM display image (presented to a UNORM swapchain as it is: the gamma is already
// in the values).
//
// Built into display_gamma_spirv.h (do not edit that file by hand):
//   dxc -spirv -T cs_6_0 -E main -fspv-target-env=vulkan1.3 -O3 port/sdk/d3d/display_gamma.hlsl -Fo <out>.spv
//   spirv-opt --target-env=vulkan1.3 -O <out>.spv -o <optimized>.spv
//   spirv-val --target-env vulkan1.3 <optimized>.spv
//   od -An -tx4 -v <optimized>.spv  (one 0x...u word per entry)
// DXIL for Direct3D 12 (registers and root signature: helper_binding.hlsli), each build with its -D as above:
//   dxc -T cs_6_0 -E main -HV 2021 -O3 -WX port/sdk/d3d/display_gamma.hlsl -Fo <out>.dxil

#include "helper_binding.hlsli"

XE_VK([[vk::binding(0, 0)]]) Texture2D<float4> source : register(t0);
XE_VK([[vk::binding(1, 0)]] [[vk::image_format("rgba8")]]) RWTexture2D<float4> destination : register(u0);
XE_VK([[vk::binding(2, 0)]]) cbuffer ramp : register(b0)
{
    uint4 header;
    uint4 table[256];
};

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

float apply_pwl(uint input, uint segment)
{
    const uint base = (segment & 0xffffu) >> 6;
    const uint delta = segment >> 22;
    return saturate((float(base) + float((input & 7u) * delta) * (1.0f / 8.0f)) * (1.0f / 1023.0f));
}

float apply_table(uint word)
{
    return float(word >> 6) * (1.0f / 1023.0f);
}

float4 display_gamma(float3 raw)
{
    float3 output;
    if (header.y != 0u)
    {
        const uint3 input = uint3(float_to_7e3(raw.r), float_to_7e3(raw.g), float_to_7e3(raw.b));
        if (header.x == 2u)
        {
            output.r = apply_pwl(input.r, table[input.r >> 3].x);
            output.g = apply_pwl(input.g, table[input.g >> 3].y);
            output.b = apply_pwl(input.b, table[input.b >> 3].z);
        }
        else if (header.x == 1u)
        {
            const uint3 idx = uint3(float3(input) * (255.0f / 1023.0f) + 0.5f);
            output.r = apply_table(table[idx.r].x);
            output.g = apply_table(table[idx.g].y);
            output.b = apply_table(table[idx.b].z);
        }
        else
        {
            output = float3(input) * (1.0f / 1023.0f);
        }
    }
    else
    {
        const float3 color = saturate(raw);
        if (header.x == 2u)
        {
            // UNORM to integer as D3D10+ defines it
            const uint3 input = uint3(color * 1023.0f + 0.5f);
            output.r = apply_pwl(input.r, table[input.r >> 3].x);
            output.g = apply_pwl(input.g, table[input.g >> 3].y);
            output.b = apply_pwl(input.b, table[input.b >> 3].z);
        }
        else if (header.x == 1u)
        {
            const uint3 input = uint3(color * 255.0f + 0.5f);
            output.r = apply_table(table[input.r].x);
            output.g = apply_table(table[input.g].y);
            output.b = apply_table(table[input.b].z);
        }
        else
        {
            output = color;
        }
    }
    return float4(output, 1.0f);
}

[RootSignature(XE_HELPER_ROOT_SIGNATURE)]
#ifdef __spirv__
[numthreads(32, 4, 1)]
#else
[numthreads(8, 8, 1)]
#endif
void main(uint3 id : SV_DispatchThreadID)
{
    uint width, height;
    destination.GetDimensions(width, height);
#ifdef __spirv__
    id.x *= 2u;
    if (any(id.xy >= uint2(width, height)))
#else
    if (id.x >= width || id.y >= height)
#endif
        return;
    const float3 first = source.Load(int3(id.xy, 0)).rgb;
#ifdef __spirv__
    const uint2 next = uint2(id.x + 1u, id.y);
    if (next.x < width)
    {
        const float3 second = source.Load(int3(next, 0)).rgb;
        destination[next] = display_gamma(second);
    }
#endif
    destination[id.xy] = display_gamma(first);
}

// port/sdk/d3d/depth_pack.hlsl: a depth resolve as the Xenos resolve leaves it in memory, for a fetch of it through a
// 32-bit color header (vulkan_device.cpp packed_depth_view): each texel is the 24_8 word (depth << 8) | stencil, its
// bytes from the low one in R, G, B, A (the k_8_8_8_8 components X, Y, Z, W, after the 8in32 swap). The depth is 20e4
// float on a 24_8_FLOAT (D24FS8) surface, unorm24 on a 24_8 (D24S8) one.
//
// Two builds of this file, both in depth_pack_spirv.h (do not edit it by hand):
//   k_depth_pack_20e4_spirv: 20e4 depth (24_8_FLOAT);
//     dxc -spirv -T cs_6_0 -E main -fspv-target-env=vulkan1.3 -O3 port/sdk/d3d/depth_pack.hlsl -Fo <out>.spv
//   k_depth_pack_unorm24_spirv (-D DEPTH_PACK_UNORM24): unorm24 depth (24_8);
//     dxc -spirv -T cs_6_0 -E main -fspv-target-env=vulkan1.3 -O3 -D DEPTH_PACK_UNORM24 \
//         port/sdk/d3d/depth_pack.hlsl -Fo <out>.spv
//   od -An -tx4 -v <out>.spv  (one 0x...u word per entry)
// DXIL for Direct3D 12 (registers and root signature: helper_binding.hlsli), each build with its -D as above:
//   dxc -T cs_6_0 -E main -HV 2021 -O3 -WX port/sdk/d3d/depth_pack.hlsl -Fo <out>.dxil

#include "helper_binding.hlsli"

XE_VK([[vk::binding(0, 0)]]) Texture2D<float> depth : register(t0);    // the depth aspect
XE_VK([[vk::binding(1, 0)]] [[vk::image_format("rgba8")]]) RWTexture2D<float4> destination : register(u0);
XE_VK([[vk::binding(2, 0)]]) Texture2D<uint> stencil : register(t1);   // the stencil aspect

// float32 -> 20e4 (4-bit exponent, 20-bit mantissa, [0, 2)), rounding to nearest even: xenia-edge pixel_formats.xesli
// XeFloat32To20e4 (after CFloat24 of d3dref9.dll)
uint float32_to_20e4(uint f)
{
    f = min(f <= 0x7fffffffu ? f : 0u, 0x3ffffff8u); // negative and NaN -> 0, the largest value below 2
    const uint denormalized = ((f & 0x7fffffu) | 0x800000u) >> min(113u - (f >> 23u), 24u);
    uint f24 = f < 0x38800000u ? denormalized : f + 0xc8000000u;
    f24 += 3u + ((f24 >> 3u) & 1u);
    return (f24 >> 3u) & 0xffffffu;
}

[RootSignature(XE_HELPER_ROOT_SIGNATURE)]
[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint width, height;
    destination.GetDimensions(width, height);
    if (id.x >= width || id.y >= height)
        return;
    const float z = depth.Load(int3(id.xy, 0));
#ifdef DEPTH_PACK_UNORM24
    const uint z24 = (uint)(saturate(z) * 16777215.0f + 0.5f);
#else
    const uint z24 = float32_to_20e4(asuint(z));
#endif
    const uint word = (z24 << 8) | (stencil.Load(int3(id.xy, 0)) & 0xffu);
    destination[id.xy] = float4((uint4(word, word, word, word) >> uint4(0, 8, 16, 24)) & 0xffu) / 255.0f;
}

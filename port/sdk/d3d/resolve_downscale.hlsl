// port/sdk/d3d/resolve_downscale.hlsl: HALO3_RES_SCALE's guest-resolution copy of a resolved image. A resolve of a
// scaled render target holds N x N host texels per guest texel; a guest-resolution post-process pass (a reduced
// render target, vulkan_device.cpp target_res_scale) samples it with taps placed for guest texels (downsample_4x4_block
// +-1 texel, the bloom downsamples), so it would read only a few of each guest texel's host texels. This writes the
// N x N box average of the source's host texels into each destination texel, N = source width / destination width:
// the guest-size image the console's own resolve would hold, supersampled.
//
// Built into resolve_downscale_spirv.h (do not edit that file by hand):
//   dxc -spirv -T cs_6_0 -E main -fspv-target-env=vulkan1.3 -O3 port/sdk/d3d/resolve_downscale.hlsl -Fo <out>.spv
//   od -An -tx4 -v <out>.spv  (one 0x...u word per entry)
// DXIL for Direct3D 12 (registers and root signature: helper_binding.hlsli), each build with its -D as above:
//   dxc -T cs_6_0 -E main -HV 2021 -O3 -WX port/sdk/d3d/resolve_downscale.hlsl -Fo <out>.dxil
// The destination's format is the source image's own (any color format with storage use): the storage write is
// untyped (shaderStorageImageWriteWithoutFormat).

#include "helper_binding.hlsli"

XE_VK([[vk::binding(0, 0)]]) Texture2D<float4> source : register(t0);
XE_VK([[vk::binding(1, 0)]] [[vk::image_format("unknown")]]) RWTexture2D<float4> destination : register(u0);

[RootSignature(XE_HELPER_ROOT_SIGNATURE)]
[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint width, height, source_width, source_height;
    destination.GetDimensions(width, height);
    if (id.x >= width || id.y >= height)
        return;
    source.GetDimensions(source_width, source_height);
    const uint2 n = uint2(source_width / width, source_height / height);
    const int2 first = int2(id.xy * n);
    float4 sum = 0.0;
    for (uint y = 0; y < n.y; y++)
        for (uint x = 0; x < n.x; x++)
            sum += source.Load(int3(first + int2(x, y), 0));
    destination[id.xy] = sum / (float)(n.x * n.y);
}

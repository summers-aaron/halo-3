// port/sdk/d3d/resolve_scale.hlsl: a Resolve's exponent bias (D3DRESOLVE_EXPONENTBIAS, RB_COPY_DEST_INFO): the
// resolved colors are scaled by 2^bias on their way out of EDRAM. Writes a rectangle of the source render target,
// scaled, into the same rectangle of a scratch image of the same format, from which the resolve copies as usual.
//
// Two builds of this file (do not edit the headers by hand):
//   resolve_scale_spirv.h: the R16G16B16A16_SFLOAT scratch image (the 7e3 / float16 EDRAM targets);
//     dxc -spirv -T cs_6_0 -E main -fspv-target-env=vulkan1.3 -O3 port/sdk/d3d/resolve_scale.hlsl -Fo <out>.spv
//   resolve_scale_any_spirv.h (-D RESOLVE_SCALE_ANY_FORMAT): a scratch image of any float format (the 16_16 /
//     16_16_16_16 fixed-point EDRAM targets, R32G32[B32A32]_SFLOAT), written without a declared storage format
//     (shaderStorageImageWriteWithoutFormat, as pwl_convert.hlsl);
//     dxc -spirv -T cs_6_0 -E main -fspv-target-env=vulkan1.3 -O3 -D RESOLVE_SCALE_ANY_FORMAT \
//         port/sdk/d3d/resolve_scale.hlsl -Fo <out>.spv
//   od -An -tx4 -v <out>.spv  (one 0x...u word per entry)
// DXIL for Direct3D 12 (registers and root signature: helper_binding.hlsli), each build with its -D as above:
//   dxc -T cs_6_0 -E main -HV 2021 -O3 -WX port/sdk/d3d/resolve_scale.hlsl -Fo <out>.dxil

#include "helper_binding.hlsli"

XE_VK([[vk::binding(0, 0)]]) Texture2D<float4> source : register(t0);
#ifdef RESOLVE_SCALE_ANY_FORMAT
XE_VK([[vk::binding(1, 0)]] [[vk::image_format("unknown")]]) RWTexture2D<float4> destination : register(u0);
#else
XE_VK([[vk::binding(1, 0)]] [[vk::image_format("rgba16f")]]) RWTexture2D<float4> destination : register(u0);
#endif
XE_VK([[vk::binding(2, 0)]]) cbuffer parameters : register(b0)
{
    int4 rect;   // x, y, width, height
    float4 scale; // .x: 2^bias; .y: non-zero to clamp to [.z, .w] (float EDRAM -> fixed-point destination: [0, 1]
                  // unsigned, [-1, 1] signed)
};

[RootSignature(XE_HELPER_ROOT_SIGNATURE)]
[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if (id.x >= (uint)rect.z || id.y >= (uint)rect.w)
        return;
    const int2 at = rect.xy + int2(id.xy);
    float4 v = source.Load(int3(at, 0)) * scale.x;
    destination[at] = scale.y != 0.0 ? clamp(v, scale.z, scale.w) : v;
}

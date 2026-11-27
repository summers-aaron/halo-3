// port/sdk/d3d/d3d12_copy.hlsl: the Direct3D 12 backend's graphics copies (d3d12_backend.cpp), the ones Direct3D 12
// cannot do with a copy command and Vulkan does with vkCmdBlitImage / vkCmdCopyImage:
//   COPY_COLOR    a rectangle of a color image into a rectangle of another's render target, scaled by nearest texel
//                 (vkCmdBlitImage, VK_FILTER_NEAREST: texel floor((pixel centre - destination origin) * source size /
//                 destination size) of the source rectangle) and converted to the target's format by value;
//   COPY_PRESENT  the display image onto the swapchain's back buffer with bilinear filtering (vkCmdBlitImage,
//                 VK_FILTER_LINEAR), the viewport being the letterboxed / pillarboxed destination rectangle;
//   COPY_DEPTH    a rectangle of a depth image's depth into another depth image of its format (SV_Depth): Direct3D 12
//                 copies depth / stencil subresources only whole (CopyTextureRegion);
//   COPY_STENCIL  one bit of the same rectangle's stencil: the pixel is discarded unless the source's stencil has bit
//                 options.x, and the pipeline replaces only that bit (stencil write mask 1 << bit, reference 0xff),
//                 after the rectangle's stencil was cleared to 0; eight draws copy the stencil (shaders cannot write
//                 stencil without SV_StencilRef, which not every device has).
// One vertex shader (vs_main) draws a triangle covering the viewport; the scissor keeps the destination rectangle.
//
// Built into d3d12_copy_dxil.h (do not edit that file by hand; tmp/d3d12_phase3_backend/scripts/dxil_headers.sh):
//   dxc -T vs_6_0 -E vs_main -HV 2021 -O3 -WX port/sdk/d3d/d3d12_copy.hlsl -Fo <out>.dxil
//   dxc -T ps_6_0 -E ps_main -HV 2021 -O3 -WX -D COPY_COLOR port/sdk/d3d/d3d12_copy.hlsl -Fo <out>.dxil
//   (and -D COPY_PRESENT, -D COPY_DEPTH, -D COPY_STENCIL the same way)
// Root signature (every entry point embeds it): root constants b0 (the rectangles, the stencil bit), an SRV table
// t0-t1 (t0 the source color / depth, t1 the source stencil, its view mapping the stencil into .x) and a static linear
// clamping sampler s0.

#define XE_COPY_ROOT_SIGNATURE \
    "RootConstants(num32BitConstants = 12, b0), " \
    "DescriptorTable(SRV(t0, numDescriptors = 2, flags = DESCRIPTORS_VOLATILE)), " \
    "StaticSampler(s0, filter = FILTER_MIN_MAG_MIP_LINEAR, addressU = TEXTURE_ADDRESS_CLAMP, " \
    "addressV = TEXTURE_ADDRESS_CLAMP, addressW = TEXTURE_ADDRESS_CLAMP)"

cbuffer copy_constants : register(b0)
{
    int4 source_rect;      // x, y, width, height in source texels
    int4 destination_rect; // x, y, width, height in render target pixels
    uint4 options;         // x: the stencil bit (COPY_STENCIL)
};

#if defined(COPY_COLOR) || defined(COPY_PRESENT)
Texture2D<float4> source : register(t0);
#endif
#ifdef COPY_PRESENT
SamplerState linear_clamp : register(s0);
#endif
#ifdef COPY_DEPTH
Texture2D<float> source_depth : register(t0);
#endif
#ifdef COPY_STENCIL
Texture2D<uint> source_stencil : register(t1);
#endif

// The source texel a pixel copies 1:1 (depth, stencil).
int2 source_texel(float2 position)
{
    return source_rect.xy + (int2(position) - destination_rect.xy);
}

[RootSignature(XE_COPY_ROOT_SIGNATURE)]
float4 vs_main(uint id : SV_VertexID) : SV_Position
{
    const float2 corner = float2((id << 1) & 2u, id & 2u);
    return float4(corner * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), 0.0f, 1.0f);
}

#if defined(COPY_COLOR)
[RootSignature(XE_COPY_ROOT_SIGNATURE)]
float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    const float2 scale = float2(source_rect.zw) / float2(destination_rect.zw);
    const int2 texel = source_rect.xy + int2(floor((position.xy - float2(destination_rect.xy)) * scale));
    return source.Load(int3(texel, 0));
}
#elif defined(COPY_PRESENT)
[RootSignature(XE_COPY_ROOT_SIGNATURE)]
float4 ps_main(float4 position : SV_Position) : SV_Target0
{
    const float2 uv = (position.xy - float2(destination_rect.xy)) / float2(destination_rect.zw);
    return float4(source.SampleLevel(linear_clamp, uv, 0.0f).rgb, 1.0f);
}
#elif defined(COPY_DEPTH)
[RootSignature(XE_COPY_ROOT_SIGNATURE)]
float ps_main(float4 position : SV_Position) : SV_Depth
{
    return source_depth.Load(int3(source_texel(position.xy), 0));
}
#elif defined(COPY_STENCIL)
[RootSignature(XE_COPY_ROOT_SIGNATURE)]
void ps_main(float4 position : SV_Position)
{
    if (((source_stencil.Load(int3(source_texel(position.xy), 0)) >> options.x) & 1u) == 0u)
        discard;
}
#endif

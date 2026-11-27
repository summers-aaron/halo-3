// port/sdk/d3d/helper_binding.hlsli: the binding of the port's compute helpers (edram_7e3, pwl_convert, depth_pack,
// display_gamma, resolve_scale, resolve_downscale) for both of their targets, from one declaration per resource:
//   Vulkan (dxc -spirv, the *_spirv.h headers): XE_VK keeps the [[vk::binding(i, 0)]] / [[vk::image_format]]
//     attributes, which take precedence over the registers: descriptor set 0 bindings 0-2, unchanged (the SPIR-V is
//     word for word what it was before the registers were added);
//   Direct3D 12 (dxc -T cs_6_0, DXIL): XE_VK drops the attributes (DXC would warn that it ignores them) and the
//     registers apply, under XE_HELPER_ROOT_SIGNATURE: root parameter 0 a descriptor table of SRV t0-t1 then UAV u0,
//     root parameter 1 a root CBV b0 (one compute root signature for all six).
// Vulkan binding 0 (source / depth) is t0, binding 1 (destination) u0, binding 2 b0 (a cbuffer) or t1 (stencil).
#ifndef HELPER_BINDING_HLSLI
#define HELPER_BINDING_HLSLI

#ifdef __spirv__
#define XE_VK(...) __VA_ARGS__
#else
#define XE_VK(...)
#endif

#define XE_HELPER_ROOT_SIGNATURE \
    "DescriptorTable(SRV(t0, numDescriptors = 2, flags = DESCRIPTORS_VOLATILE), " \
    "UAV(u0, flags = DESCRIPTORS_VOLATILE | DATA_VOLATILE)), CBV(b0)"

#endif

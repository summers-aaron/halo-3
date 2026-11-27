// dxc -spirv -T cs_6_0 -E main -fspv-target-env=vulkan1.3 -O3 depth_gather.hlsl -Fo depth_gather.spv
// od -An -tx4 -v depth_gather.spv (one 0x...u word per entry in depth_gather_spirv.h).

[[vk::binding(0, 0)]] Texture2D<float> depth;
[[vk::binding(1, 0)]] Texture2D<uint> stencil;
[[vk::binding(2, 0)]] RWStructuredBuffer<uint4> depth_plane;
[[vk::binding(3, 0)]] RWStructuredBuffer<uint> stencil_plane;
[[vk::binding(4, 0)]] cbuffer parameters
{
    uint width;
    uint height;
    uint scale;
    uint gather_rows;
};

// At sample zero, each run of 2 * scale host pixels skips an equally long run.
// Both pixels of an even-aligned pair therefore come from consecutive source pixels.
uint3 source_pair_start(uint3 pair)
{
    uint3 skipped_pairs;
    if (scale <= 2u)
        skipped_pairs = pair & (0u - scale);
    else if (scale == 3u)
        skipped_pairs = pair - pair % 3u;
    else
        skipped_pairs = pair - pair % scale;
    return (pair + skipped_pairs) * 2u;
}

void gather_row(uint4 source_x, uint source_y, uint word, uint stencil_mask)
{
    uint4 depth_bits;
    uint packed_stencil = 0;
    [unroll]
    for (uint i = 0; i < 4u; ++i)
    {
        int3 source = int3(source_x[i], source_y, 0);
        depth_bits[i] = asuint(depth.Load(source));
        packed_stencil |= stencil.Load(source) << (i * 8u); // S8_UINT
    }
    depth_plane[word] = depth_bits;
    stencil_plane[word] = packed_stencil & stencil_mask;
}

[numthreads(32, 2, 1)]
void main(uint3 thread_id : SV_DispatchThreadID)
{
    uint x = thread_id.x * 4u;
    uint y = thread_id.y * 2u;
    if (x >= width || y >= height)
        return;

    uint3 starts = source_pair_start(uint3(x / 2u, x / 2u + 1u, thread_id.y));
    uint4 source_x = uint4(starts.x, starts.x + 1u, starts.y, starts.y + 1u);
    uint source_y = gather_rows != 0 ? starts.z : y;
    uint stencil_mask = ~0u;
    uint count = width - x;
    if (count < 4u)
    {
        // The copy ignores row padding. Keep its loads in bounds and its stencil bytes zero.
        uint last_x = count == 1u ? source_x.x : (count == 2u ? source_x.y : source_x.z);
        source_x = min(source_x, last_x);
        stencil_mask >>= (4u - count) * 8u;
    }

    uint words_per_row = (width + 3u) / 4u;
    uint word = y * words_per_row + thread_id.x;
    gather_row(source_x, source_y, word, stencil_mask);
    if (y + 1u < height)
        gather_row(source_x, source_y + 1u, word + words_per_row, stencil_mask);
}

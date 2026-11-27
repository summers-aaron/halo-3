#ifndef RASTERIZER_RASTERIZER_SAMPLER_TEXTURE_CALLS_H
#define RASTERIZER_RASTERIZER_SAMPLER_TEXTURE_CALLS_H

#include "rasterizer/rasterizer.h"

#if defined(_XBOX) && !defined(__clang__)
// XDK passes this four-byte class by address for both parameter forms. These
// callers retain the reference lifetime while targeting the canonical symbol.
typedef void (*rasterizer_sampler_texture_reference_call)(long, const c_rasterizer_texture_ref &);
#else
// x86 passes this class (it has a copy constructor) by value on the stack, so the
// call keeps set_sampler_texture's own by-value signature.
typedef void (*rasterizer_sampler_texture_reference_call)(long, c_rasterizer_texture_ref);
#endif

// match: 0x44b7c0, 0x1665f8, 0x488618: union conversion retains the reference
// call alive while invoking the canonical sampler setter.
static __forceinline void rasterizer_set_sampler_texture_reference(long sampler, const c_rasterizer_texture_ref &texture)
{
    union
    {
        void (*value)(long, c_rasterizer_texture_ref);
        rasterizer_sampler_texture_reference_call reference;
    } call;
    call.value = &c_rasterizer::set_sampler_texture;
    call.reference(sampler, texture);
}

#endif // RASTERIZER_RASTERIZER_SAMPLER_TEXTURE_CALLS_H

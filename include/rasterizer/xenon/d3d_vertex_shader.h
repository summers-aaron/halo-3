#ifndef RASTERIZER_XENON_D3D_VERTEX_SHADER_H
#define RASTERIZER_XENON_D3D_VERTEX_SHADER_H

#include <stddef.h>
#include <xtl.h>

namespace D3D
{
    // The SDK shader extends the public resource with binding state followed by its microcode container.
    class CVertexShader : public D3DVertexShader
    {
        BYTE m_ShaderState[0x368 - sizeof(D3DVertexShader)];

    public:
        DWORD UCodeMagicAndFlags;
    };

    typedef char c_vertex_shader_ucode_offset[(offsetof(CVertexShader, UCodeMagicAndFlags) == 0x368) ? 1 : -1];

    inline CVertexShader *Promote(D3DVertexShader *shader)
    {
        return static_cast<CVertexShader *>(shader);
    }
}

#endif

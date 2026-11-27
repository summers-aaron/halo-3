// blamlibxbox/havok_mopp_code_definitions.obj: from the records 0xf6988 0x259d98 0x259e00
// @flags /TP /O2 /Oc
#include "cseries/crt_xdk.h"
// @address 0x259e00 ??1phys_bvh_bytecode@@UAA@XZ
// @address 0x259d98 ??_Gphys_bvh_bytecode@@UAAPAXI@Z
#include "tag_files/tag_groups.h"
#include "phys/phys_collide.h"
#include "physics/havok_mopp_code_definitions.h"
#ifdef HALO3_64BIT
#include "physics/havok_tag_unpack_64.h"
#endif

phys_bvh_bytecode::~phys_bvh_bytecode()
{
}

// @address 0xf6988 ?havok_mopp_code_definitions_fixup@@YAXPAUs_tag_block@@@Z
void havok_mopp_code_definitions_fixup(s_tag_block *mopp_codes)
{
#ifdef HALO3_64BIT
    if (mopp_codes->count <= 0 || tag_block_is_unpacked_64(mopp_codes))
        return;

    s_havok_mopp_code_definition *dst = (s_havok_mopp_code_definition *)havok_unpack_alloc_64(
        (size_t)mopp_codes->count * sizeof(s_havok_mopp_code_definition));
    const char *src_base = (const char *)mopp_codes->address;
    for (long mopp_code_index = 0; mopp_code_index < mopp_codes->count; mopp_code_index++)
    {
        const char *src = src_base + (size_t)mopp_code_index * 0x40;
        phys_bvh_bytecode::bytecode_info code_info;
        memcpy(&code_info, src + 0x10, sizeof(code_info));
        signed char build_type = *(const signed char *)(src + 0x2c);
        memcpy(&dst[mopp_code_index].mopp_code, src + 0x30, sizeof(s_tag_block));

        phys_bvh_bytecode *mopp_code = new (&dst[mopp_code_index].runtime_mopp_code) phys_bvh_bytecode(
            code_info, static_cast<const unsigned char *>(dst[mopp_code_index].mopp_code.address),
            dst[mopp_code_index].mopp_code.count);
        mopp_code->m_build_type = static_cast<phys_bvh_bytecode::bvh_build_kind>(build_type);
        mopp_code->m_object_ref_count = 128;
    }
    mopp_codes->address = dst;
    tag_block_mark_unpacked_64(mopp_codes);
#else
    for (long mopp_code_index = 0; mopp_code_index < mopp_codes->count; mopp_code_index++)
    {
        s_havok_mopp_code_definition *definition =
            TAG_BLOCK_GET_ELEMENT(mopp_codes, mopp_code_index, s_havok_mopp_code_definition);
        phys_bvh_bytecode *mopp_code = &definition->runtime_mopp_code;
        phys_bvh_bytecode::bytecode_info code_info = mopp_code->m_bvh_info;
        new (mopp_code) phys_bvh_bytecode(code_info, static_cast<const unsigned char *>(definition->mopp_code.address),
            definition->mopp_code.count);
        mopp_code->m_object_ref_count = 128;
    }
#endif
}

// hkbase/hkStreamWriter.obj: from the records 0x60f4b8 0x60f4c0 0x60f4c8
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x60f4b8 ?seek@phys_writer@@UAA?AW4phys_result@@HW4seek_whence@1@@Z
// @address 0x60f4c0 ?tell@phys_writer@@UBAHXZ
// @address 0x60f4c8 ?seek_tell_supported@phys_writer@@UBA?AVphys_bool@@XZ
#include "phys/phys_base_types.h"
#include "phys/phys_base.h"
#include "phys/phys_writer.h"
phys_result phys_writer::seek(int offset, seek_whence seek_origin)
{
    return PHYS_FAILURE;
}
int phys_writer::tell() const
{
    return -1;
}
phys_bool phys_writer::seek_tell_supported() const
{
    return phys_bool(false);
}


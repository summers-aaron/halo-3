// blamlibxbox/havok_halo_external.obj: from the records 0x25e550
// @flags /TP /O2
// @address 0x25e550 ?halo_managed_memory_address@@YA_NPBVphys_ref_object@@@Z
#include "physics/havok_halo_external.h"
#include "cache/cache_file_globals.h"
#include "cseries/pointer_utilities.h"
#include "phys/phys_object.h"


bool halo_managed_memory_address(const phys_ref_object *object)
{
    unsigned int address = address_from_pointer(object);
    long total_tags_size = cache_files_get_total_tags_size();
    unsigned int tag_cache_base_address = address_from_pointer(cache_file_globals_get_tag_cache_base_address());
    return address >= tag_cache_base_address && address <= tag_cache_base_address + total_tags_size - 1;
}

#if defined(__clang__)
// Port-only: March's body is ASSERT("havok breakpoint :-)") (physics\havok_halo_external.cpp:22),
// empty in release builds; the linker folds it into the shared empty body at 0x5bec98. Xbox
// callers (hkThreadMemory::constSizeToRow across ~45 native Havok objects) bind the
// tag-absolute: a native definition would drop as a fold and strand them (LNK2001; wpl rebinds
// folds only inside the merged /GL object). The PC link needs a body from source.
void halo_external_breakpoint()
{
}
#endif

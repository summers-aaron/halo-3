// hkdynamics/hkConstraintAtomUtil.obj: from the records 0x663ca0 0x663d38 0x663d80 0x663e50 0x663f30 0x663ff0
// @flags /TP /O2 /Oc
// @address 0x663ca0 ?allocate_contact_atom@phys_simple_contact_atom_util@@SAPAUphys_simple_contact_atom@@H@Z
// @address 0x663d38 ?free_contact_atom@phys_simple_contact_atom_util@@SAXPAUphys_simple_contact_atom@@@Z
// @address 0x663d80 ?copy_atom_contents@phys_simple_contact_atom_util@@SAXPAUphys_simple_contact_atom@@PBU2@@Z
// @address 0x663e50 ?expand_one@phys_simple_contact_atom_util@@SAPAUphys_simple_contact_atom@@PAU2@@Z
// @address 0x663f30 ?remove_at_and_copy@phys_simple_contact_atom_util@@SAXPAUphys_simple_contact_atom@@H@Z
// @address 0x663ff0 ?optimize_capacity@phys_simple_contact_atom_util@@SAPAUphys_simple_contact_atom@@PAU2@H@Z
#include <vectorintrinsics.h>
#include "phys/phys_dynamics.h"
#include "phys/phys_atoms.h"

#include "phys/phys_thread_memory.h"
enum { PHYS_MAX_CONTACT_POINT = 256 };
// 8-byte aligned, as the unit's own info block type is (its copyContents copies this block)
#include "phys/phys_contact_point_props.h"
#include "phys/phys_simple_contact_atom_util.h"
namespace {
static __forceinline void release_contact_atom(phys_simple_contact_atom *atom) {
    phys_thread_memory::get_singleton_instance().free_chunk(atom, atom->m_all_atoms_size, PHYS_MEM_DYNAMICS);
}
}
phys_simple_contact_atom *phys_simple_contact_atom_util::allocate_contact_atom(int atom_contact_count)
{
    int size = atom_contact_count * (int)(sizeof(phys_contact_point) + sizeof(phys_contact_point_props))
        + (int)sizeof(phys_simple_contact_atom);
    size = (size + 15) & ~15;
    phys_simple_contact_atom *atom = (phys_simple_contact_atom *)phys_thread_memory::get_singleton_instance().allocate_chunk(size, PHYS_MEM_DYNAMICS);
    atom->m_all_atoms_size = (unsigned short)size;
    atom->m_reserved_contact_count = (unsigned short)atom_contact_count;
    atom->m_bvh_info.init();
    atom->m_type = phys_atom::ATOM_CONTACT;
    atom->m_contact_point_count = 0;
    return atom;
}
void phys_simple_contact_atom_util::free_contact_atom(phys_simple_contact_atom *atom)
{
    release_contact_atom(atom);
}
void phys_simple_contact_atom_util::copy_atom_contents(
    phys_simple_contact_atom *destination, const phys_simple_contact_atom *source)
{
    destination->m_bvh_info = source->m_bvh_info;
    destination->m_contact_point_count = source->m_contact_point_count;
    phys_contact_point *dst_contacts = destination->get_contact_points();
    phys_contact_point_props *dst_contact_props = destination->get_contact_point_props();
    const phys_contact_point *src_contacts = source->get_contact_points();
    const phys_contact_point_props *src_contact_props = source->get_contact_point_props();
    for (int i = 0; i < source->m_contact_point_count; ++i) {
        *(dst_contacts++) = *(src_contacts++);
        *(dst_contact_props++) = *(src_contact_props++);
    }
}
phys_simple_contact_atom *phys_simple_contact_atom_util::expand_one(phys_simple_contact_atom *atom)
{
    phys_simple_contact_atom *result = atom;
    phys_simple_contact_atom *local_atom = result;
    if (local_atom->m_contact_point_count >= local_atom->m_reserved_contact_count) {
        int new_contact_cnt = phys_math::max_of_two<int>(local_atom->m_contact_point_count, 2);
        new_contact_cnt += new_contact_cnt;
        new_contact_cnt = phys_math::min_of_two<int>(new_contact_cnt, PHYS_MAX_CONTACT_POINT);
        result = allocate_contact_atom(new_contact_cnt);
        copy_atom_contents(result, atom);
        release_contact_atom(atom);
        local_atom = result;
    }
    ++local_atom->m_contact_point_count;
    phys_contact_point_props *property = local_atom->get_contact_point_props() +
        local_atom->m_contact_point_count - 1;
    phys_string::mem_fill4(property, 0, sizeof(*property) / sizeof(int));
    return result;
}
phys_simple_contact_atom *phys_simple_contact_atom_util::optimize_capacity(phys_simple_contact_atom *atom, int free_contact_point_count)
{
    phys_simple_contact_atom *result = atom;
    int atom_contact_count = result->m_contact_point_count + free_contact_point_count;
    if (atom_contact_count * 2 <= result->m_reserved_contact_count) {
        int new_contact_cnt = result->m_reserved_contact_count >> 1;
        result = allocate_contact_atom(new_contact_cnt);
        copy_atom_contents(result, atom);
        release_contact_atom(atom);
    }
    return result;
}
void phys_simple_contact_atom_util::remove_at_and_copy(phys_simple_contact_atom *atom, int index)
{
    --atom->m_contact_point_count;
    phys_contact_point *contact = atom->get_contact_points() + index;
    phys_contact_point_props *property = atom->get_contact_point_props() + index;
    for (int i = index; i < atom->m_contact_point_count; ++i) {
        *contact = *(contact + 1);
        *property = *(property + 1);
        ++contact;
        ++property;
    }
}

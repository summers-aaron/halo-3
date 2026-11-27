// hkbase/hkMultiThreadLock.obj: from the records 0x700128 0x700130 0x700138 0x700148 0x700168 0x700188 0x700248 0x7003b8 0x700460 0x7004f8 0x700598 0x700638
// @flags /TP /O2 /Oc
// @address 0x700128 ?check_access@phys_multi_thread_lock@@QBAXW4access_mode@1@@Z
// @address 0x700130 ?check_access_with_parent@phys_multi_thread_lock@@SAXPBV1@W4access_mode@1@ABV1@1@Z
// @address 0x700138 ?disable_checks@phys_multi_thread_lock@@QAAXXZ
// @address 0x700148 ?enable_checks@phys_multi_thread_lock@@QAAXXZ
// @address 0x700168 ?is_checking_enabled@phys_multi_thread_lock@@QBA_NXZ
// @address 0x700248 ?mark_for_read_impl@phys_multi_thread_lock@@IAAXW4read_mode@1@@Z
// @address 0x7003b8 ?mark_for_write_impl@phys_multi_thread_lock@@IAAXXZ
// @address 0x700460 ?is_marked_for_write_impl@phys_multi_thread_lock@@IAA_NXZ
// @address 0x7004f8 ?unmark_for_read_impl@phys_multi_thread_lock@@IAAXXZ
// @address 0x700598 ?unmark_for_write_impl@phys_multi_thread_lock@@IAAXXZ
#include "physics/havok_memory.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_critical_section.h"
#include "phys/phys_dynamics.h"

phys_critical_section_base *phys_critical_section_base::s_list_head = 0;
phys_thread_local_data<int> phys_critical_section::m_time_locks;
phys_semaphore phys_critical_section_base::s_list_semaphore = phys_semaphore(1, 1);
phys_critical_section *phys_multi_thread_lock::m_display_lock = 0;

void phys_multi_thread_lock::check_access(access_mode type) const {
    (void)type;
}
void phys_multi_thread_lock::check_access_with_parent(const phys_multi_thread_lock *parent, access_mode lock_parent_access,
    const phys_multi_thread_lock &child, access_mode child_access_type) {
    if (!parent) return;
    unsigned int parent_thread_id = parent->m_lock_thread_id;
    if (parent_thread_id == MT_MARKING_OFF) return;
    if (parent_thread_id != MT_LOCK_READ_SELF_ONLY) {
        parent->check_access(lock_parent_access);
        if (lock_parent_access >= child_access_type) return;
    }
    if (child_access_type == PHYS_ACCESS_IGNORE || parent_thread_id == MT_MARKING_OFF || parent_thread_id == MARK_UNLOCKED ||
        parent_thread_id == MT_LOCK_READ_SELF_ONLY) {
        child.check_access(child_access_type);
    } else if (parent_thread_id == MT_LOCK_READ_ONLY && child_access_type != PHYS_ACCESS_RO) {
        child.check_access(child_access_type);
    }
}
void phys_multi_thread_lock::disable_checks() { m_lock_thread_id = MT_MARKING_OFF; }
void phys_multi_thread_lock::enable_checks() {
    if (m_lock_thread_id == MT_MARKING_OFF) {
        m_lock_thread_id = MARK_UNLOCKED;
        m_lock_count = 0;
    }
}
bool phys_multi_thread_lock::is_checking_enabled() const { return m_lock_thread_id != MT_MARKING_OFF; }
void phys_multi_thread_lock::mark_for_read_impl(read_mode mode) {
    switch (m_lock_thread_id) {
    case MT_MARKING_OFF: return;
    case MT_LOCK_READ_SELF_ONLY:
    case MT_LOCK_READ_ONLY:
    case MARK_UNLOCKED:
        m_display_lock->enter();
        m_lock_thread_id = mode == READ_MODE_RECURSIVE ? MT_LOCK_READ_ONLY : MT_LOCK_READ_SELF_ONLY;
        break;
    default:
        m_display_lock->enter();
        break;
    }
    m_lock_bit_stack <<= 1;
    ++m_lock_count;
    m_display_lock->leave();
}
void phys_multi_thread_lock::mark_for_write_impl() {
    if (m_lock_thread_id == MT_MARKING_OFF) return;
    phys_critical_section_lock lock(m_display_lock);
    unsigned int lock_thread_id = static_cast<unsigned int>(phys_thread::get_current_thread_id());
    m_lock_bit_stack = (m_lock_bit_stack << 1) | 1;
    ++m_lock_count;
    if (m_lock_thread_id == MARK_UNLOCKED) m_lock_thread_id = lock_thread_id;
}
bool phys_multi_thread_lock::is_marked_for_write_impl() {
    if (m_lock_thread_id == MT_MARKING_OFF) return true;
    phys_critical_section_lock lock(m_display_lock);
    unsigned int lock_thread_id = static_cast<unsigned int>(phys_thread::get_current_thread_id());
    if (m_lock_thread_id == lock_thread_id) {
        bool result = true;
        return result;
    } else {
        bool result = false;
        return result;
    }
}
void phys_multi_thread_lock::unmark_for_read_impl() {
    if (m_lock_thread_id == MT_MARKING_OFF) return;
    phys_critical_section_lock lock(m_display_lock);
    --m_lock_count;
    m_lock_bit_stack >>= 1;
    if (m_lock_count == 0) {
        m_lock_thread_id = MARK_UNLOCKED;
    }
}
void phys_multi_thread_lock::unmark_for_write_impl() {
    if (m_lock_thread_id == MT_MARKING_OFF) return;
    phys_critical_section_lock lock(m_display_lock);
    --m_lock_count;
    m_lock_bit_stack >>= 1;
    if (m_lock_count == 0) m_lock_thread_id = MARK_UNLOCKED;
}

// @address 0x700188 ?static_init@phys_multi_thread_lock@@SAXXZ
void phys_multi_thread_lock::static_init()
{
    m_display_lock = new phys_critical_section(1000);
}
// @address 0x700638 ?static_quit@phys_multi_thread_lock@@SAXXZ
void phys_multi_thread_lock::static_quit()
{
    if (m_display_lock) {
        delete m_display_lock;
        m_display_lock = 0;
    }
}

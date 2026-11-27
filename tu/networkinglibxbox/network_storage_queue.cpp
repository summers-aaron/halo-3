// networkinglibxbox/network_storage_queue.obj: from the records 0x959c8 0x4042a0 0x4043a8 0x404410 0x404538
// @flags /TP /O2 /Oc
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <ppcintrinsics.h>
#include "cseries/cseries.h"
#include "networking/logic/storage/network_storage_queue.h"
#include "networking/logic/storage/network_storage_item.h"
#include "memory/crc.h"
// @address 0x26630 k_network_storage_queue_limits
// @rdata 0x82026630..0x82026638
extern "C" {
const long k_network_storage_queue_limits[2] = { 1, 1 };
}
// @rdata-end

struct s_network_storage_queue_entry {
    c_network_storage_item_interface *active;
    long item_index;
    long buffer_size;
    s_network_storage_file_header *buffer;
    long pad;
};
typedef long t_network_storage_queue[16];
static long g_network_storage_queues[2][16];
static s_network_storage_queue_entry g_network_storage_queue_items[32];

static __forceinline char *network_storage_queue_lists(s_network_storage_queue_entry *members)
{
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x4042a0: the queue lists are addressed at -0x80 from the member array base register, not by their own lis/addi
    return (char *)members - sizeof(g_network_storage_queues);
#else
    (void)members;
    return (char *)g_network_storage_queues;
#endif
}

static __forceinline t_network_storage_queue *network_storage_queue_lists_end(s_network_storage_queue_entry *members)
{
    char *const &queues = network_storage_queue_lists(members);
    return (t_network_storage_queue *)queues + 2;
}

static __forceinline s_network_storage_queue_entry *network_storage_queue_get_member(long index);

static __forceinline void network_storage_queue_clear_member(
    c_network_storage_item_interface **active, long *item_index, long *buffer_size, s_network_storage_file_header **buffer, long clear_value, long **current);

// @address 0x4042a0 ?network_storage_queue_dispose_member@@YAXJW4e_network_storage_queue_task_result@@@Z
void network_storage_queue_dispose_member(long member_index, e_network_storage_queue_task_result task_result)
{
    s_network_storage_queue_entry *members = g_network_storage_queue_items;
    s_network_storage_queue_entry *entry = &members[member_index];
    if (entry->active != 0) {
        entry->active->notify_task_completed(task_result);
        // match: 0x4042a0: zero-bit rotate preserves 32-bit GPR value
        const long clear_value = (long)_rotl(0, 0);
        long *current;
        long slot_index;
        if (entry->active->get_type() == 4 && entry->active->get_queue_id() == -1) {
            entry->active->abort_online_task();
            entry->active->dispose_temp_buffer(clear_value);
            network_storage_queue_clear_member(&entry->active, &entry->item_index, &entry->buffer_size, &entry->buffer, clear_value, &current);
        } else {
            network_storage_queue_clear_member(&entry->active, &entry->item_index, &entry->buffer_size, &entry->buffer, clear_value, &current);
        }
        do {
            slot_index = clear_value;
            do {
                if (*current == member_index) {
                    *current = -1;
                    // match: 0x4042a0: zero-bit rotate preserves 32-bit GPR value
                    member_index = (long)_rotl(member_index, 0);
                }
                if ((long)slot_index > 0 && current[-1] == -1) {
                    current[-1] = *current;
                    *current = -1;
                    // match: 0x4042a0: zero-bit rotate preserves 32-bit GPR value
                    members = (s_network_storage_queue_entry *)(size_t)_rotl((unsigned long)(size_t)members, 0);
                }
                ++slot_index;
                ++current;
            } while ((long)slot_index < 16);
        } while ((ptrdiff_t)current < (ptrdiff_t)network_storage_queue_lists_end(members));
    }
}

static __forceinline s_network_storage_queue_entry *network_storage_queue_get_member(long index)
{
    return &g_network_storage_queue_items[index];
}

static __forceinline void network_storage_queue_clear_member(c_network_storage_item_interface ** active,
    // match: 0x4042a0: __restrict keeps the *active/*item_index stores ahead of the *current store
    long *__restrict item_index, long *__restrict buffer_size, s_network_storage_file_header ** buffer, long clear_value, long **__restrict current)
{
    const long store_value = (long)clear_value;
    *buffer = (s_network_storage_file_header *)(ptrdiff_t)store_value;
    *buffer_size = store_value;
    *current = (long *)network_storage_queue_lists(g_network_storage_queue_items);
    *active = (c_network_storage_item_interface *)(ptrdiff_t)store_value;
    *item_index = -1;
}

// @address 0x4043a8 ?network_storage_queue_clear@@YAXXZ
void network_storage_queue_clear(void)
{
    memset(network_storage_queue_lists(g_network_storage_queue_items), -1, sizeof(g_network_storage_queues));
    for (int entry_index = 0; entry_index < 32; entry_index++) {
        s_network_storage_queue_entry *entry = network_storage_queue_get_member(entry_index);
        entry->active = 0;
        entry->item_index = -1;
        entry->buffer_size = 0;
        entry->buffer = 0;
    }
}

static __forceinline long network_storage_queue_get_unused_member_index(void)
{
    long result = -1;
    for (long index = 0; result == -1 && index < 32; index++) {
        if (g_network_storage_queue_items[index].active == 0)
            result = index;
    }
    return result;
}
static __forceinline long network_storage_queue_get_unused_queue_entry_index(void)
{
    long result = -1;
    for (long index = 0; result == -1 && index < 16; index++) {
        if (g_network_storage_queues[1][index] == -1)
            result = index;
    }
    return result;
}
// @address 0x404538 ?network_storage_queue_download_item@@YAJPAVc_network_storage_item_interface@@PAUs_network_storage_file_header@@J@Z
long network_storage_queue_download_item(c_network_storage_item_interface *item, s_network_storage_file_header *buffer, long buffer_size)
{
    long member_index = network_storage_queue_get_unused_member_index();
    long queue_entry_index = network_storage_queue_get_unused_queue_entry_index();
    if (member_index != -1 && queue_entry_index != -1) {
        g_network_storage_queues[1][queue_entry_index] = member_index;
        g_network_storage_queue_items[member_index].active = item;
        g_network_storage_queue_items[member_index].item_index = -1;
        g_network_storage_queue_items[member_index].buffer_size = buffer_size;
        g_network_storage_queue_items[member_index].buffer = buffer;
    }
    return member_index;
}

static __forceinline long network_storage_queue_get_unused_upload_queue_entry_index(void)
{
    long result = -1;
    for (long index = 0; result == -1 && index < 16; index++) {
        if (g_network_storage_queues[0][index] == -1)
            result = index;
    }
    return result;
}
// @address 0x404410 ?network_storage_queue_upload_item@@YAJPAVc_network_storage_item_interface@@PAUs_network_storage_file_header@@J@Z
long network_storage_queue_upload_item(c_network_storage_item_interface *item, s_network_storage_file_header *buffer, long buffer_size)
{
    long member_index = network_storage_queue_get_unused_member_index();
    long queue_entry_index = network_storage_queue_get_unused_upload_queue_entry_index();
    if (item->get_type() == 1 || item->get_type() == 2) {
        buffer->checksum = 0xFFFFFFFF;
        buffer->checksum = crc_checksum_buffer(0xFFFFFFFF, buffer, buffer_size);
    }
    if (member_index != -1 && queue_entry_index != -1) {
        g_network_storage_queue_items[member_index].active = item;
        g_network_storage_queue_items[member_index].item_index = -1;
        g_network_storage_queue_items[member_index].buffer_size = buffer_size;
        g_network_storage_queue_items[member_index].buffer = buffer;
        g_network_storage_queues[0][queue_entry_index] = member_index;
    }
    return member_index;
}

static __forceinline long *network_storage_queue_get_slots(s_network_storage_queue_entry *members, const long &queue_index)
{
    const unsigned long &word_offset = queue_index * (sizeof(t_network_storage_queue) / sizeof(long));
    char *const &queues = network_storage_queue_lists(members);
    const unsigned long &byte_offset = word_offset * sizeof(long);
    return (long *)(queues + byte_offset);
}
// @address 0x959c8 ?network_storage_queue_update@@YAXXZ
void network_storage_queue_update(void)
{
    const long *limits = k_network_storage_queue_limits;
    s_network_storage_queue_entry *members = g_network_storage_queue_items;
    for (long queue_index = 0; queue_index < 2; ++queue_index) {
        long limit = limits[queue_index];
        long *slots = network_storage_queue_get_slots(members, queue_index);
        BLAM_ASSUME(members == g_network_storage_queue_items);
        long active_task_count = 0;
        unsigned long queue_changed = 4;
        long *scan = slots;
        do {
            for (long scan_index = 0; scan_index < 4; ++scan_index) {
                long member_index = scan[scan_index];
                if (member_index != -1) {
                    s_network_storage_queue_entry *member = network_storage_queue_get_member(member_index);
                if (member->item_index != -1)
                    ++active_task_count;
                }
            }
            scan += 4;
            --queue_changed;
        } while (queue_changed != 0);
        for (long slot_index = 0; slot_index < 16; ++slot_index) {
            if (active_task_count >= limit || (unsigned char)queue_changed)
                break;
            long member_index = slots[slot_index];
            if (member_index == -1)
                continue;
            s_network_storage_queue_entry *entry = network_storage_queue_get_member(member_index);
            bool postponed = false;
            if (entry->item_index != -1)
                continue;
            long result = 0;
            long task = -1;
            switch (queue_index) {
            case 0:
                result = 1;
                break;
            case 1: {
                short status = entry->active->get_type();
                if (status == 3)
                    result = 1;
                else {
                    if (status == 4) {
                        postponed = true;
                        queue_changed = true;
                    }
                    if (!postponed)
                        result = 1;
                }
                break;
            }
            }
            if (postponed)
                continue;
            if (result == 0) {
                entry->item_index = task;
                result = 1;
                ++active_task_count;
                continue;
            }
            e_network_storage_queue_task_result task_result;
            switch (result - 2) {
            case 0:
                task_result = _network_storage_queue_task_result_value_3;
                break;
            case 1:
                task_result = _network_storage_queue_task_result_value_4;
                break;
            case 2:
                task_result = _network_storage_queue_task_result_value_5;
                break;
            case 3:
                task_result = _network_storage_queue_task_result_value_6;
                break;
            default:
                task_result = _network_storage_queue_task_result_value_2;
                break;
            }
            network_storage_queue_dispose_member(member_index, task_result);
            if (slot_index + 1 < 16)
                --slot_index;
            queue_changed = true;
        }
        // match: 0x959c8: zero-bit rotate preserves 32-bit GPR value
        limits = (const long *)(size_t)_rotl((unsigned long)(size_t)limits, 0);
        // match: 0x959c8: zero-bit rotate preserves 32-bit GPR value
        members = (s_network_storage_queue_entry *)(size_t)_rotl((unsigned long)(size_t)members, 0);
    }
}

// networkinglibxbox/network_storage_item.obj: from the records 0x4461b0 0x446288 0x446308
// @flags /TP /O2 /J /Oc
#include "networking/logic/storage/network_storage_queue.h"
#include "networking/logic/storage/network_storage_item.h"
// @address 0x446308 ?notify_task_completed@c_network_storage_item_interface@@QAAXW4e_network_storage_queue_task_result@@@Z

// @address 0x119a494 g_network_storage_default_data_failures
extern "C" {
long g_network_storage_default_data_failures;
}
static __forceinline void set_flag(c_network_storage_item_interface *item, unsigned short mask, bool value) {
    if (value) item->flags |= mask; else item->flags &= ~mask;
}
__forceinline bool c_network_storage_item_interface::use_default_data_proc() {
    bool result = false;
    if (m_definition != 0 && m_definition->use_default_data_proc != 0) {
        result = m_definition->use_default_data_proc(this);
        set_flag(this, 2, result);
        set_flag(this, 8, result);
    }
    return result;
}

// @address 0x4461b0 ?begin_download@c_network_storage_item_interface@@QAA_NW4e_controller_index@@_KJ@Z
bool c_network_storage_item_interface::begin_download(
    e_controller_index controller, unsigned __int64 identifier, long operation_flags)
{
    bool result = false;
    if ((short)get_current_action() == 0) {
        value = controller;
        wide = identifier;
        flags2 = (unsigned short)operation_flags;
        if (ready() != 0 && (flags & 8) == 0) {
            result = true;
        } else {
            m_queue_id = network_storage_queue_download_item(this, buffer, buffer_size);
            if (m_queue_id != -1) {
                current_task = 1;
                if (valid_state != 4) {
                    flags = (unsigned short)(flags & 0xfffffffd);
                }
                result = true;
            }
        }
    }
    return result;
}

// @address 0x446288 ?begin_upload@c_network_storage_item_interface@@QAA_NW4e_controller_index@@_KJ@Z
bool c_network_storage_item_interface::begin_upload(e_controller_index controller, unsigned __int64 identifier, long operation_flags)
{
    bool result = false;
    if (current_task == 0) {
        value = controller;
        wide = identifier;
        flags2 = (unsigned short)operation_flags;
        m_queue_id = network_storage_queue_upload_item(this, buffer, buffer_size);
        if (m_queue_id != -1) {
            current_task = 2;
            result = true;
        }
    }
    return result;
}

void c_network_storage_item_interface::notify_task_completed(enum e_network_storage_queue_task_result _sym_p1) {
    bool succeeded = false;
    this->previous_task = this->current_task;
    this->task_result = (short)((int)_sym_p1);
    this->current_task = 0;
    this->m_queue_id = -1;
    int old_flags = this->flags;
    if (((int)_sym_p1) == 1) {
        succeeded = true;
        if (this->previous_task == 2) {
            this->flags &= ~0x10;
            this->flags &= ~4;
        } else if (this->previous_task == 1) {
            this->flags &= ~8;
            this->flags &= ~4;
            this->flags &= ~0x10;
            this->flags |= 2;
        }
    } else {
        if (this->previous_task == 1 && ((int)_sym_p1) == 6) {
            this->flags |= 4;
        }
        if (((int)_sym_p1) == 7 && (this->flags2 & 1) == 0) {
            if (this->previous_task == 1) {
                begin_download(this->value,
                    this->wide, this->flags2);
            } else if (this->previous_task == 2) {
                begin_upload(this->value,
                    this->wide, this->flags2);
            }
        }
    }
    if (this->m_definition != 0) {
        int merged = old_flags | (this->flags & 4);
        if (this->previous_task == 2) {
            if (this->m_definition->upload_completed_proc != 0 &&
                this->m_definition->upload_completed_proc(this) == 0)
            {
                this->flags = (unsigned short)merged;
                succeeded = false;
            }
        } else if (this->previous_task == 1) {
            if (this->m_definition->download_completed_proc != 0 &&
                this->m_definition->download_completed_proc(this) == 0)
            {
                this->flags = (unsigned short)merged;
                succeeded = false;
            }
        }
    }
    if (succeeded == false && (this->flags2 & 2) == 0) {
        bool used = use_default_data_proc();
        if (used == false) {
            if (this->previous_task == 1) {
                g_network_storage_default_data_failures = g_network_storage_default_data_failures + 1;
                begin_download(this->value,
                    this->wide, this->flags2);
            } else if (this->previous_task == 2) {
                g_network_storage_default_data_failures = g_network_storage_default_data_failures + 1;
                begin_upload(this->value,
                    this->wide, this->flags2);
            } else {
                g_network_storage_default_data_failures = g_network_storage_default_data_failures + 1;
            }
        }
    }
}

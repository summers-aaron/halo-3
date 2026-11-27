#ifndef SAVED_GAMES_STORAGE_DEVICE_H
#define SAVED_GAMES_STORAGE_DEVICE_H

#include <stdlib.h>
#include "saved_games/storage_device_types.h"
#include "cseries/async_xoverlapped_types.h"
#include "interface/controller_inline.h"

class c_storage_device;
class c_storage_device_selector_overlapped_task : public c_overlapped_task {
    c_storage_device *m_storage_device;
    e_controller_index m_controller_index;
    unsigned long m_storage_bytes;
    unsigned long *m_device_id;
    bool m_force_selection;
public:
    __forceinline c_storage_device_selector_overlapped_task(const char *file, long line) : c_overlapped_task(file, line) {}
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);
    virtual void success(unsigned long return_result);
    virtual void failure(unsigned long calling_result, unsigned long overlapped_error, unsigned long overlapped_extended_error);
    __forceinline void initialize(c_storage_device *device, e_controller_index controller, unsigned long bytes, unsigned long *id) {
        m_storage_device = device;
        m_controller_index = controller;
        m_storage_bytes = bytes;
        m_device_id = id;
        short controller_flags = (short)controller_get(controller)->flags;
        m_force_selection = (bool)(_rotl(controller_flags, 29) & 1) || (bool)(_rotl(controller_flags, 28) & 1);
    }
};
struct s_storage_device_data {
    unsigned long device_id;
    unsigned long device_type;
    unsigned __int64 total_bytes;
    unsigned __int64 free_bytes;
    wchar_t name[28];
};
class c_storage_device {
public:
    unsigned long m_flags;
public:
    unsigned long m_device_id;
private:
    e_controller_index m_controller_index;
    c_storage_device_selector_overlapped_task m_task;
public:
    __forceinline c_storage_device() : m_flags(0), m_device_id(0xcacacaca), m_controller_index(k_no_controller), m_task(__FILE__, __LINE__) {}
    __forceinline ~c_storage_device() {}
    __forceinline unsigned long device_id() const { return m_device_id; }
    __forceinline unsigned long get_device_id() const { return m_device_id; }
    void select_device(unsigned long storage_bytes);
    void storage_device_selection_succeeded(unsigned long return_result);
    static bool get_device_data(unsigned long device_id, e_storage_device_type *storage_device_type, unsigned __int64 *total_bytes,
        unsigned __int64 *free_bytes, wchar_t *name);
    __forceinline void set_device_selection_cancelled(bool cancelled) {
        if (cancelled) m_flags |= 2;
        else m_flags &= ~2;
    }
    __forceinline bool is_being_selected() const { return m_task.busy(); }
    __forceinline void release() { m_flags &= ~1; m_device_id = 0xcacacaca; }
    __forceinline void cancel_and_release() { unsigned long flags = m_flags; m_device_id = 0xcacacaca; m_flags = (flags & ~3) | 2; }
    __forceinline e_controller_index get_controller_index() const { return m_controller_index; }
    __forceinline void set_controller_index(e_controller_index controller_index) { m_controller_index = controller_index; }
    __forceinline bool is_valid() const { return (m_flags & 1) != 0; }
    __forceinline void set_valid(bool valid) {
        if (valid) m_flags |= 1;
        else m_flags &= ~1;
    }
};

#endif

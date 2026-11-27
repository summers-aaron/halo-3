#ifndef SAVED_GAMES_CONTENT_CATALOGUE_TASK_TYPES_H
#define SAVED_GAMES_CONTENT_CATALOGUE_TASK_TYPES_H

#include "cseries/async_xoverlapped_types.h"
#include "saved_games/content_item_task_types.h"

class c_content_catalogue;
struct s_content_item;
struct _XCONTENT_DATA;

class c_content_item_overlapped_task : public c_overlapped_task {
public:
    __forceinline c_content_item_overlapped_task(const char *file, long line) :
        c_overlapped_task(file, line), m_task_type(_content_item_task_type_none), m_content_item(0), m_create_disposition(0),
        m_flush_completed(0)
    {
    }
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);
    virtual void success(unsigned long return_result);
    virtual void failure(unsigned long calling_result, unsigned long overlapped_error, unsigned long overlapped_extended_error);
    virtual void complete();
    __forceinline e_content_item_task_type get_task_type() const { return m_task_type; }
    __forceinline void set_task_type(e_content_item_task_type task_type) { m_task_type = task_type; }
    __forceinline void set_content_item(s_content_item *content_item) { m_content_item = content_item; }
    __forceinline void set_content_creation_flags(long flags) { m_create_disposition = flags; }
    __forceinline void set_maximum_content_size_bytes(long bytes) { m_flush_completed = bytes; }
    __forceinline s_content_item *get_content_item() const { return m_content_item; }
private:
    unsigned long start_content_create(void *overlapped);
    long start_content_delete(void *overlapped);
    long start_content_flush(void *overlapped);
    long start_content_close(void *overlapped);
    e_content_item_task_type m_task_type;
    s_content_item *m_content_item;
    unsigned long m_create_disposition;
    unsigned long m_flush_completed;
};
class c_content_enumeration_overlapped_task : public c_overlapped_task {
public:
    __forceinline c_content_enumeration_overlapped_task(const char *file, long line) :
        c_overlapped_task(file, line), m_enumeration_handle(0), m_catalogue(0), m_controller_index(-1), m_content_type(-1),
        m_device_id(0xcacacaca), m_content_data(0), m_content_count(0), m_other_content_count(0)
    {
    }
    __forceinline void initialize(c_content_catalogue *catalogue, long controller_index, unsigned long device_id)
    {
        m_catalogue = catalogue;
        m_controller_index = controller_index;
        m_content_type = 0;
        m_device_id = device_id;
        m_content_count = 0;
        m_other_content_count = 0;
    }
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);
    virtual void success(unsigned long return_result);
    virtual void failure(unsigned long calling_result, unsigned long overlapped_error, unsigned long overlapped_extended_error);
    virtual void complete();
    virtual bool is_result_successful(unsigned long calling_result, unsigned long overlapped_error, unsigned long overlapped_extended_error);
private:
    void *m_enumeration_handle;
    c_content_catalogue *m_catalogue;
    long m_controller_index;
    long m_content_type;
    unsigned long m_device_id;
    _XCONTENT_DATA *m_content_data;
    long m_content_count;
    long m_other_content_count;
};

#endif

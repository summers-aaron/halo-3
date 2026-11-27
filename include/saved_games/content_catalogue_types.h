#ifndef SAVED_GAMES_CONTENT_CATALOGUE_TYPES_H
#define SAVED_GAMES_CONTENT_CATALOGUE_TYPES_H

struct s_content_catalogues;
extern s_content_catalogues g_content_catalogue_globals;

#include <stddef.h>
#include "saved_games/storage_device.h"
#include "saved_games/content_catalogue.h"
#include "saved_games/content_catalogue_task_types.h"
#include "saved_games/content_item_status_types.h"
#include "saved_games/content_catalogue_flush.h"
#include "saved_games/content_item_types.h"
struct s_data_array;

class c_content_catalogue {
protected:
    void dispose_content_item_array();
    void content_enumeration_success(_XCONTENT_DATA *content_data, long content_count, long other_content_count);
    friend class c_content_enumeration_overlapped_task;
public:
    c_content_catalogue();
    ~c_content_catalogue();
    s_content_item *try_and_get_item_by_file_reference(const s_file_reference *file, long *content_item_index) const;
    long try_and_get_item_by_type_and_display_name(const wchar_t *display_name);
    long try_and_get_personal_storage_item(const wchar_t *name, e_content_item_status content_item_status);
    bool begin_enumeration();
    void dispose();
    __forceinline void initialize(long controller_index);
public:
    __forceinline s_data_array *get_personal_storage_link() { return m_data; }
    __forceinline const s_content_item *get_content_item_for_deletion() const { return &m_item_for_deletion; }
    __forceinline e_controller_index get_controller_index() const { return (e_controller_index)m_controller_index; }
    __forceinline c_content_item_overlapped_task *get_content_item_task() { return &m_content_item_task; }
    __forceinline void block_until_finished(const s_content_item *item, bool first, bool second)
    { content_catalogue_block_until_finished(this, item, first, second); }
    long m_controller_index;
    s_data_array *m_data;
    s_content_item *m_items;
    s_content_item m_item_for_deletion;
    c_content_item_overlapped_task m_content_item_task;
    c_content_enumeration_overlapped_task m_enumeration_task;
    long last_content_index;
    c_synchronized_long m_lock;
    c_content_catalogue_flags m_flags;
};
struct s_content_catalogues_base {
    __forceinline s_content_catalogues_base(s_content_catalogues *self)
    {
        // match: 0x282898: /INCLUDE root blocks singleton constant-propagation of this before member ctors
        __assume(self == &g_content_catalogue_globals);
    }
};
struct s_content_catalogues : s_content_catalogues_base {
    bool initialized;
    c_content_catalogue catalogues[4];
    c_storage_device storage_devices[4];
    s_content_catalogues();
    __forceinline ~s_content_catalogues() {}
};

typedef char content_catalogue_item_task_offset_check[(sizeof(void *) != 4 || ((offsetof(c_content_catalogue, m_content_item_task) == 0x250))) ? 1 : -1];
typedef char content_catalogue_enumeration_task_offset_check[
    (sizeof(void *) != 4 || ((offsetof(c_content_catalogue, m_enumeration_task) == 0x268))) ? 1 : -1];
typedef char content_catalogues_catalogues_offset_check[(offsetof(s_content_catalogues, catalogues) == 8) ? 1 : -1];
typedef char content_catalogue_size_check[(sizeof(void *) != 4 || ((sizeof(c_content_catalogue) == 0x2a0))) ? 1 : -1];
typedef char content_catalogue_last_content_offset_check[(sizeof(void *) != 4 || ((offsetof(c_content_catalogue, last_content_index) == 0x290))) ? 1 : -1];
typedef char content_catalogue_flags_offset_check[(sizeof(void *) != 4 || ((offsetof(c_content_catalogue, m_flags) == 0x298))) ? 1 : -1];


static __forceinline s_content_catalogues &content_catalogues() { return g_content_catalogue_globals; }


#endif

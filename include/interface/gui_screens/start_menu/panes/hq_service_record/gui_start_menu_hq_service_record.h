#ifndef INTERFACE_GUI_START_MENU_HQ_SERVICE_RECORD_H
#define INTERFACE_GUI_START_MENU_HQ_SERVICE_RECORD_H

#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"

struct s_service_record_motd_chunk
{
    unsigned long text_size;
    char text[512];
};

class c_service_record_motd_downloader
{
public:
    __forceinline c_service_record_motd_downloader() :
        m_last_retry_count(m_downloader.get_retry_count()),
        m_data(0),
        m_data_size(0)
    {
    }

    __forceinline long get_data(const char **buffer, long *buffer_size) { return m_downloader.get_data(buffer, buffer_size); }
    __forceinline long get_retry_count() const { return m_downloader.get_retry_count(); }
    __forceinline c_http_buffer_downloader *buffer_downloader() { return &m_downloader; }
private:
    c_http_stored_buffer_downloader<2721> m_downloader;
public:
    long m_last_retry_count;
    const s_service_record_motd_chunk *m_data;
    long m_data_size;
};

// The service-record update owns the downloader layout; campaign owns the
// initialize and list-item handlers. Both units use this complete definition.
class c_gui_start_menu_hq_service_record : public c_start_menu_pane_screen_widget
{
public:
    __forceinline c_gui_start_menu_hq_service_record(long name) :
        c_start_menu_pane_screen_widget(name, NONE),
        m_image_applied(false),
        m_motd_tag(0),
        m_image_tag(0)
    {
    }
    virtual c_gui_bitmap_widget *create_bitmap_widget(const s_runtime_bitmap_widget_definition *definition);
    virtual void initialize();
    virtual bool handle_list_item_chosen(const c_controller_input_message *message, long list, c_gui_list_item_widget *item, c_gui_data *data);
    virtual void update(unsigned long current_milliseconds);

protected:
    virtual bool controller_event_should_go_to_parent(const c_controller_input_message *) { return true; }

public:
    c_service_record_motd_downloader m_motd_downloader;
    // The image downloader stores its own 0xf000-byte buffer: the saved-film pane models the same downloader as
    // c_http_stored_buffer_downloader<0xf000>, and nothing ever calls set_download_buffer, so the plain base
    // would have no buffer. Base plus buffer ends exactly at m_image_applied.
    c_http_stored_buffer_downloader<0xf000> m_image_downloader;
    bool m_image_applied;
    long m_motd_tag;
    long m_image_tag;
};
typedef char gui_start_menu_hq_service_record_size[
    (sizeof(void *) != 4 || sizeof(c_gui_start_menu_hq_service_record) == 0x122e8) ? 1 : -1];
typedef char gui_start_menu_hq_service_record_image_downloader_offset[
    (sizeof(void *) != 4 || offsetof(c_gui_start_menu_hq_service_record, m_image_downloader) == 0x2c48) ? 1 : -1];
typedef char gui_start_menu_hq_service_record_image_applied_offset[
    (sizeof(void *) != 4 || offsetof(c_gui_start_menu_hq_service_record, m_image_applied) == 0x122d8) ? 1 : -1];

#endif

#ifndef INTERFACE_GUI_SCREENS_CAMPAIGN_GUI_SCREEN_DOWNLOAD_WIDGET_H
#define INTERFACE_GUI_SCREENS_CAMPAIGN_GUI_SCREEN_DOWNLOAD_WIDGET_H

#include "interface/gui_screens/campaign/gui_screen_screenshot_viewer.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "networking/online/online_file_work_item_types.h"
#include <stddef.h>

class c_gui_screen_download_widget : public c_gui_screen_screenshot_viewer
{
public:
    __forceinline c_gui_screen_download_widget(long name) : c_gui_screen_screenshot_viewer(name), m_download_state(0) {}

    virtual void initialize();
    virtual void dispose();
    virtual void update(unsigned long current_milliseconds);
    virtual wchar_t *get_selected_item_title(wchar_t *output, long capacity);
    virtual wchar_t *get_selected_item_description(wchar_t *output, long capacity);

    __forceinline void set_file_share_item(unsigned __int64 owner, unsigned __int64 target, long slot, const char *filename,
        const s_online_file_description *description);

    unsigned __int64 m_owner_xuid;
    unsigned __int64 m_target_xuid;
    long m_slot;
    char m_filename[33];
    s_online_file_description m_description;
    long m_download_state;
    c_http_stored_buffer_downloader<0x1000> m_manifest_downloader;
    c_http_stored_buffer_downloader<0x4d800> m_content_downloader;
    char m_decompressed[0x4d800];
    unsigned long m_decompressed_size;
};

typedef char download_filename_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_download_widget, m_filename) == 0x1a9c))) ? 1 : -1];
typedef char download_description_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_download_widget, m_description) == 0x1ac0))) ? 1 : -1];
typedef char download_state_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_download_widget, m_download_state) == 0x1bc8))) ? 1 : -1];
typedef char download_manifest_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_download_widget, m_manifest_downloader) == 0x1bcc))) ? 1 : -1];
typedef char download_content_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_download_widget, m_content_downloader) == 0x325c))) ? 1 : -1];
typedef char download_decompressed_offset[(sizeof(void *) != 4 || ((offsetof(c_gui_screen_download_widget, m_decompressed) == 0x510ec))) ? 1 : -1];
typedef char download_screen_size[(sizeof(void *) != 4 || ((sizeof(c_gui_screen_download_widget) == 0x9e8f0))) ? 1 : -1];

#endif

#ifndef SAVED_GAMES_CONTENT_ITEM_TYPES_H
#define SAVED_GAMES_CONTENT_ITEM_TYPES_H
#include "saved_games/content_catalogue_flush.h"
#include <string.h>
#include <stddef.h>
#include "multithreading/synchronized_value.h"
#include "saved_games/game_content_types.h"
#include "saved_games/saved_game_item_metadata_packed.h"
#include "saved_games/content_catalogue.h"
#include "saved_games/content_item.h"
#include "interface/controller_types.h"
#include "tag_files/file_reference.h"
class c_content_catalogue;
#ifndef _XTL_
struct _XCONTENT_DATA
{
    unsigned long DeviceID;
    unsigned long dwContentType;
    wchar_t szDisplayName[128];
    char szFileName[42];
};
#endif
struct s_content_item {
    __forceinline s_content_item() : identifier(0), flags(0), game_content_type(_game_content_type_none), content_catalogue(0)
    {
        memset(&metadata, 0, sizeof(metadata));
        memset(&content_data, 0, sizeof(content_data));
    }
    short identifier;
    c_synchronized_long flags;
    e_game_content_type game_content_type;
    c_content_catalogue *content_catalogue;
    s_saved_game_item_metadata_packed metadata;
    _XCONTENT_DATA content_data;
    __forceinline bool is_valid() const;
    __forceinline bool is_open() const { return flags.test_bit(2); }
    __forceinline bool is_busy() const { return flags.test_bit(3); }
    __forceinline e_game_content_type get_game_content_type() const { return game_content_type; }
    __forceinline void block_until_finished();
    bool open(bool create, bool no_profile_transfer, long maximum_content_size_bytes, bool synchronous);
    friend bool content_item_create(s_content_item *item, e_game_content_type content_type, c_content_catalogue *catalogue,
        const wchar_t *name, const wchar_t *display_name, long flags, long maximum_size, bool synchronous);
    bool get_file_reference(s_file_reference *reference) const;
    bool get_directory_reference(s_file_reference *reference);
    const char *get_container_full_path(char *buffer, long buffer_size, bool drive_only) const;
    wchar_t *get_display_name(wchar_t *display_name, long display_name_size) const;
    void close(bool synchronous);
    __forceinline ~s_content_item() { close(true); }
protected:
    void set_valid(bool valid);
    __forceinline void set_open(bool open) { flags.set_bit(2, open); }
    __forceinline void set_corrupt(bool corrupt) { flags.set_bit(1, corrupt); }
    __forceinline void set_busy(bool busy) { flags.set_bit(3, busy); }
    __forceinline e_controller_index get_controller_index() const;
    __forceinline c_content_catalogue *get_content_catalogue() const { return content_catalogue; }
    void set_game_content_type(e_game_content_type content_type);
    void set_content_catalogue(c_content_catalogue *catalogue);
    void set_content_data(const _XCONTENT_DATA *data);
    bool flush(bool synchronous);
    void create_from_enumeration(e_game_content_type content_type, c_content_catalogue *catalogue, const _XCONTENT_DATA *data);
    friend class c_content_item_overlapped_task;
    friend class c_content_catalogue;
    friend void content_catalogue_flush_item(c_content_catalogue *catalogue, long content_item_index, bool synchronous);
public:
    bool make_ready_for_access(long content_item_index, bool synchronous);
};
typedef char content_item_size[(sizeof(void *) != 4 || ((sizeof(s_content_item) == 0x240))) ? 1 : -1];
typedef char content_item_metadata_offset[(sizeof(void *) != 4 || ((offsetof(s_content_item, metadata) == 0x10))) ? 1 : -1];
typedef char content_item_content_data_offset[(sizeof(void *) != 4 || ((offsetof(s_content_item, content_data) == 0x108))) ? 1 : -1];
#endif

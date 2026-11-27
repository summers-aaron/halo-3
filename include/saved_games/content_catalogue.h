#ifndef SAVED_GAMES_CONTENT_CATALOGUE_H
#define SAVED_GAMES_CONTENT_CATALOGUE_H

#include "shell/shell.h"

// Functions defined in tu/blamlibxbox/content_catalogue.cpp.


struct c_content_catalogue_flags {
    unsigned long value;
    __forceinline bool test(long bit) const { return (value & (1UL << bit)) != 0; }
    __forceinline void set(long bit, bool enabled) {
        if (enabled) value |= 1UL << bit; else value &= ~(1UL << bit);
    }
};
class c_content_catalogue;
struct s_file_reference;
struct s_saved_game_item_metadata_packed;

extern "C" void content_catalogue_close_items_for_scenario();
extern "C" void content_catalogue_dispose_all_on_content_change();
extern "C" int content_catalogue_get_next_item_index(c_content_catalogue *catalogue, int index);
extern "C" void content_catalogue_update_active_map_items(bool active);
extern "C" void content_catalogue_trim_whitespace(unsigned short *string);
#include "saved_games/content_catalogue_lookup.h"
extern "C" void content_catalogue_delete_item(c_content_catalogue *catalogue, long content_item_index, bool synchronous);
extern "C" bool content_catalogue_get_item_display_name(const c_content_catalogue *catalogue, long content_item_index, wchar_t *display_name, long size);
extern "C" bool content_catalogue_is_item_corrupt(const c_content_catalogue *catalogue, long content_item_index);
extern "C" bool content_catalogue_is_item_created(const c_content_catalogue *catalogue, long content_item_index);
extern "C" bool content_catalogue_item_metadata_matches(const c_content_catalogue *catalogue, long content_item_index,
    s_saved_game_item_metadata_packed *metadata);
extern "C" unsigned char content_catalogue_update_item_metadata(c_content_catalogue *catalogue, int content_item_index);

extern "C" void content_catalogue_dispose_all(void);
extern "C" int content_catalogue_is_name_available(c_content_catalogue *catalogue, const wchar_t *name, long content_type);
extern "C" long content_catalogue_get_remaining_slots(c_content_catalogue *catalogue, long game_content_type, long unused);
extern "C" void content_catalogue_reset_controller_state(c_content_catalogue *catalogue, long controller_index);
extern "C" void content_catalogue_close_unlisted_map_items(c_content_catalogue *catalogue, int *left, int *right, long count);
#include "saved_games/content_catalogue_device_selection.h"
extern "C" bool content_catalogue_is_item_name_valid(c_content_catalogue *catalogue, e_controller_index controller_index,
    const wchar_t *name, long content_type);

struct s_content_item;

extern "C" void content_catalogue_block_until_finished(c_content_catalogue *catalogue, const s_content_item *item, bool first, bool second);
extern "C" s_content_item *content_catalogue_try_and_get_item(const c_content_catalogue *catalogue, long content_item_index);

enum e_game_content_type;
#include "saved_games/content_catalogue_close.h"
extern "C" bool content_catalogue_has_storage_device(const c_content_catalogue *catalogue);
extern "C" bool content_catalogue_has_pending_tasks(const c_content_catalogue *catalogue);

const char *content_catalogue_build_new_saved_game_file_type_container_name(e_game_content_type content_type, char *buffer, long buffer_size);

#endif // SAVED_GAMES_CONTENT_CATALOGUE_H

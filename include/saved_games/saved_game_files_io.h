#ifndef SAVED_GAMES_SAVED_GAME_FILES_IO_H
#define SAVED_GAMES_SAVED_GAME_FILES_IO_H

class c_synchronized_long;
struct s_buffered_file_writer;

extern "C" long saved_game_files_update_metadata_async(long controller_index, long content_item_index, const wchar_t *name,
    const wchar_t *description, c_synchronized_long *completion, c_synchronized_long *done);
extern "C" bool buffered_file_write(s_buffered_file_writer *writer, const void *data, long size);

#endif // SAVED_GAMES_SAVED_GAME_FILES_IO_H

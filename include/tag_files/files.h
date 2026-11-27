#ifndef TAG_FILES_FILES_H
#define TAG_FILES_FILES_H

// Functions defined in tu/blamlibxbox/files.cpp.

struct s_file_reference;

s_file_reference *file_reference_create_from_path(s_file_reference *reference, const char *path, bool is_directory);
s_file_reference *file_reference_set_name(s_file_reference *reference, const char *name);
char *file_reference_get_name(const s_file_reference *reference, unsigned long parts, char *buffer, long size);
bool file_create_parent_directories_if_not_present(const s_file_reference *reference);
extern "C" void file_initialize_save_directory(void);

extern "C" {
extern const char k_saved_game_autosave_directory[];
}

#endif // TAG_FILES_FILES_H

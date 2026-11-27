#ifndef TAG_FILES_FILES_WINDOWS_H
#define TAG_FILES_FILES_WINDOWS_H

// Functions defined in tu/blamlibxbox/files_windows.cpp that other units call.

struct s_file_last_modification_date;
struct s_file_reference;
struct s_find_file_data;

bool file_exists(const s_file_reference *reference);
bool file_rename(s_file_reference *reference, const char *new_name);
bool file_copy_to(const s_file_reference *source, const s_file_reference *destination, bool overwrite);
bool file_close(s_file_reference *reference);
bool file_set_position(s_file_reference *reference, unsigned long position, bool suppress_errors);
unsigned long file_get_eof(const s_file_reference *reference);
bool file_read(s_file_reference *reference, unsigned long size, bool suppress_errors, void *buffer);
bool file_write(s_file_reference *reference, unsigned long size, const void *buffer);
bool file_get_size(const s_file_reference *reference, unsigned long *size);
void find_files_start(s_find_file_data *find_file_data, unsigned long flags, const s_file_reference *file_reference, const char *search_spec);
void find_files_end(s_find_file_data *find_file_data);
bool find_files_next(s_find_file_data *find_file_data, s_file_reference *file_reference, s_file_last_modification_date *file_last_modification_date);
void file_path_remove_name(char *path);
void file_path_split(char *path, char **drive, char **directory, char **root, char **extension, bool is_file);
void file_location_get_full_path(short location, const char *path, char *full_path, long full_path_length);
bool file_create(s_file_reference *reference);
bool file_delete(s_file_reference *reference);
bool file_delete_recursive(s_file_reference *reference);

struct named_object;

extern "C" void file_path_add_name(char *path, long maximum_path_length, const char *name);
extern "C" void file_error(const char *function_name, named_object *first, named_object *second, bool quiet);
extern "C" bool file_is_cache_path(const char *path);
bool file_open(s_file_reference *reference, unsigned long flags, unsigned long *error);

#endif // TAG_FILES_FILES_WINDOWS_H

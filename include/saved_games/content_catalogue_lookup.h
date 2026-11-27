#ifndef SAVED_GAMES_CONTENT_CATALOGUE_LOOKUP_H
#define SAVED_GAMES_CONTENT_CATALOGUE_LOOKUP_H

class c_content_catalogue;
struct s_file_reference;

extern "C" long content_catalogue_find_item_by_file_reference(const c_content_catalogue *catalogue, const s_file_reference *file);

#endif // SAVED_GAMES_CONTENT_CATALOGUE_LOOKUP_H

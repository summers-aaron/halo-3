#ifndef SAVED_GAMES_CONTENT_CATALOGUE_CLOSE_H
#define SAVED_GAMES_CONTENT_CATALOGUE_CLOSE_H

class c_content_catalogue;

extern "C" void content_catalogue_close_item(c_content_catalogue *catalogue, long content_item_index, bool synchronous);

#endif // SAVED_GAMES_CONTENT_CATALOGUE_CLOSE_H

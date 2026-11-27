#ifndef SAVED_GAMES_CONTENT_CATALOGUE_FLUSH_H
#define SAVED_GAMES_CONTENT_CATALOGUE_FLUSH_H

class c_content_catalogue;
extern "C" void content_catalogue_flush_item(c_content_catalogue *catalogue, long content_item_index, bool synchronous);

#endif

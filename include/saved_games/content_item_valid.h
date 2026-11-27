#ifndef SAVED_GAMES_CONTENT_ITEM_VALID_H
#define SAVED_GAMES_CONTENT_ITEM_VALID_H

__forceinline bool s_content_item::is_valid() const
{
    bool valid = false;
    if (flags.test_bit(0)) {
        if (content_catalogue != 0 &&
            (identifier != 0 || content_catalogue->get_content_item_for_deletion() == this))
            valid = true;
    }
    return valid;
}

#endif

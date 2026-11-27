// blamlibxbox/data_mining.obj: from the records 0x2197a0
// @flags /TP /O2
#include "cseries/data_mining_event_types.h"

// @address 0x2197a0 ??0c_datamine@@QAA@JPBDJZZ
c_datamine::c_datamine(long level, const char *event_name, long category_count, ...)
{
    this->level = level;
    this->category_count = 0;
    format = 0;
    output = 0;
    event = 0;
    position = 0;
}

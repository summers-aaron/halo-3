// interfacelibxbox/chud_globals_definitions.obj: from the records 0x186b80
// @flags /TP /O2
#include "cseries/cseries.h"
#include "interface/chud/chud.h"
#include "tag_files/tag_types.h"

struct s_chud_skin_color_table {
  long row_count;
  TAG_POINTER(long) values;                    // 130 per row, the first unused
};

// The table's value at (row, column), both clamped (0 when the table is empty). The clamps are
// MIN(MAX(...)) macros evaluate the clamp operands twice; the first parameter is unused.
// @address 0x186b80 ?chud_globals_get_skin_color@@YAKPBUs_chud_globals_definition@@JJ@Z
unsigned long chud_globals_get_skin_color(const s_chud_globals_definition *definition, long row, long column)
{
  // match: 0x186b80: taking the parameter address preserves its home stack store.
  const s_chud_globals_definition * const *unused_address = &definition;
  (void)unused_address;
  s_chud_skin_color_table *table = ((s_chud_skin_color_table *)chud_globals);
  long result = 0;
  row = MIN(MAX(row, 0), table->row_count - 1);
  column = MIN(MAX(column, 0), 27);
  if (table->row_count > 0)
    result = table->values[row * 130 + column + 1];
  return (unsigned long)result;
}

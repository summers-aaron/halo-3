// interfacelibxbox/gui_game_browser_search_criteria.obj: from the records 0x4e8238
// @flags /TP /O2
// @address 0x4e8238 ?get_current_model_element@c_gui_model_widget@@QAAPAUs_runtime_model_widget_element@@XZ

#include "interface/gui_screens/game_browser/gui_game_browser_search_criteria.h"
#include "interface/gui_model_widget_types.h"
__forceinline s_runtime_model_widget_element *c_gui_model_widget::find_model_element(long key)
{
  s_runtime_model_widget_element *result = 0;
  for (unsigned long i = 0; i < 2; i++) {
    if (m_definition.elements[i].identifier == key) {
      result = &m_definition.elements[i];
      break;
    }
  }
  return result;
}

// The slot whose identifier matches the mode's key (197, 198 or the stored one), else the first slot.
s_runtime_model_widget_element *c_gui_model_widget::get_current_model_element()
{
  long key;
  switch (m_player_appearance.player_model_choice) {
  case 0:
    key = 197;
    break;
  case 1:
    key = 198;
    break;
  default:
    key = m_name;
    break;
  }
  s_runtime_model_widget_element *result = m_definition.elements;
  s_runtime_model_widget_element *slot = find_model_element(key);
  if (slot)
    result = slot;
  return result;
}

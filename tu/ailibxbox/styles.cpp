// ailibxbox/styles.obj: from the records 0xb4710
// @flags /TP /O2
#include "memory/tls_globals.h"
#include "ai/styles.h"
#include "memory/data_types.h"
#include "tag_files/tag_types.h"
#include "ai/squad_types.h"
#include "ai/actor_datum.h"
#include "ai/command_script_data.h"
#include "tag_files/tag_groups.h"
static __forceinline style_definition *tag_get_or_null(int tag_index)
{
  style_definition *definition = 0;
  if (tag_index != -1) {
    definition = (style_definition *)tag_get(NONE, tag_index);
  }
  return definition;
}

// @address 0xb4710 ?actor_get_task_style@@YAPAUstyle_definition@@J@Z
style_definition *actor_get_task_style(long actor_index)
{
  style_definition *definition = 0;
  actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
  if (actor->command_index != -1) {
    command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), actor->command_index);
    if (script->style_index_valid) {
      definition = (style_definition *)tag_get(NONE, script->style_index);
    }
  }
  if (definition == 0 && actor->squad_index != -1) {
    squad_datum *squad = datum_get<squad_datum>(tls_squad_data(), actor->squad_index);
    definition = tag_get_or_null(squad->style_index);
  }
  return definition;
}

// @address 0x72d070 ?global_style_status_mapping@@3PAFA
short global_style_status_mapping[] = { 1, 2, 3 };

// @address 0x72d078 ?global_attitude_names@@3PAPBDA
const char *global_attitude_names[] = {
    "Normal",
    "Timid",
    "Aggressive",
};

// @address 0x72d084 ?global_subattitude_names@@3PAPBDA
const char *global_subattitude_names[] = {
    "Default",
    "Normal",
    "Timid",
    "Aggressive",
};

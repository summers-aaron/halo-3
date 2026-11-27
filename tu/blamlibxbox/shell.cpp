// blamlibxbox/shell.obj: from the records 0x1980f0
// @flags /TP /O2 /Oc

#include <string.h>

#include "shell/shell.h"

// @address 0x3adc8 shell_compatible_build_string
// @rdata 0x8203adc8..0x8203ade8
static const char shell_compatible_build_string[32] = "11855.07.08.20.2317.halo3_ship";
// @rdata-end

// The single compatible build: number 11855 with its full build string.
// @address 0x7c5d2c g_shell_compatible_builds
shell_name_table g_shell_compatible_builds = { { { 11855, shell_compatible_build_string } } };

// @address 0x8a1eac g_shell_application_paused
bool g_shell_application_paused;


// @address 0x1980f0 shell_build_string_is_compatible
extern "C" unsigned char shell_build_string_is_compatible(const char *name)
{
  unsigned int i;
  bool result = false;

  for (i = 0; i < 1; i++) {
    if (strcmp(name, g_shell_compatible_builds.entries[i].name) == 0) {
      result = true;
      break;
    }
  }
  return result;
}

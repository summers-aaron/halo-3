// hkconstraintsolver/hkSolverCheck.obj: from the records 0x677420 0x677460 0x6777e0
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x677420 ?find_char@@YAPBDPBDH@Z
// @address 0x6777e0 out_of_line

#include <string.h>
#include "phys/phys_err_stream.h"
#include "phys/phys_solver_check.h"
#include "phys/phys_solver_time_date.h"
// @address 0x7ceb74 g_phys_solver_check_count
extern "C" {
// @address 0x17404 k_phys_solver_key_masks
const int k_phys_solver_key_masks[12] = {
    0x72e6ef51, 0x6e453245, 0x176af579, 0x3641e1ef,
    0x159a9fb2, 0x4f2a6a81, 0x4caea74a, 0xe227fec2,
    0x89c57fec, 0x32749123, 0x563a5b70, 0x32269fe2
};
}
// @address 0x72643c phys_component_strings
const char *phys_component_strings[] = {
    "Constraint Solver",
    "Ragdoll Technology",
    "Vehicles",
    "Continuous Physics",
    "MOPP(tm) Mesh Compressor",
    "Behavior"
};

const char *find_char(const char *text, int character)
{
    const char target = (char)character;
    while (*text != target) {
        if (*text == '\0')
            return 0;
        ++text;
    }
    return text;
}

extern "C" {
// match: 0x677460: named storage preserves runtime strlen; owner: ??_C@_0DL@LJNMPJIO@The?5following?5component?5is?5not?5e@
// @address 0x175c4 k_havok_prime_component_not_enabled_message
// @rdata 0x820175c4..0x82017600
const char k_havok_prime_component_not_enabled_message[] = "The following component is not enabled in Havok Prime:\n\n\t\t";
// @rdata-end
}

// @address 0x677460 process_flying_colors
extern "C" signed char process_flying_colors(int product)
{
    unsigned char key_value_high = (unsigned char)(PHYS_KEYVALUE >> 31);
    for (int index = 0; index < 12; ++index) {
        int key = k_phys_solver_key_masks[index];
        const char *keycode_first_dot = find_char(PHYS_KEYCODE, '.');
        if (keycode_first_dot == 0) {
            phys_error_message("Havok Physics evaluation key has expired or is invalid.\n"
                    "Please contact Havok.com for an extension.\n"
                    "No simulation possible.");
            return 0;
        }
        if (strstr(keycode_first_dot, "Prime") != 0) {
            if (product == 1 || product == 2) {
                char message[256];
                message[0] = 0;
                strncat(message, k_havok_prime_component_not_enabled_message,
                    strlen(k_havok_prime_component_not_enabled_message) < 254 - strlen(message)
                        ? strlen(k_havok_prime_component_not_enabled_message)
                        : 254 - strlen(message));
                strncat(message, phys_component_strings[product],
                    strlen(k_havok_prime_component_not_enabled_message) < 254 - strlen(message)
                        ? strlen(k_havok_prime_component_not_enabled_message)
                        : 254 - strlen(message));
                phys_error_message(message);
            }
        }
        const char *second_dot = find_char(keycode_first_dot + 1, '.');
        if (second_dot == 0) {
            phys_error_message("Havok Physics evaluation key has expired or is invalid.\n"
                    "Please contact Havok.com for an extension.\n"
                    "No simulation possible.");
            return 0;
        }
        const char *p = second_dot + 1;
        if (key_value_high != 0) {
            int key_expiration = (int)((PHYS_KEYVALUE & 0x7fffffff) ^ (unsigned int)key);
            __int64 now = phys_solver_system_clock();
            if ((__int64)key_expiration <= now || (__int64)key_expiration - now >= 0x209a5ad)
                continue;
            if (product == 1 || product == 2) {
                if ((unsigned int)(index - 6) <= 3) {
                    phys_error_message("Product mismatch: Prime keyvalue detected in non prime keycode.\n"
                    "Please check your keycode or contact your Havok Account Manager.");
                    return 0;
                }
            }
            if (product != 4 && index >= 10) {
                phys_error_message("Product mismatch: FX keyvalue detected in non FX keycode.\n"
                    "Please check your keycode or contact your Havok Account Manager.");
                return 0;
            }
            return 1;
        }
        else {
            unsigned int hash = 0;
            if (*p != 0) {
                do {
                    hash = hash * 23 + *++p;
                } while (*p != 0);
            }
            if (PHYS_KEYVALUE == ((hash ^ (unsigned int)key) & 0x7fffffff)) {
                if (product == 1 || product == 2) {
                    if ((unsigned int)(index - 6) <= 3) {
                        phys_error_message("Product mismatch: Prime keyvalue detected in non prime keycode.\n"
                    "Please check your keycode or contact your Havok Account Manager.");
                        return 0;
                    }
                }
                if (product != 4 && index >= 10) {
                    phys_error_message("Product mismatch: FX keyvalue detected in non FX keycode.\n"
                    "Please check your keycode or contact your Havok Account Manager.");
                    return 0;
                }
                return 1;
            }
        }
    }
    phys_error_message("Havok Physics evaluation key has expired or is invalid.\n"
                    "Please contact Havok.com for an extension.\n"
                    "No simulation possible.");
    return 0;
}

extern "C" void out_of_line(void)
{
    ++g_phys_solver_check_count;
}


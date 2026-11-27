#ifndef INTERFACE_GUI_WIDGET_TYPES_H
#define INTERFACE_GUI_WIDGET_TYPES_H

#include "math/real_math_types.h"
#include "shell/shell.h"
#include "tag_files/tag_types.h"
#include <string.h>

#include "interface/gui_widget_enums.h"

class c_gui_widget;
template<typename t_type> class c_robust_pointer;
extern "C" c_robust_pointer<c_gui_widget> *robust_pointer_initialize_to_null(c_robust_pointer<c_gui_widget> *pointer, c_gui_widget *value);

template<typename t_type>
class c_robust_pointer {
public:
    // The constructor initializes each redundant copy of the stored pointer.
    __forceinline c_robust_pointer(t_type *value = 0) { robust_pointer_initialize_to_null(this, value); }
    void *m_values[3];
};

#pragma pack(push, 2)
struct s_gui_rectangle {
    union {
        short values[4];
        struct {
            short top;
            short left;
            short bottom;
            short right;
        };
        struct {
            short y0;
            short x0;
            short y1;
            short x1;
        };
    };
};
#pragma pack(pop)

struct s_gui_tag_reference {
    unsigned long group;
    TAG_POINTER(const char) name;
    long name_length;
    long index;
    __forceinline s_gui_tag_reference() { memset(this, 0, sizeof(*this)); }
#if defined(_WIN64) || defined(__LP64__)
    __forceinline s_gui_tag_reference(unsigned long tag) : group(tag), name_length(0), index(-1) { name = ""; }
#else
    __forceinline s_gui_tag_reference(unsigned long tag) : group(tag), name(""), name_length(0), index(-1) {}
#endif
};

union gui_real_rectangle2d {
    real_point2d points[4];

    static void transform_real_point3d(real_point3d *point, const rectangle2d *rectangle);
    gui_real_rectangle2d *rotate_about_local_point(const real_point2d *origin, float angle, float aspect);
    gui_real_rectangle2d *scale_about_local_point(const real_point2d *origin, const real_vector2d *scale);
};

struct s_gui_widget_render_data {
    long type;
    unsigned long flags;
    union {
        gui_real_rectangle2d projected_bounds;
        real_point2d corners[4];
    };
    e_controller_index local_controller_index;
};

typedef char gui_wtypes_size_robust[(sizeof(void *) != 4 || ((sizeof(c_robust_pointer<void>) == 12))) ? 1 : -1];
typedef char gui_wtypes_size_rect[(sizeof(s_gui_rectangle) == 8) ? 1 : -1];
typedef char gui_wtypes_size_tagref[(sizeof(s_gui_tag_reference) == 16) ? 1 : -1];
typedef char gui_wtypes_size_realrect[(sizeof(gui_real_rectangle2d) == 0x20) ? 1 : -1];
typedef char gui_wtypes_size_rdata[(sizeof(s_gui_widget_render_data) == 0x2c) ? 1 : -1];

#endif // INTERFACE_GUI_WIDGET_TYPES_H

#ifndef OBJECTS_WIDGETS_MUFFINS_H
#define OBJECTS_WIDGETS_MUFFINS_H

// Functions defined in tu/objectslibxbox/muffins.cpp.

#include <stddef.h>
#include "math/matrix_math_types.h"
#include "cache/restricted_memory_regions.h"

struct s_data_array;
struct s_muffin_definition_piece;

void muffins_update_base_address(void *object);

// The muffin data allocator (restricted region 3), defined in muffins.cpp.
extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &muffins_update_base_address> g_muffin_data_allocator;

// The muffin data array (thread-local slot; muffins_initialize creates it, "muffin", 10 x 0x1890).
extern "C" {
extern __declspec(thread) s_data_array *g_muffin_data;
}
static __forceinline s_data_array *&tls_muffin_data() { return g_muffin_data; }

struct s_muffin_piece
{
    unsigned char flags;
    unsigned char node_index;
    unsigned char object_node_index;
    unsigned char mesh_index;
    unsigned short countdown;
    unsigned char scale;
    long marker_name;
    long start_time;
    real_point3d position;
    real_vector3d velocity;
    real_matrix4x3 matrix;
    real_matrix4x3 render_matrices[2];
};

struct s_muffin_datum
{
    short identifier;
    unsigned char flags;
    unsigned char piece_count;
    s_muffin_piece pieces[32];
    long definition_index;
    long object_index;
    float time;

    static s_muffin_datum *get(long muffin_index);
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char muffin_piece_size[(sizeof(s_muffin_piece) == 0xc4) ? 1 : -1];
typedef char muffin_piece_matrix_offset[(offsetof(s_muffin_piece, matrix) == 0x28) ? 1 : -1];
typedef char muffin_datum_size[(sizeof(s_muffin_datum) == 0x1890) ? 1 : -1];
typedef char muffin_datum_object_offset[(offsetof(s_muffin_datum, object_index) == 0x1888) ? 1 : -1];
#endif

void muffins_dispose();
void __tls_set_g_muffin_data_allocator(void *object);
void muffins_transfer_to_new_object(long old_object_index, long new_object_index);
bool object_has_muffins(long object_index);
void muffins_initialize(void);
void muffins_delete(long muffin_index);
long muffins_new(long object_index, long widget_index, real_matrix4x3 *matrix);
void muffins_render_debug(long object_index, long muffin_index);
void muffins_render_opaque(long muffin_index, long entry_point);
bool muffins_get_is_transparent(long muffin_index);
void muffins_update(float dt);
void muffins_transfer(long widget_index, long old_object_index, long new_object_index);

extern "C" void muffins_destroy_piece_with_effect(s_muffin_piece *piece, s_muffin_datum *muffin,
    const s_muffin_definition_piece *piece_definition);
extern "C" void muffins_initialize_data_for_new_map();
extern "C" void muffins_dispose_data_from_old_map();
bool muffin_create(long muffin_index, long widget_index);

#endif // OBJECTS_WIDGETS_MUFFINS_H

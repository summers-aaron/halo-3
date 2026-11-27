#ifndef STRUCTURES_STRUCTURES_H
#define STRUCTURES_STRUCTURES_H

#include "objects/cluster_reference.h"

// Functions defined in tu/blamlibxbox/structures.cpp that other units call.

struct cluster_portal;
union real_point3d;
struct s_cluster_reference;
struct s_game_cluster_bit_vectors;
struct structure_bsp;

void structure_instanced_geometry_marker_begin();
bool structure_instanced_geometry_mark(long bsp_index, long instanced_geometry_index);
float structure_distance_to_portal(const structure_bsp *bsp, const cluster_portal *portal, const real_point3d *point);
long structure_clusters_in_bitvector(const s_game_cluster_bit_vectors *vectors, long maximum_count, s_cluster_reference *references, long *total_count);
long structure_clusters_in_sphere(s_cluster_reference cluster_reference, const real_point3d *sphere_center, float sphere_radius,
    long maximum_cluster_count, s_cluster_reference *cluster_references, long *cluster_count);
extern "C" unsigned char structure_instanced_geometry_mark_get_user_data(int bsp_index, int instanced_geometry_index, int *user_data);

union real_vector3d;

long structure_clusters_in_cone(s_cluster_reference reference, const real_point3d *point, const real_vector3d *direction, float height,
    float sine, float cosine, long maximum_count, s_cluster_reference *references, long *total_count);

void structures_initialize(void);
void structures_dispose(void);
void structures_initialize_for_new_map(void);
void structures_dispose_from_old_map(void);
void structures_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask);
void structures_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask);
#endif // STRUCTURES_STRUCTURES_H

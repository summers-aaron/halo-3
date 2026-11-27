#ifndef PHYSICS_PHYSICS_MODEL_DEFINITION_TYPES_H
#define PHYSICS_PHYSICS_MODEL_DEFINITION_TYPES_H

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "game/materials.h"
#include "physics/physics_model_constraint_definition.h"
#include "physics/physics_model_constraint_reference.h"

class phys_shape;
struct physics_model_material;

struct s_physics_model_powered_chain
{
    s_tag_block nodes;
    s_tag_block links;
};

struct s_physics_model_motor_reference { short motor_type; short motor_index; };

struct s_physics_model_powered_chain_link
{
    s_physics_model_constraint_reference constraint;
    s_physics_model_motor_reference motors[3];
};

struct s_physics_model_ragdoll_motors
{
    s_physics_model_motor_reference twist_motor;
    s_physics_model_motor_reference cone_motor;
    s_physics_model_motor_reference plane_motor;
};

struct s_physics_model_node_constraint_edge
{
    c_global_material_type material_index_a;
    c_global_material_type material_index_b;
    short node_a;
    short node_b;
    s_tag_block constraints;
    long material_name_a;
    long material_name_b;
};
typedef char physics_node_edge_material_b_offset[(offsetof(s_physics_model_node_constraint_edge, material_index_b) == 2) ? 1 : -1];
typedef char physics_node_edge_size[(sizeof(s_physics_model_node_constraint_edge) == 28) ? 1 : -1];

struct s_physics_model_node_constraint_edge_constraint {  /* 36 bytes: an edge's constraint reference plus its friction override */
    short type;
    short index;
    long flags;                                             /* bit 2: not created with the component */
    float max_friction_torque;                              /* 0 = use the constraint definition's */
    s_tag_block ragdoll_motors;                             /* s_physics_model_ragdoll_motors */
    s_tag_block limited_hinge_motors;                       /* s_physics_model_motor_reference */
};

struct s_physics_model_limited_hinge_constraint
{
    s_physics_model_constraint_bodies bodies;
    // Reserved bytes in the serialized constraint record.
    char pad74[4];
    float max_friction_torque;
    float min_angle;
    float max_angle;
};

struct s_physics_model_ragdoll_constraint
{
    s_physics_model_constraint_bodies bodies;
    // Reserved bytes in the serialized constraint record.
    char pad74[4];
    float min_twist;
    float max_twist;
    float min_cone;
    float max_cone;
    float min_plane;
    float max_plane;
    float max_friction_torque;
};

struct s_physics_model_stiff_spring_constraint
{
    s_physics_model_constraint_bodies bodies;
    // Reserved bytes in the serialized constraint record.
    char pad74[4];
    float spring_length;
};

struct s_physics_model_prismatic_constraint
{
    s_physics_model_constraint_bodies bodies;
    // Reserved bytes in the serialized constraint record.
    char pad74[4];
    float min_limit;
    float max_limit;
    float max_friction_force;
};

struct s_physics_model_rigid_body;

struct s_physics_model_node
{
    long name;
    unsigned short flags;
    short parent_node_index;
    short sibling_node_index;
    short child_node_index;
};

struct s_physics_model_phantom
{
    long fallback_name;
    long name;
    short phantom_type_index;
    unsigned char proxy_collision_group;
    unsigned char runtime_collision_group;
};

struct s_physics_model_phantom_type {
    unsigned long flags;
    signed char minimum_rigid_body_size, maximum_rigid_body_size;
    long marker_name, alignment_marker_name;
    // Reserved bytes in the serialized phantom type.
    char pad10[8];
    float linear_exponent, linear_dead_zone;
    float radial_acceleration, radial_maximum_velocity;
    float centripetal_acceleration, centripetal_maximum_velocity;
    float axial_acceleration, axial_maximum_velocity;
    // Reserved bytes in the serialized phantom type.
    char pad38[0x54 - 0x38];
    float angular_exponent, angular_acceleration, angular_maximum_velocity;
    // Reserved bytes in the serialized phantom type.
    char pad60[8];
};



#ifdef HALO3_64BIT
struct physics_sphere_record { char prefix[0x50]; char shape[0x50]; };
struct physics_multi_sphere_record { char prefix[0x20]; char shape[0xB0]; };
struct physics_capsule_record { char prefix[0x20]; char shape[0x50]; };
struct physics_box_record { char prefix[0x60]; char shape[0x80]; };
struct physics_triangle_record { char prefix[0x20]; char shape[0x70]; };
struct physics_convex_record { char prefix[0x20]; char shape[0x80]; };
struct physics_list_record { char shape[0x60]; };
struct physics_mopp_record { char shape[0x40]; };
struct physics_phantom_record { char shape[0x58]; };
#else
struct physics_sphere_record { char prefix[64]; char shape[48]; };
struct physics_multi_sphere_record { char prefix[32]; char shape[160]; };
struct physics_capsule_record { char prefix[32]; char shape[64]; };
struct physics_box_record { char prefix[80]; char shape[96]; };
struct physics_triangle_record { char prefix[32]; char shape[96]; };
struct physics_convex_record { char prefix[32]; char shape[96]; };
struct physics_list_record { char shape[80]; };
struct physics_mopp_record { char shape[32]; };
struct physics_phantom_record { char shape[44]; };
#endif

// Physics model tag blocks.
struct s_physics_model_definition
{
    unsigned long flags;
    float mass;
    float deactivator_scale_a;
    float deactivator_scale_b;
    float custom_shape_radius;
    float maximum_penetration_depth_scale;
    signed char version; // serialized version, read by physics_model_fixup
    s_tag_block spring_motors;
    s_tag_block position_motors;
    s_tag_block phantom_types;
    s_tag_block powered_chains;
    s_tag_block node_edges;
    c_typed_tag_block<s_physics_model_rigid_body> rigid_bodies;
    union
    {
        s_tag_block phantoms;
        c_typed_tag_block<physics_model_material> materials;
    };
    c_typed_tag_block<physics_sphere_record> spheres;
    c_typed_tag_block<physics_multi_sphere_record> multi_spheres;
    c_typed_tag_block<physics_capsule_record> capsules;
    c_typed_tag_block<physics_box_record> boxes;
    c_typed_tag_block<physics_triangle_record> triangles;
    c_typed_tag_block<physics_convex_record> convex_vertices;
    s_tag_block polyhedron_vertices;
    s_tag_block polyhedron_planes;
    s_tag_block mass_distributions;
    c_typed_tag_block<physics_list_record> lists;
    s_tag_block list_children;
    c_typed_tag_block<physics_mopp_record> mopps;
    s_tag_data mopp_codes;
    s_tag_block hinge_constraints;
    s_tag_block ragdoll_constraints;
    s_tag_block regions;
    s_tag_block nodes;
    s_tag_block errors;
    s_tag_block point_to_path_curves;
    s_tag_block limited_hinge_constraints;
    s_tag_block ball_and_socket_constraints;
    s_tag_block stiff_spring_constraints;
    s_tag_block prismatic_constraints;
    c_typed_tag_block<physics_phantom_record> phantom_shapes;
};

typedef char physics_model_version_offset[(offsetof(s_physics_model_definition, version) == 0x18) ? 1 : -1];
typedef char physics_model_materials_offset[(offsetof(s_physics_model_definition, materials) == 0x64) ? 1 : -1];
typedef char physics_model_polyhedron_vertices_offset[(offsetof(s_physics_model_definition, polyhedron_vertices) == 0xb8) ? 1 : -1];
typedef char physics_model_polyhedron_planes_offset[(offsetof(s_physics_model_definition, polyhedron_planes) == 0xc4) ? 1 : -1];
typedef char physics_model_list_children_offset[(offsetof(s_physics_model_definition, list_children) == 0xe8) ? 1 : -1];
typedef char physics_model_mopp_codes_offset[(offsetof(s_physics_model_definition, mopp_codes) == 0x100) ? 1 : -1];

typedef char physics_model_rigid_bodies_offset[(offsetof(s_physics_model_definition, rigid_bodies) == 0x58) ? 1 : -1];
typedef char physics_model_nodes_offset[(offsetof(s_physics_model_definition, nodes) == 0x138) ? 1 : -1];
typedef char physics_model_definition_size[(sizeof(void *) != 4 || sizeof(s_physics_model_definition) == 0x198) ? 1 : -1];


// Physics model record sizes.
typedef char s_physics_model_limited_hinge_constraint_size[(sizeof(s_physics_model_limited_hinge_constraint) == 132) ? 1 : -1];
typedef char s_physics_model_motor_reference_size[(sizeof(s_physics_model_motor_reference) == 4) ? 1 : -1];
typedef char s_physics_model_powered_chain_link_size[(sizeof(s_physics_model_powered_chain_link) == 16) ? 1 : -1];
typedef char s_physics_model_node_size[(sizeof(s_physics_model_node) == 12) ? 1 : -1];
typedef char s_physics_model_node_constraint_edge_size[(sizeof(s_physics_model_node_constraint_edge) == 28) ? 1 : -1];
typedef char s_physics_model_node_constraint_edge_constraint_size[(sizeof(s_physics_model_node_constraint_edge_constraint) == 36) ? 1 : -1];
typedef char s_physics_model_phantom_size[(sizeof(s_physics_model_phantom) == 12) ? 1 : -1];
typedef char physics_phantom_type_marker_offset[(offsetof(s_physics_model_phantom_type, marker_name) == 8) ? 1 : -1];
typedef char physics_phantom_type_linear_exponent_offset[(offsetof(s_physics_model_phantom_type, linear_exponent) == 24) ? 1 : -1];
typedef char s_physics_model_phantom_type_size[(sizeof(s_physics_model_phantom_type) == 104) ? 1 : -1];
typedef char s_physics_model_powered_chain_size[(sizeof(s_physics_model_powered_chain) == 24) ? 1 : -1];
typedef char s_physics_model_prismatic_constraint_size[(sizeof(s_physics_model_prismatic_constraint) == 132) ? 1 : -1];
typedef char s_physics_model_ragdoll_constraint_size[(sizeof(s_physics_model_ragdoll_constraint) == 148) ? 1 : -1];
typedef char s_physics_model_ragdoll_motors_size[(sizeof(s_physics_model_ragdoll_motors) == 12) ? 1 : -1];
typedef char s_physics_model_stiff_spring_constraint_size[(sizeof(s_physics_model_stiff_spring_constraint) == 124) ? 1 : -1];

typedef char physics_powered_chain_links_offset[(offsetof(s_physics_model_powered_chain, links) == 0xc) ? 1 : -1];
typedef char physics_node_edge_material_name_a_offset[(offsetof(s_physics_model_node_constraint_edge, material_name_a) == 0x14) ? 1 : -1];
typedef char physics_node_edge_material_name_b_offset[(offsetof(s_physics_model_node_constraint_edge, material_name_b) == 0x18) ? 1 : -1];
typedef char physics_limited_hinge_friction_offset[(offsetof(s_physics_model_limited_hinge_constraint, max_friction_torque) == 0x78) ? 1 : -1];
typedef char physics_ragdoll_twist_offset[(offsetof(s_physics_model_ragdoll_constraint, min_twist) == 0x78) ? 1 : -1];
typedef char physics_stiff_spring_length_offset[(offsetof(s_physics_model_stiff_spring_constraint, spring_length) == 0x78) ? 1 : -1];
typedef char physics_prismatic_limit_offset[(offsetof(s_physics_model_prismatic_constraint, min_limit) == 0x78) ? 1 : -1];
typedef char physics_model_mass_offset[(offsetof(s_physics_model_definition, mass) == 4) ? 1 : -1];
typedef char physics_model_mass_distributions_offset[(offsetof(s_physics_model_definition, mass_distributions) == 0xd0) ? 1 : -1];
typedef char physics_model_regions_offset[(offsetof(s_physics_model_definition, regions) == 0x12c) ? 1 : -1];

// Serialized gaps are part of the record layouts.
typedef char physics_limited_hinge_padding_offset[(offsetof(s_physics_model_limited_hinge_constraint, pad74) == 0x74) ? 1 : -1];
typedef char physics_ragdoll_padding_offset[(offsetof(s_physics_model_ragdoll_constraint, pad74) == 0x74) ? 1 : -1];
typedef char physics_stiff_spring_padding_offset[(offsetof(s_physics_model_stiff_spring_constraint, pad74) == 0x74) ? 1 : -1];
typedef char physics_prismatic_padding_offset[(offsetof(s_physics_model_prismatic_constraint, pad74) == 0x74) ? 1 : -1];
typedef char physics_phantom_marker_padding_offset[(offsetof(s_physics_model_phantom_type, pad10) == 0x10) ? 1 : -1];
typedef char physics_phantom_linear_padding_offset[(offsetof(s_physics_model_phantom_type, pad38) == 0x38) ? 1 : -1];
typedef char physics_phantom_angular_padding_offset[(offsetof(s_physics_model_phantom_type, pad60) == 0x60) ? 1 : -1];
typedef char physics_model_errors_offset[(offsetof(s_physics_model_definition, errors) == 0x144) ? 1 : -1];
typedef char physics_model_point_to_path_curves_offset[(offsetof(s_physics_model_definition, point_to_path_curves) == 0x150) ? 1 : -1];

typedef char physics_phantom_type_index_offset[(offsetof(s_physics_model_phantom, phantom_type_index) == 8) ? 1 : -1];
typedef char physics_phantom_proxy_group_offset[(offsetof(s_physics_model_phantom, proxy_collision_group) == 10) ? 1 : -1];
typedef char physics_phantom_runtime_group_offset[(offsetof(s_physics_model_phantom, runtime_collision_group) == 11) ? 1 : -1];

#endif // PHYSICS_PHYSICS_MODEL_DEFINITION_TYPES_H

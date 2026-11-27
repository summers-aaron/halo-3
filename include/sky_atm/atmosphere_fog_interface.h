#ifndef SKY_ATM_ATMOSPHERE_FOG_INTERFACE_H
#define SKY_ATM_ATMOSPHERE_FOG_INTERFACE_H

struct s_weighted_atmosphere_parameters;
struct s_cluster_reference;
struct s_game_cluster_bit_vectors;

class c_atmosphere_fog_interface
{
public:
    static s_weighted_atmosphere_parameters m_default_parameters;

    static void set_constant(const s_weighted_atmosphere_parameters *, bool, bool);
    static void disable_atmosphere();
    static void populate_atmosphere_parameters(s_cluster_reference cluster, s_weighted_atmosphere_parameters *parameters);
    static void initialize();
    static void dispose();
    static void dispose_from_old_map();
    static void initialize_for_new_structure_bsp(unsigned long structure_bsp_mask);
    static void dispose_from_old_structure_bsp(unsigned long structure_bsp_mask);
    static void change_pvs(const s_game_cluster_bit_vectors *old_pvs, const s_game_cluster_bit_vectors *new_pvs, bool force);
};

#endif // SKY_ATM_ATMOSPHERE_FOG_INTERFACE_H

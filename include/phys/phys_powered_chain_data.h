#ifndef INCLUDE_PHYS_POWERED_CHAIN_DATA_H
#define INCLUDE_PHYS_POWERED_CHAIN_DATA_H
#include "phys/phys_constraint_chain.h"
#include "phys/phys_atoms.h"
#include "phys/phys_quat.h"

class phys_type_desc;

class phys_powered_chain_data : public phys_chain_data {
public:
    struct constraint_info {
        phys_vec4 m_pivot_in_a;
        phys_vec4 m_pivot_in_b;
        phys_quat m_a_to_c;
        phys_quat m_b_to_c;
        phys_motor *m_axis_motors[3];
        phys_bool m_swap_bodies;
    };
    phys_bridge_atoms m_data_atoms;
    phys_array<constraint_info> m_chain_infos;
    float m_query_tau;
    float m_solver_damping;
    float m_cfm_lin_add;
    float m_cfm_lin_mul;
    float m_cfm_ang_add;
    float m_cfm_ang_mul;
    float m_max_error_dist;

    phys_powered_chain_data();
    phys_powered_chain_data(phys_fixup_tag flag);
    virtual ~phys_powered_chain_data();
    static __forceinline void *operator new(size_t, void *storage) { return storage; }
    static __forceinline void operator delete(void *storage)
    {
        phys_powered_chain_data *object = static_cast<phys_powered_chain_data *>(storage);
        phys_thread_memory *memory = phys_thread_memory::get_instance_ptr();
        memory->free_chunk(storage, object->m_mem_size_and_flags, PHYS_MEM_CONSTRAINT);
    }
    __forceinline virtual phys_bool is_valid_aabb() const { return true; }
    __forceinline virtual int get_constraint_info_count() { return m_chain_infos.getSize(); }
    virtual int get_type_id() const;
    virtual void get_constraint_info(phys_constraint_data::constraint_info &info) const;
    virtual void get_runtime_info(phys_bool want_runtime, phys_constraint_data::runtime_info &info) const;
    static void enable_motor(phys_constraint_chain *instance, int constraint_idx, int chain_motor_index);
    static void disable_chain_motor(phys_constraint_chain *instance, int constraint_idx, int chain_motor_index);
    void add_info_in_body_space(const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b, const phys_quat &a_from_c,
        phys_motor *first_motor, phys_motor *second_motor, phys_motor *third_motor);
    struct solver_result { float m_applied_impulse; float m_internal_solver_data; };
    __forceinline unsigned char *get_constraint_flags(void *runtime) {
        return static_cast<unsigned char *>(static_cast<void *>(static_cast<solver_result *>(runtime) + m_chain_infos.getSize() * 6));
    }
    virtual void build_jacobian(const phys_constraint_query_in &unused_query_in, phys_constraint_query_out &out);
    __forceinline float *get_motor_runtime_quats(void *runtime) {
        return static_cast<float *>(static_cast<void *>(get_constraint_flags(runtime) + ((m_chain_infos.getSize() + 3) & ~3)));
    }
};

extern const phys_type_desc phys_powered_chain_data_type_desc;
extern "C" phys_constraint_runtime *phys_constraint_get_runtime(phys_constraint_internal *internal);

#endif

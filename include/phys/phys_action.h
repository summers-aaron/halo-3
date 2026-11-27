#ifndef INCLUDE_PHYS_ACTION_H
#define INCLUDE_PHYS_ACTION_H

#include "phys/phys_base.h"

class phys_world;
class phys_island;
class phys_entity;
class phys_volume;
class phys_step_info;

class phys_action : public phys_ref_object {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_ACTION);
    inline phys_action(unsigned long action_user_data)
        : m_world(0), m_sim_island(0), m_user_data(action_user_data), m_name(0) {}
    // March 0x8350a1d8 calls the default ReferencedObject constructor.
    inline phys_action(phys_fixup_tag flag) {}
    virtual ~phys_action() {}
    virtual void apply_action(const phys_step_info &step_info) = 0;
    virtual void get_entities(phys_array<phys_entity *> &entities) = 0;
    virtual void get_volumes(phys_array<phys_volume *> &phantoms) {}
    virtual void on_entity_removed(phys_entity *entity) = 0;
    virtual phys_action *clone(const phys_array<phys_entity *> &entities, const phys_array<phys_volume *> &phantoms) const = 0;

    inline phys_world *get_action_world() { return m_world; }
    inline phys_island *get_island() { return m_sim_island; }
    inline void set_owner_world(phys_world *world) { m_world = world; }
    inline void set_island(phys_island *island) { m_sim_island = island; }
    inline unsigned long get_user_data() const { return m_user_data; }
    inline void set_user_data(unsigned long data) { m_user_data = data; }
    inline const char *get_action_name() const { return m_name; }
    inline void set_action_name(const char *name) { m_name = name; }
private:
    phys_world *m_world;
    phys_island *m_sim_island;
protected:
    unsigned long m_user_data;
    const char *m_name;
    friend class phys_world;
    friend class phys_world_op_util;
    friend class phys_island;
};

typedef char phys_action_size[(sizeof(void *) != 4 || ((sizeof(phys_action) == 24) )) ? 1 : -1];

#endif

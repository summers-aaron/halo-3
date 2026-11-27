#ifndef NETWORKING_REPLICATION_REPLICATION_ENTITY_MANAGER_H
#define NETWORKING_REPLICATION_REPLICATION_ENTITY_MANAGER_H
class c_replication_entity_manager_view;
struct s_replication_allocation_block;
class c_simulation_entity_database;
struct s_replication_entity_data
{
    unsigned char flags;
    unsigned char seed;
    unsigned short views;
    __forceinline bool allocated() const { return (flags & 1) != 0; }
};
// Full storage and method declarations from replication_entity_manager.cpp.
class c_replication_entity_manager
{
public:
    c_simulation_entity_database *m_client;
    c_replication_entity_manager_view *m_views[15];
    unsigned long m_view_mask;
    s_replication_entity_data m_entity_data[1024];
    long m_next_entity;
private:
    long preallocate_entity();
    long create_local_entity_internal(long entity_index);
public:
    __forceinline s_replication_entity_data *get_entity(long entity_index) { return &m_entity_data[entity_index & 1023]; }
    __forceinline c_simulation_entity_database *get_client() { return m_client; }
private:
    unsigned char rotate_seed(unsigned char seed);
    void delete_entity_internal(long entity_index);
public:
    __forceinline void register_view(long view_index, c_replication_entity_manager_view *view);
    void reset();
    static void replication_entity_manager_view_notify_delete_remote_entity(c_replication_entity_manager *, long, bool);
    static void replication_entity_manager_view_process_creation(c_replication_entity_manager *, long, long, unsigned long, long,
        s_replication_allocation_block *);
    long create_local_entity();
    void delete_local_entity(long entity_index);
    void make_all_local();
private:
    friend class c_replication_entity_manager_view;
    void acknowledge_entity_deletion(long view_index, long entity_index);
};
typedef char replication_entity_manager_size[(sizeof(void *) != 4 || sizeof(c_replication_entity_manager) == 0x1048) ? 1 : -1];
#endif

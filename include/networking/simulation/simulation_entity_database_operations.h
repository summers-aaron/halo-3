#ifndef NETWORKING_SIMULATION_SIMULATION_ENTITY_DATABASE_OPERATIONS_H
#define NETWORKING_SIMULATION_SIMULATION_ENTITY_DATABASE_OPERATIONS_H
#include "networking/delivery/network_delivery_types.h"
#include "simulation/simulation_entity_types.h"
#include "networking/simulation/simulation_entity_database.h"
#include "networking/replication/replication_types.h"
struct manager;
class c_simulation_entity_definition;
class c_replication_entity_manager;
class c_bitstream;
class c_replication_outgoing_event;

class c_simulation_entity_database {
public:
  bool initialized;
  bool direct_access;
#ifdef HALO3_64BIT
  void *world;
#else
  long world;
#endif
  manager *entities;
  c_simulation_entity_definition **listeners;
  s_simulation_entity channels[1024];

  __forceinline s_simulation_entity *entity_get(long entity_index) { return &channels[entity_index & 0x3ff]; }
  __forceinline s_simulation_entity *entity_try_and_get(long entity_index) {
    s_simulation_entity *entity = 0;
    if (entity_index != -1) {
      long absolute_index = entity_index & 0x3ff;
      if (channels[absolute_index].entity_index == entity_index)
        entity = &channels[absolute_index];
    }
    return entity;
  }
  __forceinline void event_reference_remove(long entity_index, const c_replication_outgoing_event *event);
  __forceinline void event_reference_add(long entity_index, const c_replication_outgoing_event *event);
  __forceinline c_simulation_entity_definition *get_entity_definition(long entity_type) { return listeners[entity_type + 1]; }
  __forceinline void entity_create_internal(long entity_index, e_simulation_entity_type entity_type, long creation_data_size,
      void *creation_data, long state_data_size, void *state_data);
  __forceinline bool entity_allocate_creation_data(e_simulation_entity_type entity_type, long *creation_data_size, void **creation_data);
  __forceinline bool entity_allocate_state_data(e_simulation_entity_type entity_type, long *state_data_size, void **state_data);
  e_network_read_result read_creation_from_packet(long, long *, unsigned long *, long, long *, s_replication_allocation_block *, c_bitstream *);
  e_network_read_result read_update_from_packet(long, unsigned long *, long, long *, s_replication_allocation_block *, c_bitstream *);
  long entity_create(e_simulation_entity_type entity_type);
  void reset();
  void process_pending_updates();
  void entity_capture_creation_data(long entity_index);
  void entity_update(long entity_index, unsigned long update_mask, bool latch);
  bool entity_write_update(
      long entity_index, unsigned long update_mask, const void *data, c_bitstream *packet, long bits_reserved, unsigned long *written_mask);
  unsigned long generate_current_entity_update_mask(long entity_index);
  long calculate_creation_requirements(long entity_index, unsigned long update_mask, const void *data, long *priority);
  void notify_mark_entity_for_deletion(long entity_index, bool force_cleanup);
  void process_creation(long entity_index, long entity_type, unsigned long update_mask, long block_count, s_replication_allocation_block *blocks);

private:
  friend class c_replication_entity_manager;
  void entity_delete_internal(long entity_index);
};
#endif

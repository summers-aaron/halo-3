// networkinglibxbox/simulation_entity_database.obj: from the records 0x409530 0x4095c0 0x4096b8 0x409838 0x4098f0 0x409970 0x409ad0 0x409c08 0x409f80 0x40a078 0x40a2c8 0x40a358 0x40a448
// @flags /TP /O2
#include "networking/delivery/network_delivery_types.h"
#include "simulation/simulation_entity_types.h"
#include "networking/simulation/simulation_entity_database.h"
#include "networking/simulation/simulation_accessors.h"
#include "memory/tls_globals.h"
#include "networking/simulation/simulation_view_telemetry.h"
#include "memory/data_types.h"
#include "memory/bitstream.h"
// @address 0x409530 ?reset@c_simulation_entity_database@@QAAXXZ
// @address 0x4096b8 ?entity_create@c_simulation_entity_database@@QAAJW4e_simulation_entity_type@@@Z
// @address 0x409838 ?entity_capture_creation_data@c_simulation_entity_database@@QAAXJ@Z
// @address 0x4098f0 ?entity_update@c_simulation_entity_database@@QAAXJK_N@Z
// @address 0x409970 ?calculate_creation_requirements@c_simulation_entity_database@@QAAJJKPBXPAJ@Z
// @address 0x409ad0 ?entity_write_update@c_simulation_entity_database@@QAA_NJKPBXPAVc_bitstream@@JPAK@Z
// @address 0x409f80 ?process_creation@c_simulation_entity_database@@QAAXJJKJPAUs_replication_allocation_block@@@Z
// @address 0x40a2c8 ?notify_mark_entity_for_deletion@c_simulation_entity_database@@QAAXJ_N@Z
// @address 0x40a358 ?generate_current_entity_update_mask@c_simulation_entity_database@@QAAKJ@Z
// @address 0x40a448 ?entity_delete_internal@c_simulation_entity_database@@AAAXJ@Z

#include <string.h>
#include "networking/network_memory.h"
#include "simulation/simulation_queue_entities.h"
#include "simulation/simulation_priority.h"
#include "simulation/simulation_gamestate_entities.h"
#include "networking/network_memory_types.h"

#include "networking/replication/replication_entity_manager_view.h"
#include "networking/replication/replication_entity_manager.h"
#include "networking/replication/replication_types.h"
#include "simulation/simulation_queue_types.h"
#include "simulation/simulation_world_types.h"


struct manager {
  long unknown;
  c_replication_entity_manager_view *views[15];
  __forceinline void set_entity_dirty(long index, unsigned long mask) {
    for (long i=0; i<15; i++) if (views[i]) views[i]->set_entity_dirty(index, mask);
  }
};


#include "networking/simulation/simulation_entity_definition_types.h"

#include "networking/simulation/simulation_entity_database_operations.h"

__forceinline void c_simulation_entity_database::entity_create_internal(long entity_index, e_simulation_entity_type entity_type, long creation_data_size,
      void *creation_data, long state_data_size, void *state_data) {
    s_simulation_entity *entity = entity_get(entity_index);
    entity->entity_index = entity_index;
    entity->entity_type = entity_type;
    entity->exists_in_gameworld = false;
    entity->gamestate_index = -1;
    entity->update_mask = 0;
    entity->latched_update_mask = 0;
    entity->event_reference_count = 0;
    entity->creation_data_size = creation_data_size;
    entity->creation_data = creation_data;
    entity->state_data_size = state_data_size;
    entity->state_data = state_data;
  }

__forceinline bool c_simulation_entity_database::entity_allocate_creation_data(e_simulation_entity_type entity_type,
    long *creation_data_size, void **creation_data) {
    c_simulation_entity_definition **definitions = listeners; c_simulation_entity_definition *entity_definition = definitions[entity_type + 1];
    void *allocation = 0;
    bool success = true;
    long size = entity_definition->creation_data_size();
    if (size > 0) {
      allocation = network_shared_memory_globals.heap->allocate(size);
      if (allocation) {
        memset(allocation, 0, size);
      } else {
        success = false;
      }
    }
    *creation_data_size = size;
    *creation_data = allocation;
    return success;
  }

__forceinline bool c_simulation_entity_database::entity_allocate_state_data(e_simulation_entity_type entity_type, long *state_data_size, void **state_data) {
    c_simulation_entity_definition **definitions = listeners; c_simulation_entity_definition *entity_definition = definitions[entity_type + 1];
    bool success = true;
    long size = entity_definition->state_data_size();
    void *allocation = network_shared_memory_globals.heap->allocate(size);
    if (allocation) {
      memset(allocation, 0, size);
    } else {
      success = false;
    }
    *state_data_size = size;
    *state_data = allocation;
    return success;
  }

static __forceinline void simulation_entity_database_gamestate_entity_set_simulation_entity_index(long gamestate_index, long simulation_entity_index) {
  long absolute_index = gamestate_index & 0xffff;
  ((s_simulation_gamestate_entity_datum *)tls_simulation_gamestate_entity_data()->data)[absolute_index].entity_index = simulation_entity_index;
}

static __forceinline void simulation_entity_database_set_block(s_replication_allocation_block *const &block, long size, unsigned short type, void *data) {
  block->block_type = type;
  block->block_size = (short)size;
  block->block_data = data;
}

void c_simulation_entity_database::reset()
{
  direct_access = true;
  if (!initialized) {
    memset(channels, 0, 1024 * sizeof(s_simulation_entity));
  }
  for (long i = 0; i < 1024; i++) {
    s_simulation_entity *entity = &channels[i];
    if (initialized) {
      if (entity->entity_index != -1) {
        entity_delete_internal(entity->entity_index);
      }
    } else {
      entity->entity_index = -1;
      entity->entity_type = -1;
    }
  }
  direct_access = false;
}

long c_simulation_entity_database::entity_create(e_simulation_entity_type entity_type)
{
  long entity_index = -1;
  long creation_data_size;
  void *creation_data = 0;
  long state_data_size;
  void *state_data = 0;
  if (entity_allocate_creation_data(entity_type, &creation_data_size, &creation_data)) {
    if (entity_allocate_state_data(entity_type, &state_data_size, &state_data)) {
      entity_index = ((c_replication_entity_manager *)entities)->create_local_entity();
      if (entity_index != -1) {
        entity_create_internal(entity_index, entity_type, creation_data_size, creation_data, state_data_size, state_data);
        return entity_index;
      }
    }
  }
  if (creation_data) {
    ::network_heap_free_block((void *)(creation_data));
  }
  if (state_data) {
    ::network_heap_free_block((void *)(state_data));
  }
  return entity_index;
}

void c_simulation_entity_database::entity_capture_creation_data(long entity_index)
{
  s_simulation_entity *entity = entity_get(entity_index);
  c_simulation_entity_definition *entity_definition = get_entity_definition(entity->entity_type);
  entity_definition->build_creation_data(entity->gamestate_index, entity->creation_data_size, entity->creation_data);
  bool baseline_valid = entity_definition->build_baseline_state_data(
      entity->creation_data_size, entity->creation_data, entity->state_data_size, entity->state_data);
  entity->exists_in_gameworld = true;
  entity->update_mask = (1i64 << entity_definition->update_flag_count()) - 1;
  entity->latched_update_mask = 0;
}

void c_simulation_entity_database::entity_update(long entity_index, unsigned long update_mask, bool latch)
{
  s_simulation_entity *entity = &channels[entity_index & 0x3ff];
  listeners[entity->entity_type + 1]->gameworld_attachment_valid(entity->gamestate_index);
  entity->update_mask |= update_mask;
  if (latch) {
    entity->latched_update_mask |= update_mask;
  }
}

long c_simulation_entity_database::calculate_creation_requirements(long entity_index, unsigned long update_mask, const void *data, long *priority)
{
  s_simulation_entity *entity = entity_get(entity_index);
  c_simulation_entity_definition *entity_definition = get_entity_definition(entity->entity_type);
  const s_simulation_view_telemetry_data *telemetry = (const s_simulation_view_telemetry_data *)data;
  long required_bits = -1;
  if ((telemetry->view_inactive && !entity_definition->entity_replication_required_for_view_activation(entity))
      || !entity_definition->entity_can_be_created(entity, telemetry)) {
    *priority = 0;
  } else {
    *priority = (long)(simulation_calculate_entity_creation_priority((const s_simulation_entity *)entity, telemetry, 0) * 65536.0f);
    required_bits = entity_definition->creation_minimum_required_bits(entity, telemetry);
    unsigned long valid_update_mask = entity_definition->initial_update_mask();
    if (valid_update_mask) {
      s_entity_update_data update;
      update.update_mask = update_mask & valid_update_mask;
      update.last_update_time = 0;
      update.telemetry = telemetry;
      required_bits += 1 + entity_definition->update_minimum_required_bits(entity, &update);
    }
  }
  return required_bits;
}

bool c_simulation_entity_database::entity_write_update(
    long entity_index, unsigned long update_mask, const void *data, c_bitstream *packet, long bits_reserved, unsigned long *written_mask)
{
  s_simulation_entity *entity = entity_get(entity_index);
  c_simulation_entity_definition *entity_definition = get_entity_definition(entity->entity_type);
  bool success = true;
  *written_mask = 0;
  packet->write_integer("entity-type", entity->entity_type, 5);
  entity_definition->entity_creation_encode(entity->creation_data_size, entity->creation_data, (const s_simulation_view_telemetry_data *)data, packet, true);
  unsigned long valid_mask = entity_definition->initial_update_mask();
  if (valid_mask) {
    unsigned long mask = valid_mask & update_mask;
    packet->write_bool("has-initial-update", mask != 0);
    if (mask) {
      if (!entity_definition->entity_update_encode(
              true, mask, written_mask, entity->state_data_size, entity->state_data, 
              (const s_simulation_view_telemetry_data *)data, packet, bits_reserved, true) ||
          *written_mask != mask)
      {
        success = false;
      }
    }
  }
  return success;
}

void c_simulation_entity_database::process_creation(
    long entity_index, long entity_type, unsigned long update_mask, long block_count, s_replication_allocation_block *blocks)
{
  long creation_data_size = blocks[0].block_size;
  void *creation_data = blocks[0].block_data;
  long state_data_size = blocks[1].block_size;
  void *state_data = blocks[1].block_data;
  long gamestate_index = *(long *)blocks[2].block_data;
  s_simulation_queue_element *simulation_queue_element = *(s_simulation_queue_element **)blocks[3].block_data;

  ::network_heap_free_block((void *)(blocks[2].block_data));
  ::network_heap_free_block((void *)(blocks[3].block_data));
  memset(blocks, 0, block_count * sizeof(s_replication_allocation_block));

  entity_create_internal(entity_index, (e_simulation_entity_type)entity_type, creation_data_size, creation_data, state_data_size, state_data);
  s_simulation_entity *entity = entity_get(entity_index);
  entity->gamestate_index = gamestate_index;
  simulation_entity_database_gamestate_entity_set_simulation_entity_index(entity->gamestate_index, entity->entity_index);
  simulation_get_world()->simulation_queue_enqueue(simulation_queue_element);
  entity->exists_in_gameworld = true;
}

void c_simulation_entity_database::notify_mark_entity_for_deletion(long entity_index, bool force_cleanup)
{
  s_simulation_entity *entity = entity_get(entity_index);
  if (entity->gamestate_index != -1) {
    simulation_entity_database_gamestate_entity_set_simulation_entity_index(entity->gamestate_index, -1);
    simulation_queue_entity_deletion_insert((s_simulation_entity *)entity, force_cleanup);
  }
  entity->gamestate_index = -1;
  entity->exists_in_gameworld = false;
  entity->update_mask = 0;
  entity->latched_update_mask = 0;
}

unsigned long c_simulation_entity_database::generate_current_entity_update_mask(long entity_index)
{
  s_simulation_entity *entity = entity_get(entity_index);
  c_simulation_entity_definition *entity_definition = get_entity_definition(entity->entity_type);
  long baseline_state_data_size = entity_definition->state_data_size();
  char baseline_state_data[1024];
  memset(baseline_state_data, 0, baseline_state_data_size);
  bool baseline_valid = entity_definition->build_baseline_state_data(
      entity->creation_data_size, entity->creation_data, baseline_state_data_size, baseline_state_data);
  unsigned long update_mask = (1i64 << entity_definition->update_flag_count()) - 1;
  if (!entity_definition->build_updated_state_data(entity, &update_mask, baseline_state_data_size, baseline_state_data)) {
    update_mask = 0;
  }
  return update_mask;
}
void c_simulation_entity_database::entity_delete_internal(long entity_index)
{
  s_simulation_entity *entity;
  if (direct_access) {
    long absolute_index = entity_index & 0x3ff;
    entity = &channels[absolute_index];
  } else {
    entity = entity_get(entity_index);
  }
  if (entity->creation_data) {
    ::network_heap_free_block((void *)(entity->creation_data));
    entity->creation_data = 0;
    entity->creation_data_size = 0;
  }
  if (entity->state_data) {
    ::network_heap_free_block((void *)(entity->state_data));
    entity->state_data = 0;
    entity->state_data_size = 0;
  }
  entity->entity_index = -1;
  entity->entity_type = -1;
}

// @address 0x4095c0 ?process_pending_updates@c_simulation_entity_database@@QAAXXZ
void c_simulation_entity_database::process_pending_updates()
{
  for (long i=0; i<1024; i++) {
    s_simulation_entity *e=&this->channels[i];
    if (e->entity_index!=-1 && e->update_mask) {
      if (e->exists_in_gameworld) {
        c_simulation_entity_definition *d=this->get_entity_definition(e->entity_type);
        unsigned long actual=e->update_mask;
        if (d->build_updated_state_data(e, &actual, e->state_data_size, e->state_data)) {
          actual |= e->latched_update_mask;
          if (actual) this->entities->set_entity_dirty(e->entity_index, actual);
        }
      }
      e->update_mask=0;
      e->latched_update_mask=0;
    }
  }
}


// @address 0x40a078 ?read_update_from_packet@c_simulation_entity_database@@QAA?AW4e_network_read_result@@JPAKJPAJPAUs_replication_allocation_block@@PAVc_bitstream@@@Z
e_network_read_result c_simulation_entity_database::read_update_from_packet(long entity_index, unsigned long *out_update_mask,
    // match: 0x40a078: pointer __restrict qualifier prevents alias reload
    long maximum_block_count, long *__restrict block_count, s_replication_allocation_block *__restrict blocks,
    c_bitstream * packet)
{
  e_network_read_result result = _network_read_result_error;
  s_simulation_entity *entity = entity_try_and_get(entity_index);
  if (entity) {
    c_simulation_entity_definition *definition = get_entity_definition(entity->entity_type);
    bool success = false;
    long state_data_size = entity->state_data_size;
    void *state_data = network_shared_memory_globals.heap->allocate(state_data_size);
    if (state_data) {
      if (packet->read_only_for_consistency()) {
        memset(state_data, 0, entity->state_data_size);
        success = definition->build_baseline_state_data(entity->creation_data_size, entity->creation_data, entity->state_data_size, state_data);
        if (!success) result = _network_read_result_error;
      } else {
        memcpy(state_data, entity->state_data, entity->state_data_size);
        success = true;
      }
    } else result = _network_read_result_discard;
    s_simulation_queue_element **queue_element =
        (s_simulation_queue_element **)network_shared_memory_globals.heap->allocate(sizeof(s_simulation_queue_element *));
    if (!queue_element) result = _network_read_result_discard;
    if (success) {
      unsigned long update_mask = 0;
      long update_state_data_size = entity->state_data_size;
      if (definition->entity_update_decode(false, &update_mask, update_state_data_size, state_data, packet, true)) {
        s_simulation_queue_entity_data data;
        data.entity_index = entity_index;
        data.entity_type = entity->entity_type;
        data.creation_data_size = entity->creation_data_size;
        data.creation_data = entity->creation_data;
        data.state_data_size = entity->state_data_size;
        data.state_data = state_data;
        if (!packet->read_only_for_consistency() &&
            !simulation_queue_entity_update_allocate(&data, entity->gamestate_index, update_mask, queue_element))
            success = false;
        if (success) {
          *out_update_mask = update_mask;
          simulation_entity_database_set_block(&blocks[*block_count], entity->state_data_size, 15, state_data);
          simulation_entity_database_set_block(&blocks[*block_count + 1], 4, 11, queue_element);
          *block_count = *block_count + 2;
          return _network_read_result_success;
        }
      } else {
        result = _network_read_result_error;
      }
    }
    if (state_data) ::network_heap_free_block((void *)(state_data));
    if (queue_element) ::network_heap_free_block((void *)(queue_element));
    return result;
  }
  return _network_read_result_error;
}

// @address 0x409c08 ?read_creation_from_packet@c_simulation_entity_database@@QAA?AW4e_network_read_result@@JPAJPAKJ0PAUs_replication_allocation_block@@PAVc_bitstream@@@Z
e_network_read_result c_simulation_entity_database::read_creation_from_packet(long entity_index, long *out_entity_type,
    // match: 0x409c08: pointer __restrict qualifier prevents alias reload
    unsigned long *out_update_mask, long maximum_block_count, long *__restrict block_count,
    s_replication_allocation_block * blocks, c_bitstream * packet)
{
  e_network_read_result result = _network_read_result_error;
  long entity_type = packet->read_integer("entity-type", 5);
  c_simulation_entity_definition *definition = get_entity_definition(entity_type);
  if (definition) {
    long creation_size = definition->creation_data_size();
    long state_size = definition->state_data_size();
    void *creation_data = 0;
    void *state_data = 0;
    long *gamestate_index = 0;
    const long gamestate_size = sizeof(long);
    s_simulation_queue_element **queue_element = 0;
    const long queue_size = sizeof(s_simulation_queue_element *);
    if (creation_size > 0) {
      creation_data = network_shared_memory_globals.heap->allocate(creation_size);
      if (!creation_data) result = _network_read_result_discard;
    }
    state_data = network_shared_memory_globals.heap->allocate(state_size);
    if (!state_data) result = _network_read_result_discard;
    gamestate_index = (long *)network_shared_memory_globals.heap->allocate(gamestate_size);
    if (!gamestate_index) result = _network_read_result_discard;
    queue_element = (s_simulation_queue_element **)network_shared_memory_globals.heap->allocate(queue_size);
    if (!queue_element) result = _network_read_result_discard;
    if ((!creation_size || creation_data) && state_data && gamestate_index && queue_element) {
      if (creation_size > 0) memset(creation_data, 0, creation_size);
      if (definition->entity_creation_decode(creation_size, creation_data, packet, true) &&
          definition->build_baseline_state_data(creation_size, creation_data, state_size, state_data)) {
        unsigned long valid_mask = definition->initial_update_mask();
        bool success = true;
        unsigned long update_mask = 0;
        if (valid_mask && packet->read_bool("initial-update")) {
          if (!definition->entity_update_decode(true, &update_mask, state_size, state_data, packet, true))
            success = false;
          else if (update_mask & ~valid_mask)
            success = false;
        }
        if (success) {
          s_simulation_queue_entity_data data;
          data.entity_index = entity_index;
          data.entity_type = entity_type;
          data.creation_data_size = creation_size;
          data.creation_data = creation_data;
          data.state_data_size = state_size;
          data.state_data = state_data;
          if (!packet->read_only_for_consistency() &&
              !simulation_queue_entity_creation_allocate(&data, update_mask, queue_element, gamestate_index))
              success = false;
        }
        if (success) {
            *out_entity_type = entity_type;
            *out_update_mask = update_mask;
            blocks[*block_count].block_type = 14;
            blocks[*block_count].block_size = (short)creation_size;
            blocks[*block_count].block_data = creation_data;
            blocks[*block_count + 1].block_type = 15;
            blocks[*block_count + 1].block_size = (short)state_size;
            blocks[*block_count + 1].block_data = state_data;
            blocks[*block_count + 2].block_type = 12;
            blocks[*block_count + 2].block_size = (short)gamestate_size;
            blocks[*block_count + 2].block_data = gamestate_index;
            blocks[*block_count + 3].block_type = 11;
            blocks[*block_count + 3].block_size = (short)queue_size;
            blocks[*block_count + 3].block_data = queue_element;
            *block_count += 4;
            creation_data = 0;
            state_data = 0;
            gamestate_index = 0;
            queue_element = 0;
            result = _network_read_result_success;
        } else {
          result = _network_read_result_error;
        }
      } else {
        result = _network_read_result_error;
      }
    }
    if (creation_data) ::network_heap_free_block((void *)(creation_data));
    if (state_data) ::network_heap_free_block((void *)(state_data));
    if (gamestate_index) ::network_heap_free_block((void *)(gamestate_index));
    if (queue_element) ::network_heap_free_block((void *)(queue_element));
  } else {
    result = _network_read_result_error;
  }
  return result;
}

// hkdynamics/hkEntityCallbackUtil.obj: from the records 0x650928 0x6509b0 0x650a18 0x650b28 0x650b90 0x650bf8 0x650c70 0x650ce8 0x650d60
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "phys/phys_dynamics.h"
#include "phys/phys_constraint.h"
#include "phys/phys_action.h"
#include "phys/phys_entity_callback_util.h"
#include "phys/phys_entity_listener.h"
#include "phys/phys_collision_listener.h"
// @address 0x650928 phys_entity_callbacks_drop_nulls
extern "C" void phys_entity_callbacks_drop_nulls(phys_small_array<void *> *cleanup_array)
{
  for (int i = cleanup_array->m_size - 1; i >= 0; i--) {
    if (cleanup_array->m_data[i] == 0) {
      cleanup_array->m_size = (unsigned short)(cleanup_array->m_size - 1);
      for (int j = i; j < cleanup_array->m_size; j++) {
        cleanup_array->m_data[j] = cleanup_array->m_data[j + 1];
      }
    }
  }
}
// @address 0x650b90 ?notify_entity_deleted@phys_entity_callback_util@@SAXPAVphys_entity@@@Z
void phys_entity_callback_util::notify_entity_deleted(phys_entity *entity)
{
  phys_small_array<phys_entity_listener *> &listen = entity->m_entity_listeners;
  for (int i = listen.m_size - 1; i >= 0; i--) {
    if (listen.m_data[i] != 0) {
      listen.m_data[i]->on_entity_deleted(entity);
    }
  }
}
// @address 0x650bf8 ?notify_contact_point_added_impl@phys_entity_callback_util@@KAXPAVphys_entity@@AAUphys_contact_added_event@@@Z
void phys_entity_callback_util::notify_contact_point_added_impl(phys_entity *entity, phys_contact_added_event &event)
{
  phys_small_array<phys_collision_listener *> &listen = entity->m_collision_listeners;
  event.m_callback_fired_from = entity;
  for (int i = listen.m_size - 1; i >= 0; i--) {
    if (listen.m_data[i] != 0) {
      listen.m_data[i]->contact_point_added_callback(event);
    }
  }
  phys_entity_callbacks_drop_nulls((phys_small_array<void *> *)&listen);
}
// @address 0x650ce8 ?notify_contact_point_removed_impl@phys_entity_callback_util@@KAXPAVphys_entity@@AAUphys_contact_removed_event@@@Z
void phys_entity_callback_util::notify_contact_point_removed_impl(phys_entity *entity, phys_contact_removed_event &event)
{
  phys_small_array<phys_collision_listener *> &listen = entity->m_collision_listeners;
  event.m_callback_fired_from = entity;
  for (int i = listen.m_size - 1; i >= 0; i--) {
    if (listen.m_data[i] != 0) {
      listen.m_data[i]->contact_point_removed_callback(event);
    }
  }
  phys_entity_callbacks_drop_nulls((phys_small_array<void *> *)&listen);
}
// @address 0x650a18 ?notify_entity_removed@phys_entity_callback_util@@SAXPAVphys_entity@@@Z
void phys_entity_callback_util::notify_entity_removed(phys_entity *entity)
{
  phys_small_array<phys_entity_listener *> &listen = entity->m_entity_listeners;
  for (int i = listen.m_size - 1; i >= 0; i--) {
    if (listen.m_data[i] != 0) {
      listen.m_data[i]->on_entity_removed(entity);
    }
  }
  {
    phys_small_array<phys_constraint_internal> &constraints = entity->m_master_constraints;
    while (constraints.m_size) {
      constraints.m_data[0].m_contact_constraint->on_entity_removed(entity);
    }
  }
  {
    phys_array<phys_constraint *> &constraints = entity->m_slave_constraints;
    while (constraints.m_size) {
      constraints.m_data[0]->on_entity_removed(entity);
    }
  }
  {
    phys_small_array<phys_action *> &actions = entity->m_actions;
    while (actions.m_size) {
      actions.m_data[0]->on_entity_removed(entity);
    }
  }
}

// @address 0x650d60 ?notify_contact_process_impl@phys_entity_callback_util@@KAXPAVphys_entity@@AAUphys_contact_process_event@@@Z
void phys_entity_callback_util::notify_contact_process_impl(phys_entity *entity, phys_contact_process_event &event)
{
  phys_small_array<phys_collision_listener *> &listen = entity->m_collision_listeners;
  event.m_callback_fired_from = entity;
  for (int i = listen.m_size - 1; i >= 0; i--) {
    if (listen.m_data[i] != 0) {
      listen.m_data[i]->contact_process_callback(event);
    }
  }
  phys_entity_callbacks_drop_nulls((phys_small_array<void *> *)&listen);
}

// @address 0x650c70 ?notify_contact_point_confirmed_impl@phys_entity_callback_util@@KAXPAVphys_entity@@AAUphys_contact_confirmed_event@@@Z
void phys_entity_callback_util::notify_contact_point_confirmed_impl(phys_entity *entity, phys_contact_confirmed_event &event)
{
  phys_small_array<phys_collision_listener *> &listen = entity->m_collision_listeners;
  event.m_callback_fired_from = entity;
  for (int i = listen.m_size - 1; i >= 0; i--) {
    if (listen.m_data[i] != 0) {
      listen.m_data[i]->contact_point_confirmed_callback(event);
    }
  }
  phys_entity_callbacks_drop_nulls((phys_small_array<void *> *)&listen);
}

// @address 0x6509b0 ?notify_entity_added@phys_entity_callback_util@@SAXPAVphys_entity@@@Z
void phys_entity_callback_util::notify_entity_added(class phys_entity *p0)
{
  phys_small_array<phys_entity_listener *> &listen = p0->m_entity_listeners;
  for (int i = listen.m_size - 1; i >= 0; i--) {
    if (listen.m_data[i] != 0) {
      listen.m_data[i]->on_entity_added(p0);
    }
  }
}

// @address 0x650b28 ?notify_entity_shape_set@phys_entity_callback_util@@SAXPAVphys_entity@@@Z
void phys_entity_callback_util::notify_entity_shape_set(class phys_entity *p0)
{
  phys_small_array<phys_entity_listener *> &listen = p0->m_entity_listeners;
  for (int i = listen.m_size - 1; i >= 0; i--) {
    if (listen.m_data[i] != 0) {
      listen.m_data[i]->on_entity_shape_set(p0);
    }
  }
}

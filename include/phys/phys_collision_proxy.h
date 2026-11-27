#ifndef INCLUDE_PHYS_COLLISION_PROXY_H
#define INCLUDE_PHYS_COLLISION_PROXY_H

#include <stddef.h>
#include "phys/phys_shape_key.h"

class phys_shape;
class phys_transform;
class phys_motion_state;
class phys_collider;

class phys_collision_proxy {
public:
    // March 0x834a4778.
    __forceinline void *operator new(size_t, void *p) { return p; }
    __forceinline phys_collision_proxy() {}
    __forceinline phys_collision_proxy(const phys_shape *shape, const phys_motion_state *motion_state)
    {
        m_proxy_shape = shape;
        m_motion = motion_state;
        m_parent = 0;
        m_shape_key = PHYS_INVALID_SHAPE_KEY;
    }
    __forceinline phys_collision_proxy(const phys_shape *shape, const phys_transform *t)
    {
        m_proxy_shape = shape;
        m_motion = t;
        m_parent = 0;
        m_shape_key = PHYS_INVALID_SHAPE_KEY;
    }
    // March stores the parent first, then copies its motion.
    __forceinline phys_collision_proxy(const phys_collision_proxy *parent) : m_parent(parent) { m_motion = parent->m_motion; }
    // March 0x834a7340 and 0x8347d1b0: parent and replacement motion only.
    __forceinline phys_collision_proxy(const phys_collision_proxy *parent, const phys_motion_state *motion_state)
        : m_parent(parent) { m_motion = motion_state; }
    __forceinline phys_collision_proxy(const phys_collision_proxy *parent, const phys_transform *transform)
        : m_parent(parent) { m_motion = transform; }

    __forceinline const phys_shape *get_shape() const { return m_proxy_shape; }
    __forceinline phys_shape_key get_shape_key() const { return m_shape_key; }
    __forceinline const phys_transform &get_transform() const { return *static_cast<const phys_transform *>(m_motion); }
    __forceinline const phys_motion_state *get_motion_state() const { return static_cast<const phys_motion_state *>(m_motion); }
    __forceinline const phys_collision_proxy *get_parent_proxy() const { return m_parent; }
    __forceinline const phys_collider *get_root_collider() const {
        const phys_collision_proxy *b = this;
        while (b->m_parent) {
            b = b->m_parent;
        }
        return reinterpret_cast<const phys_collider *>(b);
    }
    __forceinline void set_proxy_shape(const phys_shape *shape, phys_shape_key key) {
        m_proxy_shape = shape;
        m_shape_key = key;
    }
    // March 0x834a7370 inherits the parent's shape key before storing the shape.
    __forceinline void set_proxy_shape(const phys_shape *shape) { m_shape_key = m_parent->m_shape_key; m_proxy_shape = shape; }
    // March 0x834c1d90.
    __forceinline void set_motion_state(const phys_motion_state *motion_state) { m_motion = motion_state; }

    const phys_shape *m_proxy_shape;
    phys_shape_key m_shape_key;
    const void *m_motion;
    const phys_collision_proxy *m_parent;
};

#endif

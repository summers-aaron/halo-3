#ifndef INCLUDE_ANIMATIONS_ANIMATION_ID_H
#define INCLUDE_ANIMATIONS_ANIMATION_ID_H

// An animation in a model animation graph: (graph index, animation index), both NONE when cleared.
// The user-declared constructor and destructor make it non-POD, so functions returning it by value
// take a hidden result pointer.
class c_animation_id {
public:
    __forceinline c_animation_id() : m_graph_index(-1), m_animation_index(-1) {}
    __forceinline ~c_animation_id() {}
    __forceinline void clear() { m_graph_index = -1; m_animation_index = -1; }
    __forceinline bool valid() const { return m_animation_index >= 0; }
    // March 827dd710: compare the subgraph, then the animation within it.
    bool operator!=(const c_animation_id &other) const { return m_graph_index != other.m_graph_index || m_animation_index != other.m_animation_index; }

    union { short m_graph_index; short graph_index; };
    union { short m_animation_index; short animation_index; };
};

typedef char animation_id_size_must_be_4[(sizeof(c_animation_id) == 4) ? 1 : -1];

#endif // INCLUDE_ANIMATIONS_ANIMATION_ID_H

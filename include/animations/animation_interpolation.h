#ifndef ANIMATIONS_ANIMATION_INTERPOLATION_H
#define ANIMATIONS_ANIMATION_INTERPOLATION_H

// Blends node orientations in or out over a duration. Methods read from the March 2007 debug build
// (`local/odin-bin/marchbody class c_interpolator_control`): every method but the constructor and destructor
// is out of line in tu/blamlibxbox/animation_interpolation.cpp. This keeps
// update 0xd63f8, get_ratio 0x10fb00 and set_status 0x11a2e8 out of line and inlines the rest everywhere, so
// those are written here.

template<long k_maximum_count> class c_static_flags;
struct real_orientation;

enum e_interpolation_type {
    _interpolation_type_linear,
    _interpolation_type_ease_in_ease_out = 1,   // March get_ratio calls ease_in_ease_out for 1
};

class c_interpolator_control {
public:
    // March 82bc1fa0 (inline)
    __forceinline c_interpolator_control() : m_time(0.0f), m_duration(0.0f), m_interpolation_type(0), m_flags(0) {}

    bool update();
    void set_status(bool on, float duration);
    float get_ratio() const;
    void interpolate_node_orientations(long node_count, const c_static_flags<255> *node_mask,
        real_orientation *orientations, real_orientation *source_orientations) const;
    void approximate_interpolate_node_orientations(long node_count, const c_static_flags<255> *node_mask,
        real_orientation *orientations, real_orientation *source_orientations) const;

    // Inlined everywhere; no out-of-line copy.
    __forceinline bool enabled() const { return m_duration != 0.0f; }
    __forceinline bool finished() const { return (bool)((m_flags >> 1) & 1); }
    __forceinline bool interpolating_on() const { return enabled() && (m_flags & 1) != 0; }
    __forceinline bool interpolating_off() const { return enabled() && (m_flags & 1) == 0; }
    __forceinline float get_time_remaining() const
    {
        float result = 0.0f;
        if (enabled())
            result = (m_flags & 1) ? m_duration - m_time : m_time;
        return result;
    }
    __forceinline void disable() { m_time = 0.0f; m_duration = 0.0f; m_flags = 0; }
    __forceinline void set_interpolation_type(e_interpolation_type interpolation_type) { m_interpolation_type = (unsigned short)interpolation_type; }

    float m_time;
    float m_duration;
    unsigned short m_interpolation_type;                                         // set_interpolation_type
    unsigned short m_flags;                                                      // bit 0 on, bit 1 finished
};
typedef char interpolator_control_size[(sizeof(c_interpolator_control) == 12) ? 1 : -1];

// Data defined in tu/blamlibxbox/animation_interpolation.cpp that other units use.

extern const float k_standard_interpolation_time;

#endif // ANIMATIONS_ANIMATION_INTERPOLATION_H

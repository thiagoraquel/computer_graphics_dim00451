#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "common.hpp"
#include "paramset.hpp"
#include "geometry.hpp"

namespace gc {

class Background {
public:
    enum mapping_t { screen, spherical };
    static constexpr real_type max_channel_value = 255.0f;

    Background(mapping_t mt = screen) : m_mapping{mt} {}
    virtual ~Background() = default;

    virtual Spectrum sampleUV(real_type u, real_type v) const = 0;

protected:
    mapping_t m_mapping;
};

class BackgroundSingleColor : public Background {
public:
    BackgroundSingleColor(const Color24& color, mapping_t mt = screen);
    BackgroundSingleColor(const Spectrum& color, mapping_t mt = screen);
    Spectrum sampleUV(real_type u, real_type v) const override;

private:
    Spectrum m_single_color;
};

class BackgroundMultiColor : public Background {
public:
    enum Corner { tl = 0, bl, br, tr }; // Order used in your .cpp

    BackgroundMultiColor(const std::array<Spectrum, 4>& colors, mapping_t mt = screen)
        : Background{mt}, m_corners{colors} {}

    Spectrum sampleUV(real_type u, real_type v) const override;

private:
    std::array<Spectrum, 4> m_corners;
    static Spectrum lerp(const Spectrum& S, const Spectrum& E, float t);
};

// Factory function
Background* create_color_background(std::string_view type, const ParamSet& ps);

} // namespace gc

#endif
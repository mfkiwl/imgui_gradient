#pragma once

#include "ColorRGBA.hpp"
#include "RelativePosition.hpp"

namespace ImGuiGradient {

inline auto operator==(const ColorRGBA& a, const ColorRGBA& b) -> bool
{
    return (a.x == b.x) &&
           (a.y == b.y) &&
           (a.z == b.z) &&
           (a.w == b.w);
}

struct Mark {
    RelativePosition position{0.f};
    ColorRGBA        color{0.f, 0.f, 0.f, 1.f};

    friend auto operator==(const Mark& a, const Mark& b) -> bool
    {
        return a.position == b.position &&
               a.color == b.color;
    };
};

} // namespace ImGuiGradient
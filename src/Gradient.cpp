#include <imgui_gradient/Gradient.h>
#include <imgui_gradient/utils.h>

namespace ImGuiGradient {

static void default_gradient_value(Marks& marks)
{
    marks.add_mark(Mark{0.f, ImVec4{0.f, 0.f, 0.f, 1.f}});
    marks.add_mark(Mark{1.f, ImVec4{1.f, 1.f, 1.f, 1.f}});
}

Gradient::Gradient()
{
    default_gradient_value(m_marks);
}
void Gradient::reset()
{
    m_marks.clear();
    default_gradient_value(m_marks);
}

ImVec4 Gradient::get_color_at(float position, PositionMode mode) const
{
    switch (mode)
    {
    case PositionMode::clamp:
        return compute_color_at(RelativePosition{
            ImClamp(position, 0.f, 1.f)});
    case PositionMode::repeat:
        return compute_color_at(RelativePosition{
            utils::repeat_position(position)});
    case PositionMode::mirror_clamp:
        return compute_color_at(RelativePosition{
            utils::mirror_clamp_position(position)});
    case PositionMode::mirror_repeat:
        return compute_color_at(RelativePosition{
            utils::mirror_repeat_position(position)});
    default:
        assert(false && "[Gradient::get_color_at] Invalid enum value");
        return ImVec4{-1.f, -1.f, -1.f, -1.f};
    }
}

ImVec4 Gradient::compute_color_at(RelativePosition position) const
{
    const Mark* lower = nullptr;
    const Mark* upper = nullptr;
    for (const Mark& mark : m_marks.m_list)
    {
        if (mark.position > position &&
            (!upper || mark.position < upper->position))
        {
            upper = &mark;
        }
        if (mark.position < position &&
            (!lower || mark.position > lower->position))
        {
            lower = &mark;
        }
    }
    if (!lower && !upper)
    {
        return ImVec4{0.f, 0.f, 0.f, 1.f};
    }
    else if (upper && !lower)
    {
        lower = upper;
    }
    else if (!upper && lower)
    {
        upper = lower;
    }

    if (upper == lower)
    {
        return upper->color;
    }
    else
    {
        float distance = upper->get_position() - lower->get_position();
        float mix      = (position.get() - lower->get_position()) / distance;

        // lerp
        return ImVec4{(1.f - mix), (1.f - mix), (1.f - mix), (1.f - mix)} * lower->color +
               ImVec4{mix, mix, mix, mix} * upper->color;
    }
};

} // namespace ImGuiGradient

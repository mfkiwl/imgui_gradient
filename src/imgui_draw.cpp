#pragma once

#include "Gradient.hpp"
#include "Interpolation.hpp"
#include "Settings.hpp"
#include "internal.hpp"

namespace ImGuiGradient {

static void draw_uniform_square(
    ImDrawList&  draw_list,
    const ImVec2 top_left_corner,
    const ImVec2 bottom_rigth_corner,
    ImColor      color
)
{
    static constexpr float rounding = 1.f;
    draw_list.AddRectFilled(top_left_corner, bottom_rigth_corner, color, rounding, ImDrawFlags_Closed);
}

static void draw_gradient_between_two_colors(
    ImDrawList&  draw_list,
    const ImVec2 top_left_corner,
    const ImVec2 bottom_rigth_corner,
    ImColor colorA, ImColor colorB
)
{
    draw_list.AddRectFilledMultiColor(
        top_left_corner, bottom_rigth_corner,
        colorA, colorB, colorB, colorA
    );
}

void draw_gradient(
    Gradient&     gradient,
    ImDrawList&   draw_list,
    Interpolation interpolation_mode,
    ImVec2        gradient_bar_pos, // TODO(ASG) Rename as gradient_pos
    // ImVec2        size
    float height,
    float width
)
{
    float current_starting_x = gradient_bar_pos.x;
    for (auto markIt = gradient.get_marks().begin(); markIt != gradient.get_marks().end(); ++markIt)
    {
        const Mark& mark = *markIt;

        ImColor colorBU32 = ImGui::ColorConvertFloat4ToU32(mark.color);
        ImColor colorAU32 = (markIt != gradient.get_marks().begin())
                                ? ImGui::ColorConvertFloat4ToU32(std::prev(markIt)->color)
                                : colorBU32;

        const float from = current_starting_x;
        const float to   = gradient_bar_pos.x + mark.position.get() * (width);
        if (mark.position.get() != 0.f)
        {
            if (interpolation_mode == Interpolation::Linear)
            {
                draw_gradient_between_two_colors(
                    draw_list,
                    ImVec2(from, gradient_bar_pos.y),
                    ImVec2(to, gradient_bar_pos.y + height),
                    colorAU32, colorBU32
                );
            }
            else
            {
                draw_uniform_square(
                    draw_list,
                    ImVec2(from, gradient_bar_pos.y),
                    ImVec2(to, gradient_bar_pos.y + height),
                    colorBU32
                );
            }
        }
        current_starting_x = to;
    }
    // If last element not at the end position extend its color to the end position
    if (gradient.get_marks().back().position.get() != 1.f)
    {
        ImColor colorBU32 = ImGui::ColorConvertFloat4ToU32(gradient.get_marks().back().color);
        draw_uniform_square(
            draw_list,
            ImVec2(current_starting_x, gradient_bar_pos.y),
            ImVec2(gradient_bar_pos.x + width, gradient_bar_pos.y + height),
            colorBU32
        );
    }
}

static void draw_background_mark(
    ImDrawList&  draw_list,
    const ImVec2 pos,
    ImColor      arrow_mark,
    const float  arrow_border,
    const float  offset
)
{
    const float arrow_inside_border = arrow_border - offset;

    const auto arrow_border_x = ImVec2{arrow_border, 0.f};
    const auto arrow_border_y = ImVec2{0.f, arrow_border};

    const auto offset_y = ImVec2{0.f, offset};

    const auto arrow_inside_border_x = ImVec2{arrow_inside_border, 0.f};
    const auto arrow_inside_border_y = ImVec2{0.f, arrow_inside_border};

    draw_list.AddTriangleFilled(
        pos - arrow_border_y,
        pos - arrow_border_x,
        pos + arrow_border_x,
        arrow_mark
    );
    draw_uniform_square(
        draw_list,
        pos - arrow_border_x,
        pos + arrow_border_x + ImVec2{0.f, 2.f} * arrow_border_y,
        arrow_mark
    );
    draw_uniform_square(
        draw_list,
        pos - arrow_inside_border_x + offset_y,
        pos + arrow_inside_border_x + ImVec2{0.f, 2.f} * arrow_inside_border_y + offset_y,
        arrow_mark
    );
}

static void draw_arrow_selected(
    ImDrawList&  draw_list,
    const ImVec2 pos,
    ImColor      selected_color,
    const float  arrow_inside_border,
    const float  arrow_selected,
    const float  offset
)
{
    const auto offset_y = ImVec2{0.f, offset};

    const auto arrow_inside_border_x = ImVec2{arrow_inside_border, 0.f};
    const auto arrow_inside_border_y = ImVec2{0.f, arrow_inside_border};

    const auto arrow_selected_x = ImVec2{arrow_selected, 0.f};
    const auto arrow_selected_y = ImVec2{0.f, arrow_selected};

    draw_list.AddTriangleFilled(
        pos - arrow_selected_y - offset_y,
        pos + offset_y - arrow_selected_x, pos + arrow_selected_x + offset_y,
        selected_color
    );
    draw_list.AddRect(
        pos - arrow_inside_border_x + offset_y,
        pos + arrow_inside_border_x + ImVec2{0.f, 2.f} * arrow_inside_border_y + offset_y, selected_color, 1.0f, ImDrawFlags_Closed
    );
}

static void draw_mark(
    ImDrawList&  draw_list,
    const ImVec2 pos,
    ImColor      background_mark_color,
    ImColor      mark_color,
    float        arrow_border,
    bool         mark_is_selected
)
{
    static constexpr auto offset{1.f};

    draw_background_mark(
        draw_list,
        pos,
        background_mark_color,
        arrow_border, offset
    );
    if (mark_is_selected)
    {
        const float arrow_selected      = 4.f; // TODO(ASG) Rename
        const float arrow_inside_border = arrow_border - offset;

        draw_arrow_selected(
            draw_list,
            pos,
            internal::selected_mark_color(),
            arrow_inside_border, arrow_selected, offset
        );
    }

    const auto square_height   = 3.f;
    const auto square_height_x = ImVec2{square_height, 0.f};
    const auto square_height_y = ImVec2{0.f, square_height};
    draw_uniform_square(
        draw_list,
        pos - square_height_x + square_height_y,
        pos + square_height_x + square_height_y * square_height_y,
        mark_color
    );
}

static auto mark_invisible_button(
    const ImVec2 vec,
    float        arrow_border,
    float        gradient_editor_height
) -> bool
{
    ImGui::SetCursorScreenPos(vec - ImVec2{arrow_border * 1.5f, gradient_editor_height});
    const auto button_size = ImVec2{arrow_border * 3.f, gradient_editor_height + arrow_border * 2.f};
    ImGui::InvisibleButton("mark", button_size);
    return ImGui::IsItemHovered();
}

void mark_invisble_hitbox(
    ImDrawList&    draw_list,
    const ImVec2   pos,
    const ImColor& mark_color,
    float          gradient_editor_height,
    bool           mark_is_selected
)
{
    const float arrow_border = 6.f;
    draw_mark(
        draw_list,
        pos,
        mark_invisible_button(pos, arrow_border, gradient_editor_height)
            ? internal::hovered_mark_color()
            : internal::mark_color(),
        mark_color,
        arrow_border,
        mark_is_selected
    );
}

} // namespace ImGuiGradient
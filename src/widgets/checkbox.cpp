#include "widgets/checkbox.hpp"
#include "imgui_internal.h"
#include "widgets/animated.hpp"
#include "util/config.hpp"

using namespace ImGui;

TABBY_NS_BEGIN

WidgetState checkbox(std::string_view label, bool& value) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return WidgetState::idle();

    ImVec2 size = ImGui::GetContentRegionAvail();
    size.x = tabby::TabbyGlobalCfg::get().widgetWidth;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());
    const ImVec2 label_size = CalcTextSize(label.data(), NULL, true);

    const float square_sz = GetFrameHeight();
    size.y = label_size.y + style.FramePadding.y * 2.0f;
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect total_bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ItemSize(total_bb, style.FramePadding.y);
    const bool is_visible = ItemAdd(total_bb, id);
    const bool is_multi_select = (g.LastItemData.ItemFlags & ImGuiItemFlags_IsMultiSelect) != 0;
    if (!is_visible)
        if (!is_multi_select || !g.BoxSelectState.UnclipMode || !g.BoxSelectState.UnclipRect.Overlaps(total_bb)) // Extra layer of "no logic clip" for box-select support
        {
            IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
            return WidgetState::idle();
        }

    // Range-Selection/Multi-selection support (header)
    bool checked = value;
    if (is_multi_select)
        MultiSelectItemHeader(id, &checked, NULL);

    bool hovered, held;
    bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);

    // Range-Selection/Multi-selection support (footer)
    if (is_multi_select)
        MultiSelectItemFooter(id, &checked, &pressed);
    else if (pressed)
        checked = !checked;

    if (value != checked)
    {
        value = checked;
        pressed = true; // return value
        MarkItemEdited(id);
    }
    
    float uiScale = tabby::TabbyGlobalCfg::get().uiScale;

    const ImVec2 check_size = ImVec2(square_sz, square_sz);
    
    const float check_bg_size = 8.0f * uiScale;
    const float leftmost_padding = 10.0f * uiScale;

    const ImRect check_bb(
        ImVec2(pos.x + size.x - check_bg_size * 2.0f - leftmost_padding, pos.y), 
        ImVec2(pos.x + size.x - leftmost_padding, pos.y + size.y)
    );
    const bool mixed_value = (g.LastItemData.ItemFlags & ImGuiItemFlags_MixedValue) != 0;
    if (is_visible)
    {
        RenderNavCursor(total_bb, id);
        ImU32 frame_col = GetColorU32(hovered ? ImGuiCol_FrameBgActive : ImGuiCol_FrameBgHovered);
        // RenderFrame(check_bb.Min, check_bb.Max, GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), true, style.FrameRounding);
        ImU32 check_col = GetColorU32(ImGuiCol_CheckMark);
        if (mixed_value)
        {
            // Undocumented tristate/mixed/indeterminate checkbox (#2644)
            // This may seem awkwardly designed because the aim is to make ImGuiItemFlags_MixedValue supported by all widgets (not just checkbox)
            ImVec2 pad(ImMax(1.0f, IM_TRUNC(square_sz / 3.6f)), ImMax(1.0f, IM_TRUNC(square_sz / 3.6f)));
            window->DrawList->AddRectFilled(ImVec2(check_bb.Min.x + pad.x, check_bb.Min.y + pad.y), ImVec2(check_bb.Max.x - pad.x, check_bb.Max.y + pad.y), check_col, style.FrameRounding);
        }
        else if (value)
        {
            const float pad = ImMax(1.0f, IM_TRUNC(square_sz / 6.0f));
            // RenderCheckMark(window->DrawList, ImVec2(check_bb.Min.x + pad, check_bb.Min.y + pad), check_col, square_sz - pad * 2.0f);
        }

        ImVec2 center_left = ImVec2(
            0,
            check_bb.GetHeight() * 0.5f
        );

        ImVec2 center_right = ImVec2(
            check_bb.GetWidth(),
            check_bb.GetHeight() * 0.5f
        );

        struct CirclePositionData {
            float x;
            float y;
            float active;

            ImColor color;
        };
        CirclePositionData& d = tabby::getStoredAnimValue(label, CirclePositionData{
            .x = center_left.x,
            .y = center_left.y,
            .active = 0.0f,
            .color = GetColorU32(ImGuiCol_Text)
        });

        d.x = tabby::interpolateValue(d.x, value ? center_right.x : center_left.x);
        d.y = tabby::interpolateValue(d.y, value ? center_right.y : center_left.y);
        d.active = tabby::interpolateValue(d.active, value ? 1.0f : 0.0f);

        ImVec2 circle_pos(check_bb.Min.x + d.x, check_bb.Min.y + d.y);
        d.color = tabby::interpolateValue(d.active, GetColorU32(ImGuiCol_Text), GetColorU32(ImGuiCol_TextDisabled));

        window->DrawList->AddRectFilled(
            ImVec2(check_bb.Min.x + center_left.x - check_bg_size, check_bb.Min.y + center_left.y - check_bg_size),
            ImVec2(check_bb.Min.x + center_right.x + check_bg_size, check_bb.Min.y + center_right.y + check_bg_size),
            GetColorU32(
                hovered ? ImGuiCol_FrameBgActive : ImGuiCol_FrameBgHovered
            ),
            8.0f * uiScale
        );

        window->DrawList->AddRect(
            ImVec2(check_bb.Min.x + center_left.x - check_bg_size, check_bb.Min.y + center_left.y - check_bg_size),
            ImVec2(check_bb.Min.x + center_right.x + check_bg_size, check_bb.Min.y + center_right.y + check_bg_size),
            GetColorU32(
                hovered ? ImGuiCol_FrameBgActive : ImGuiCol_FrameBgHovered
            ),
            8.0f * uiScale,
            0,
            1.5f * uiScale
        );

        window->DrawList->AddCircleFilled(
            circle_pos,
            6.0f * uiScale,
            d.color
        );
    }
    const ImVec2 label_pos = total_bb.Min;
    if (g.LogEnabled)
        LogRenderedText(&label_pos, mixed_value ? "[~]" : value ? "[x]" : "[ ]");
    if (is_visible && label_size.x > 0.0f)
        RenderTextClipped(
            total_bb.Min,
            total_bb.Max,
            label.data(),
            NULL,
            &label_size,
            ImVec2(0.0f, 0.5f),
            &total_bb
        );

    return WidgetState{
        .pressed = pressed,
        .hovered = hovered,
        .held = held,
    };
}

TABBY_NS_END
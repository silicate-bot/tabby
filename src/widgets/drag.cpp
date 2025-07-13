#include "widgets/drag.hpp"
#include <string_view>

#include "imgui_internal.h"
#include "widgets/animated.hpp"
#include <format>

TABBY_NS_BEGIN

WidgetState tabby::_drag_impl(
    std::string_view label,
    void* value,
    void* min,
    void* max,
    float speed,
    DragType t
) {
    ImGuiWindow* wnd = ImGui::GetCurrentWindow();
    if (wnd->SkipItems) {
        return WidgetState::idle();
    }

    // ImVec2 size = ImGui::GetContentRegionAvail();
    ImVec2 size = ImVec2(wnd->Size.x / 2.0f, 0.0f);

    ImGuiContext& g = *GImGui;
    const auto id = wnd->GetID(label.data());
    const ImVec2 label_size = ImGui::CalcTextSize(label.data(), NULL, true);
    size.y = label_size.y + g.Style.FramePadding.y * 2.0f;

    const ImRect bb(
        wnd->DC.CursorPos,
        ImVec2(wnd->DC.CursorPos.x + size.x, wnd->DC.CursorPos.y + size.y)
    );
    ImGui::ItemSize(bb, g.Style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id)) {
        return WidgetState::idle();
    }

    ImGuiDataType data_type;
    const char* format = nullptr;

#define DRAGTYPE(t, imgui_t, fmt) \
    case DragType::t: \
        data_type = ImGuiDataType_##imgui_t; \
        format = fmt; \
        break;

    switch (t) {
        DRAGTYPE(u8, U8, "%d")
        DRAGTYPE(u16, U16, "%d")
        DRAGTYPE(u32, U32, "%d")
        DRAGTYPE(u64, U64, "%d")
        DRAGTYPE(i8, S8, "%d")
        DRAGTYPE(i16, S16, "%d")
        DRAGTYPE(i32, S32, "%d")
        DRAGTYPE(i64, S64, "%d")
        DRAGTYPE(f32, Float, "%.2f")
        DRAGTYPE(f64, Double, "%.2f")
    };

#undef DRAGTYPE



    const bool hovered = ImGui::ItemHoverable(bb, id, g.LastItemData.ItemFlags);
    bool temp_input_active = ImGui::TempInputIsActive(id);
    if (!temp_input_active) {
        const bool clicked = hovered && ImGui::IsMouseClicked(0, ImGuiInputFlags_None, id);
        const bool double_clicked = (hovered && g.IO.MouseClickedCount[0] == 2 && ImGui::TestKeyOwner(ImGuiKey_MouseLeft, id));
        const bool clicked_and_stopped = (hovered && g.ActiveId == id && g.IO.MouseReleased[0] && !ImGui::IsMouseDragPastThreshold(0, g.IO.MouseDragThreshold));

        const bool make_active = clicked || double_clicked || g.NavActivateId == id;

        if (clicked_and_stopped) {
            g.NavActivateId = id;
            g.NavActivateFlags = ImGuiActivateFlags_PreferInput;
            temp_input_active = true;
        }

        if (make_active && (clicked || double_clicked)) {
            ImGui::SetKeyOwner(ImGuiKey_MouseLeft, id);
        }

        if (make_active) {
            memcpy(&g.ActiveIdValueOnActivation, value, ImGui::DataTypeGetInfo(data_type)->Size);
        }

        if (make_active && double_clicked) {
            temp_input_active = true;
        }

        if (make_active && !temp_input_active) {
            ImGui::SetActiveID(id, wnd);
            ImGui::SetFocusID(id, wnd);
            ImGui::FocusWindow(wnd);
        }
    }


    if (temp_input_active) {
        bool changed = ImGui::TempInputScalar(bb, id, label.data(), data_type, value, format, NULL, NULL);
        return WidgetState {
            .hovered = hovered,
            .changed = changed,
        };
    }

    tabby::DragData& animated = getStoredAnimValue(label.data(), tabby::DragData{
        .x = bb.Min.x + bb.GetWidth() / 2.0f,
        .y = bb.Min.y + g.Style.FramePadding.y,
    });

    animated.hovered = interpolateValue(animated.hovered, hovered ? 1.0f : 0.0f);
    ImU32 frame_col = interpolateValue(
        animated.hovered,
        ImGui::GetColorU32(ImGuiCol_FrameBgHovered),
        ImGui::GetColorU32(ImGuiCol_FrameBg)
    );

    ImGui::RenderNavCursor(bb, id);
    ImGui::RenderFrame(bb.Min, bb.Max, frame_col, true, g.Style.FrameRounding);

    const bool changed = ImGui::DragBehavior(id, data_type, value, speed, min, max, label.data(), ImGuiSliderFlags_None);
    if (changed) {
        ImGui::MarkItemEdited(id);
    }

    char format_buf[64];
    ImGui::DataTypeFormatString(format_buf, 64, data_type, value, format);
    std::string formatted = std::format("{}", format_buf);

    ImVec2 value_size = ImGui::CalcTextSize(formatted.c_str(), nullptr, true);

    float actualTextPositionX = bb.Min.x + bb.GetWidth() / 2.0f - value_size.x / 2.0f;
    ImVec2 text_pos = ImVec2(actualTextPositionX, bb.Min.y + g.Style.FramePadding.y);

    animated.x = interpolateValue(animated.x, text_pos.x);
    animated.y = interpolateValue(animated.y, text_pos.y);

    float text_x = animated.x + value_size.x / 2.0f - bb.GetWidth() / 2.0f;
    float text_y = animated.y - g.Style.FramePadding.y;

    ImGui::RenderTextClipped(
        ImVec2(text_x, text_y),
        bb.Max,
        formatted.c_str(),
        nullptr,
        &value_size,
        ImVec2(0.5f, 0.5f),
        &bb
    );

    return WidgetState {
        .hovered = hovered,
        .changed = changed,
    };
}

TABBY_NS_END
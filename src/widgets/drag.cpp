#include "widgets/drag.hpp"

#include <format>
#include <string_view>

#include "imgui_internal.h"
#include "widgets/animated.hpp"

TABBY_NS_BEGIN

WidgetState tabby::_drag_impl(std::string_view label, void* value, void* min,
                              void* max, float speed, DragType t,
                              std::string_view f) {
    ImGuiContext& g = *GImGui;
    tabby::DragData& animated =
        getStoredAnimValue(label.data(), tabby::DragData{
                                             .x = 0.0,
                                             .y = g.Style.FramePadding.y,
                                         });

    ImGuiWindow* wnd = ImGui::GetCurrentWindow();
    if (wnd->SkipItems) {
        return WidgetState::idle();
    }

    // ImVec2 size = ImGui::GetContentRegionAvail();
    ImVec2 size = ImVec2(tabby::TabbyGlobalCfg::get().widgetWidth, 0.0f);

    const auto id = wnd->GetID(label.data());
    const ImVec2 label_size = ImGui::CalcTextSize(label.data(), NULL, true);
    size.y = label_size.y + g.Style.FramePadding.y * 2.0f;
    ImVec2 size_half = ImVec2(size.x * 0.5f, size.y);

    const ImVec2 total_size = size;
    const ImVec2 frame_size =
        ImVec2(size_half.x - ImGui::GetStyle().ItemSpacing.x / 2.0, size.y);

    const ImRect frame_bb(
        ImVec2(wnd->DC.CursorPos.x + total_size.x - size_half.x +
                   ImGui::GetStyle().ItemSpacing.x / 2.0,
               wnd->DC.CursorPos.y),
        ImVec2(wnd->DC.CursorPos.x + total_size.x,
               wnd->DC.CursorPos.y + size.y));
    const ImRect total_bb(wnd->DC.CursorPos,
                          ImVec2(wnd->DC.CursorPos.x + total_size.x,
                                 wnd->DC.CursorPos.y + size.y));

    ImGui::ItemSize(total_bb, g.Style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id, &frame_bb, ImGuiItemFlags_None)) {
        return WidgetState::idle();
    }

    ImGuiDataType data_type;
    const char* format = nullptr;

#define DRAGTYPE(t, imgui_t, fmt)            \
    case DragType::t:                        \
        data_type = ImGuiDataType_##imgui_t; \
        format = fmt;                        \
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

    const bool hovered =
        ImGui::ItemHoverable(frame_bb, id, g.LastItemData.ItemFlags);
    bool temp_input_active = ImGui::TempInputIsActive(id);
    if (!temp_input_active) {
        const bool clicked =
            hovered && ImGui::IsMouseClicked(0, ImGuiInputFlags_None, id);
        const bool double_clicked =
            (hovered && g.IO.MouseClickedCount[0] == 2 &&
             ImGui::TestKeyOwner(ImGuiKey_MouseLeft, id));
        const bool clicked_and_stopped =
            (hovered && g.ActiveId == id && g.IO.MouseReleased[0] &&
             !ImGui::IsMouseDragPastThreshold(0, g.IO.MouseDragThreshold));

        const bool make_active =
            clicked || double_clicked || g.NavActivateId == id;

        // if (clicked_and_stopped) {
        //     g.NavActivateId = id;
        //     g.NavActivateFlags = ImGuiActivateFlags_PreferInput;
        //     temp_input_active = true;
        // }

        if (make_active && (double_clicked || clicked)) {
            ImGui::SetKeyOwner(ImGuiKey_MouseLeft, id);
        }

        if (make_active) {
            memcpy(&g.ActiveIdValueOnActivation, value,
                   ImGui::DataTypeGetInfo(data_type)->Size);
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
        bool changed = ImGui::TempInputScalar(
            total_bb, id, label.data(), data_type, value, format, NULL, NULL);
        return WidgetState{
            .hovered = hovered,
            .changed = changed,
        };
    }

    animated.hovered =
        interpolateValue(animated.hovered, hovered ? 1.0f : 0.0f);
    ImU32 frame_col = interpolateValue(
        animated.hovered, ImGui::GetColorU32(ImGuiCol_FrameBgHovered),
        ImGui::GetColorU32(ImGuiCol_FrameBg));

    ImGui::RenderNavCursor(frame_bb, id);
    ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true,
                       g.Style.FrameRounding);

    const bool changed =
        ImGui::DragBehavior(id, data_type, value, speed, min, max, label.data(),
                            ImGuiSliderFlags_None);
    if (changed) {
        ImGui::MarkItemEdited(id);
    }

    char format_buf[64];
    ImGui::DataTypeFormatString(format_buf, 64, data_type, value, format);

#define FMT_TYPE(ty, cxxty)                                              \
    case DragType::ty: {                                                 \
        formatted = std::vformat(                                        \
            f, std::make_format_args(*reinterpret_cast<cxxty*>(value))); \
        break;                                                           \
    }

    std::string formatted;
    switch (t) {
        FMT_TYPE(u8, uint8_t)
        FMT_TYPE(u16, uint16_t)
        FMT_TYPE(u32, uint32_t)
        FMT_TYPE(u64, uint64_t)
        FMT_TYPE(i8, int8_t)
        FMT_TYPE(i16, int16_t)
        FMT_TYPE(i32, int32_t)
        FMT_TYPE(i64, int64_t)
        FMT_TYPE(f32, float)
        FMT_TYPE(f64, double)
    }

#undef FMT_TYPE

    ImVec2 value_size = ImGui::CalcTextSize(formatted.c_str(), nullptr, true);

    float actualTextPositionX =
        frame_bb.GetWidth() / 2.0f - value_size.x / 2.0f;
    ImVec2 text_pos = ImVec2(actualTextPositionX, g.Style.FramePadding.y);

    animated.x = interpolateValue(animated.x, text_pos.x);
    animated.y = interpolateValue(animated.y, text_pos.y);

    ImGui::RenderTextClipped(
        ImVec2(frame_bb.Min.x + animated.x, frame_bb.Min.y), frame_bb.Max,
        formatted.c_str(), nullptr, &value_size, ImVec2(0.0f, 0.5f), &frame_bb);

    ImGui::RenderTextClipped(
        total_bb.Min, ImVec2(total_bb.Min.x + label_size.x, total_bb.Max.y),
        label.data(), nullptr, &label_size, ImVec2(0.0f, 0.5f), &total_bb);

    return WidgetState{
        .hovered = hovered,
        .changed = changed,
    };
}

TABBY_NS_END

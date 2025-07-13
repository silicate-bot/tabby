#include "widgets/button.hpp"
#include "imgui_internal.h"
#include "widgets/animated.hpp"

TABBY_NS_BEGIN

WidgetState button(std::string_view content) {
    ImGuiWindow* wnd = ImGui::GetCurrentWindow();
    ImVec2 size = ImVec2(ImGui::GetContentRegionMax().x / 2.0f, 0.0f);
    if (wnd->SkipItems) {
        return WidgetState::idle();
    }

    ImGuiContext& g = *GImGui;
    const ImGuiID id = wnd->GetID(content.data());
    const ImVec2 label_size = ImGui::CalcTextSize(content.data(), NULL, true);

    size.x -= g.Style.FramePadding.x * 1.35f;
    size.y = label_size.y + g.Style.FramePadding.y * 2.0f;

    ImVec2 pos = wnd->DC.CursorPos;
    size = ImGui::CalcItemSize(size, label_size.x + g.Style.FramePadding.x * 2.0f, label_size.y + g.Style.FramePadding.y * 2.0f);

    ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));

    ImGui::ItemSize(bb, g.Style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id)) {
        return WidgetState::idle();
    }

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

    struct ButtonState {
        float pressedState = 0.0f;
        float hoveredState = 0.0f;
        float heldState = 0.0f;

        ImColor bgColor = 0;
    };

    ButtonState& animated = getStoredAnimValue(content.data(), ButtonState{});
    animated.pressedState = interpolateValue(animated.pressedState, pressed ? 1.0f : 0.0f);
    animated.hoveredState = interpolateValue(animated.hoveredState, hovered ? 1.0f : 0.0f);
    animated.heldState = interpolateValue(animated.heldState, held ? 1.0f : 0.0f);

    animated.bgColor = interpolateValue(animated.hoveredState, ImGui::GetColorU32(ImGuiCol_ButtonHovered), ImGui::GetColorU32(ImGuiCol_Button));
    if (held && hovered) {
        animated.bgColor = interpolateValue(animated.heldState, ImGui::GetColorU32(ImGuiCol_ButtonActive), animated.bgColor);
    }

    ImGui::RenderNavCursor(bb, id);
    ImGui::RenderFrame(bb.Min, bb.Max, animated.bgColor, true, g.Style.FrameRounding);
    // ImGui::RenderFrameBorder(bb.Min, bb.Max, g.Style.FrameRounding);

    ImGui::RenderTextClipped(
        ImVec2(bb.Min.x + g.Style.FramePadding.x, bb.Min.y + g.Style.FramePadding.y),
        ImVec2(bb.Max.x - g.Style.FramePadding.x, bb.Max.y - g.Style.FramePadding.y),
        content.data(), NULL, &label_size, ImVec2(0.5f, 0.5f), &bb);
    
    return WidgetState{
        .pressed = pressed,
        .hovered = hovered,
        .held = held
    };
}

TABBY_NS_END
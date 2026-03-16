#include "widgets/dropdown.hpp"

#include "imgui.h"
#include "widgets/animated.hpp"
#include "widgets/button.hpp"
#include "widgets/text.hpp"

TABBY_NS_BEGIN

static void drawPopup(std::string_view label, const ImVec2& pos,
                      const ImVec2& size, DropdownState& state,
                      std::function<void()> renderPopup = nullptr) {
    float uiScale = tabby::TabbyGlobalCfg::get().uiScale;
    struct WindowSize {
        float width;
        float height;
        float opacity;

        float checkX;
        float checkY;
    };

    WindowSize& w =
        tabby::getStoredAnimValue(std::string(label) + std::string("_popup"),
                                  WindowSize{0.0f, 0.0f, 1.0f});
    if (!state.showPopup) {
        w.height = tabby::interpolateValue(w.height, 0.0f);
        w.opacity = tabby::interpolateValue(w.opacity, 0.0f);
        return;
    }

    w.height = tabby::interpolateValue(w.height, size.y);
    w.opacity = tabby::interpolateValue(w.opacity, 1.0f);

    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(ImVec2(size.x, w.height));

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, w.opacity);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                        ImVec2(18.0f * uiScale, 12.0f * uiScale));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f * uiScale);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, size);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                        {12.0f * uiScale, 6.0f * uiScale});

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_Tooltip;

    std::string popupName = std::string(label) + "_popup";

    ImGui::Begin(popupName.c_str(), nullptr, flags);

    if (renderPopup) {
        renderPopup();
    }

    std::string_view check = "\uf00c";
    auto window = ImGui::GetCurrentWindow();

    // enumerate suggestions
    for (size_t i = 0; i < state.options.size(); ++i) {
        auto& option = state.options.at(i);
        ImGui::PushID(
            static_cast<int>(i));  // Ensure unique ID for each suggestion
        bool isHighlighted = (state.highlightedIndex == static_cast<int>(i));

        if (isHighlighted) {
            ImGui::PushStyleColor(ImGuiCol_Text,
                                  ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text,
                                  ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
        }

        if (state.selectedIndex == static_cast<int>(i)) {
            w.checkY = tabby::interpolateValue(
                w.checkY, window->DC.CursorPos.y - window->Pos.y, 50.0f);

            ImGui::RenderTextClipped(
                ImVec2(window->DC.CursorPos.x, w.checkY + window->Pos.y),
                ImVec2(window->DC.CursorPos.x + ImGui::GetContentRegionMax().x -
                           ImGui::GetStyle().WindowPadding.x,
                       w.checkY + window->Pos.y + ImGui::GetTextLineHeight()),
                check.data(), nullptr, nullptr, ImVec2(1.0f, 0.5f),
                &window->ClipRect);
        }

        if (ImGui::Selectable(option.data(), false,
                              ImGuiSelectableFlags_NoAutoClosePopups)) {
            state.showPopup = false;
            state.clickedIndex = static_cast<int>(i);
        }

        if (ImGui::IsItemHovered()) {
            state.highlightedIndex = static_cast<int>(i);
        }

        ImGui::PopID();

        ImGui::PopStyleColor();
    }

    // close if clicking outside the popup
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) &&
        ImGui::IsMouseClicked(0)) {
        state.showPopup = false;
    }

    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow)) {
        state.showPopup = false;
    }


    ImGui::End();
    ImGui::PopStyleVar(5);
    // ImGui::PopStyleColor();
}

static WidgetState _button(std::string_view content, std::string_view preview,
                           ImGuiButtonFlags flags = ImGuiButtonFlags_None,
                           bool active = false, bool border = true, bool labelVisible = true) {
    ImGuiWindow* wnd = ImGui::GetCurrentWindow();

    ImVec2 size = ImVec2(tabby::TabbyGlobalCfg::get().widgetWidth, 0.0f);
    if (wnd->SkipItems) {
        return WidgetState::idle();
    }

    ImGuiContext& g = *GImGui;
    const ImGuiID id = wnd->GetID(content.data());
    const ImVec2 label_size = ImGui::CalcTextSize(content.data(), NULL, true);

    // size.x -= g.Style.FramePadding.x * 1.35f;
    size.y = label_size.y + g.Style.FramePadding.y * 2.0f;

    ImVec2 pos = wnd->DC.CursorPos;
    size =
        ImGui::CalcItemSize(size, label_size.x + g.Style.FramePadding.x * 2.0f,
                            label_size.y + g.Style.FramePadding.y * 2.0f);

    float halfWidth = size.x * 0.5f;
    float space = ImGui::GetStyle().ItemSpacing.x / 2.0;
    if (content.empty() || !labelVisible) {
        halfWidth = size.x;
        space = 0.0;
    }

    ImRect total_bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImRect frame_bb(ImVec2(pos.x + size.x - halfWidth +
                               space,
                           pos.y),
                    ImVec2(pos.x + size.x, pos.y + size.y));

    ImGui::ItemSize(total_bb, g.Style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id)) {
        return WidgetState::idle();
    }

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(frame_bb, id, &hovered, &held);

    struct ButtonState {
        float pressedState = 0.0f;
        float hoveredState = 0.0f;
        float heldState = 0.0f;
        float borderState = 0.0f;

        ImColor bgColor = 0;
        ImColor borderColor = 0;
    };

    ButtonState& animated = getStoredAnimValue(content.data(), ButtonState{});
    animated.pressedState =
        interpolateValue(animated.pressedState, pressed ? 1.0f : 0.0f);
    animated.hoveredState =
        interpolateValue(animated.hoveredState, hovered ? 1.0f : 0.0f);
    animated.heldState =
        interpolateValue(animated.heldState, (held || active) ? 1.0f : 0.0f);
    animated.borderState =
        interpolateValue(animated.borderState, border ? 1.0f : 0.0f);

    animated.bgColor = interpolateValue(
        animated.hoveredState, ImGui::GetColorU32(ImGuiCol_FrameBgHovered),
        ImGui::GetColorU32(ImGuiCol_FrameBg));
    if ((held && hovered) || active) {
        animated.bgColor = interpolateValue(
            animated.heldState, ImGui::GetColorU32(ImGuiCol_FrameBgActive),
            animated.bgColor);
    }
    animated.borderColor = interpolateValue(
        animated.borderState, ImGui::GetColorU32(ImGuiCol_Border),
        ImGui::GetColorU32(ImGuiCol_WindowBg));

    ImGui::RenderNavCursor(frame_bb, id);
    ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, animated.bgColor, true,
                       g.Style.FrameRounding);

    if (!labelVisible) {
        halfWidth = 0.0;
    }

    if (labelVisible) {
        ImGui::RenderTextClipped(ImVec2(pos.x, pos.y + g.Style.FramePadding.y),
                                 ImVec2(pos.x + halfWidth - g.Style.FramePadding.x,
                                        pos.y + size.y - g.Style.FramePadding.y),
                                 content.data(), NULL, &label_size,
                                 ImVec2(0.0f, 0.5f), &total_bb);
    }

    const ImVec2 previewSize = ImGui::CalcTextSize(preview.data(), NULL, true);
    ImGui::RenderTextClipped(ImVec2(pos.x + halfWidth + g.Style.FramePadding.x,
                                    pos.y + g.Style.FramePadding.y),
                             ImVec2(pos.x + size.x - g.Style.FramePadding.x,
                                    pos.y + size.y - g.Style.FramePadding.y),
                             preview.data(), NULL, &previewSize,
                             ImVec2(0.0f, 0.5f), &total_bb);

    std::string_view downIcon = "";
    const ImVec2 downIconSize =
        ImGui::CalcTextSize(downIcon.data(), NULL, true);

    ImGui::RenderTextClipped(ImVec2(pos.x + halfWidth + g.Style.FramePadding.x,
                                    pos.y + g.Style.FramePadding.y),
                             ImVec2(pos.x + size.x - g.Style.FramePadding.x,
                                    pos.y + size.y - g.Style.FramePadding.y),
                             downIcon.data(), NULL, &downIconSize,
                             ImVec2(1.0f, 0.5f), &total_bb);

    return {.pressed = pressed,
            .hovered = hovered,
            .held = held,
            .changed = pressed || hovered || held};
}

WidgetState dropdown(std::string_view label, DropdownState& state,
                     int& selectedIndex, std::function<void()> renderPopup, bool labelVisible) {
    tabby::WidgetState widgetState = tabby::WidgetState::idle();
    state.selectedIndex = std::clamp(
        selectedIndex, 0, static_cast<int>(state.options.size()) - 1);

    if (_button(label,
        state.options[state.selectedIndex],
        ImGuiButtonFlags_None,
        false,
        true,
        labelVisible).pressed) {
        state.showPopup = !state.showPopup;
    }

    float itemCount =
        static_cast<float>(std::min((int)state.options.size(), 5));

    float uiScale = tabby::TabbyGlobalCfg::get().uiScale;
    float offset = labelVisible ? (ImGui::GetItemRectMin().x + ImGui::GetItemRectMax().x) / 2.0f : ImGui::GetItemRectMin().x;
    float size = labelVisible ? 0.5f : 1.0f;
    drawPopup(
        label,
        ImVec2(offset,
               ImGui::GetItemRectMax().y + ImGui::GetStyle().ItemSpacing.y),
        ImVec2(ImGui::GetItemRectSize().x * size,
               ImGui::GetTextLineHeight() * itemCount +
                   6.0f * uiScale * itemCount + 24.0f * uiScale),
        state, renderPopup);

    if (state.clickedIndex != -1) {
        selectedIndex = state.clickedIndex;
        state.selectedIndex = selectedIndex;
        state.clickedIndex = -1;
        widgetState.changed = true;
    }


    if (ImGui::GetItemRectMin().y < ImGui::GetCurrentWindow()->Rect().GetTL().y ||
        ImGui::GetItemRectMax().y > ImGui::GetCurrentWindow()->Rect().GetBL().y) {
        state.showPopup = false;
    }

    return widgetState;
}

TABBY_NS_END

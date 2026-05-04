#include "widgets/overlay.hpp"

#include <functional>
#include <string_view>
#include <widgets/text.hpp>

#include "imgui.h"
#include "util/config.hpp"
#include "widgets/animated.hpp"

TABBY_NS_BEGIN

static void drawPopup(std::string_view label, const ImVec2& pos,
                      const ImVec2& size, bool& show,
                      std::function<void()> layoutInner,
                      std::function<void()> renderPopup = nullptr) {
    ImGuiWindow* wnd = ImGui::GetCurrentWindow();
    float uiScale = tabby::TabbyGlobalCfg::get().uiScale;
    struct WindowSize {
        float width;
        float height;
        float opacity;

        float checkX;
        float checkY;
    };

    WindowSize& w =
        tabby::getStoredAnimValue(std::string(label) + std::string("_overlaypopup"),
                                  WindowSize{0.0f, 0.0f, 1.0f});
    if (!show) {
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

    layoutInner();

    if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) &&
        ImGui::IsMouseClicked(0)) {
        show = false;
    }

    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow)) {
        show = false;
    }


    ImGui::End();
    ImGui::PopStyleVar(5);
    // ImGui::PopStyleColor();
}

void overlay(std::function<void()> innerFn, std::function<void()> popupFn, std::function<void()> shaderFn) {
    const auto beforePos = ImGui::GetCursorScreenPos();
    innerFn();
    const ImVec2 popupPos = ImGui::GetCursorScreenPos();
    const ImVec2 afterPos = {ImGui::GetCurrentWindow()->Rect().GetBR().x, popupPos.y};

    const ImRect triggerRect(beforePos, afterPos);
    const std::string overlayId = std::to_string(ImGui::GetItemID());
    bool& open = tabby::getStoredAnimValue(overlayId + std::string("_open"), false);

    if (const ImVec2 mousePos = ImGui::GetMousePos();
        ImGui::IsMouseReleased(ImGuiMouseButton_Right) && triggerRect.Contains(mousePos)) {
        open = !open;
    }

    if (open) {
        drawPopup(overlayId, popupPos, { 600.0, 300.0 }, open, popupFn, shaderFn);
    }
}

TABBY_NS_END

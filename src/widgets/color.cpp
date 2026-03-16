#include "widgets/color.hpp"

#include <functional>
#include <string>

#include "imgui.h"
#include "imgui_internal.h"
#include "util/config.hpp"
#include "widgets/animated.hpp"
#include "widgets/state.hpp"
#include "widgets/text.hpp"

TABBY_NS_BEGIN

static void drawPopup(std::string_view label, const ImVec2& pos,
                      const ImVec2& size, tabby::ColorState& state,
                      std::function<void()> renderPopup = nullptr) {
    float uiScale = tabby::TabbyGlobalCfg::get().uiScale;
    struct WindowSize {
        float width;
        float height;
        float opacity;
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
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_Tooltip;

    std::string popupName = std::string(label) + "_popup";

    ImGui::Begin(popupName.c_str(), nullptr, flags);
    ImGuiWindow* wnd = ImGui::GetCurrentWindow();

    if (renderPopup) {
        renderPopup();
    }

    ImGui::PushItemWidth(
        ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x * 2.0
    );
    ImGui::ColorPicker4("INTERNAL___color_picker", state.colors.data(),
        ImGuiColorEditFlags_NoAlpha |
        ImGuiColorEditFlags_NoOptions |
        ImGuiColorEditFlags_NoLabel |
        ImGuiColorEditFlags_NoSidePreview |
        ImGuiColorEditFlags_NoSmallPreview |
        ImGuiColorEditFlags_NoTooltip
    );

    auto window = ImGui::GetCurrentWindow();

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

tabby::WidgetState color(std::string_view label, tabby::ColorState& state,
                         std::function<void()> renderPopup) {
    tabby::WidgetState w = WidgetState::idle();
    ImGuiContext& g = *GImGui;

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

    const ImVec2 pos = wnd->DC.CursorPos;
    const ImVec2 total_size = size;
    const ImVec2 pos_max = ImVec2(pos.x + total_size.x, pos.y + size.y);
    const ImRect total_bb(pos, pos_max);

    const ImVec4 colorsImgui = ImVec4(state.colors[0], state.colors[1],
                                      state.colors[2], state.colors[3]);

    ImGui::RenderTextClipped(pos, pos_max, label.data(), NULL, &label_size,
                             ImVec2(0.0, 0.5), &total_bb);

    wnd->DC.CursorPos = ImVec2(pos_max.x - ImGui::GetFrameHeight(), pos.y);

    std::string idColorBtn = std::string("##ColorBtn") + std::string(label);

    if (ImGui::ColorButton(idColorBtn.data(), colorsImgui,
                           ImGuiColorEditFlags_NoTooltip)) {
        state.showPopup = !state.showPopup;
    }

    float popupSize = ImGui::GetWindowWidth() / 2.0;

    if (ImGui::GetItemRectMin().y < ImGui::GetCurrentWindow()->Rect().GetTL().y ||
        ImGui::GetItemRectMax().y > ImGui::GetCurrentWindow()->Rect().GetBL().y) {
        state.showPopup = false;
    }

    drawPopup(
        label,
        ImVec2(ImGui::GetItemRectMax().x - popupSize,
               ImGui::GetItemRectMax().y + ImGui::GetStyle().ItemSpacing.y),
        ImVec2(popupSize, 180.0 * tabby::TabbyGlobalCfg::get().uiScale), state,
        renderPopup);

    return w;
}

TABBY_NS_END

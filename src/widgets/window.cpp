#include "widgets/window.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "util/config.hpp"

TABBY_NS_BEGIN

double _window_begin(std::string_view content) {
    ImGuiStyle& style = ImGui::GetStyle();

    ImGuiIO& io = ImGui::GetIO();
    // io.MouseDrawCursor = true; // always draw mouse cursor

    float uiScale = tabby::TabbyGlobalCfg::get().uiScale;

    ImGui::SetNextWindowSizeConstraints(
        ImVec2(720.0f * uiScale, 500.0f * uiScale),
        ImVec2(720.0f * uiScale, 500.0f * uiScale)
    );

    style.WindowRounding = 24.0f * uiScale;
    style.WindowPadding = { 24.0f * uiScale, 24.0f * uiScale };
    style.WindowBorderSize = 0.0f;
    style.FrameRounding = 6.0f * uiScale;
    style.ItemInnerSpacing = { 4.0f * uiScale, 4.0f * uiScale };
    style.FrameBorderSize = 1.5f * uiScale;
    style.FramePadding = { 18.0f * uiScale, 6.0f * uiScale };
    style.ItemSpacing = { 8.0f * uiScale, 8.0f * uiScale };
    style.ChildRounding = 24.0 * uiScale;

    style.Colors[ImGuiCol_Separator] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);

    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.145f, 0.145f, 0.145f, 0.0f);

    style.Colors[ImGuiCol_FrameBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.05f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.1f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);

    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.3f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.4f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(1.0f, 1.0f, 1.0f, 0.4f);

    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.0f, 1.0f, 1.0f, 0.4f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.6f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.6f);

    style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 1.0f, 1.0f, 0.8f);

    style.FontScaleMain = uiScale;

    ImGui::Begin(content.data(), 0,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus
    );

    double widgetWidth = ImGui::GetWindowSize().x;
    return widgetWidth;
}

void _window_end() {
    ImGui::End();
}

void _stack_begin() {
    ImGui::BeginGroup();
}

void _stack_end() {
    ImGui::EndGroup();
}

std::pair<float, float> _section_begin(std::string_view title, float width, bool bg) {
    float uiScale = tabby::TabbyGlobalCfg::get().uiScale;
    width *= uiScale;
    bool width_is_auto = width < 0.0f;
    auto initialWindowPadding = ImGui::GetStyle().WindowPadding;
    if (width < 0.0f) {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(48.0f * uiScale, 48.0f * uiScale));
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetStyle().WindowPadding.x);
    }

    float orig_pos = ImGui::GetCursorPosX();
    if (width < 0.0f) {
        // width = ImGui::GetContentRegionAvail().x + ImGui::GetStyle().WindowPadding.x;
        width = ImGui::GetContentRegionAvail().x;
    }

    // if (ImGui::GetCursorPosX() == ImGui::GetStyle().WindowPadding.x) {
    //     ImGui::SetCursorPosX(0);
    // }

    int flags = ImGuiWindowFlags_NoBackground;
    if (!width_is_auto) {
        flags |= ImGuiWindowFlags_NoScrollWithMouse;
    }
    ImGui::SetCursorPosY(0);
    if (bg) {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(
            ImVec2(ImGui::GetCursorScreenPos().x - ImGui::GetStyle().WindowPadding.x, ImGui::GetCursorScreenPos().y),
            ImVec2(ImGui::GetCursorScreenPos().x + width, ImGui::GetCursorScreenPos().y + ImGui::GetWindowSize().y),
            ImGui::GetColorU32(ImVec4(0.145f, 0.145f, 0.145f, 0.5f)),
            24.0f * uiScale,
            ImDrawFlags_RoundCornersLeft
        );

        drawList->AddRectFilled(
            ImVec2(ImGui::GetCursorScreenPos().x + width, ImGui::GetCursorScreenPos().y + 2.0f),
            ImVec2(ImGui::GetCursorScreenPos().x + width + 2.5f * uiScale, ImGui::GetCursorScreenPos().y + ImGui::GetWindowSize().y - 2.0f),
            ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.2f))
        );
    }

    if (!width_is_auto) {
        width -= ImGui::GetStyle().WindowPadding.x;
    }


    ImGui::BeginChild(title.data(), ImVec2(
        width + initialWindowPadding.x,
        // width - ImGui::GetStyle().WindowPadding.x + initialWindowPadding.x,
        // ImGui::GetWindowSize().y  - ImGui::GetStyle().WindowPadding.y + initialWindowPadding.y
        ImGui::GetWindowSize().y
    ), false, flags);
    ImGui::SetScrollX(0);

    ImGui::SetCursorPosY(ImGui::GetStyle().WindowPadding.y);

    if (!width_is_auto) {
        width += ImGui::GetStyle().WindowPadding.x;
    } else {
        ImGui::PopStyleVar();
    }

    // ImGui::SetCursorPosX(orig_pos);

    // if (width < 0.0f) {
    //     ImGui::SetCursorPosX(orig_pos + ImGui::GetStyle().ItemSpacing.x);
    // }

    // if (bg) {
    //     ImGui::PopStyleColor();
    // }

    return std::make_pair(orig_pos + width, width - ImGui::GetStyle().WindowPadding.x);
}

double _fraction_getSpacing() {
    return ImGui::GetStyle().ItemSpacing.x;
}

void _section_end(float new_pos) {
    ImGui::Dummy(ImVec2(tabby::TabbyGlobalCfg::get().widgetWidth, ImGui::GetStyle().WindowPadding.y));
    ImGui::EndChild();

    float padding = ImGui::GetStyle().WindowPadding.y;
    ImGui::SetCursorPos(ImVec2(new_pos, padding));
}

TABBY_NS_END

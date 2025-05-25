#include "widgets/window.hpp"
#include "imgui.h"

TABBY_NS_BEGIN

void _window_begin(std::string_view content) {
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 24.0;
    style.WindowPadding = { 48.0, 48.0 };

    ImGui::Begin(content.data(), 0, 
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse
    );
}

void _window_end() {
    ImGui::End();
}

TABBY_NS_END
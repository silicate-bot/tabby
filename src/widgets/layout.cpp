#include "widgets/layout.hpp"

#include "imgui.h"
#include "util/config.hpp"

TABBY_NS_BEGIN

void same_line() { ImGui::SameLine(); }

void spacer(double spacing) {
    ImGui::SameLine(0.0f,
                    spacing == 0.0
                        ? ImGui::GetStyle().ItemSpacing.x
                        : (spacing * tabby::TabbyGlobalCfg::get().uiScale));
    // fill up all content enough so that no widgets are actually clipped off
    // this is basically available size - the width of the remaining content
    // auto windowSize = ImGui::GetWindowSize();
    // auto availSize = ImGui::GetContentRegionAvail();
    // ImGui::Dummy(ImVec2(availSize.x - windowSize.x / 2.0f, 0.0f));
    // ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.x);
}

void off_the_screen() {
    // goodbye
    ImGui::SetCursorPos(ImVec2(-100000.0f, -100000.0f));
}

void divider(bool addPaddingAbove) {
    if (addPaddingAbove) {
        ImGui::Dummy(ImVec2(0.0f, ImGui::GetStyle().ItemSpacing.y));
    }
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, ImGui::GetStyle().ItemSpacing.y));
}

TABBY_NS_END

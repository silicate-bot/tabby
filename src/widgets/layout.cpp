#include "widgets/layout.hpp"
#include "imgui.h"

TABBY_NS_BEGIN

void same_line() {
    ImGui::SameLine();
}

void spacer() {
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.x);
    // fill up all content enough so that no widgets are actually clipped off
    // this is basically available size - the width of the remaining content
    auto windowSize = ImGui::GetWindowSize();
    auto availSize = ImGui::GetContentRegionAvail();
    ImGui::Dummy(ImVec2(availSize.x - windowSize.x / 2.0f, 0.0f));
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.x);
}

TABBY_NS_END
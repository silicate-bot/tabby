#include "widgets/text.hpp"
#include "imgui.h"

TABBY_NS_BEGIN

void text(std::string_view content) {
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", content.data());
}

void text(std::string_view content, const Font& font) {
    font.apply();
    text(content);
    font.unapply();
}

TABBY_NS_END
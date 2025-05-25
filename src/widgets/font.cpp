#include "widgets/font.hpp"
#include "imgui.h"

TABBY_NS_BEGIN

Font Font::load(std::filesystem::path path, float fontSize) {
    void* font = ImGui::GetIO().Fonts->AddFontFromFileTTF(path.string().c_str(), fontSize);

    return Font(font);
}

void Font::apply() const {
    ImGui::PushFont((ImFont*)inner);
}

void Font::unapply() const {
    if (ImGui::GetFont() != inner) return; 

    ImGui::PopFont();
}

TABBY_NS_END
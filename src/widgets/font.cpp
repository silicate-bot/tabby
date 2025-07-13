#include "widgets/font.hpp"
#include "imgui.h"

TABBY_NS_BEGIN

Font Font::load(std::filesystem::path path, float fontSize) {
    ImFontConfig fontCfg;
    fontCfg.GlyphExtraAdvanceX = -1.0f;

    ImFont* font = ImGui::GetIO().Fonts->AddFontFromFileTTF(path.string().c_str(), fontSize, &fontCfg);

    return Font((void*)font);
}

void Font::apply() const {
    ImGui::PushFont((ImFont*)inner);
}

void Font::unapply() const {
    if (ImGui::GetFont() != inner) return; 

    ImGui::PopFont();
}

TABBY_NS_END
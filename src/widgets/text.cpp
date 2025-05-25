#include "widgets/text.hpp"
#include "imgui.h"

namespace tabby {
    void Text::draw() {
	ImGui::Text("%s", this->text.c_str());
    }
}


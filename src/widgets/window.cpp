#include "widgets/window.hpp"
#include "imgui.h"

void tabby::Window::draw() {
    ImGui::Begin(this->title.c_str());
}

void tabby::Window::postDraw() {
    ImGui::End();
}


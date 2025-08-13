#include "widgets/radio.hpp"
#include "imgui.h"

TABBY_NS_BEGIN

WidgetState _radio_impl(
    std::string_view label,
    bool value
) {
    WidgetState state = WidgetState::idle();
    if (ImGui::RadioButton(label.data(), value)) {
        state.pressed = true;
        state.changed = true;
    }

    state.hovered = ImGui::IsItemHovered();
    state.held = ImGui::IsItemActive();

    return state;
}

TABBY_NS_END
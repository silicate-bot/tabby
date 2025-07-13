#pragma once
#ifndef TABBY_WIDGETS_ANIMATED
#define TABBY_WIDGETS_ANIMATED

#include "util/defines.hpp"
#include <string_view>
#include "imgui_internal.h"
#include <unordered_map>
#include <print>

TABBY_NS_BEGIN


struct DragData {
    float hovered = 0.0f;
    float clicked = 0.0f;

    float x = 0.0f;
    float y = 0.0f;
};

template<typename T>
T& getStoredAnimValue(
    std::string_view id,
    T initial
) {
    ImGuiID imguiID = ImHashStr(id.data(), id.size(), 0);

    static std::unordered_map<ImGuiID, T> animatedValues;
    auto it = animatedValues.find(imguiID);
    if (it == animatedValues.end()) {
        animatedValues[imguiID] = initial;
        return initial;
    }

    return it->second;
}

template<typename T>
T interpolateValue(
    T value,
    T target,
    float speed = 15.0f
) {
    return ImLerp(value, target, ImGui::GetIO().DeltaTime * speed);
}

inline ImColor interpolateValue(
    float time,
    ImColor activeColor,
    ImColor inactiveColor
) {
    return ImGui::ColorConvertFloat4ToU32(ImLerp(inactiveColor.Value, activeColor.Value, time));
}

TABBY_NS_END

#endif
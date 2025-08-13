#pragma once
#ifndef TABBY_WIDGETS_ANIMATED
#define TABBY_WIDGETS_ANIMATED

#include "util/defines.hpp"
#include "util/config.hpp"

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
        animatedValues.insert({imguiID, initial});

        it = animatedValues.find(imguiID);
        if (it == animatedValues.end()) {
            abort();
        }

        return it->second;
    }

    return it->second;
}

template<typename T>
T interpolateValue(
    T value,
    T target,
    float speed = 15.0f
) {
    float speedMod = TabbyGlobalCfg::get().animationSpeed;
    if (!TabbyGlobalCfg::get().playAnimations) {
        return target;
    }

    // don't exceed 30fps delta time so animations don't lag
    return ImLerp(value, target, std::min(ImGui::GetIO().DeltaTime, (1.0f / 30.0f)) * speed * speedMod);
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

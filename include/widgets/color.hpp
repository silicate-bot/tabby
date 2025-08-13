#pragma once
#ifndef TABBY_WIDGETS_COLOR
#define TABBY_WIDGETS_COLO$

#include <array>
#include <functional>
#include <string_view>

#include "util/defines.hpp"
#include "widgets/state.hpp"

TABBY_NS_BEGIN

struct ColorState {
    bool showPopup = false;
    std::array<float, 4> colors = {0.0, 0.0, 0.0, 0.0};
};

tabby::WidgetState color(std::string_view label, ColorState& state,
                         std::function<void()> renderPopup);

TABBY_NS_END

#endif

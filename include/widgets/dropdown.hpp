#pragma once
#ifndef TABBY_WIDGETS_DROPDOWN
#define TABBY_WIDGETS_DROPDOWN

#include <functional>
#include <string>
#include <string_view>
#include <vector>

#include "util/defines.hpp"
#include "widgets/state.hpp"

TABBY_NS_BEGIN

struct DropdownState {
    bool showPopup = false;
    std::vector<std::string_view> options;
    int selectedIndex = -1;
    int highlightedIndex = -1;
    int clickedIndex = -1;
};

WidgetState dropdown(std::string_view label, DropdownState& state,
                     int& selectedIndex,
                     std::function<void()> renderPopup = nullptr, bool visibleLabel = true);

TABBY_NS_END

#endif

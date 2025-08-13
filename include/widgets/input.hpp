#ifndef TABBY_WIDGETS_INPUT
#define TABBY_WIDGETS_INPUT

#include "util/defines.hpp"
#include <string_view>
#include <string>
#include <vector>
#include <functional>
#include "widgets/state.hpp"

TABBY_NS_BEGIN

WidgetState input_text(std::string_view label, std::string_view hint, std::string& buffer);

struct AutocompleteState {
    bool showPopup = false;
    int selectedIndex = -1;
    int highlightedIndex = -1;
    int clickedIndex = -1;
    bool needsScroll = false;
    std::vector<std::string> suggestions;
    std::string* buffer = nullptr;
};

WidgetState input_text_autocomplete(std::string_view label, std::string_view hint, std::string& buffer, AutocompleteState& state, std::function<void()> renderPopup = nullptr);

TABBY_NS_END

#endif
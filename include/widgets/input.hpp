#ifndef TABBY_WIDGETS_INPUT
#define TABBY_WIDGETS_INPUT

#include "util/defines.hpp"
#include <string_view>
#include <string>
#include <vector>

TABBY_NS_BEGIN

void input_text(std::string_view label, std::string_view hint, std::string& buffer);

struct AutocompleteState {
    bool showPopup = false;
    int selectedIndex = -1;
    int clickedIndex = -1;
    std::vector<std::string> suggestions;
    std::string* buffer = nullptr;
};

void input_text_autocomplete(std::string_view label, std::string_view hint, std::string& buffer, AutocompleteState& state);

TABBY_NS_END

#endif
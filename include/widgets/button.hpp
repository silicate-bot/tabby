#pragma once
#ifndef TABBY_WIDGETS_BUTTON
#define TABBY_WIDGETS_BUTTON

#include "util/defines.hpp"
#include "widgets/state.hpp"

#include <string>

TABBY_NS_BEGIN

WidgetState button(std::string_view content, float width = 1.0f);
WidgetState button_selector(std::string_view content, bool selected);

TABBY_NS_END

#endif // TABBY_WIDGETS_TEXT


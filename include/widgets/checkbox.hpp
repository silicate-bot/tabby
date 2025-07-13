#ifndef TABBY_WIDGETS_CHECKBOX
#define TABBY_WIDGETS_CHECKBOX

#include "util/defines.hpp"
#include <string_view>
#include "widgets/state.hpp"

TABBY_NS_BEGIN

WidgetState checkbox(std::string_view label, bool& value);

TABBY_NS_END

#endif
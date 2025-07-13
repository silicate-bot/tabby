#pragma once
#ifndef TABBY_WIDGETS_TEXT
#define TABBY_WIDGETS_TEXT

#include "util/defines.hpp"
#include "widgets/font.hpp"

#include <string>

TABBY_NS_BEGIN

void text(std::string_view content);
void text(std::string_view content, const Font& font);

TABBY_NS_END

#endif // TABBY_WIDGETS_TEXT

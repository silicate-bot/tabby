#pragma once
#ifndef TABBY_WIDGETS_LAYOUT
#define TABBY_WIDGETS_LAYOUT

#include "util/defines.hpp"

TABBY_NS_BEGIN

void same_line();
void spacer(double spacing = 0.0);
void off_the_screen();
void divider(bool addPaddingAbove = true);

TABBY_NS_END

#endif
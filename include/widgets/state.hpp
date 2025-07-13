#pragma once
#ifndef TABBY_WIDGETS_STATE
#define TABBY_WIDGETS_STATE

#include "util/defines.hpp"

TABBY_NS_BEGIN

struct WidgetState {
    bool pressed = false;
    bool hovered = false;
    bool held = false;
    bool changed = false;

    static WidgetState idle() { return WidgetState{}; }
};

TABBY_NS_END

#endif


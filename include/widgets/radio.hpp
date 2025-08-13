#pragma once
#ifndef TABBY_WIDGETS_RADIO
#define TABBY_WIDGETS_RADIO

#include "util/defines.hpp"
#include "widgets/state.hpp"
#include <string_view>

TABBY_NS_BEGIN

WidgetState _radio_impl(
    std::string_view label,
    bool value
);

template<typename T>
WidgetState radio(
    T& value,
    T target,
    std::string_view label
) {
    WidgetState state = _radio_impl(label, value == target);
    if (state.pressed) {
        value = target;
    }

    return state;
}

TABBY_NS_END

#endif
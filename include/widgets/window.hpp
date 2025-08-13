#pragma once
#ifndef TABBY_WIDGETS_WINDOW
#define TABBY_WIDGETS_WINDOW

#include <concepts>
#include <string>

#include "util/config.hpp"
#include "util/defines.hpp"
#include "widgets/layout.hpp"

TABBY_NS_BEGIN

double _window_begin(std::string_view title);
void _window_end();

template <typename F>
    requires std::is_invocable_v<F>
void group(F&& f, double width) {
    double previousWidth = TabbyGlobalCfg::get().widgetWidth;
    TabbyGlobalCfg::get().widgetWidth = width;

    f();

    TabbyGlobalCfg::get().widgetWidth = previousWidth;
}

void _stack_begin();
void _stack_end();

template <typename F>
    requires std::is_invocable_v<F>
void stack(F&& f) {
    _stack_begin();

    f();

    _stack_end();
}

double _fraction_getSpacing();

template <typename F>
    requires std::is_invocable_v<F>
void fraction(double denominator, F&& f, double customSpacing = 0.0) {
    double spacing = customSpacing == 0.0
                         ? _fraction_getSpacing()
                         : customSpacing * tabby::TabbyGlobalCfg::get().uiScale;
    spacing *= (denominator - 1.0) / denominator;

    group(f, TabbyGlobalCfg::get().widgetWidth / denominator - spacing);
}

template <typename F>
    requires std::is_invocable_v<F>
void inset(F&& f) {
    double spacing = _fraction_getSpacing() * 4.0;
    tabby::spacer(spacing);

    spacing *= tabby::TabbyGlobalCfg::get().uiScale;
    group([&]() { stack(f); },
          tabby::TabbyGlobalCfg::get().widgetWidth - spacing);
}

template <typename F>
    requires std::is_invocable_v<F>
void window(std::string_view title, F&& f) {
    double width = _window_begin(title);

    group(f, width);

    _window_end();
}

std::pair<float, float> _section_begin(std::string_view title,
                                       float width = -1.0f, bool bg = false);
void _section_end(float new_pos);

template <typename F>
    requires std::is_invocable_v<F>
void section(std::string_view title, F&& f, float width = -1.0f,
             bool bg = false) {
    auto [new_pos, section_width] = _section_begin(title, width, bg);

    group(f, section_width);

    _section_end(new_pos);
}

template <typename T, typename F>
    requires std::is_invocable_v<F>
void tab(T& value, T target, F&& f) {
    if (value == target) {
        f();
    }
}

TABBY_NS_END

#endif

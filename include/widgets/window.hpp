#ifndef TABBY_WIDGETS_WINDOW
#define TABBY_WIDGETS_WINDOW

#include "util/defines.hpp"
#include <concepts>
#include <string>

TABBY_NS_BEGIN

void _window_begin(std::string_view title);
void _window_end();

template<typename F>
    requires std::is_invocable_v<F>
void window(std::string_view title, F&& f) {
    _window_begin(title);

    f();

    _window_end();
}

TABBY_NS_END

#endif

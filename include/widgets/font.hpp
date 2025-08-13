#pragma once
#ifndef TABBY_WIDGETS_FONT
#define TABBY_WIDGETS_FONT

#include "util/defines.hpp"
#include <filesystem>

TABBY_NS_BEGIN

class Font {
private:
    void* inner;

public:
    Font(void* inner) : inner(inner) {}
    static Font load(std::filesystem::path path, float fontSize);

    void apply() const;
    void unapply() const;
};

class ScopedFont {
    Font* ref;

public:
    ScopedFont(Font& ref) : ref(&ref) {
        ref.apply();
    }

    ~ScopedFont() {
        ref->unapply();
    }
};

TABBY_NS_END

#endif
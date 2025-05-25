#ifndef TABBY_WIDGETS_TEXT
#define TABBY_WIDGETS_TEXT

#include "util/defines.hpp"
#include "widgets/drawable.hpp"

#include <string>

TABBY_NS_BEGIN

class Text : public Drawable {
    std::string text;

    void draw() override;
};

TABBY_NS_END

#endif // TABBY_WIDGETS_TEXT

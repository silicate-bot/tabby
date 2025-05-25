#ifndef TABBY_WIDGETS_WINDOW
#define TABBY_WIDGETS_WINDOW

#include "util/defines.hpp"
#include "widgets/drawable.hpp"

#include <string>

TABBY_NS_BEGIN

struct Window : public Drawable {
    std::string title;

    void draw() override;
    void postDraw() override;
};

TABBY_NS_END

#endif

#ifndef TABBY_WIDGETS_DRAWABLE
#define TABBY_WIDGETS_DRAWABLE

#include "util/defines.hpp"
#include <vector>
#include <memory>

TABBY_NS_BEGIN

class Drawable {
    std::vector<std::unique_ptr<Drawable>> m_children;

public:
    virtual void draw() = 0;
    virtual void postDraw() {};

    void add(std::unique_ptr<Drawable> widget) {
	m_children.push_back(std::move(widget));
    }
    
    void drawAll() {
	this->draw();
	for (auto& widget : m_children) {
	    widget->drawAll();
	}
	this->postDraw();
    }

    virtual ~Drawable() = default;
};

TABBY_NS_END

#endif

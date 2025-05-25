#ifndef TABBY_CONTEXT
#define TABBY_CONTEXT

#include "util/defines.hpp"
#include "widgets/window.hpp"

#include <functional>
#include <vector>

TABBY_NS_BEGIN

class Context : public Drawable {
public:

    struct ctx_init_params {
#ifdef TABBY_USE_GLFW
	void* glfw_window;
#endif
#ifdef TABBY_USE_WIN32
	void* hdc;
#endif
    };

    void init(ctx_init_params params);
    void draw() override;
    void postDraw() override;
};

TABBY_NS_END

#endif // TABBY_CONTEXT

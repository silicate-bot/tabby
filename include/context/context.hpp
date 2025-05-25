#ifndef TABBY_CONTEXT
#define TABBY_CONTEXT

#include "util/defines.hpp"

#include <functional>
#include <vector>
#include <concepts>

TABBY_NS_BEGIN

class Context {
public:

    struct CtxInitParams {
#ifdef TABBY_USE_GLFW
	void* glfwWindow;
#endif
#ifdef TABBY_USE_WIN32
	void* hdc;
#endif
    };

    void init(CtxInitParams params);

    void newFrame();
    void render();

    template<typename F>
        requires std::is_invocable_v<F>
    void draw(F&& f) {
        this->newFrame();
		f();
        this->render();
    }
};

TABBY_NS_END

#endif // TABBY_CONTEXT

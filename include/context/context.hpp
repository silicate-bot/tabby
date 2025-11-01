#ifndef TABBY_CONTEXT
#define TABBY_CONTEXT

#include "util/defines.hpp"

#include <functional>
#include <vector>
#include <concepts>

#include "Windows.h"

TABBY_NS_BEGIN

class Context {
public:

    struct CtxInitParams {
	void* hdc;
    };

    void* h_wnd = nullptr;


    void init(CtxInitParams params);
    void destroy();

    void newFrame();
    void render();
    bool handleWndproc(
        HWND hWnd,
        UINT msg,
        WPARAM wParam,
        LPARAM lParam
    );

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

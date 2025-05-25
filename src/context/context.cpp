#include "context/context.hpp"
#include "imgui.h"
#include <print>

#ifdef TABBY_USE_WIN32
#include <Windows.h>
#include "backends/imgui_impl_win32.h"
#endif

#ifdef TABBY_USE_OPENGL3
#include "backends/imgui_impl_opengl3.h"
#endif

#ifdef TABBY_USE_GLFW
#include <GLFW/glfw3.h>
#include "backends/imgui_impl_glfw.h"
#endif

namespace tabby {
    void Context::init(ctx_init_params params) {
	ImGui::CreateContext();

#ifdef TABBY_USE_OPENGL3
	std::println("Initializing OpenGL3...");
	ImGui_ImplOpenGL3_Init(); // Assume OpenGL is loaded
#endif
#ifdef TABBY_USE_WIN32
	h_wnd = WindowFromHDC((HDC*)params.hdc);
	ImGui_ImplWin32_InitForOpenGL(h_wnd);
#endif
#ifdef TABBY_USE_GLFW
	std::println("Initializing GLFW...");
	ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)params.glfw_window, true);
#endif
    }
    
    void Context::draw() {
#ifdef TABBY_USE_OPENGL3
	ImGui_ImplOpenGL3_NewFrame();
#endif
#ifdef TABBY_USE_WIN32
	ImGui_ImplWin32_NewFrame();
#endif
#ifdef TABBY_USE_GLFW
	ImGui_ImplGlfw_NewFrame();
#endif

	ImGui::NewFrame();
    }

    void Context::postDraw() {
	ImGui::Render();
#ifdef TABBY_USE_OPENGL3
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
    }
}

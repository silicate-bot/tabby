#include "context/context.hpp"
#include "imgui.h"
#include <print>

#include <Windows.h>
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_opengl3.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace tabby
{
	void Context::init(CtxInitParams params)
	{
		ImGui::CreateContext();

		ImGui_ImplOpenGL3_Init(); // Assume OpenGL is loaded
		h_wnd = WindowFromDC((HDC)params.hdc);
		ImGui_ImplWin32_InitForOpenGL(h_wnd);

		ImGui::GetIO().Fonts->Build();
		ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
	}

	void Context::destroy() {
    	ImGui_ImplWin32_Shutdown();
    	ImGui_ImplOpenGL3_Shutdown();

	    ImGui::DestroyContext();
	}

	void Context::newFrame() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();
	}

	void Context::render() {
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

    bool Context::handleWndproc(
        HWND hWnd,
        UINT msg,
        WPARAM wParam,
        LPARAM lParam
    ) {
        return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
    }
}

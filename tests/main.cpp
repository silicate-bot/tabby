#include <GLFW/glfw3.h>
#include <iostream>
#include <print>
#include <tabby.hpp>

void error_cb(int error, const char* desc) {
    std::print(std::cerr, "Error: {}, Description: {}", error, desc);
}

int main() {
    glfwSetErrorCallback(error_cb);
    if (!glfwInit()) {
	std::print(std::cerr, "Failed to initialize.");
	return 1;
    }

    GLFWwindow* wnd = glfwCreateWindow(960, 540, "tabby", 0, 0);
    if (!wnd) {
	std::print(std::cerr, "No window created");
	return 1;
    }

    glfwMakeContextCurrent(wnd);

    tabby::Context ctx;
    ctx.init({ wnd });

    while (!glfwWindowShouldClose(wnd)) {
	glfwPollEvents();
	
	int wnd_w, wnd_h;
	glfwGetFramebufferSize(wnd, &wnd_w, &wnd_h);
	glViewport(0, 0, wnd_w, wnd_h);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	ctx.draw();

	glfwSwapBuffers(wnd);
    }

    glfwDestroyWindow(wnd);
    glfwTerminate();
}

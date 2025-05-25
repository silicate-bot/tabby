#include <GLFW/glfw3.h>
#include <iostream>
#include <print>
#include <tabby.hpp>

void error_cb(int error, const char *desc)
{
    std::print(std::cerr, "Error: {}, Description: {}", error, desc);
}

int main()
{
    glfwSetErrorCallback(error_cb);
    if (!glfwInit())
    {
        std::print(std::cerr, "Failed to initialize.");
        return 1;
    }

    GLFWwindow *wnd = glfwCreateWindow(960, 540, "tabby", 0, 0);
    if (!wnd)
    {
        std::print(std::cerr, "No window created");
        return 1;
    }

    glfwMakeContextCurrent(wnd);

    tabby::Context ctx;
    ctx.init({wnd});

    tabby::Font font = tabby::Font::load("C:\\Users\\Natalia\\AppData\\Local\\Microsoft\\Windows\\Fonts\\Inter-variablefont_opsz,wght.ttf", 20.0);
    tabby::Font fontBig = tabby::Font::load("C:\\Users\\Natalia\\AppData\\Local\\Microsoft\\Windows\\Fonts\\Rinter.ttf", 24.0);

    while (!glfwWindowShouldClose(wnd))
    {
        glfwPollEvents();

        int wnd_w, wnd_h;
        glfwGetFramebufferSize(wnd, &wnd_w, &wnd_h);
        glViewport(0, 0, wnd_w, wnd_h);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        ctx.draw([&](){
            tabby::ScopedFont s(font);

            tabby::window("meow!", [&](){
                tabby::text("big text", fontBig);
                tabby::text("raaaah Raaaah Killing people");
            });
        });

        glfwSwapBuffers(wnd);
    }

    glfwDestroyWindow(wnd);
    glfwTerminate();
}

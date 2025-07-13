// #include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <print>
#include <tabby.hpp>
#include <image/stb_image.h>

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
    tabby::Font fontBig = tabby::Font::load("C:\\Users\\Natalia\\AppData\\Local\\Microsoft\\Windows\\Fonts\\Rinter.ttf", 28.0);

    // std::filesystem::path backgroundImg = "assets\\background.png";
    // if (!std::filesystem::exists(backgroundImg))
    // {
    //     std::print(std::cerr, "Background image not found at: {}\n", backgroundImg.string());
    //     return 1;
    // }

    // GLuint bgTex;

    // glGenTextures(1, &bgTex);
    // glBindTexture(GL_TEXTURE_2D, bgTex);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // int width, height, channels;
    // unsigned char *data = stbi_load(backgroundImg.string().c_str(), &width, &height, &channels, 0);
    // if (!data)
    // {
    //     std::print(std::cerr, "Failed to load image: {}\n", backgroundImg.string());
    //     return 1;
    // }

    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    // stbi_image_free(data);
    // glBindTexture(GL_TEXTURE_2D, 0);

    // float verts [] = {
    //     -1.0f, -1.0f, 0.0f, 0.0f,
    //     1.0f, -1.0f, 1.0f, 0.0f,
    //     -1.0f, 1.0f, 0.0f, 1.0f,
    //     1.0f, 1.0f, 1.0f, 1.0f
    // };

    // GLuint vbo;
    // glGenBuffers(1, &vbo);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    // GLuint vao;
    // glGenVertexArrays(1, &vao);
    // glBindVertexArray(vao);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    // glEnableVertexAttribArray(0);

    double tps = 240.0f;
    std::string text = "edit me";
    std::vector<std::string> suggestions = {
        "Arcturus",
        "Wistfully",
        "ballistic wistfully",
        "CYCLOLCYC",
        "Psychomaki",
        "Necrotic Majesty",
        "Necrotic Majesty_2"
    };

    tabby::AutocompleteState state;
    bool frame_advance = false;

    while (!glfwWindowShouldClose(wnd))
    {
        glfwPollEvents();

        int wnd_w, wnd_h;
        glfwGetFramebufferSize(wnd, &wnd_w, &wnd_h);
        glViewport(0, 0, wnd_w, wnd_h);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        // glBindTexture(GL_TEXTURE_2D, bgTex);
        // glBindVertexArray(vao);
        // glEnableVertexAttribArray(0);
        // glEnableVertexAttribArray(1);
        // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        // glDisableVertexAttribArray(0);
        // glDisableVertexAttribArray(1);
        // glBindVertexArray(0);

        ctx.draw([&](){
            tabby::ScopedFont s(font);

            tabby::window("meow!", [&](){
                tabby::text("very silly cat", fontBig);

                tabby::text("Replay Name");
                tabby::spacer();

                // filter suggestions (case insensitive)
                state.suggestions.clear();
                for (const auto& suggestion : suggestions) {
                    std::string lowerSuggestion = suggestion;
                    std::transform(lowerSuggestion.begin(), lowerSuggestion.end(), lowerSuggestion.begin(), ::tolower);
                    std::string lowerText = text;
                    std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);
                    if (lowerSuggestion.find(lowerText) != std::string::npos) {
                        state.suggestions.push_back(suggestion);
                    }
                }

                tabby::input_text_autocomplete("Replay Name##Input", "Enter replay name", text, state);

                if (tabby::button("Load").pressed) {
                    std::print("Button clicked!\n");
                }

                tabby::same_line();

                if (tabby::button("Save").pressed) {
                    std::print("Button clicked!\n");
                }

                tabby::text("TPS");
                tabby::spacer();
                if (tabby::drag("TPS##Input", tps, 0.0, 100000.0, 1.0).changed) {
                    std::print("TPS changed to: {}\n", tps);
                }

                tabby::checkbox("Frame Advance", frame_advance);
            });
        });

        glfwSwapBuffers(wnd);
    }

    glfwDestroyWindow(wnd);
    glfwTerminate();
}

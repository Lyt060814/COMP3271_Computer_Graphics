#include "triangle_app.hpp"

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <array>
#include <iostream>

namespace {

constexpr int initial_width = 600;
constexpr int initial_height = 600;

constexpr std::array<std::array<std::array<double, 2>, 3>, 4> example_triangles{
    {
     {{{-0.5, -0.5}, {0.0, 0.5}, {0.5, -0.5}}},
     {{{-0.5, -0.5}, {-0.25, 0.0}, {0.0, -0.5}}},
     {{{-0.25, 0.0}, {0.0, 0.5}, {0.25, 0.0}}},
     {{{0.0, -0.5}, {0.25, 0.0}, {0.5, -0.5}}},
     }
};

bool previous_left_pressed = false;
bool previous_clear_pressed = false;
bool previous_example_pressed = false;

void framebuffer_size_callback(GLFWwindow *, int width, int height) {
    glViewport(0, 0, width, height);
}

void load_example() {
    ClearTriangles();
    for (const auto &triangle : example_triangles) {
        for (const auto &point : triangle) { MouseInteraction(point[0], point[1]); }
    }
}

void process_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    const bool clear_pressed = glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS;
    if (clear_pressed && !previous_clear_pressed) { ClearTriangles(); }
    previous_clear_pressed = clear_pressed;

    const bool example_pressed = glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS;
    if (example_pressed && !previous_example_pressed) { load_example(); }
    previous_example_pressed = example_pressed;

    const bool left_pressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    if (!left_pressed && previous_left_pressed) {
        double cursor_x = 0.0;
        double cursor_y = 0.0;
        int window_width = 1;
        int window_height = 1;
        glfwGetCursorPos(window, &cursor_x, &cursor_y);
        glfwGetWindowSize(window, &window_width, &window_height);

        const double x = cursor_x / static_cast<double>(window_width) * 2.0 - 1.0;
        const double y = 1.0 - cursor_y / static_cast<double>(window_height) * 2.0;
        MouseInteraction(x, y);
    }
    previous_left_pressed = left_pressed;
}

} // namespace

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW.\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

    GLFWwindow *window = glfwCreateWindow(
        initial_width, initial_height, "Assignment 1 - Part A: Triangle Drawing", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create an OpenGL 2.1 window.\n";
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    int framebuffer_width = 0;
    int framebuffer_height = 0;
    glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
    glViewport(0, 0, framebuffer_width, framebuffer_height);

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT);
        DrawTriangles();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

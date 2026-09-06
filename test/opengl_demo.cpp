// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

// settings
constexpr unsigned int SCR_WIDTH = 600;
constexpr unsigned int SCR_HEIGHT = 600;

int mode = 1;

// process all input:
// query GLFW whether relevant keys are pressed/released  and react accordingly
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);
}

int main() {
    // glfw: initialize and configure
    glfwInit();

    // glfw window creation, set viewport with width=800 and height=600
    auto window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL_Demo", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // render loop
    while (!glfwWindowShouldClose(window)) {

        processInput(window);

        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT); // Clear the display

        glBegin(GL_TRIANGLES); // Draw filled triangle
        // Specify color for each vertex of triangle
        glColor3f(1.f, 0.f, 0.f);
        // Specify position for each vertex of triangle
        glVertex3f(-0.3f, -0.3f, 0.f);
        glColor3f(0.f, 1.f, 0.f);
        glVertex3f(0.3f, -0.3f, 0.f);
        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(0.f, 0.3f, 0.f);
        glEnd(); // OpenGL draws the filled triangle

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
        // etc.)
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // glfw: terminate, clearing addl previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

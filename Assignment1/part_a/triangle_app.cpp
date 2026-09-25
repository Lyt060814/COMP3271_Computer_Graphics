#include "triangle_app.hpp"

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <array>
#include <vector>

namespace {

struct Triangle {
    std::array<std::array<double, 2>, 3> vertices{};
    int color_index = 0;
};

std::vector<Triangle> triangles;
Triangle triangle_to_draw;
int point_count = 0;

constexpr std::array<std::array<double, 3>, 11> color_array{
    {
     {0.9, 0.0, 0.0},
     {0.0, 0.5, 0.4},
     {0.1, 0.2, 0.46},
     {0.9, 0.9, 0.0},
     {0.0, 1.0, 0.0},
     {0.0, 1.0, 1.0},
     {0.0, 0.0, 1.0},
     {1.0, 0.0, 1.0},
     {0.9, 0.6, 0.0},
     {0.9, 1.0, 0.6},
     {0.2, 0.2, 0.2},
     }
};

} // namespace

void ClearTriangles() {
    triangles.clear();
    point_count = 0;
}

void DrawTriangles() {

    glColor3d(1.0, 1.0, 1.0);
    glPointSize(5.0F);
    // ===== STUDENT_TASK_BEGIN: part_a_draw_triangles =====
    glBegin(GL_POINTS);
    for (int i = 0; i < point_count; ++i) {
        const auto &point = triangle_to_draw.vertices[static_cast<std::size_t>(i)];
        glVertex2d(point[0], point[1]);
    }
    glEnd();

    glBegin(GL_TRIANGLES);
    for (const Triangle &triangle : triangles) {
        const auto &color = color_array[static_cast<std::size_t>(triangle.color_index)];
        glColor3d(color[0], color[1], color[2]);
        for (const auto &vertex : triangle.vertices) { glVertex2d(vertex[0], vertex[1]); }
    }
    glEnd();
    // ===== STUDENT_TASK_END: part_a_draw_triangles =====
}

void MouseInteraction(double m_x, double m_y) {
    // ===== STUDENT_TASK_BEGIN: part_a_mouse_interaction =====
    triangle_to_draw.vertices[static_cast<std::size_t>(point_count)] = {m_x, m_y};
    ++point_count;

    if (point_count == 3) {
        triangle_to_draw.color_index =
            static_cast<int>(triangles.size() % color_array.size());
        triangles.push_back(triangle_to_draw);
        point_count = 0;
    }
    // ===== STUDENT_TASK_END: part_a_mouse_interaction =====
}

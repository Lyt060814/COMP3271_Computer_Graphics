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
    // ===== STUDENT_TASK_END: part_a_draw_triangles =====
}

void MouseInteraction(double m_x, double m_y) {
    // ===== STUDENT_TASK_BEGIN: part_a_mouse_interaction =====
    // ===== STUDENT_TASK_END: part_a_mouse_interaction =====
}

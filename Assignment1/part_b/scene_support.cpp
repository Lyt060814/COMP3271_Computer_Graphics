#include "scene_support.hpp"

#include <imgui.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <utility>

namespace {

void append_quad(
    std::vector<SceneVertex> &vertices, std::vector<unsigned int> &indices,
    const std::array<Vec3, 4> &positions, const Vec3 &normal) {
    const auto first = static_cast<unsigned int>(vertices.size());
    for (const Vec3 &position : positions) { vertices.push_back({position, normal}); }
    indices.insert(indices.end(), {first, first + 1, first + 2, first, first + 2, first + 3});
}

void append_triangle(
    std::vector<SceneVertex> &vertices, std::vector<unsigned int> &indices, const Vec3 &a,
    const Vec3 &b, const Vec3 &c) {
    const Vec3 normal = Normalize(Cross(b - a, c - a));
    const auto first = static_cast<unsigned int>(vertices.size());
    vertices.push_back({a, normal});
    vertices.push_back({b, normal});
    vertices.push_back({c, normal});
    indices.insert(indices.end(), {first, first + 1, first + 2});
}

void append_box(
    std::vector<SceneVertex> &vertices, std::vector<unsigned int> &indices, const Vec3 &center,
    const Vec3 &half_extents) {
    const Vec3 min = center - half_extents;
    const Vec3 max = center + half_extents;
    append_quad(
        vertices, indices,
        {
            {{min.x, min.y, max.z},
             {max.x, min.y, max.z},
             {max.x, max.y, max.z},
             {min.x, max.y, max.z}}
    },
        {0.0F, 0.0F, 1.0F});
    append_quad(
        vertices, indices,
        {
            {{max.x, min.y, min.z},
             {min.x, min.y, min.z},
             {min.x, max.y, min.z},
             {max.x, max.y, min.z}}
    },
        {0.0F, 0.0F, -1.0F});
    append_quad(
        vertices, indices,
        {
            {{max.x, min.y, max.z},
             {max.x, min.y, min.z},
             {max.x, max.y, min.z},
             {max.x, max.y, max.z}}
    },
        {1.0F, 0.0F, 0.0F});
    append_quad(
        vertices, indices,
        {
            {{min.x, min.y, min.z},
             {min.x, min.y, max.z},
             {min.x, max.y, max.z},
             {min.x, max.y, min.z}}
    },
        {-1.0F, 0.0F, 0.0F});
    append_quad(
        vertices, indices,
        {
            {{min.x, max.y, max.z},
             {max.x, max.y, max.z},
             {max.x, max.y, min.z},
             {min.x, max.y, min.z}}
    },
        {0.0F, 1.0F, 0.0F});
    append_quad(
        vertices, indices,
        {
            {{min.x, min.y, min.z},
             {max.x, min.y, min.z},
             {max.x, min.y, max.z},
             {min.x, min.y, max.z}}
    },
        {0.0F, -1.0F, 0.0F});
}

void set_matrix_uniform(const Shader &shader, const char *name, const Mat4 &matrix) {
    shader.use();
    const GLint location = glGetUniformLocation(shader.handle(), name);
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix.data());
}

} // namespace

Mesh::Mesh(std::vector<SceneVertex> vertices, std::vector<unsigned int> indices, GLenum primitive) :
    index_count_(static_cast<GLsizei>(indices.size())), primitive_(primitive) {
    vertex_array_.bind();

    vertex_buffer_.bind(GL_ARRAY_BUFFER);
    glBufferData(
        GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(SceneVertex)),
        vertices.data(), GL_STATIC_DRAW);

    element_buffer_.bind(GL_ELEMENT_ARRAY_BUFFER);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
        indices.data(), GL_STATIC_DRAW);

    vertex_array_.set_attrib(
        0, 3, GL_FLOAT, GL_FALSE, sizeof(SceneVertex),
        reinterpret_cast<const void *>(offsetof(SceneVertex, position)));
    vertex_array_.set_attrib(
        1, 3, GL_FLOAT, GL_FALSE, sizeof(SceneVertex),
        reinterpret_cast<const void *>(offsetof(SceneVertex, normal)));

    GLBuffer::unbind(GL_ARRAY_BUFFER);
    GLVertexArray::unbind();
}

void Mesh::draw() const {
    vertex_array_.bind();
    glDrawElements(primitive_, index_count_, GL_UNSIGNED_INT, nullptr);
    GLVertexArray::unbind();
}

std::unique_ptr<Mesh> MakeBoxMesh(float half_x, float half_y, float half_z) {
    std::vector<SceneVertex> vertices;
    std::vector<unsigned int> indices;
    append_box(vertices, indices, {}, {half_x, half_y, half_z});
    return std::make_unique<Mesh>(std::move(vertices), std::move(indices), GL_TRIANGLES);
}

std::unique_ptr<Mesh> MakePyramidMesh() {
    std::vector<SceneVertex> vertices;
    std::vector<unsigned int> indices;
    constexpr float base = 0.55F;
    constexpr float bottom = -0.5F;
    constexpr float top = 0.65F;
    const Vec3 apex{0.0F, top, 0.0F};

    append_quad(
        vertices, indices,
        {
            {{-base, bottom, -base},
             {base, bottom, -base},
             {base, bottom, base},
             {-base, bottom, base}}
    },
        {0.0F, -1.0F, 0.0F});
    append_triangle(vertices, indices, {-base, bottom, base}, {base, bottom, base}, apex);
    append_triangle(vertices, indices, {base, bottom, base}, {base, bottom, -base}, apex);
    append_triangle(vertices, indices, {base, bottom, -base}, {-base, bottom, -base}, apex);
    append_triangle(vertices, indices, {-base, bottom, -base}, {-base, bottom, base}, apex);

    return std::make_unique<Mesh>(std::move(vertices), std::move(indices), GL_TRIANGLES);
}

std::unique_ptr<Mesh> MakeSphereMesh(float radius, int slices, int stacks) {
    std::vector<SceneVertex> vertices;
    std::vector<unsigned int> indices;
    constexpr float pi = 3.14159265358979323846F;

    for (int stack = 0; stack <= stacks; ++stack) {
        const float latitude = pi * static_cast<float>(stack) / static_cast<float>(stacks);
        const float y = std::cos(latitude);
        const float ring_radius = std::sin(latitude);
        for (int slice = 0; slice <= slices; ++slice) {
            const float longitude =
                2.0F * pi * static_cast<float>(slice) / static_cast<float>(slices);
            const Vec3 normal{
                ring_radius * std::cos(longitude), y, ring_radius * std::sin(longitude)};
            vertices.push_back({normal * radius, normal});
        }
    }

    const int row_size = slices + 1;
    for (int stack = 0; stack < stacks; ++stack) {
        for (int slice = 0; slice < slices; ++slice) {
            const auto top_left = static_cast<unsigned int>(stack * row_size + slice);
            const auto bottom_left = static_cast<unsigned int>((stack + 1) * row_size + slice);
            indices.insert(
                indices.end(),
                {top_left, bottom_left, top_left + 1, top_left + 1, bottom_left, bottom_left + 1});
        }
    }

    return std::make_unique<Mesh>(std::move(vertices), std::move(indices), GL_TRIANGLES);
}

std::unique_ptr<Mesh> MakeLineMesh(const std::vector<Vec3> &positions) {
    std::vector<SceneVertex> vertices;
    std::vector<unsigned int> indices;
    vertices.reserve(positions.size());
    indices.reserve(positions.size());
    for (std::size_t i = 0; i < positions.size(); ++i) {
        vertices.push_back({
            positions[i], {0.0F, 1.0F, 0.0F}
        });
        indices.push_back(static_cast<unsigned int>(i));
    }
    return std::make_unique<Mesh>(std::move(vertices), std::move(indices), GL_LINES);
}

std::unique_ptr<Mesh> MakeGridMesh() {
    std::vector<Vec3> positions;
    constexpr int line_count = 10;
    constexpr float spacing = 0.5F;
    constexpr float extent = line_count * spacing;
    for (int i = -line_count; i <= line_count; ++i) {
        const float coordinate = static_cast<float>(i) * spacing;
        positions.push_back({-extent, 0.0F, coordinate});
        positions.push_back({extent, 0.0F, coordinate});
        positions.push_back({coordinate, 0.0F, -extent});
        positions.push_back({coordinate, 0.0F, extent});
    }
    return MakeLineMesh(positions);
}

Vec3 OrbitCamera::position() const {
    const float horizontal = distance_ * std::cos(pitch_);
    return {
        target_.x + horizontal * std::sin(yaw_),
        target_.y + distance_ * std::sin(pitch_),
        target_.z + horizontal * std::cos(yaw_),
    };
}

Mat4 OrbitCamera::view_matrix() const {
    return MakeLookAt(position(), target_, {0.0F, 1.0F, 0.0F});
}

void OrbitCamera::update_from_input() {
    ImGuiIO &io = ImGui::GetIO();
    if (!io.WantCaptureMouse && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
        yaw_ -= io.MouseDelta.x * 0.008F;
        pitch_ += io.MouseDelta.y * 0.008F;
        pitch_ = std::clamp(pitch_, -1.4F, 1.4F);
    }
    if (!io.WantCaptureMouse && io.MouseWheel != 0.0F) {
        distance_ *= std::pow(0.9F, io.MouseWheel);
        distance_ = std::clamp(distance_, 2.0F, 20.0F);
    }
}

SceneRenderer::SceneRenderer() :
    shader_("Assignment1/shaders/scene.vert", "Assignment1/shaders/scene.frag"),
    grid_(MakeGridMesh()),
    axis_x_(MakeLineMesh({
        {0.0F, 0.002F, 0.0F},
        {2.0F, 0.002F, 0.0F}
})),
    axis_y_(MakeLineMesh({{0.0F, 0.002F, 0.0F}, {0.0F, 2.0F, 0.0F}})),
    axis_z_(MakeLineMesh({{0.0F, 0.002F, 0.0F}, {0.0F, 0.002F, 2.0F}})) { }

void SceneRenderer::begin_frame(
    int framebuffer_width, int framebuffer_height, const OrbitCamera &camera) {
    const float aspect = framebuffer_height > 0 ? static_cast<float>(framebuffer_width)
                                                      / static_cast<float>(framebuffer_height)
                                                : 1.0F;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(1.0F);

    shader_.use();
    set_matrix_uniform(shader_, "u_view", camera.view_matrix());
    set_matrix_uniform(
        shader_, "u_projection", MakePerspective(Radians(50.0F), aspect, 0.1F, 100.0F));
}

void SceneRenderer::draw_world_grid() {
    draw_mesh(*grid_, Mat4::Identity(), {0.32F, 0.34F, 0.38F}, false);
    glLineWidth(3.0F);
    draw_mesh(*axis_x_, Mat4::Identity(), {0.95F, 0.12F, 0.12F}, false);
    draw_mesh(*axis_y_, Mat4::Identity(), {0.12F, 0.9F, 0.2F}, false);
    draw_mesh(*axis_z_, Mat4::Identity(), {0.15F, 0.35F, 1.0F}, false);
    glLineWidth(1.0F);
}

void SceneRenderer::draw_mesh(
    const Mesh &mesh, const Mat4 &model, const Vec3 &color, bool lit, bool selected) {
    set_matrix_uniform(shader_, "u_model", model);
    shader_.set_uniform("u_color", color.x, color.y, color.z);
    shader_.set_uniform("u_lit", lit);
    shader_.set_uniform("u_selected", selected);
    mesh.draw();
}

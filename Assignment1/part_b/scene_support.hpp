#pragma once

#include "gl_base.hpp"
#include "math.hpp"

#include <memory>
#include <vector>

struct SceneVertex {
    Vec3 position;
    Vec3 normal;
};

class Mesh {
  public:
    Mesh(std::vector<SceneVertex> vertices, std::vector<unsigned int> indices, GLenum primitive);

    void draw() const;

  private:
    GLVertexArray vertex_array_;
    GLBuffer vertex_buffer_;
    GLBuffer element_buffer_;
    GLsizei index_count_ = 0;
    GLenum primitive_ = GL_TRIANGLES;
};

std::unique_ptr<Mesh> MakeBoxMesh(float half_x, float half_y, float half_z);
std::unique_ptr<Mesh> MakePyramidMesh();
std::unique_ptr<Mesh> MakeSphereMesh(float radius, int slices, int stacks);
std::unique_ptr<Mesh> MakeGridMesh();
std::unique_ptr<Mesh> MakeLineMesh(const std::vector<Vec3> &positions);

class OrbitCamera {
  public:
    Vec3 position() const;
    Mat4 view_matrix() const;
    void update_from_input();

  private:
    Vec3 target_{0.0F, 0.6F, 0.0F};
    float yaw_ = 0.65F;
    float pitch_ = 0.45F;
    float distance_ = 7.0F;
};

class SceneRenderer {
  public:
    SceneRenderer();

    void begin_frame(int framebuffer_width, int framebuffer_height, const OrbitCamera &camera);
    void draw_world_grid();
    void draw_mesh(
        const Mesh &mesh, const Mat4 &model, const Vec3 &color, bool lit = true,
        bool selected = false);

  private:
    Shader shader_;
    std::unique_ptr<Mesh> grid_;
    std::unique_ptr<Mesh> axis_x_;
    std::unique_ptr<Mesh> axis_y_;
    std::unique_ptr<Mesh> axis_z_;
};

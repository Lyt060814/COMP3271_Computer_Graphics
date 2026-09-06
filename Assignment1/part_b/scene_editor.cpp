#include "scene_editor.hpp"

#include "math.hpp"
#include "scene_support.hpp"

#include <imgui.h>

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace {

enum class MeshId { Cuboid, Sphere, Pyramid };

struct SceneObject {
    std::string name;
    MeshId mesh = MeshId::Cuboid;
    Transform transform;
    Transform initial_transform;
    Vec3 color;
};

} // namespace

class SceneEditor::Impl {
  public:
    Impl() :
        cuboid_(MakeBoxMesh(0.7F, 0.35F, 0.45F)),
        sphere_(MakeSphereMesh(0.55F, 24, 16)),
        pyramid_(MakePyramidMesh()) {
        objects_ = {
            {"Green Sphere",
             MeshId::Sphere,
             {{-0.75F, 0.55F, 0.0F}, {}, {1.0F, 1.0F, 1.0F}},
             {{-0.75F, 0.55F, 0.0F}, {}, {1.0F, 1.0F, 1.0F}},
             {0.18F, 0.75F, 0.32F}},
            {"Orange Cuboid",
             MeshId::Cuboid,
             {{0.75F, 0.35F, 0.0F}, {}, {1.0F, 1.0F, 1.0F}},
             {{0.75F, 0.35F, 0.0F}, {}, {1.0F, 1.0F, 1.0F}},
             {0.9F, 0.48F, 0.15F} },
            {"Blue Pyramid",
             MeshId::Pyramid,
             {{2.25F, 0.5F, 0.0F}, {}, {1.0F, 1.0F, 1.0F}},
             {{2.25F, 0.5F, 0.0F}, {}, {1.0F, 1.0F, 1.0F}},
             {0.18F, 0.42F, 0.9F} },
        };
    }

    void draw_ui() {
        ImGui::SetNextWindowPos({12.0F, 44.0F}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({280.0F, 300.0F}, ImGuiCond_FirstUseEver);
        ImGui::Begin("Hierarchy");
        ImGui::TextUnformatted("Object Transform Scene");
        ImGui::Separator();
        for (int index = 0; index < static_cast<int>(objects_.size()); ++index) {
            if (ImGui::Selectable(objects_[index].name.c_str(), selected_index_ == index)) {
                selected_index_ = index;
            }
        }
        ImGui::Spacing();
        if (ImGui::Button("Reset Scene")) {
            for (SceneObject &object : objects_) { object.transform = object.initial_transform; }
        }
        ImGui::End();

        const ImGuiIO &io = ImGui::GetIO();
        ImGui::SetNextWindowPos(
            {std::max(12.0F, io.DisplaySize.x - 382.0F), 44.0F}, ImGuiCond_Always);
        ImGui::SetNextWindowSize({370.0F, 340.0F}, ImGuiCond_FirstUseEver);
        ImGui::Begin("Inspector");
        SceneObject &object = objects_[selected_index_];
        ImGui::Text("Selected: %s", object.name.c_str());
        ImGui::Separator();
        ImGui::DragFloat3("Position", &object.transform.translation.x, 0.03F);
        ImGui::DragFloat3("Rotation", &object.transform.rotation_degrees.x, 0.5F);
        if (ImGui::DragFloat3(
                "Scale", &object.transform.scale.x, 0.02F, 0.1F, 5.0F, "%.2f",
                ImGuiSliderFlags_AlwaysClamp)) {
            object.transform.scale.x = std::clamp(object.transform.scale.x, 0.1F, 5.0F);
            object.transform.scale.y = std::clamp(object.transform.scale.y, 0.1F, 5.0F);
            object.transform.scale.z = std::clamp(object.transform.scale.z, 0.1F, 5.0F);
        }
        if (ImGui::Button("Reset Transform")) { object.transform = object.initial_transform; }
        ImGui::Spacing();
        ImGui::TextDisabled("Right drag: orbit camera");
        ImGui::TextDisabled("Mouse wheel: zoom camera");
        ImGui::End();
    }

    void update() { camera_.update_from_input(); }

    void render(int framebuffer_width, int framebuffer_height) {
        renderer_.begin_frame(framebuffer_width, framebuffer_height, camera_);
        renderer_.draw_world_grid();

        for (int index = 0; index < static_cast<int>(objects_.size()); ++index) {
            const SceneObject &object = objects_[index];
            renderer_.draw_mesh(
                mesh_for(object.mesh), ComposeModelMatrixEuler(object.transform), object.color,
                true, index == selected_index_);
        }

        const SceneObject &selected = objects_[selected_index_];
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(2.0F);
        glDepthFunc(GL_LEQUAL);
        renderer_.draw_mesh(
            mesh_for(selected.mesh), ComposeModelMatrixEuler(selected.transform),
            {1.0F, 0.76F, 0.12F}, false);
        glDepthFunc(GL_LESS);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

  private:
    const Mesh &mesh_for(MeshId id) const {
        switch (id) {
        case MeshId::Cuboid: return *cuboid_;
        case MeshId::Sphere: return *sphere_;
        case MeshId::Pyramid: return *pyramid_;
        }
        return *cube_;
    }

    SceneRenderer renderer_;
    OrbitCamera camera_;
    std::unique_ptr<Mesh> cube_;
    std::unique_ptr<Mesh> cuboid_;
    std::unique_ptr<Mesh> sphere_;
    std::unique_ptr<Mesh> pyramid_;
    std::vector<SceneObject> objects_;
    int selected_index_ = 0;
};

SceneEditor::SceneEditor() : impl_(std::make_unique<Impl>()) { }
SceneEditor::~SceneEditor() = default;

void SceneEditor::draw_ui() { impl_->draw_ui(); }

void SceneEditor::update() { impl_->update(); }

void SceneEditor::render(int framebuffer_width, int framebuffer_height) {
    impl_->render(framebuffer_width, framebuffer_height);
}

#include "airplane_scene.hpp"

#include "math.hpp"
#include "scene_support.hpp"

#include <imgui.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <string>

namespace {

struct AircraftEulerAngles {
    float pitch = 0.0F;
    float yaw = 0.0F;
    float roll = 0.0F;
};

struct AirplanePart {
    Vec3 center;
    Vec3 size;
};

constexpr std::array<AirplanePart, 4> airplane_parts = {
    AirplanePart{{0.0F, 0.0F, 0.0F},     {0.9F, 0.18F, 0.18F}},
    AirplanePart{{-0.05F, -0.05F, 0.0F}, {0.6F, 0.04F, 1.3F} },
    AirplanePart{{-0.4F, 0.02F, 0.0F},   {0.3F, 0.04F, 0.52F}},
    AirplanePart{{-0.4F, 0.18F, 0.0F},   {0.3F, 0.26F, 0.04F}},
};

Quat aircraft_euler_orientation(const AircraftEulerAngles &angles) {
    // The airplane points along +X: roll is local X, pitch is Z, and yaw is Y.
    const Quat roll = MakeAxisAngleQuat({1.0F, 0.0F, 0.0F}, angles.roll);
    const Quat pitch = MakeAxisAngleQuat({0.0F, 0.0F, 1.0F}, angles.pitch);
    const Quat yaw = MakeAxisAngleQuat({0.0F, 1.0F, 0.0F}, angles.yaw);
    return QuatNormalize(QuatMultiply(yaw, QuatMultiply(pitch, roll)));
}

Mat4 compose_aircraft_euler_model(const Vec3 &translation, const AircraftEulerAngles &angles) {
    // Pitch is the middle angle. At +/-90 degrees, the yaw and roll axes coincide.
    return MakeTranslationMatrix(translation) * MakeRotationYMatrix(Radians(angles.yaw))
           * MakeRotationZMatrix(Radians(angles.pitch)) * MakeRotationXMatrix(Radians(angles.roll));
}

float degrees(float radians) {
    constexpr float pi = 3.14159265358979323846F;
    return radians * 180.0F / pi;
}

AircraftEulerAngles aircraft_euler_angles(const Quat &orientation) {
    const Mat4 rotation = MakeRotationMatrix(orientation);
    const float sine_pitch = std::clamp(rotation.at(1, 0), -1.0F, 1.0F);
    const float pitch = std::asin(sine_pitch);
    const float cosine_pitch = std::cos(pitch);

    float yaw = 0.0F;
    float roll = 0.0F;
    if (std::abs(cosine_pitch) > 1.0e-5F) {
        yaw = std::atan2(-rotation.at(2, 0), rotation.at(0, 0));
        roll = std::atan2(-rotation.at(1, 2), rotation.at(1, 1));
    } else if (sine_pitch > 0.0F) {
        // At +90 degrees only yaw + roll is observable, so choose roll = 0.
        yaw = std::atan2(rotation.at(0, 2), rotation.at(2, 2));
    } else {
        // At -90 degrees only yaw - roll is observable, so choose roll = 0.
        yaw = std::atan2(rotation.at(0, 2), rotation.at(0, 1));
    }
    return {degrees(pitch), degrees(yaw), degrees(roll)};
}

} // namespace

class AirplaneScene::Impl {
  public:
    Impl() : cube_(MakeBoxMesh(0.5F, 0.5F, 0.5F)) { }

    void draw_ui() {
        ImGui::SetNextWindowPos({12.0F, 44.0F}, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize({450.0F, 560.0F}, ImGuiCond_FirstUseEver);
        ImGui::Begin("Airplane Controls");
        ImGui::TextUnformatted("Euler Angles vs. Quaternion");
        ImGui::Separator();

        if (ImGui::RadioButton("Euler compose", !use_quaternion_) && use_quaternion_) {
            const AircraftEulerAngles angles = aircraft_euler_angles(orientation_);
            pitch_ = angles.pitch;
            yaw_ = angles.yaw;
            roll_ = angles.roll;
            use_quaternion_ = false;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Quaternion update", use_quaternion_) && !use_quaternion_) {
            orientation_ = aircraft_euler_orientation(euler_angles());
            use_quaternion_ = true;
        }

        const float previous_pitch = pitch_;
        const float previous_yaw = yaw_;
        const float previous_roll = roll_;
        const char *angle_suffix = use_quaternion_ ? " input (deg)" : " (deg)";
        const bool pitch_changed = draw_angle("Pitch", angle_suffix, pitch_);
        const bool yaw_changed = draw_angle("Yaw", angle_suffix, yaw_);
        const bool roll_changed = draw_angle("Roll", angle_suffix, roll_);
        if (use_quaternion_ && (pitch_changed || yaw_changed || roll_changed)) {
            apply_quaternion_delta(
                pitch_ - previous_pitch, yaw_ - previous_yaw, roll_ - previous_roll);
        }
        if (pitch_changed) { pull_up_ = false; }

        ImGui::Spacing();
        if (ImGui::Button("Pull Up!")) { pull_up_ = true; }
        ImGui::SameLine();
        if (ImGui::Button("Reset")) { reset(); }

        ImGui::Spacing();
        const bool locked = !use_quaternion_ && std::abs(std::abs(pitch_) - 90.0F) < 3.0F;
        if (locked) {
            ImGui::TextColored(
                {1.0F, 0.35F, 0.25F, 1.0F}, "GIMBAL LOCK: pitch = %.1f deg!", pitch_);
            ImGui::TextWrapped(
                "Yaw and roll now rotate the airplane about the same axis. One degree of freedom "
                "is lost.");
        } else if (use_quaternion_) {
            ImGui::TextColored(
                {0.3F, 0.9F, 0.45F, 1.0F}, "QUATERNION: local axes remain independent");
            ImGui::TextWrapped(
                "The controls apply local-axis rotations to the stored quaternion. Yaw remains "
                "available when the airplane points vertically.");
            ImGui::TextDisabled(
                "q = [%.3f, %.3f, %.3f, %.3f]", orientation_.w, orientation_.x, orientation_.y,
                orientation_.z);
        } else {
            ImGui::TextWrapped(
                "Pull the pitch up to 90 degrees, then drag Yaw or Roll to observe gimbal lock.");
        }

        ImGui::Spacing();
        ImGui::TextDisabled("Right drag: orbit camera");
        ImGui::TextDisabled("Mouse wheel: zoom camera");
        ImGui::End();
    }

    void update() {
        camera_.update_from_input();

        const ImGuiIO &io = ImGui::GetIO();
        if (pull_up_) {
            const float previous_pitch = pitch_;
            if (pitch_ < 89.5F) {
                pitch_ = std::min(90.0F, pitch_ + 35.0F * io.DeltaTime);
            } else {
                pitch_ = 90.0F;
                pull_up_ = false;
            }
            if (use_quaternion_) { apply_quaternion_delta(pitch_ - previous_pitch, 0.0F, 0.0F); }
        }
    }

    void render(int framebuffer_width, int framebuffer_height) {
        renderer_.begin_frame(framebuffer_width, framebuffer_height, camera_);
        renderer_.draw_world_grid();
        draw_airplane(model_matrix());
    }

  private:
    static bool draw_angle(const char *name, const char *suffix, float &value) {
        const std::string label = std::string(name) + suffix;
        return ImGui::DragFloat(label.c_str(), &value, 0.5F, -180.0F, 180.0F);
    }

    AircraftEulerAngles euler_angles() const { return {pitch_, yaw_, roll_}; }

    void apply_quaternion_delta(float pitch, float yaw, float roll) {
        const Quat local_delta = aircraft_euler_orientation({pitch, yaw, roll});
        orientation_ = QuatNormalize(QuatMultiply(orientation_, local_delta));
    }

    Mat4 model_matrix() const {
        if (use_quaternion_) {
            return ComposeModelMatrixQuat(position_, orientation_, {1.0F, 1.0F, 1.0F});
        }
        return compose_aircraft_euler_model(position_, euler_angles());
    }

    void draw_airplane(const Mat4 &model) {
        for (const AirplanePart &part : airplane_parts) {
            const Mat4 part_model =
                model * MakeTranslationMatrix(part.center) * MakeScaleMatrix(part.size);
            renderer_.draw_mesh(*cube_, part_model, {0.95F, 0.85F, 0.6F});
        }
    }

    void reset() {
        pitch_ = 0.0F;
        yaw_ = 0.0F;
        roll_ = 0.0F;
        orientation_ = {};
        pull_up_ = false;
        position_ = {0.0F, 1.0F, 0.0F};
    }

    SceneRenderer renderer_;
    OrbitCamera camera_;
    std::unique_ptr<Mesh> cube_;
    bool use_quaternion_ = false;
    bool pull_up_ = false;
    float pitch_ = 0.0F;
    float yaw_ = 0.0F;
    float roll_ = 0.0F;
    Quat orientation_{};
    Vec3 position_{0.0F, 1.0F, 0.0F};
};

AirplaneScene::AirplaneScene() : impl_(std::make_unique<Impl>()) { }
AirplaneScene::~AirplaneScene() = default;

void AirplaneScene::draw_ui() { impl_->draw_ui(); }

void AirplaneScene::update() { impl_->update(); }

void AirplaneScene::render(int framebuffer_width, int framebuffer_height) {
    impl_->render(framebuffer_width, framebuffer_height);
}

#include "airplane_scene.hpp"
#include "scene.hpp"
#include "scene_editor.hpp"

#include "gl_base.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>
#include <memory>

namespace {

enum class SceneChoice { ObjectTransforms, AirplaneQuaternions };

std::unique_ptr<Scene> load_scene(SceneChoice choice) {
    switch (choice) {
    case SceneChoice::ObjectTransforms: return std::make_unique<SceneEditor>();
    case SceneChoice::AirplaneQuaternions: return std::make_unique<AirplaneScene>();
    }
    return std::make_unique<SceneEditor>();
}

SceneChoice draw_scene_menu(SceneChoice active_scene) {
    SceneChoice requested_scene = active_scene;
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Load Scene")) {
            if (ImGui::MenuItem(
                    "Object Transforms", nullptr, active_scene == SceneChoice::ObjectTransforms)) {
                requested_scene = SceneChoice::ObjectTransforms;
            }
            if (ImGui::MenuItem(
                    "Airplane & Quaternions", nullptr,
                    active_scene == SceneChoice::AirplaneQuaternions)) {
                requested_scene = SceneChoice::AirplaneQuaternions;
            }
            ImGui::EndMenu();
        }
        ImGui::Separator();
        ImGui::TextUnformatted(
            active_scene == SceneChoice::ObjectTransforms ? "Object Transforms"
                                                          : "Airplane & Quaternions");
        ImGui::EndMainMenuBar();
    }
    return requested_scene;
}

} // namespace

int main() {
    try {
        GLFWContext glfw_context;
        GLWindow window(1100, 720, "Assignment 1 - Part B: Transformations");
        glfwSwapInterval(1);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.IniFilename = nullptr;

        ImFontConfig font_config;
        font_config.SizePixels = 24.0F;
        io.Fonts->AddFontDefault(&font_config);

        ImGui::StyleColorsDark();
        ImGui::GetStyle().ScaleAllSizes(1.3F);
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");

        {
            SceneChoice active_scene = SceneChoice::ObjectTransforms;
            std::unique_ptr<Scene> scene = load_scene(active_scene);
            while (!window.should_close()) {
                window.poll_events();
                if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                }

                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                const SceneChoice requested_scene = draw_scene_menu(active_scene);
                if (requested_scene != active_scene) {
                    active_scene = requested_scene;
                    scene = load_scene(active_scene);
                }
                scene->draw_ui();
                scene->update();

                int framebuffer_width = 0;
                int framebuffer_height = 0;
                window.get_framebuffer_size(framebuffer_width, framebuffer_height);
                glViewport(0, 0, framebuffer_width, framebuffer_height);
                glClearColor(0.055F, 0.065F, 0.085F, 1.0F);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                scene->render(framebuffer_width, framebuffer_height);

                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                window.swap_buffers();
            }
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    } catch (const std::exception &error) {
        std::cerr << "Assignment 1 Part B failed: " << error.what() << '\n';
        return 1;
    }
    return 0;
}

#pragma once

#include "scene.hpp"

#include <memory>

class SceneEditor : public Scene {
  public:
    SceneEditor();
    ~SceneEditor() override;

    SceneEditor(const SceneEditor &) = delete;
    SceneEditor &operator=(const SceneEditor &) = delete;

    void draw_ui() override;
    void update() override;
    void render(int framebuffer_width, int framebuffer_height) override;

  private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

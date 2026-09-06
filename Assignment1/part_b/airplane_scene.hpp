#pragma once

#include "scene.hpp"

#include <memory>

class AirplaneScene : public Scene {
  public:
    AirplaneScene();
    ~AirplaneScene() override;

    AirplaneScene(const AirplaneScene &) = delete;
    AirplaneScene &operator=(const AirplaneScene &) = delete;

    void draw_ui() override;
    void update() override;
    void render(int framebuffer_width, int framebuffer_height) override;

  private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

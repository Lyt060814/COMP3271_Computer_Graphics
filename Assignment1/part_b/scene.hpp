#pragma once

class Scene {
  public:
    virtual ~Scene() = default;

    virtual void draw_ui() = 0;
    virtual void update() = 0;
    virtual void render(int framebuffer_width, int framebuffer_height) = 0;
};

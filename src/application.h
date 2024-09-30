#pragma once
#include "shader_util.h"
#include <memory>

struct GLFWwindow;

struct ApplicationConfig {
  unsigned int window_x, window_y;
  bool fullscreen;

  unsigned int sim_res_x, sim_res_y;
};

class Application {
public:
  Application(const ApplicationConfig&);
  ~Application();

  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;
  Application(Application&&) = default;
  Application& operator=(Application&&) = default;

  void run();

private:
  ApplicationConfig config;

  GLFWwindow* window;
  void init_context();

  unsigned int screen_quad_VAO, screen_quad_VBO, screen_quad_EBO;
  std::unique_ptr<GraphicsShaderProgram> screen_quad_shader;
  void init_screen_quad();
  void init_screen_quad_shaders();
  void render_screen_quad() const;

  unsigned int screen_texture;
  std::unique_ptr<ComputeShaderProgram> screen_compute_shader;
  void init_screen_texture();
  void init_compute_shaders();
  void dispatch_compute_shaders() const;

  void update_title();
  void process_input();

  float delta_time;
};
#pragma once
#include "shader_util.h"
#include <memory>
#include <array>

struct GLFWwindow;

struct ApplicationConfig {
  unsigned int window_x, window_y;
  bool fullscreen;

  unsigned int sim_res_x, sim_res_y;
  unsigned int agent_count;
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

  unsigned int screen_quad_vao, screen_quad_vbo, scree_quad_ebo;
  std::unique_ptr<GraphicsShaderProgram> screen_quad_shader;
  void init_screen_quad();
  void init_screen_quad_shader();
  void render_screen_quad() const;

  std::array<unsigned int, 2> screen_textures;
  void init_screen_textures();

  unsigned int agents_ssbo;
  std::unique_ptr<ComputeShaderProgram> agents_update_shader;
  void init_agents_ssbo();
  void init_agents_update_shader();
  void dispatch_agents_update_shader() const;

  std::unique_ptr<ComputeShaderProgram> screen_update_shader;
  void init_screen_update_shader();
  void dispatch_screen_update_shader() const;

  void update_title();
  void process_input();

  float delta_time;
};
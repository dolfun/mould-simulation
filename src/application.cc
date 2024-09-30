#include "application.h"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fmt/core.h>
#include <stdexcept>
#include <random>

#define WINDOW_TITLE "Mould Simulation"

Application::Application(const ApplicationConfig& _config) : config { _config } {
  init_context();
  init_screen_quad();
  init_screen_quad_shaders();
  init_screen_texture();
  init_compute_shaders();
}

Application::~Application() {
  glDeleteVertexArrays(1, &screen_quad_VAO);
  glDeleteBuffers(1, &screen_quad_VBO);
  glDeleteBuffers(1, &screen_quad_EBO);
  screen_quad_shader.reset();

  glDeleteTextures(1, &screen_texture);
  screen_compute_shader.reset();

  glfwTerminate();
}

void Application::run() {
  while (!glfwWindowShouldClose(window)) {
    static float prev_time = glfwGetTime();
    float current_time = glfwGetTime();
    delta_time = current_time - prev_time;
    prev_time = current_time;

    update_title();
    process_input();

    dispatch_compute_shaders();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    render_screen_quad();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void Application::init_context() {
  int success = glfwInit();
  if (!success) {
    throw std::runtime_error("Failed to initialize GLFW.");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  GLFWmonitor* monitor = (config.fullscreen ? glfwGetPrimaryMonitor() : nullptr);
  window = glfwCreateWindow(config.window_x, config.window_y, WINDOW_TITLE, monitor, nullptr);
  if (window == nullptr) {
    throw std::runtime_error("Failed to create GLFW window.");
  }
  glfwMakeContextCurrent(window);

  int version = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
  if (version == 0) {
    throw std::runtime_error("Failed to initialize OpenGL context.");
  }

  glViewport(0, 0, config.window_x, config.window_y);
}

void Application::init_screen_quad() {
  glGenVertexArrays(1, &screen_quad_VAO);
  glBindVertexArray(screen_quad_VAO);

  glGenBuffers(1, &screen_quad_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, screen_quad_VBO);
  constexpr float vertex_data[] = {
     1.0f,  1.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 1.0f,
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

  glGenBuffers(1, &screen_quad_EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screen_quad_EBO);
  constexpr unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
  };
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  unsigned int stride = 4 * sizeof(float);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(2 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

void Application::init_screen_quad_shaders() {
  auto vertex_shader_source = Shader::load_source_from_file("shaders/screen_quad.vert");
  auto fragment_shader_source = Shader::load_source_from_file("shaders/screen_quad.frag");
  Shader vertex_shader { vertex_shader_source, GL_VERTEX_SHADER };
  Shader fragment_shader { fragment_shader_source, GL_FRAGMENT_SHADER };
  screen_quad_shader = std::make_unique<GraphicsShaderProgram>(vertex_shader, fragment_shader);
}

void Application::render_screen_quad() const {
  screen_quad_shader->use();
  glBindVertexArray(screen_quad_VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Application::init_screen_texture() {
  glGenTextures(1, &screen_texture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, screen_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, config.sim_res_x, config.sim_res_y, 0, GL_RGBA, GL_FLOAT, nullptr);
  glBindImageTexture(0, screen_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
}

void Application::init_compute_shaders() {
  auto compute_shader_source = Shader::load_source_from_file("shaders/screen_quad.comp");
  Shader compute_shader { compute_shader_source, GL_COMPUTE_SHADER };
  screen_compute_shader = std::make_unique<ComputeShaderProgram>(compute_shader);
}

void Application::dispatch_compute_shaders() const {
  screen_compute_shader->use();
  screen_compute_shader->set_uniform("resolution", glm::ivec2(config.sim_res_x, config.sim_res_y));
  static int seed = [] {
    std::random_device dev;
    std::mt19937 rng { dev() };
    std::uniform_int_distribution<int> dist;
    return dist(rng);
  } ();
  screen_compute_shader->set_uniform("seed", seed);

  glDispatchCompute(config.sim_res_x, config.sim_res_y, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Application::update_title() {
  static std::string title;
  static unsigned int frame_count = 0;
  if (frame_count % 60 == 0) {
    title = fmt::format("{} [{:.3} FPS]", WINDOW_TITLE, 1.0f / delta_time);
    frame_count = 0;
  }
  ++frame_count;
  glfwSetWindowTitle(window, title.c_str());
}

void Application::process_input() {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}
#include "application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fmt/core.h>
#include <stdexcept>

Application::Application(const ApplicationConfig& _config) : config { _config } {
  init_context();
  init_screen_quad();
}

Application::~Application() {
  glDeleteVertexArrays(1, &screen_quad_VAO);
  glDeleteBuffers(1, &screen_quad_VBO);
  glDeleteBuffers(1, &screen_quad_EBO);
  screen_quad_shader.reset();

  glfwTerminate();
}

void Application::run() {
  while (!glfwWindowShouldClose(window)) {
    process_input();

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
  window = glfwCreateWindow(config.window_x, config.window_y, "Mould Simulation", monitor, nullptr);
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

  auto vertex_shader_source = Shader::load_source_from_file("shaders/screen_quad.vs");
  auto fragment_shader_source = Shader::load_source_from_file("shaders/screen_quad.fs");
  Shader vertex_shader { vertex_shader_source, GL_VERTEX_SHADER };
  Shader fragment_shader { fragment_shader_source, GL_FRAGMENT_SHADER };
  screen_quad_shader = std::make_unique<GraphicsShaderProgram>(vertex_shader, fragment_shader);
}

void Application::render_screen_quad() const {
  screen_quad_shader->use();
  glBindVertexArray(screen_quad_VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Application::process_input() {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}
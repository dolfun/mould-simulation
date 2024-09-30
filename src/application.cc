#include "application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <fmt/core.h>

Application::Application(const ApplicationConfig& _config) : config { _config } {
  init_context();
}

Application::~Application() {
  glfwTerminate();
}

void Application::run() {
  while (!glfwWindowShouldClose(window)) {
    process_input();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

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

void Application::process_input() {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }
}
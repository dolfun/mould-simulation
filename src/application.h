#pragma once

struct GLFWwindow;

struct ApplicationConfig {
  unsigned int window_x, window_y;
  bool fullscreen;
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
  void process_input();
};
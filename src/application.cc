#include "application.h"
#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fmt/core.h>
#include <stdexcept>
#include <vector>
#include <random>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define WINDOW_TITLE "Mould Simulation"

Application::Application(const ApplicationConfig& _config) : config { _config } {
  init_context();
  init_imgui();
  init_screen_quad();
  init_screen_quad_shader();
  init_agents_ssbo();
  init_agents_update_shader();
  init_screen_textures();
  init_screen_update_shader();
}

Application::~Application() {
  glDeleteVertexArrays(1, &screen_quad_vao);
  glDeleteBuffers(1, &screen_quad_vbo);
  glDeleteBuffers(1, &scree_quad_ebo);
  screen_quad_shader.reset();

  glDeleteBuffers(1, &agents_ssbo);
  agents_update_shader.reset();

  glDeleteTextures(2, screen_textures.data());
  screen_update_shader.reset();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
}

void Application::run() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static float prev_time = glfwGetTime();
    float current_time = glfwGetTime();
    delta_time = current_time - prev_time;
    prev_time = current_time;

    update_title();
    process_input();

    dispatch_agents_update_shader();
    dispatch_screen_update_shader();
    glCopyImageSubData(
      screen_textures[0], GL_TEXTURE_2D, 0, 0, 0, 0,
      screen_textures[1], GL_TEXTURE_2D, 0, 0, 0, 0,
      config.sim_res_x, config.sim_res_y, 1
    );

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    render_screen_quad();

    if (to_render_ui) {
      render_ui();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
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

void Application::init_imgui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->AddFontFromFileTTF("VeraMono.ttf", 17.5);

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();
}

void Application::render_ui() const {
  ImGui::ShowDemoWindow();
}

void Application::init_screen_quad() {
  glGenVertexArrays(1, &screen_quad_vao);
  glBindVertexArray(screen_quad_vao);

  glGenBuffers(1, &screen_quad_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, screen_quad_vbo);
  constexpr float vertex_data[] = {
     1.0f,  1.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 1.0f,
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

  glGenBuffers(1, &scree_quad_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scree_quad_ebo);
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

void Application::init_screen_quad_shader() {
  auto vertex_shader_source = Shader::load_source_from_file("shaders/screen_quad.vert");
  auto fragment_shader_source = Shader::load_source_from_file("shaders/screen_quad.frag");
  Shader vertex_shader { vertex_shader_source, GL_VERTEX_SHADER };
  Shader fragment_shader { fragment_shader_source, GL_FRAGMENT_SHADER };
  screen_quad_shader = std::make_unique<GraphicsShaderProgram>(vertex_shader, fragment_shader);
}

void Application::render_screen_quad() const {
  screen_quad_shader->use();
  glBindVertexArray(screen_quad_vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Application::init_screen_textures() {
  glGenTextures(2, screen_textures.data());
  for (std::size_t i = 0; i < screen_textures.size(); ++i) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, screen_textures[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, config.sim_res_x, config.sim_res_y, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(i, screen_textures[i], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
  }
}

void Application::init_agents_ssbo() {
  struct Agent {
    glm::vec2 position;
    glm::vec2 velocity;
  };

  std::vector<Agent> agents(config.agent_count);
  std::random_device dev;
  std::mt19937 rng { dev() };
  std::uniform_real_distribution<float> dist;
  for (auto& [pos, vel] : agents) {
    pos = { dist(rng), dist(rng) };

    float angle = 2.0 * glm::pi<float>() * dist(rng);
    vel = { glm::cos(angle), glm::sin(angle) };
    vel *= 0.075;
  }

  glGenBuffers(1, &agents_ssbo);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, agents_ssbo);
  std::size_t buffer_size = agents.size() * sizeof(Agent);
  glBufferData(GL_SHADER_STORAGE_BUFFER, buffer_size, agents.data(), GL_DYNAMIC_COPY);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, agents_ssbo);
}

void Application::init_agents_update_shader() {
  auto compute_shader_source = Shader::load_source_from_file("shaders/agents_update.comp");
  Shader compute_shader { compute_shader_source, GL_COMPUTE_SHADER };
  agents_update_shader = std::make_unique<ComputeShaderProgram>(compute_shader);
}

void Application::dispatch_agents_update_shader() const {
  agents_update_shader->use();
  static int seed = [] {
    std::random_device dev;
    std::mt19937 rng { dev() };
    std::uniform_int_distribution<int> dist;
    return dist(rng);
  } ();
  agents_update_shader->set_uniform("seed", seed);
  agents_update_shader->set_uniform("resolution", glm::ivec2(config.sim_res_x, config.sim_res_y));
  agents_update_shader->set_uniform("agent_count", config.agent_count);
  agents_update_shader->set_uniform("dt", delta_time);

  static unsigned int local_group_size = agents_update_shader->local_group_size().x;
  unsigned int group_count = (config.agent_count + local_group_size - 1) / local_group_size;
  glDispatchCompute(group_count, 1, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Application::init_screen_update_shader() {
  auto compute_shader_source = Shader::load_source_from_file("shaders/screen_update.comp");
  Shader compute_shader { compute_shader_source, GL_COMPUTE_SHADER };
  screen_update_shader = std::make_unique<ComputeShaderProgram>(compute_shader);
}

void Application::dispatch_screen_update_shader() const {
  screen_update_shader->use();
  screen_update_shader->set_uniform("resolution", glm::ivec2(config.sim_res_x, config.sim_res_y));
  screen_update_shader->set_uniform("dt", delta_time);

  static glm::ivec3 local_group_size = screen_update_shader->local_group_size();
  unsigned int group_count_x = (config.sim_res_x + local_group_size.x - 1) / local_group_size.x;
  unsigned int group_count_y = (config.sim_res_y + local_group_size.y - 1) / local_group_size.y;
  glDispatchCompute(group_count_x, group_count_y, 1);
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

  static bool to_render_ui_key_pressed = false;
  if (glfwGetKey(window, GLFW_KEY_F12) == GLFW_PRESS && !to_render_ui_key_pressed) {
    to_render_ui_key_pressed = true;
    to_render_ui = !to_render_ui;

  } else if (glfwGetKey(window, GLFW_KEY_F12) == GLFW_RELEASE) {
    to_render_ui_key_pressed = false;
  }
}
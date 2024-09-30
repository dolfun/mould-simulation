#include "application.h"
#include <exception>
#include <fmt/core.h>

int main() {
  try {
    ApplicationConfig config = {
      .window_x = 1024,
      .window_y = 768,
      .fullscreen = false,
      .sim_res_x = 64,
      .sim_res_y = 64,
    };

    Application app { config };
    app.run();

  } catch (const std::exception& e) {
    fmt::println("Exception occured: {}", e.what());
  }

  return 0;
}
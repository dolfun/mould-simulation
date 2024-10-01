#include "application.h"
#include <exception>
#include <fmt/core.h>

int main() {
  try {
    ApplicationConfig config = {
      .window_x = 1920,
      .window_y = 1080,
      .fullscreen = true,
      .sim_res_x = 1920,
      .sim_res_y = 1080,
      .agent_count = 500'000,
      .agent_speed = 0.1,
      .turn_speed = 10.0,
      .diffuse_rate = 75.0,
      .evaporate_rate = 1.0,
      .sensor_span = 15.0,
      .sensor_range = 0.025,
      .sensor_size = 1,
    };

    Application app { config };
    app.run();

  } catch (const std::exception& e) {
    fmt::println("Exception occured: {}", e.what());
  }

  return 0;
}
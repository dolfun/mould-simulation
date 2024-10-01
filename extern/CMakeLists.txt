set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(glad)
add_subdirectory(fmt)

set (
  imgui_sources
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_tables.cpp
  imgui/imgui_widgets.cpp
  imgui/imgui.cpp
  imgui/backends/imgui_impl_glfw.cpp
  imgui/backends/imgui_impl_opengl3.cpp
)
add_library(imgui ${imgui_sources})
target_include_directories(imgui PUBLIC imgui/ imgui/backends)
target_link_libraries(imgui PRIVATE glfw)

configure_file(VeraMono.ttf ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/VeraMono.ttf COPYONLY)
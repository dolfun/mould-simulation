add_library(shader_util shader_util.h shader_util.cc)
target_compile_features(shader_util PRIVATE cxx_std_23)
target_link_libraries(shader_util PRIVATE glad PUBLIC glm)

add_library(application application.h application.cc)
target_compile_features(application PRIVATE cxx_std_23)
target_link_libraries(application PRIVATE fmt glfw glad glm imgui PUBLIC shader_util)

add_executable(main main.cc)
target_link_libraries(main PRIVATE application fmt)

function (add_shader src)
  configure_file(${src} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${src} COPYONLY)
endfunction()

add_shader(shaders/screen_quad.vert)
add_shader(shaders/screen_quad.frag)
add_shader(shaders/screen_update.comp)
add_shader(shaders/agents_update.comp)
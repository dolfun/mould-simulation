set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_library(shader shader_util.h shader_util.cc)
target_compile_features(shader PRIVATE cxx_std_23)
target_link_libraries(shader PRIVATE glad glm)

add_library(application application.h application.cc)
target_compile_features(application PRIVATE cxx_std_23)
target_link_libraries(application PRIVATE fmt glfw glad glm shader)

add_executable(main main.cc)
target_link_libraries(main PRIVATE application fmt)

function (add_shader src)
  configure_file(${src} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${src} COPYONLY)
endfunction()

add_shader(shaders/screen_quad.vert)
add_shader(shaders/screen_quad.frag)
add_shader(shaders/screen_update.comp)
add_shader(shaders/agents_update.comp)
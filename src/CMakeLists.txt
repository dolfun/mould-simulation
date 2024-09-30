add_library(application application.h application.cc)
target_compile_features(application PRIVATE cxx_std_23)
target_link_libraries(application PRIVATE fmt glfw glad glm)

add_executable(main main.cc)
target_link_libraries(main PRIVATE application fmt)
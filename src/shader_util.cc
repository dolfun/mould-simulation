#include "shader_util.h"
#include <glad/glad.h>
#include <fstream>
#include <array>
#include <stdexcept>

std::string Shader::load_source_from_file(std::string path) {
  std::ifstream file { path };
  std::string source {
    std::istreambuf_iterator<char>(file),
    std::istreambuf_iterator<char>()
  };
  return source;
}

Shader::Shader(const std::string& source, unsigned int type) {
  id = glCreateShader(type);
  
  const char* source_c_str = source.c_str();
  glShaderSource(id, 1, &source_c_str, nullptr);
  glCompileShader(id);

  int success;
  glGetShaderiv(id, GL_COMPILE_STATUS, &success);
  if (!success) {
    constexpr std::size_t log_size = 1024;
    std::array<char, log_size> log{};
    glGetShaderInfoLog(id, log_size, nullptr, log.data());
    throw std::runtime_error(std::string { log.begin(), log.end() });
  }
}

Shader::~Shader() {
  glDeleteShader(id);
}

ShaderProgram::ShaderProgram(std::initializer_list<unsigned int> shader_ids) {
  id = glCreateProgram();
  for (const auto& shader_id : shader_ids) {
    glAttachShader(id, shader_id);
  }
  glLinkProgram(id);

  int success;
  glGetProgramiv(id, GL_LINK_STATUS, &success);
  if (!success) {
    constexpr std::size_t log_size = 1024;
    std::array<char, log_size> log{};
    glGetProgramInfoLog(id, log_size, nullptr, log.data());
    throw std::runtime_error(std::string { log.begin(), log.end() });
  }
}

ShaderProgram::~ShaderProgram() {
  glDeleteProgram(id);
}

void ShaderProgram::use() const {
  glUseProgram(id);
}
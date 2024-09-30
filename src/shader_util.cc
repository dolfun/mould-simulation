#include "shader_util.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
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

template <>
void ShaderProgram::set_uniform<int>(const std::string& name, const int& val) const {
  glUniform1i(glGetUniformLocation(id, name.c_str()), val);
}

template <>
void ShaderProgram::set_uniform<float>(const std::string& name, const float& val) const {
  glUniform1f(glGetUniformLocation(id, name.c_str()), val);
}

template <>
void ShaderProgram::set_uniform<glm::ivec2>(const std::string& name, const glm::ivec2& val) const {
  glUniform2iv(glGetUniformLocation(id, name.c_str()), 1, &val[0]);
}

template <>
void ShaderProgram::set_uniform<glm::ivec3>(const std::string& name, const glm::ivec3& val) const {
  glUniform3iv(glGetUniformLocation(id, name.c_str()), 1, &val[0]);
}

template <>
void ShaderProgram::set_uniform<glm::ivec4>(const std::string& name, const glm::ivec4& val) const {
  glUniform4iv(glGetUniformLocation(id, name.c_str()), 1, &val[0]);
}

template <>
void ShaderProgram::set_uniform<glm::vec2>(const std::string& name, const glm::vec2& val) const {
  glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &val[0]);
}

template <>
void ShaderProgram::set_uniform<glm::vec3>(const std::string& name, const glm::vec3& val) const {
  glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &val[0]);
}

template <>
void ShaderProgram::set_uniform<glm::vec4>(const std::string& name, const glm::vec4& val) const {
  glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &val[0]);
}

glm::ivec3 ComputeShaderProgram::local_group_size() const {
  glm::ivec3 local_group_size;
  glGetProgramiv(id, GL_COMPUTE_WORK_GROUP_SIZE, reinterpret_cast<GLint*>(&local_group_size));
  return local_group_size;
}
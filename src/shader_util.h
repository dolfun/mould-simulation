#pragma once
#include <string>
#include <glm/vec3.hpp>

class Shader {
public:
  Shader(const std::string&, unsigned int);
  ~Shader();

  Shader(const Shader&) = delete;
  Shader& operator=(const Shader&) = delete;
  Shader(Shader&&) = default;
  Shader& operator=(Shader&&) = default;

  static std::string load_source_from_file(std::string);

private:
  friend class GraphicsShaderProgram;
  friend class ComputeShaderProgram;
  unsigned int id;
};

class ShaderProgram {
public:
  ~ShaderProgram();

  ShaderProgram(const ShaderProgram&) = delete;
  ShaderProgram& operator=(const ShaderProgram&) = delete;
  ShaderProgram(ShaderProgram&&) = default;
  ShaderProgram& operator=(ShaderProgram&&) = default;

  void use() const;

  template <typename T>
  void set_uniform(const std::string&, const T&) const;

protected:
  ShaderProgram(std::initializer_list<unsigned int>);
  unsigned int id;
};

class GraphicsShaderProgram : public ShaderProgram {
public:
  GraphicsShaderProgram(const Shader& vertex_shader, const Shader& fragment_shader)
    : ShaderProgram { { vertex_shader.id, fragment_shader.id } } {}
};

class ComputeShaderProgram : public ShaderProgram {
public:
  ComputeShaderProgram(const Shader& compute_shader)
    : ShaderProgram { compute_shader.id } {}

  glm::ivec3 local_group_size() const;
};
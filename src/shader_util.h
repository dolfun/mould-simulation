#pragma once
#include <string>

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

protected:
  ShaderProgram(std::initializer_list<unsigned int>);
  unsigned int id;
};

class GraphicsShaderProgram : public ShaderProgram {
public:
  GraphicsShaderProgram(const Shader& vertex_shader, const Shader& fragment_shader)
    : ShaderProgram { { vertex_shader.id, fragment_shader.id } } {}
};
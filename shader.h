#ifndef SHADER_H
#define SHADER_H

#include "includes/glad/glad.h" // include glad to get all the required OpenGL headers
#include "includes/glm/detail/type_vec.hpp"
#include "includes/glm/glm.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
public:
  // the program ID
  unsigned int ID;

  // constructor reads and builds the shader
  Shader(const char *vertexPath, const char *fragmentPath);
  Shader(const char *vertexPath, const char *fragmentPath,
         const char *geometryPath);
  // use/activate the shader
  void use();
  // utility uniform functions
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setMat4(const std::string &name, glm::mat4 value) const;
  void setVec3(const std::string &name, float r, float g, float b) const;
  void setVec3(const std::string &name, glm::vec3 vec) const;
  void setVec2(const std::string &name, glm::vec2 vec) const;
};

#endif

#ifndef MESH_H
#define MESH_H

#include "includes/glm/glm.hpp"
#include "shader.h"
#include <string>
#include <vector>

using namespace std;

struct Vertex {
  glm::vec3 Position;
  glm::vec3 Normal;
  glm::vec2 TexCoords;
};

struct Texture {
  unsigned int id;
  string type;
  string path;
};

class Mesh {
public:
  vector<Vertex> vertices;
  vector<unsigned int> indices;
  vector<Texture> textures;

  Mesh(vector<Vertex> vertices, vector<unsigned int> indices,
       vector<Texture> textures);
  void Draw(Shader &shader);
  unsigned int VAO, VBO, EBO;

private:
  void setupMesh();
};

#endif

#include "mesh.h"
#include "shader.h"
#include <assimp/scene.h>
#include <string>

using namespace std;

class Model {
public:
  Model(string path) { loadModel(path); }
  void Draw(Shader &shader);

private:
  vector<Mesh> meshes;
  string directory;
  vector<Texture> textures_loaded;

  void loadModel(string path);
  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                       string typeName);
};

#include "camera.h"
#include "includes/glad/glad.h"
#include "includes/glm/detail/func_geometric.hpp"
#include "includes/glm/detail/type_mat.hpp"
#include "includes/glm/glm.hpp"
#include "includes/glm/gtc/matrix_transform.hpp"
#include "includes/glm/gtc/type_ptr.hpp"
#include "model.h"
#include "shader.h"
#include "stb_image.h"
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <cmath>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>
using namespace std;

Camera cam;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
    firstMouse = false;
    cam.Start(xpos, ypos);
  } else {
    cam.RotateCamera(xpos, ypos);
  }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  cam.Zoom(yoffset);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    cam.MoveCamera(Direction::Up, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    cam.MoveCamera(Direction::Down, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    cam.MoveCamera(Direction::Left, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    cam.MoveCamera(Direction::Right, deltaTime);
  }
}

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    cout << "Failed to create GLFW window" << endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    cout << "Failed to initialize GLAD" << endl;
    return -1;
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_FRAMEBUFFER_SRGB);

  Model backpack("assets/backpack/backpack.obj");

  glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
  glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
  glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

  Shader ourShader("shader.vs", "shader.fs");

  ourShader.use();
  ourShader.setFloat("material.shininess", 32.0f);
  ourShader.setVec3("spotLight.ambient", ambientColor);
  ourShader.setVec3("spotLight.diffuse", diffuseColor);
  ourShader.setVec3("spotLight.specular", lightColor);
  ourShader.setFloat("spotLight.innerCutOff", glm::cos(glm::radians(12.5f)));
  ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5)));

  ourShader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
  ourShader.setVec3("dirLight.ambient", ambientColor);
  ourShader.setVec3("dirLight.diffuse", diffuseColor);
  ourShader.setVec3("dirLight.specular", lightColor);

  glm::vec3 pointLightPositions[] = {
      glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(2.3f, -3.3f, -4.0f),
      glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.0f, 0.0f, -3.0f)};

  for (int i = 0; i < sizeof(pointLightPositions) / sizeof(glm::vec3); i++) {
    string name = "pointLights[" + to_string(i) + "].";
    ourShader.setVec3(name + "position", pointLightPositions[i]);
    ourShader.setFloat(name + "constant", 1.0f);
    ourShader.setFloat(name + "linear", 0.09f);
    ourShader.setFloat(name + "quadratic", 0.032f);
    ourShader.setVec3(name + "ambient", ambientColor);
    ourShader.setVec3(name + "diffuse", diffuseColor);
    ourShader.setVec3(name + "specular", lightColor);
  }

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = cam.GetView();
    glm::mat4 projection = cam.GetProjection();

    ourShader.use();
    ourShader.setMat4("view", view);
    ourShader.setMat4("projection", projection);
    ourShader.setVec3("viewPos", cam.GetPos());
    ourShader.setVec3("spotLight.position", cam.GetPos());
    ourShader.setVec3("spotLight.direction", cam.GetFront());

    glm::mat4 model = glm::mat4(1.0f);
    ourShader.setMat4("model", model);
    backpack.Draw(ourShader);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

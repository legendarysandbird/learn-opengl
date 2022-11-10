#include "includes/glad/glad.h"
#include "stb_image.h"
#include <GLFW/glfw3.h>

#include "includes/glm/glm.hpp"
#include "includes/glm/gtc/matrix_transform.hpp"
#include "includes/glm/gtc/type_ptr.hpp"

#include "camera.h"
#include "model.h"
#include "shader.h"

#include <iostream>
#include <map>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
void DrawFloor(unsigned int planeVAO, unsigned int floorTexture, Shader shader);
void DrawTwoContainers(unsigned int cubeVAO, unsigned int cubeTexture,
                       Shader shader);
void DrawTwoScaledUpContainers(unsigned int cubeVAO, unsigned int cubeTexture,
                               Shader shader);
unsigned int loadCubemap(vector<std::string> faces);

// settings
unsigned int SCR_WIDTH = 1600;
unsigned int SCR_HEIGHT = 1200;

// camera
Camera camera;
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  // --------------------
  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // configure global opengl state
  // -----------------------------
  glDepthFunc(GL_LEQUAL); // always pass the depth test (same effect as
                          // glDisable(GL_DEPTH_TEST))
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // build and compile shaders
  // -------------------------
  Shader shader("depth_testing.vs", "depth_testing.fs");
  Shader borderShader("depth_testing.vs", "shaderColor.fs");
  Shader screenShader("bufferShader.vs", "bufferShader.fs");
  Shader skyboxShader("skyboxShader.vs", "skyboxShader.fs");
  Shader reflectionShader("reflection.vs", "reflection.fs");

  unsigned int uniformBlockIndexShader =
      glGetUniformBlockIndex(shader.ID, "Matrices");
  unsigned int uniformBlockIndexReflection =
      glGetUniformBlockIndex(reflectionShader.ID, "Matrices");

  glUniformBlockBinding(shader.ID, uniformBlockIndexShader, 0);
  glUniformBlockBinding(reflectionShader.ID, uniformBlockIndexReflection, 0);

  unsigned int uboMatrices;
  glGenBuffers(1, &uboMatrices);

  glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
  glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0,
                    2 * sizeof(glm::mat4));

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float cubeVertices[] = {
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  -0.5f, -0.5f,
      0.0f,  0.0f,  -1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, 0.5f,  -0.5f,
      0.0f,  0.0f,  -1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,
      0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f,
      -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
      -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, 0.5f,
      1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, -0.5f,
      0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, 0.5f,
      0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
      0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,
      0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f};

  float planeVertices[] = {
      // positions          // texture Coords (note we set these higher than 1
      // (together with GL_REPEAT as texture wrapping mode). this will cause the
      // floor texture to repeat)
      5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  -5.0f, -0.5f, 5.0f,
      0.0f, 0.0f,  -5.0f, -0.5f, -5.0f, 0.0f,  2.0f,

      5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  -5.0f, -0.5f, -5.0f,
      0.0f, 2.0f,  5.0f,  -0.5f, -5.0f, 2.0f,  2.0f};

  float screenCubeVertices[] = {
      -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, -1.0f, 1.0f, 0.0f,
      -1.0f, 1.0f,  0.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
      -1.0f, 1.0f,  0.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f,
  };

  float skyboxVertices[] = {
      // positions
      -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
      -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

      1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

      -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

  float windowVertices[] = {
      // positions          // texture Coords
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

      -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};

  vector<glm::vec3> windows;
  windows.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
  windows.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
  windows.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
  windows.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
  windows.push_back(glm::vec3(0.5f, 0.0f, -0.06f));

  // cube VAO
  unsigned int cubeVAO, cubeVBO;
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &cubeVBO);
  glBindVertexArray(cubeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glBindVertexArray(0);

  // plane VAO
  unsigned int planeVAO, planeVBO;
  glGenVertexArrays(1, &planeVAO);
  glGenBuffers(1, &planeVBO);
  glBindVertexArray(planeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glBindVertexArray(0);

  // grass VAO
  unsigned int windowVAO, windowsVBO;
  glGenVertexArrays(1, &windowVAO);
  glGenBuffers(1, &windowsVBO);
  glBindVertexArray(windowVAO);
  glBindBuffer(GL_ARRAY_BUFFER, windowsVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(windowVertices), &windowVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glBindVertexArray(0);

  // screen VAO
  unsigned int screenVAO, screenVBO;
  glGenVertexArrays(1, &screenVAO);
  glGenBuffers(1, &screenVBO);
  glBindVertexArray(screenVAO);
  glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(screenCubeVertices), &screenCubeVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glBindVertexArray(0);

  unsigned int skyboxVAO, skyboxVBO;
  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);
  glBindVertexArray(skyboxVAO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glBindVertexArray(0);

  unsigned int framebuffer;
  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

  // generate texture
  unsigned int textureColorbuffer;
  glGenTextures(1, &textureColorbuffer);
  glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // attach it to currently bound framebuffer object
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         textureColorbuffer, 0);

  unsigned int rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH,
                        SCR_HEIGHT);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "ERROR::FRAMEBUFFER::Framebuffer is not complete!"
              << std::endl;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  vector<std::string> faces{
      "assets/skybox/right.jpg", "assets/skybox/left.jpg",
      "assets/skybox/top.jpg",   "assets/skybox/bottom.jpg",
      "assets/skybox/front.jpg", "assets/skybox/back.jpg"};

  // load textures
  // -------------
  unsigned int cubeTexture = loadTexture("assets/container.jpg");
  unsigned int floorTexture = loadTexture("assets/metal.png");
  unsigned int windowTexture =
      loadTexture("assets/blending_transparent_window.png");
  unsigned int cubemapTexture = loadCubemap(faces);

  // shader configuration
  // --------------------
  shader.use();
  shader.setInt("texture1", 0);

  screenShader.use();
  screenShader.setInt("screenTexture", 0);

  glm::mat4 projection = camera.GetProjection();

  glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                  glm::value_ptr(projection));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  // render loop
  // -----------
  while (!glfwWindowShouldClose(window)) {
    // per-frame time logic
    // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // input
    // -----
    processInput(window);

    glViewport(0, 0, 1600, 1200);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glEnable(GL_DEPTH_TEST);

    // render
    // ------
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = camera.GetView();
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                    glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    shader.use();

    DrawFloor(planeVAO, floorTexture, shader);

    reflectionShader.use();
    reflectionShader.setVec3("cameraPos", camera.GetPos());
    DrawTwoContainers(cubeVAO, cubemapTexture, reflectionShader);

    // Draw skybox
    glm::mat4 skyboxView = glm::mat4(glm::mat3(view));

    skyboxShader.use();
    skyboxShader.setMat4("view", skyboxView);
    skyboxShader.setMat4("projection", projection);

    glBindVertexArray(skyboxVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Draw windows
    glm::mat4 model;

    shader.use();

    glBindVertexArray(windowVAO);
    glBindTexture(GL_TEXTURE_2D, windowTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    std::map<float, glm::vec3> sorted;
    for (unsigned int i = 0; i < windows.size(); i++) {
      float distance = glm::length(camera.GetPos() - windows[i]);
      sorted[distance] = windows[i];
    }

    for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin();
         it != sorted.rend(); ++it) {
      model = glm::mat4(1.0f);
      model = glm::translate(model, it->second);
      shader.setMat4("model", model);
      glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // Second pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    screenShader.use();
    glBindVertexArray(screenVAO);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse
    // moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteVertexArrays(1, &planeVAO);
  glDeleteBuffers(1, &cubeVBO);
  glDeleteBuffers(1, &planeVBO);

  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.MoveCamera(Direction::Up, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.MoveCamera(Direction::Down, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.MoveCamera(Direction::Left, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.MoveCamera(Direction::Right, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  SCR_WIDTH = width;
  SCR_HEIGHT = height;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    camera.Start(xpos, ypos);
    firstMouse = false;
  }

  camera.RotateCamera(xpos, ypos);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.Zoom(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const *path) {
  unsigned int textureID;
  glGenTextures(1, &textureID);
  stbi_set_flip_vertically_on_load(true);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}

void DrawFloor(unsigned int planeVAO, unsigned int floorTexture,
               Shader shader) {
  glBindVertexArray(planeVAO);
  glBindTexture(GL_TEXTURE_2D, floorTexture);
  shader.setMat4("model", glm::mat4(1.0f));
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DrawTwoContainers(unsigned int cubeVAO, unsigned int cubeTexture,
                       Shader shader) {
  glm::mat4 model = glm::mat4(1.0f);
  glBindVertexArray(cubeVAO);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);
  model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
  shader.setMat4("model", model);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
  shader.setMat4("model", model);
  glDrawArrays(GL_TRIANGLES, 0, 36);
}

void DrawTwoScaledUpContainers(unsigned int cubeVAO, unsigned int cubeTexture,
                               Shader shader) {
  glm::mat4 model = glm::mat4(1.0f);
  glBindVertexArray(cubeVAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, cubeTexture);
  model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
  model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));
  shader.setMat4("model", model);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
  model = glm::scale(model, glm::vec3(1.1f, 1.1f, 1.1f));
  shader.setMat4("model", model);
  glDrawArrays(GL_TRIANGLES, 0, 36);
}

unsigned int loadCubemap(vector<std::string> faces) {
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
  stbi_set_flip_vertically_on_load(false);

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char *data =
        stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
                   0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else {
      std::cout << "Cubemap tex failed to load at path: " << faces[i]
                << std::endl;
    }
    stbi_image_free(data);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  return textureID;
}

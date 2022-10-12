#ifndef CAMERA_H
#define CAMERA_H

#include "includes/glad/glad.h"
#include "includes/glm/glm.hpp"
#include <GLFW/glfw3.h>

class Camera {
  glm::vec3 cameraPos;
  glm::vec3 cameraFront;
  glm::vec3 cameraUp;
  float cameraSpeed;
  float lastX;
  float lastY;
  float yaw;
  float pitch;
  float fov;

public:
  Camera();
  void Start(double xPos, double yPos);
  void RotateCamera(double xpos, double ypos);
  void Zoom(double xoffset, double yoffset);
  void MoveCamera(GLFWwindow *window, float deltaTime);
  glm::mat4 GetView();
  glm::mat4 GetProjection();
};

#endif

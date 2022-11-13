#ifndef CAMERA_H
#define CAMERA_H

#include "includes/glad/glad.h"
#include "includes/glm/glm.hpp"
#include <GLFW/glfw3.h>

enum Direction { Forward, Back, Left, Right, Up, Down };

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
  void Zoom(double yoffset);
  void MoveCamera(Direction direction, float deltaTime);
  glm::mat4 GetView();
  glm::mat4 GetProjection();
  glm::vec3 GetPos();
  glm::vec3 GetFront();
};

#endif

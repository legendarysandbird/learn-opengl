#include "camera.h"
#include "includes/glm/glm.hpp"
#include "includes/glm/gtc/matrix_transform.hpp"
#include <GLFW/glfw3.h>

const float sensitivity = 0.05f;

Camera::Camera() {
  cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
  cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
  cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
  cameraSpeed = 4.0f;
  lastX = 400.0f;
  lastY = 300.0f;
  yaw = -90.0f;
  pitch = 0.0f;
  fov = 45.0f;
}

void Camera::Start(double xPos, double yPos) {
  lastX = xPos;
  lastY = yPos;
}

void Camera::RotateCamera(double xpos, double ypos) {
  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f) {
    pitch = 89.0f;
  }

  if (pitch < -89.0f) {
    pitch = -89.0f;
  }

  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(direction);
}

void Camera::Zoom(double xoffset, double yoffset) {
  fov -= (float)yoffset;
  if (fov < 1.0f) {
    fov = 1.0f;
  }
  if (fov > 45.0f) {
    fov = 45.0f;
  }
}

void Camera::MoveCamera(Direction direction, float deltaTime) {
  glm::vec3 movementDir = glm::vec3(cameraFront.x, 0.0f, cameraFront.z);
  movementDir = glm::normalize(movementDir) * deltaTime;

  if (direction == Direction::Up) {
    cameraPos += cameraSpeed * movementDir;
  }
  if (direction == Direction::Down) {
    cameraPos -= cameraSpeed * movementDir;
  }
  if (direction == Direction::Left) {
    cameraPos -= cameraSpeed * glm::cross(movementDir, cameraUp);
  }
  if (direction == Direction::Right) {
    cameraPos += cameraSpeed * glm::cross(movementDir, cameraUp);
  }
}

glm::mat4 Camera::GetView() {
  glm::mat4 view;
  view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
  return view;
}

glm::mat4 Camera::GetProjection() {
  glm::mat4 projection = glm::mat4(1.0f);
  projection =
      glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
  return projection;
}

glm::vec3 Camera::GetPos() { return cameraPos; }

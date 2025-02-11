#pragma once

#include "ikosaederUtil.h"

extern glm::vec3 front;
extern glm::vec3 cameraPos;
extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;

const float LIGHT_ROTATION_SPEED = 10.0f;
const float LIGHT_ORBIT_RADIUS = 15.0f;

glm::vec3 mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);

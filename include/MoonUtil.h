#pragma once

#include "IkosaederUtil.h"

extern std::vector<float> vertices, normaks, uvs, moonVertices, moonNormals, moonUVs;
extern std::vector<unsigned int> indices, moonIndices;

extern unsigned int moonVBO, moonVAO, moonEBO, moonNormalVBO, moonTextureID;
extern glm::vec3 lightPos, lightColor, earthPos, cameraPos;

constexpr auto EARTH_ROTATION_SPEED = 100.0f;
constexpr auto MOON_ORBIT_SPEED = EARTH_ROTATION_SPEED / 27.3f;
constexpr auto MOON_ROTATION_SPEED = MOON_ORBIT_SPEED;

void initMoon();
void drawMoon(unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection);

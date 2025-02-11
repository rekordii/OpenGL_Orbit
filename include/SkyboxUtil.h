#pragma once

#include "IkosaederUtil.h"

extern unsigned int skyboxVAO, skyboxVBO, skyboxEBO, cubemapTexture, skyboxShaderProgram;

void initSkybox();
void drawSkybox(glm::mat4 view, glm::mat4 projection);

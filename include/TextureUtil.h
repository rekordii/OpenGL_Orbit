#pragma once

#include "IkosaederUtil.h"

void calculateUVs(std::vector<float>& vertices, std::vector<float>& uvs);
unsigned int loadTexture(const char* path);
unsigned int loadCubeMap(std::vector<std::string> faces);

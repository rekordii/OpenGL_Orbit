#include "../include/MoonUtil.h"
#include "../include/TextureUtil.h"

/**
 * Utility Function to initialize the Moon
 */
void initMoon()
{
    // Create moon vertices, normals, uvs, and indices (similar to the Earth)
    // ----------------------------------------------------------------------
    moonVertices = vertices;
    moonNormals = moonVertices;
    moonIndices = indices;

    calculateUVs(moonVertices, moonUVs);

    // Standard Buffer and Texture Generation
    // --------------------------------------
    glGenVertexArrays(1, &moonVAO);
    glGenBuffers(1, &moonVBO);
    glGenBuffers(1, &moonEBO);
    glGenBuffers(1, &moonNormalVBO);
    glGenBuffers(1, &moonTextureID);

    glBindVertexArray(moonVAO);

    glBindBuffer(GL_ARRAY_BUFFER, moonVBO);
    glBufferData(GL_ARRAY_BUFFER, moonVertices.size() * sizeof(float), moonVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, moonNormalVBO);
    glBufferData(GL_ARRAY_BUFFER, moonNormals.size() * sizeof(float), moonNormals.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, moonTextureID);
    glBufferData(GL_ARRAY_BUFFER, moonUVs.size() * sizeof(float), moonUVs.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, moonEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, moonIndices.size() * sizeof(unsigned int), moonIndices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, moonVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, moonNormalVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, moonTextureID);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    moonTextureID = loadTexture("resources/moon1.png");
}

/**
 * Draw Moon
 * @param shaderProgram Shader Program to use
 * @param view View Matrix
 * @param projection Projection Matrix
 */
void drawMoon(unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection)
{
    glUseProgram(shaderProgram);

    // Light and Camera Position
    // -------------------------
    unsigned int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    glUniform3fv(lightPosLoc, 1, &lightPos[0]);

    unsigned int lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    glUniform3fv(lightColorLoc, 1, &lightColor[0]);

    unsigned int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
    glUniform3fv(viewPosLoc, 1, &cameraPos[0]);

    unsigned int earthPosLoc = glGetUniformLocation(shaderProgram, "earthPos");
    glUniform3fv(earthPosLoc, 1, glm::value_ptr(earthPos));

    // Calculate and Set Model Matrix for the Moon
    // -------------------------------------------
    glm::mat4 model = glm::mat4(1.0f);
    float moonOrbitRadius = 3.0f;

    float moonX = moonOrbitRadius * cos((float)glfwGetTime() * -glm::radians(MOON_ORBIT_SPEED));
    float moonZ = moonOrbitRadius * sin((float)glfwGetTime() * -glm::radians(MOON_ORBIT_SPEED));
    glm::vec3 moonPos = glm::vec3(moonX, 0.0f, moonZ);
    model = glm::translate(model, moonPos);

    // Rotate around own axis and keep same face to the Earth
    // ------------------------------------------------------
    float moonRotationAngle = (float)glfwGetTime() * glm::radians(MOON_ROTATION_SPEED);
    model = glm::rotate(model, moonRotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));

    // Scale the moon relative to the Earth
    // ------------------------------------
    model = glm::scale(model, glm::vec3(0.27f, 0.27f, 0.27f));

    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, moonTextureID);
    unsigned int texLoc = glGetUniformLocation(shaderProgram, "texture1");
    glUniform1i(texLoc, 0);

    glBindVertexArray(moonVAO);
    glDrawElements(GL_TRIANGLES, moonIndices.size(), GL_UNSIGNED_INT, 0);
}

#include "include/MoonUtil.h"
#include "include/BackgroundUtil.h"
#include "include/SkyboxUtil.h"
#include "include/Init.h"

// Global Variables
// ----------------
std::vector<float> vertices, normals, uvs, moonVertices, moonNormals, moonUVs;
std::vector<unsigned int> indices, moonIndices;

// OpenGL Buffer and Texture IDs
// -----------------------------
unsigned int VBO, VAO, EBO, normalVBO, uvVBO, textureID, normalMap;
unsigned int moonVBO, moonVAO, moonEBO, moonNormalVBO, moonTextureID;
unsigned int skyboxVAO, skyboxVBO, skyboxEBO, cubemapTexture;
unsigned int skyboxShaderProgram;

// Function Declarations
// ---------------------

void updateLightPos();
void calculateMatrices(glm::mat4& view, glm::mat4& projection);
void drawEarth(unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection);

// Position Vectors
// ----------------
glm::vec3 lightPos(15.0f, 15.0f, 15.0f);   // Lightsource
glm::vec3 lightColor(1.0f, 0.95f, 0.8f); // Lightcolor and -intensity
glm::vec3 earthPos(0.0f, 0.0f, 0.0f); // Position of Earth

int main()
{
    // Prepare Libraries and initialize rendering Requirements
    // --------------------------------------------------------
	GLFWwindow* window = initGLFW_GLAD();
	unsigned int shaderProgram = initShaders_Buffers();

    initMoon();
    initSkybox();

    glEnable(GL_DEPTH_TEST);

    textureID = loadTexture("resources/earthmap.png");
	// Normal Map with weak effect || Normal Map with strong effect
    // (uncomment one of them)
	// ------------------------------------------------------------
	normalMap = loadTexture("resources/EarthNormal.png");
	// normalMap = loadTexture("resources/Earth_Normal.png");

    // Render Loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		updateLightPos();

		glm::mat4 view, projection;

		calculateMatrices(view, projection);
        drawSkybox(view, projection);

		drawEarth(shaderProgram, view, projection);
        drawMoon(shaderProgram, view, projection);

        // Swap Buffers and Poll IO Events
        // -------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // De-allocate all Resources
    // -------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &normalVBO);
    glDeleteBuffers(1, &uvVBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

/**
 * Update Light Position
 */
void updateLightPos()
{
    float time = glfwGetTime();
    lightPos.x = LIGHT_ORBIT_RADIUS * cos(glm::radians(LIGHT_ROTATION_SPEED * time));
    lightPos.z = LIGHT_ORBIT_RADIUS * sin(glm::radians(LIGHT_ROTATION_SPEED * time));
}

/**
 * Calculate View and Projection Matrices
 * @param view View Matrix
 * @param projection Projection Matrix
 */
void calculateMatrices(glm::mat4& view, glm::mat4& projection)
{
    view = glm::lookAt(
        cameraPos,                  // Camera Position
        cameraPos + cameraFront,    // Target Point
        glm::vec3(0.0f, 1.0f, 0.0f) // Direction
    );

    projection = glm::perspective(
        glm::radians(45.0f),                    // FOV
        (float)SCR_WIDTH / (float)SCR_HEIGHT,   // Aspect ratio
        0.1f, 100.0f                            // Sight
    );
}

/**
 * Draw Earth
 * @param shaderProgram Shader Program to use
 * @param view View Matrix
 * @param projection Projection Matrix
 */
void drawEarth(unsigned int shaderProgram, glm::mat4 view, glm::mat4 projection)
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

    // Calculate and Set Model Matrix
    // ------------------------------
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, (float)glfwGetTime() * glm::radians(EARTH_ROTATION_SPEED), glm::vec3(0.0f, 1.0f, 0.0f));

    unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Bind Texture
    // ------------
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    unsigned int texLoc = glGetUniformLocation(shaderProgram, "texture1");
    glUniform1i(texLoc, 0);

	glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMap);
	unsigned int normalMapLoc = glGetUniformLocation(shaderProgram, "normalMap");
	glUniform1i(normalMapLoc, 1);

    // Draw Earth
    // ----------
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

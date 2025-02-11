#include "../include/IkosaederUtil.h"

/**
 * Callback function for window resizing
 * @param window Window to resize
 * @param width New width
 * @param height New height
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

/**
 * Utility function to read a file into a string
 * @param filePath Path to the file
 * @return Content of the file as a string
 */
std::string readFile(const char* filePath)
{
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

/**
 * Utility function to load and compile shaders
 * @param vertexPath Path to the vertex shader
 * @param fragtmentPath Path to the fragment shader
 * @return Shader Program ID
 */
unsigned int loadShader(const char* vertexPath, const char* fragtmentPath)
{
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragtmentPath);

    // Convert into C-Strings
    // ----------------------
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    unsigned int vertexShader, fragmentShader;
    int success;
    char infoLog[512];

    // Vertex Shader
    // -------------
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

    }

    // Fragment Shader
    // ---------------
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

    }

    // Shader Program
    // --------------
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

/**
 * Function to Subdivide one Triangle into four Triangles and updating Normals and Indices
 * @param vertices Vertices of the Mesh
 * @param normals Normals of the Mesh
 * @param indices Indices of the Mesh
 */
void subdivide(std::vector<float>& vertices, std::vector<float>& normals, std::vector<unsigned int>& indices)
{
    std::vector<unsigned int> newIndices;
    std::vector<float> newVertices = vertices;
    std::vector<float> newNormals = normals;

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        // Get Indices of one Triangle
        // ---------------------------
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        // Get Vertices of one Triangle
        // ----------------------------
        glm::vec3 v0(vertices[i0 * 3], vertices[i0 * 3 + 1], vertices[i0 * 3 + 2]);
        glm::vec3 v1(vertices[i1 * 3], vertices[i1 * 3 + 1], vertices[i1 * 3 + 2]);
        glm::vec3 v2(vertices[i2 * 3], vertices[i2 * 3 + 1], vertices[i2 * 3 + 2]);

        // Calculate center Points of the Edges and normalize
        // --------------------------------------------------
        glm::vec3 v01 = glm::normalize((v0 + v1) * 0.5f);
        glm::vec3 v12 = glm::normalize((v1 + v2) * 0.5f);
        glm::vec3 v20 = glm::normalize((v2 + v0) * 0.5f);

        // Add new Vertices and Normals
        // ----------------------------
        unsigned int i01 = newVertices.size() / 3;
        newVertices.push_back(v01.x);
        newVertices.push_back(v01.y);
        newVertices.push_back(v01.z);
        newNormals.push_back(v01.x);
        newNormals.push_back(v01.y);
        newNormals.push_back(v01.z);

        unsigned int i12 = newVertices.size() / 3;
        newVertices.push_back(v12.x);
        newVertices.push_back(v12.y);
        newVertices.push_back(v12.z);
        newNormals.push_back(v12.x);
        newNormals.push_back(v12.y);
        newNormals.push_back(v12.z);

        unsigned int i20 = newVertices.size() / 3;
        newVertices.push_back(v20.x);
        newVertices.push_back(v20.y);
        newVertices.push_back(v20.z);
        newNormals.push_back(v20.x);
        newNormals.push_back(v20.y);
        newNormals.push_back(v20.z);

        // Add new Indices
        // ---------------
        newIndices.push_back(i0);
        newIndices.push_back(i01);
        newIndices.push_back(i20);

        newIndices.push_back(i01);
        newIndices.push_back(i1);
        newIndices.push_back(i12);

        newIndices.push_back(i12);
        newIndices.push_back(i2);
        newIndices.push_back(i20);

        newIndices.push_back(i01);
        newIndices.push_back(i12);
        newIndices.push_back(i20);
    }
    vertices = newVertices;
    normals = newNormals;
    indices = newIndices;
}

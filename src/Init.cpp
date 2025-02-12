#include "../include/Init.h"
#include "../include/BackgroundUtil.h"

extern std::vector<float> vertices, normals, uvs, moonVertices, moonNormals, moonUVs;
extern std::vector<unsigned int> indices, moonIndices;
extern unsigned int VBO, VAO, EBO, normalVBO, uvVBO;

/**
 * Initialize GLFW and GLAD
 * @return Window
 */
GLFWwindow* initGLFW_GLAD()
{
    // Initialize and Configure GLFW
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // GLFW Window Creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Orbit", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    // Set the callback functions
    // --------------------------
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        mouse_callback(window, xpos, ypos);
        });
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // GLAD: load all OpenGL Function Pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }
    return window;
}

/**
* Calculate the Tangent and Bitangent of a Triangle
* @param tangent Tangent
* @param bitangent Bitangent
* @param pos1 Vertex 1
* @param pos2 Vertex 2
* @param pos3 Vertex 3
* @param uv1 UV 1
* @param uv2 UV 2
* @param uv3 UV 3
*/
void calculateTangent_Bitangent(
    glm::vec3& tangent, glm::vec3& bitangent,
    glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3,
    glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3)
{
    // Edges of the triangle; position delta (diff)
    // --------------------------------------------
    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos3 - pos1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;

    // Scale
    // -----
    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    // Calculate Tangent and Bitangent ->
    // UV-Differences combined with Edge Differences
    // ---------------------------------------------
    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent = glm::normalize(tangent);

    bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent = glm::normalize(bitangent);
}

/**
 * Calculate Tangents and Bitangents for a Mesh.
 * Calculation based on Vertices, UVs and Indices
 * @param vertices Vertices
 * @param uvs UVs
 * @param indices Indices
 * @param tangents Tangents
 * @param bitangents Bitangents
 */
void tangentStuff(
    const std::vector<float>& vertices,
    const std::vector<float>& uvs,
    const std::vector<unsigned int>& indices,
    std::vector<float>& tangents,
    std::vector<float>& bitangents)
{
    tangents.resize(vertices.size(), 0.0f);
    bitangents.resize(vertices.size(), 0.0f);

    // Loop through all Triangles
    // --------------------------
    for (int i = 0; i < indices.size(); i += 3)
    {
        unsigned int i0 = indices[i] * 3;
        unsigned int i1 = indices[i + 1] * 3;
        unsigned int i2 = indices[i + 2] * 3;

        // Defining the Vertices and UV-Coords of the Triangle
        // ---------------------------------------------------
        glm::vec3 pos1(vertices[i0], vertices[i0 + 1], vertices[i0 + 2]);
        glm::vec3 pos2(vertices[i1], vertices[i1 + 1], vertices[i1 + 2]);
        glm::vec3 pos3(vertices[i2], vertices[i2 + 1], vertices[i2 + 2]);

        glm::vec2 uv1(uvs[i0 / 3 * 2], uvs[i0 / 3 * 2 + 1]);
        glm::vec2 uv2(uvs[i1 / 3 * 2], uvs[i1 / 3 * 2 + 1]);
        glm::vec2 uv3(uvs[i2 / 3 * 2], uvs[i2 / 3 * 2 + 1]);

        glm::vec3 tangent, bitangent;
        calculateTangent_Bitangent(tangent, bitangent, pos1, pos2, pos3, uv1, uv2, uv3);

        for (int j = 0; j < 3; j++)
        {
            tangents[i0 + j] += tangent[j];
            tangents[i1 + j] += tangent[j];
            tangents[i2 + j] += tangent[j];

            bitangents[i0 + j] += bitangent[j];
            bitangents[i1 + j] += bitangent[j];
            bitangents[i2 + j] += bitangent[j];
        }
    }
}

/**
 * Initialize Shaders and Buffers
 *
 * Sets the initial vertices, normals and indices
 * @return Shader Program
 */
unsigned int initShaders_Buffers()
{
    unsigned int shaderProgram = loadShader("resources/shader/vs.glsl", "resources/shader/fs.glsl");

    // set up vertex data and buffers
    // ------------------------------
    float X = 0.52573111212f, Z = 0.85065080835f;
    vertices =
    {
        -X, 0, Z,   X, 0, Z,    -X, 0, -Z,
         X, 0, -Z,  0, Z, X,    0, Z, -X,
         0, -Z, X,  0, -Z, -X,  Z, X, 0,
         -Z, X, 0,  Z, -X, 0,   -Z, -X, 0
    };
    normals = vertices;
    indices =
    {
        0,4,1,  0,9,4,  9,5,4,  4,5,8,  4,8,1,
        8,10,1, 8,3,10, 5,3,8,  5,2,3,  2,7,3,
        7,10,3, 7,6,10, 7,11,6, 11,0,6, 0,1,6,
        6,1,10, 9,0,11, 9,11,2, 9,2,5,  7,2,11
    };

    for (size_t i = 0; i < 8; i++)
    {
        subdivide(vertices, normals, indices);
    }
    calculateUVs(vertices, uvs);

    // Tangent Requirements for Normal Mapping
    // ---------------------------------------
    std::vector<float> tangents;
    std::vector<float> bitangents;
    tangentStuff(vertices, uvs, indices, tangents, bitangents);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &normalVBO);
    glGenBuffers(1, &uvVBO);
    glGenBuffers(1, &EBO);

	unsigned int tangentVBO, bitangentVBO;
	glGenBuffers(1, &tangentVBO);
	glGenBuffers(1, &bitangentVBO);

    // bind the Vertex Array Object first, then bind and set vertex buffers, and then configure vertex attributes
    // ----------------------------------------------------------------------------------------------------------
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, tangentVBO);
	glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(float), tangents.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, bitangentVBO);
	glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(float), bitangents.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, tangentVBO);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, bitangentVBO);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(4);

	return shaderProgram;
}

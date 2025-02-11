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

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &normalVBO);
    glGenBuffers(1, &uvVBO);
    glGenBuffers(1, &EBO);

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

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

	return shaderProgram;
}

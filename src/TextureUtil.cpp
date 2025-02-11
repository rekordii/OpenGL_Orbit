#define STB_IMAGE_IMPLEMENTATION
#include "../Libraries/include/stb/stb_image.h"

#include "../include/TextureUtil.h"

/**
 * Utility Function to calculate u and v Coordinates for Texture Mapping
 * @param vertices: List of Vertices
 * @param uvs: List of UVs
 */
void calculateUVs(std::vector<float>& vertices, std::vector<float>& uvs)
{
    uvs.clear();
    for (int i = 0; i < vertices.size(); i += 3)
    {
        glm::vec3 v(vertices[i], vertices[i + 1], vertices[i + 2]);
        float u = 0.5f + atan2(v.z, v.x) / (2.0f * M_PI);           // Angle in Radians in Range [0, 1]
        float vCoord = 0.5f - asin(v.y) / M_PI;                     // Angle in Radians in Range [0, 1]
        uvs.push_back(u);
        uvs.push_back(vCoord);
    }
}

/**
 * Utility function to load and prepare a Texture to map it
 * @param path Path to the Texture
 * @return Texture ID
 */
unsigned int loadTexture(const char* path)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set Texture Parameters
    // ----------------------
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load Picture
    // ------------
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        // Create Texture
        // --------------
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    return texture;
}

/**
 * Utility function to load and prepare a Texture to map it
 * @param faces List of Paths to the Texture
 * @return Texture ID
 */
unsigned int loadCubeMap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    int width, height, nrChannels;

    // Iterate over all Faces
    // ----------------------
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;

            // Create the Cubemap Texture
            // --------------------------
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

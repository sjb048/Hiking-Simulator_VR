// Skybox.cpp

#include "Skybox.h"
#include "stb_image.h"
#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Initialize the static instance to nullptr
Skybox* Skybox::instance = nullptr;

/**
 * @brief Retrieves the singleton instance of the Skybox.
 */
Skybox& Skybox::getInstance() {
    if (!instance) {
        instance = new Skybox();
    }
    return *instance;
}

// Constructor
Skybox::Skybox()
    : VAO(0), VBO(0), cubemapTexture(0), cubemapLoaded(false),
    skyboxShader("/Users/sumaia/Desktop/triangle/triangle/shaders/skyboxVert.glsl", "/Users/sumaia/Desktop/triangle/triangle/shaders/skyboxFrag.glsl") {}

// Destructor
Skybox::~Skybox() {
    cleanup();
}

/**
 * @brief Initializes the Skybox with the directory containing cubemap textures.
 */
bool Skybox::initialize(const std::string& directory) {
    if (cubemapLoaded) {
        std::cerr << "WARNING: Skybox already initialized. Skipping redundant initialization." << std::endl;
        return true;
    }

    std::cout << "INFO: Initializing Skybox VAO, VBO, and cubemap textures." << std::endl;

    // Validate and fix the directory path
    std::string correctedDirectory = directory;
    if (directory.back() != '/' && directory.back() != '\\') {
        correctedDirectory += '/';
    }

    // Define the cubemap faces in the correct order
    std::vector<std::string> faces = {
        correctedDirectory + "right.png", // Right
        correctedDirectory + "left.png", // Left
        correctedDirectory + "top.png", // Top
        correctedDirectory + "bottom.png", // Bottom
        correctedDirectory + "front.png", // Front
        correctedDirectory + "back.png"  // Back
    };

    // Debugging: Print paths to verify correctness
    std::cout << "INFO: Constructed cubemap faces paths:" << std::endl;
    for (const auto& face : faces) {
        std::cout << face << std::endl;
    }

    // Load the cubemap textures
    cubemapTexture = loadCubemap(faces);
    if (cubemapTexture == 0) {
        std::cerr << "ERROR: Failed to load cubemap textures. Skybox initialization aborted." << std::endl;
        return false;
    }

    // Define skybox vertices (cube)
    float skyboxVertices[] = {
        // Positions          
        // Back face
        -1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,

         // Front face
         -1.0f, -1.0f,  1.0f,
          1.0f, -1.0f,  1.0f,
          1.0f,  1.0f,  1.0f,

         -1.0f, -1.0f,  1.0f,
          1.0f,  1.0f,  1.0f,
         -1.0f,  1.0f,  1.0f,

         // Left face
         -1.0f,  1.0f,  1.0f,
         -1.0f,  1.0f, -1.0f,
         -1.0f, -1.0f, -1.0f,

         -1.0f, -1.0f, -1.0f,
         -1.0f, -1.0f,  1.0f,
         -1.0f,  1.0f,  1.0f,

         // Right face
          1.0f,  1.0f,  1.0f,
          1.0f, -1.0f, -1.0f,
          1.0f,  1.0f, -1.0f,

          1.0f, -1.0f, -1.0f,
          1.0f,  1.0f,  1.0f,
          1.0f, -1.0f,  1.0f,

          // Bottom face
          -1.0f, -1.0f, -1.0f,
           1.0f, -1.0f, -1.0f,
           1.0f, -1.0f,  1.0f,

          -1.0f, -1.0f, -1.0f,
           1.0f, -1.0f,  1.0f,
          -1.0f, -1.0f,  1.0f,

          // Top face
          -1.0f,  1.0f, -1.0f,
           1.0f,  1.0f , 1.0f,
           1.0f,  1.0f, -1.0f,

          -1.0f,  1.0f, -1.0f,
          -1.0f,  1.0f,  1.0f,
           1.0f,  1.0f,  1.0f
    };

    // Generate and bind VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);

    // Verify if shader is loaded
    if (!skyboxShader.isLoaded()) {
        std::cerr << "ERROR: Skybox shader failed to load." << std::endl;
        return false;
    }

    // Set the texture unit for the skybox shader
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    cubemapLoaded = true;
    std::cout << "INFO: Skybox initialized successfully." << std::endl;
    return true;
}

/**
 * @brief Renders the Skybox.
 */
void Skybox::render(const glm::mat4& view, const glm::mat4& projection) {
    if (!cubemapLoaded) {
        std::cerr << "ERROR: Skybox not initialized properly." << std::endl;
        return;
    }

    glDepthFunc(GL_LEQUAL); // Change depth function so depth test passes when values are equal to depth buffer's content

    skyboxShader.use();
    skyboxShader.setMat4("view", view);
    skyboxShader.setMat4("projection", projection);

    // Render skybox cube
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS); // Set depth function back to default
}

/**
 * @brief Cleans up OpenGL resources.
 */
void Skybox::cleanup() {
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (cubemapTexture) {
        glDeleteTextures(1, &cubemapTexture);
        cubemapTexture = 0;
    }
    std::cout << "INFO: Skybox resources cleaned up." << std::endl;
}

/**
 * @brief Helper function to load cubemap textures.
 */
GLuint Skybox::loadCubemap(const std::vector<std::string>& faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false); // Ensure correct orientation
    for (GLuint i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;
            else {
                std::cerr << "ERROR: Unknown number of channels in texture: " << faces[i] << std::endl;
                stbi_image_free(data);
                glDeleteTextures(1, &textureID);
                return 0;
            }

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
        }
        else {
            std::cerr << "ERROR: Cubemap texture failed to load at path: " << faces[i]
                << " with reason: " << stbi_failure_reason() << std::endl;
            stbi_image_free(data);
            glDeleteTextures(1, &textureID); // Clean up partially loaded texture
            return 0;
        }
    }

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Prevent seams
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

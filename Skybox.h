// Skybox.h

#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "shader.h"
#include <GL/glew.h>

/**
 * @class Skybox
 * @brief Manages the skybox, including loading textures and rendering.
 */
class Skybox {
public:
    /**
     * @brief Retrieves the singleton instance of the Skybox.
     * @return Reference to the Skybox instance.
     */
    static Skybox& getInstance();

    /**
     * @brief Initializes the Skybox with the directory containing cubemap textures.
     * @param directory Path to the skybox textures directory.
     * @return True if successful, false otherwise.
     */
    bool initialize(const std::string& directory);

    /**
     * @brief Renders the Skybox.
     * @param view View matrix without translation.
     * @param projection Projection matrix.
     */
    void render(const glm::mat4& view, const glm::mat4& projection);

    /**
     * @brief Cleans up OpenGL resources.
     */
    void cleanup();

private:
    // Private Constructor and Destructor for Singleton
    Skybox();
    ~Skybox();

    // Delete copy constructor and assignment operator
    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;

    // Member Variables
    GLuint VAO, VBO;              ///< Vertex Array Object and Vertex Buffer Object.
    GLuint cubemapTexture;        ///< Cubemap Texture ID.
    bool cubemapLoaded;           ///< Flag indicating if the cubemap was loaded successfully.
    Shader skyboxShader;          ///< Shader program for the Skybox.

    static Skybox* instance;      ///< Singleton instance.

    /**
     * @brief Helper function to load cubemap textures.
     * @param faces Vector containing paths to the cubemap faces.
     * @return Cubemap texture ID.
     */
    GLuint loadCubemap(const std::vector<std::string>& faces);
};

#endif // SKYBOX_H

// SeasonalEffect.cpp

#include "seasonEffect.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <GLFW/glfw3.h>  // For glfwGetTime()

/**
 * @brief Constructor initializes shaders.
 */
seasonEffect::seasonEffect()
    : currentSeason(Season::NONE),
    snowShader("/Users/sumaia/Desktop/triangle/triangle/shaders/snowVert.glsl", "/Users/sumaia/Desktop/triangle/triangle/shaders/snowFrag.glsl"),
//    rainShader("/Users/sumaia/Desktop/triangle/triangle/shaders/rainVert.glsl", "/Users/sumaia/Desktop/triangle/triangle/shaders/rainFrag.glsl"),
    quadVAO(0), quadVBO(0) {}

/**
 * @brief Initializes the seasonal effect system.
 */
void seasonEffect::initialize(Season initialSeason) {
    currentSeason = initialSeason;
    setupQuad();
}

/**
 * @brief Sets up a fullscreen quad for rendering effects.
 */
void seasonEffect::setupQuad() {
    // Setup a simple quad to render the effect
    float quadVertices[] = {
        // positions      // texture Coords
        -1.0f,  1.0f,     0.0f, 1.0f, // Top-left
        -1.0f, -1.0f,     0.0f, 0.0f, // Bottom-left
         1.0f, -1.0f,     1.0f, 0.0f, // Bottom-right

        -1.0f,  1.0f,     0.0f, 1.0f, // Top-left
         1.0f, -1.0f,     1.0f, 0.0f, // Bottom-right
         1.0f,  1.0f,     1.0f, 1.0f  // Top-right
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // Texture Coord attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

/**
 * @brief Renders the current seasonal effect.
 */
void seasonEffect::render(const glm::mat4& view, const glm::mat4& projection) {
    if (currentSeason == Season::NONE)
        return;

    Shader* shader = nullptr;
    if (currentSeason == Season::SNOW) {
        shader = &snowShader;
    }
//    else if (currentSeason == Season::RAIN) {
//        shader = &rainShader;
//    }

    if (!shader || !shader->isLoaded()) {
        std::cerr << "ERROR: Seasonal effect shader not loaded." << std::endl;
        return;
    }

    shader->use();

    // Set uniforms
    float time = static_cast<float>(glfwGetTime());
    shader->setFloat("time", time);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    // Disable depth test to render over the entire scene
    glDisable(GL_DEPTH_TEST);

    // Render the quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // Re-enable depth test
    glEnable(GL_DEPTH_TEST);
}

/**
 * @brief Cleans up OpenGL resources.
 */
void seasonEffect::cleanup() {
    if (quadVAO) {
        glDeleteVertexArrays(1, &quadVAO);
        quadVAO = 0;
    }
    if (quadVBO) {
        glDeleteBuffers(1, &quadVBO);
        quadVBO = 0;
    }
}

/**
 * @brief Sets the current season/effect.
 */
void seasonEffect::setSeason(Season newSeason) {
    currentSeason = newSeason;
}

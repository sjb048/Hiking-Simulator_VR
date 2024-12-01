// seasonEffect.h

#ifndef SEASONEFFECT_H
#define SEASONEFFECT_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "shader.h"

/**
 * @class SeasonalEffect
 * @brief Manages seasonal visual effects like snow and rain.
 */
class seasonEffect {
public:
    /**
     * @enum Season
     * @brief Enumerates possible seasons/effects.
     */
    enum class Season {
        NONE,
        SNOW,
        RAIN
    };

    /**
     * @brief Constructor.
     */
    seasonEffect();

    /**
     * @brief Initializes the seasonal effect system.
     * @param initialSeason The initial season to set.
     */
    void initialize(Season initialSeason);

    /**
     * @brief Renders the current seasonal effect.
     * @param view View matrix.
     * @param projection Projection matrix.
     */
    void render(const glm::mat4& view, const glm::mat4& projection);

    /**
     * @brief Cleans up OpenGL resources.
     */
    void cleanup();

    /**
     * @brief Sets the current season/effect.
     * @param newSeason The season to set.
     */
    void setSeason(Season newSeason);

private:
    Season currentSeason;       ///< Current season/effect.
    Shader snowShader;          ///< Shader for snow effect.
//    Shader rainShader;          ///< Shader for rain effect.

    GLuint quadVAO, quadVBO;    ///< VAO and VBO for rendering fullscreen quad.

    /**
     * @brief Sets up a fullscreen quad for rendering effects.
     */
    void setupQuad();
};

#endif // SEASONEFFECT_H

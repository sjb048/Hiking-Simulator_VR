// Hiker.h

#ifndef HIKER_H
#define HIKER_H

#include <GL/glew.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <string>
#include <vector>
#include "shader.h"
#include "terrain.h"

/**
 * @class Hiker
 * @brief Manages the hiker's path, movement, and rendering.
 */
class Hiker {
public:
    /**
     * @brief Constructor that sets the path file.
     * @param pathFile Path to the hiker's path data file.
     */
    Hiker(const std::string& pathFile);

    /**
     * @brief Loads hiker path data from a file and aligns it with the terrain.
     * @param terrain Reference to the Terrain object for height alignment.
     * @return True if successful, false otherwise.
     */
    bool loadPathData(const Terrain& terrain);

    /**
     * @brief Updates the hiker's position along the path based on deltaTime.
     * @param deltaTime Time elapsed since the last frame.
     * @param terrain Reference to the Terrain object for height alignment.
     */
    void updatePosition(float deltaTime, const Terrain& terrain);

    /**
     * @brief Renders the hiker's path as a red line.
     * @param view View matrix.
     * @param projection Projection matrix.
     * @param shader Shader program used for rendering.
     */
    void renderPath(const glm::mat4& view, const glm::mat4& projection, Shader& shader);

    /**
     * @brief Cleans up OpenGL resources.
     */
    void cleanup();

    /**
     * @brief Retrieves the hiker's current position.
     * @return Current position as a glm::vec3.
     */
    glm::vec3 getPosition() const;

    /**
     * @brief Sets the horizontal and vertical scaling factors.
     * @param hScale Horizontal scale.
     * @param vScale Vertical scale.
     */
    void setScales(float hScale, float vScale);

private:
    std::string pathFile;               ///< Path to the hiker's path data file.
    std::vector<glm::vec3> pathPoints;  ///< Vector of path points.
    GLuint pathVAO, pathVBO;            ///< OpenGL objects for rendering the path.
    glm::vec3 currentPosition;          ///< Current position of the hiker.
    float maxSlopeAngle;                ///< Maximum slope angle the hiker can traverse.
    float progress;                     ///< Progress between two path points.
    size_t currentPathIndex;            ///< Current index in the pathPoints vector.

    float horizontalScale; ///< Horizontal scaling factor to align with terrain.
    float heightScale;     ///< Vertical scaling factor to align with terrain.

    /**
     * @brief Sets up the VAO and VBO for the hiker's path.
     */
    void setupPathVAO();
};

#endif // HIKER_H

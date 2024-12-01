#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "shader.h"

/**
 * @class Terrain
 * @brief Handles loading, rendering, and interaction with the terrain.
 */
class Terrain {
public:
    /**
     * @brief Constructor.
     */
    Terrain();
//    Terrain(const std::string& heightmapPath, float yScale, float yShift);

    /**
     * @brief Loads terrain data from a heightmap image.
     * @param texturePath Path to the heightmap image.
     * @return True if successful, false otherwise.
     */
    bool loadTerrainData(const std::string& texturePath);

    /**
     * @brief Renders the terrain.
     * @param model Model matrix.
     * @param view View matrix.
     * @param projection Projection matrix.
     * @param cameraPosition Camera position for lighting calculations.
     */
    void render(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPosition);

    /**
     * @brief Cleans up OpenGL resources.
     */
    void cleanup();

    /**
     * @brief Retrieves the height at a specific (x, z) position using bilinear interpolation.
     * @param x X-coordinate.
     * @param z Z-coordinate.
     * @return Height value at the given position.
     */
    float getHeightAtPosition(float x, float z) const;

    // Getters
    int getWidth() const;
    int getHeight() const;
    Shader& getShader();
    float getHeightScale() const;
    float getHorizontalScale() const;

    // Setters
    void setHeightScale(float scale);
    void setHorizontalScale(float scale);
   

private:
    GLuint terrainVAO, terrainVBO, terrainEBO; ///< OpenGL objects.
    Shader terrainShader;                      ///< Shader used for terrain rendering.

    int width, height;                         ///< Dimensions of the terrain.
    std::vector<float> heights;                ///< Heightmap data.
    std::vector<glm::vec3> vertices;           ///< Vertex positions.
    std::vector<glm::vec3> normals;            ///< Vertex normals.
    std::vector<GLuint> indices;               ///< Indices for rendering.
//    std::vector<GLushort> indices;


    float heightScale;                         ///< Scaling factor for terrain height.
    float horizontalScale;                     ///< Scaling factor for terrain width and depth.

    /**
     * @brief Sets up the VAO, VBO, and EBO for the terrain.
     */
    void setupTerrainVAO();

    /**
     * @brief Calculates normals for the terrain vertices.
     */
    void calculateNormals();
};

#endif // TERRAIN_H


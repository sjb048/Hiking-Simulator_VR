#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "shader.h"
struct WaterPlane {
    glm::vec3 position; // Center position of the water plane
    glm::vec2 size;     // Size (width and depth) of the water plane
    float height;       // Height of the water plane
};
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
    ~Terrain();

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
   

    /**
     * @brief Retrieves the height at a specific (x, z) position using bilinear interpolation.
     * @param x X-coordinate.
     * @param z Z-coordinate.
     * @return Height value at the given position.
     */
    float getHeightAtPosition(float x, float z) const;
    void cleanup();

    // Getters
    int getWidth() const;
    int getHeight() const;
//    Shader& getShader();
    float getHeightScale() const;
    float getHorizontalScale() const;

    // Setters
    void setHeightScale(float scale);
    void setHorizontalScale(float scale);
    float getMinHeight() const;  //
    float getMaxHeight() const;  //
    
    void setShader(Shader* shader); // Accept a pointer
    Shader* getShader() const;      // Return a pointer

    bool loadTexture(const std::string& textureFile);
//    void generateTerrain(int size, float roughness);
    void diamondStep(int stepSize, float scale);
    void squareStep(int stepSize, float scale);
//    void generatePositions();
//    void addRandomPeaks(int numberOfPeaks, float peakHeight);
    
    std::vector<WaterPlane> waterPlanes; // Collection of water planes
    GLuint waterVAO, waterVBO, waterEBO; // OpenGL resources for water
//    GLuint waterVAO, waterVBO; // Add for water plane
    float waterHeight;         // Height of the water plane

    void setupWaterPlane();
    void renderWater(const glm::mat4& model, const glm::mat4& view,
                         const glm::mat4& projection, const glm::vec3& cameraPosition, Shader& waterShader);
    void addWaterPlane(const glm::vec3& position, const glm::vec2& size, float height);
    


//    void generateTerrainData(int size, float roughness, float minHeight, float maxHeight);
//    float RandomFloatRange(float min, float max);
private:
    GLuint terrainVAO, terrainVBO, terrainEBO; ///< OpenGL objects.
    Shader* terrainShader;                      ///< Shader used for terrain rendering.
    GLuint textureID;
    float RandomFloatRange(float min, float max);

    int width, height;                         ///< Dimensions of the terrain.
    float heightScale;                         ///< Scaling factor for terrain height.
    float horizontalScale;                     ///< Scaling factor for terrain width and depth.
    float minHeight; // Stores the minimum height value
    float maxHeight; // Stores the maximum height value
    ///<
//    std::vector<glm::vec3> vertexData;           ///< Vertex positions.
    std::vector<glm::vec3> vertices; // Member variable
    
    float textureRepeat;
    std::vector<float> heights;                ///< Heightmap data.
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;            ///< Vertex normals.
    std::vector<glm::vec2> texCoords;
    std::vector<GLuint> indices;               ///< Indices for rendering.
    ///<
   
    /**
     * @brief Sets up the VAO, VBO, and EBO for the terrain.
     */
    void setupTerrainVAO();
    
        
    /**
     * @brief Calculates normals for the terrain vertices.
     */
    void calculateNormals();
  
    void generateIndices();
};

#endif // TERRAIN_H


#include "terrain.h"
//#include "midpointterrain.h"
#include "stb_image.h"
//#include "terrainConfig.h"  //texture config
#include <iostream>
#include <GL/glew.h>
#include <glm/vec3.hpp> // Ensure glm::vec3 is included

#include <cstdlib> // For rand()
#include <ctime>   // For time()
#include <cmath>   // For sqrt()

// Constructor
Terrain::Terrain()
    : terrainShader("/Users/sumaia/Desktop/triangle/triangle/shaders/terrainVert.glsl", "/Users/sumaia/Desktop/triangle/triangle/shaders/terrainFrag.glsl"),
    terrainVAO(0), terrainVBO(0), terrainEBO(0),
    width(0), height(0),
    heightScale(200.0f), // Increased heightScale for pronounced terrain features
    horizontalScale(5.0f) {}

// Load terrain data from heightmap
bool Terrain::loadTerrainData(const std::string& texturePath) {
    int channels;
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &channels, STBI_grey);
    if (!data) {
        std::cerr << "ERROR: Failed to load heightmap from: " << texturePath << std::endl;
        return false;
    }

    if (width <= 0 || height <= 0) {
        std::cerr << "ERROR: Invalid heightmap dimensions (width: " << width
            << ", height: " << height << ")" << std::endl;
        stbi_image_free(data);
        return false;
    }

    std::cout << "INFO: Loaded heightmap with dimensions (" << width << " x " << height << ")" << std::endl;
    // Log a sample of height values
    int step = 4; // Adjust step size to reduce resolution
    int newWidth = width / step;
    int newHeight = height / step;
//    for (int z = 0; z < height; z += step) {
//        for (int x = 0; x < width; x += step) {
//            std::cout << "Height at (" << x << ", " << z << "): " << static_cast<int>(data[z * width + x]) << std::endl;
//        }
//    }
    vertices.clear();
    indices.clear();
    heights.resize(width * height);

    /// Generate vertex and heightmap data
    for (int z = 0; z < newHeight; ++z) {
        for (int x = 0; x < newWidth; ++x) {
            int dataIndex = (z * step) * width + (x * step);
            float y = (data[dataIndex] / 255.0f) * heightScale * 3.0f;  // Amplify height further
            heights.push_back(y);
            vertices.emplace_back(glm::vec3(x * horizontalScale * step, y, z * horizontalScale * step));
        }
    }

    std::cout << "INFO: Number of terrain vertices: " << vertices.size() << std::endl;
    // Update width and height
//    width = newWidth;
//    height = newHeight;
    // Generate indices for rendering
    for (int z = 0; z < newHeight - 1; ++z) {
        for (int x = 0; x < newWidth - 1; ++x) {
            GLuint topLeft = z * newWidth + x;
            GLuint topRight = topLeft + 1;
            GLuint bottomLeft = (z + 1) * newWidth + x;
            GLuint bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    std::cout << "INFO: Number of terrain indices: " << indices.size() << std::endl;

    stbi_image_free(data);

    // Calculate normals
    calculateNormals();

    setupTerrainVAO();
    return true;
}

// Calculate normals for terrain vertices for realistic lighting on the terrain.

void Terrain::calculateNormals() {
    normals.resize(vertices.size(), glm::vec3(0.0f));

    for (size_t i = 0; i < indices.size(); i += 3) {
        GLuint idx0 = indices[i];
        GLuint idx1 = indices[i + 1];
        GLuint idx2 = indices[i + 2];

        glm::vec3& v0 = vertices[idx0];
        glm::vec3& v1 = vertices[idx1];
        glm::vec3& v2 = vertices[idx2];

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

        normals[idx0] += faceNormal;
        normals[idx1] += faceNormal;
        normals[idx2] += faceNormal;
    }

    for (auto& normal : normals) {
        normal = glm::normalize(normal);
    }
}
void checkOpenGLError(const std::string& location) {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error at " << location << ": " << err << std::endl;
    }
}
// Setup VAO, VBO, EBO
void Terrain::setupTerrainVAO() {
    std::vector<float> vertexData;
    for (size_t i = 0; i < vertices.size(); ++i) {
        vertexData.push_back(vertices[i].x);
        vertexData.push_back(vertices[i].y);
        vertexData.push_back(vertices[i].z);

        vertexData.push_back(normals[i].x);
        vertexData.push_back(normals[i].y);
        vertexData.push_back(normals[i].z);
    }

    glGenVertexArrays(1, &terrainVAO);
    glGenBuffers(1, &terrainVBO);
    glGenBuffers(1, &terrainEBO);

    glBindVertexArray(terrainVAO);

    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
    

    checkOpenGLError("After glBufferData for VBO");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    checkOpenGLError("After glBufferData for EBO");

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    checkOpenGLError("After setting up vertex attributes");
    glBindVertexArray(0);

    std::cout << "INFO: Terrain VAO, VBO, and EBO setup complete." << std::endl;
}

// Render terrain
void Terrain::render(const glm::mat4& model, const glm::mat4& view,
    const glm::mat4& projection, const glm::vec3& cameraPosition) {
    if (!terrainShader.isLoaded()) {
        std::cerr << "ERROR: Failed to compile and link terrain shader!" << std::endl;
        std::cerr << terrainShader.getErrorLog() << std::endl;
        return;  // Stop rendering if shader is not loaded
    }


    terrainShader.use();

    terrainShader.setMat4("model", model);
    terrainShader.setMat4("view", view);
    terrainShader.setMat4("projection", projection);
    
    terrainShader.setVec3("viewPos", cameraPosition);
    terrainShader.setVec3("lightPos", glm::vec3(0.0f, 100.0f, 0.0f));

//    glBindVertexArray(terrainVAO);
//    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
//    glBindVertexArray(0);
    
    glBindVertexArray(terrainVAO);
//    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_SHORT, 0);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    checkOpenGLError("Terrain::render after glDrawElements");
    glBindVertexArray(0);
//

}

float Terrain::getHeightAtPosition(float x, float z) const {
    if (x < 0 || z < 0 || x >= (width - 1) * horizontalScale || z >= (height - 1) * horizontalScale)
        return 0.0f;

    x /= horizontalScale;
    z /= horizontalScale;

    int ix = static_cast<int>(x);
    int iz = static_cast<int>(z);

    float fx = x - ix;
    float fz = z - iz;

    float h00 = heights[iz * width + ix];
    float h01 = heights[iz * width + (ix + 1)];
    float h10 = heights[(iz + 1) * width + ix];
    float h11 = heights[(iz + 1) * width + (ix + 1)];

    float h0 = h00 * (1.0f - fx) + h01 * fx;
    float h1 = h10 * (1.0f - fx) + h11 * fx;

    return h0 * (1.0f - fz) + h1 * fz;
}



// Cleanup terrain resources
void Terrain::cleanup() {
    if (terrainVAO) glDeleteVertexArrays(1, &terrainVAO);
    if (terrainVBO) glDeleteBuffers(1, &terrainVBO);
    if (terrainEBO) glDeleteBuffers(1, &terrainEBO);

    terrainVAO = 0;
    terrainVBO = 0;
    terrainEBO = 0;

    std::cout << "INFO: Terrain resources cleaned up." << std::endl;
}

// Getters
int Terrain::getWidth() const { return width; }
int Terrain::getHeight() const { return height; }
Shader& Terrain::getShader() { return terrainShader; }
float Terrain::getHeightScale() const { return heightScale; }
float Terrain::getHorizontalScale() const { return horizontalScale; }

// Setters
void Terrain::setHeightScale(float scale) { heightScale = scale; }
void Terrain::setHorizontalScale(float scale) { horizontalScale = scale; }

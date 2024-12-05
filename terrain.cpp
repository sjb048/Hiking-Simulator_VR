#include "terrain.h"
//#include "midpointterrain.h"
#include "stb_image.h"
//#include "terrainConfig.h"  //texture config
#include <iostream>
#include <GL/glew.h>
#include <glm/vec3.hpp> // Ensure glm::vec3 is included
#include <cfloat>   // For FLT_MAX

#include <cstdlib> // For rand()
#include <ctime>   // For time()
#include <cmath>   // For sqrt()
#include <glm/gtc/matrix_transform.hpp>

// Constructor
Terrain::Terrain()
    :terrainShader(nullptr),
    terrainVAO(0), terrainVBO(0), terrainEBO(0),
    width(0), height(0),
    minHeight(0.0f), maxHeight(0.0f),
    textureID(0) ,
    textureRepeat(10.0f),
    heightScale(800.0f),   // Decrease heightScale for better proportion
    horizontalScale(1.0f){
//        setupWaterPlane();
    }

Terrain::~Terrain() {
    cleanup();
}
void Terrain::setShader(Shader* shader) {
    terrainShader = shader;
}

Shader* Terrain::getShader() const {
    return terrainShader;
}


float Terrain::RandomFloatRange(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}
void Terrain::diamondStep(int stepSize, float scale) {
    int halfStep = stepSize / 2;

    for (int z = halfStep; z < height; z += stepSize) {
        for (int x = halfStep; x < width; x += stepSize) {
            float avg = (
                heights[(z - halfStep) * width + (x - halfStep)] +
                heights[(z - halfStep) * width + (x + halfStep) % width] +
                heights[(z + halfStep) % height * width + (x - halfStep)] +
                heights[(z + halfStep) % height * width + (x + halfStep) % width]
            ) / 4.0f;
            float roughness = 2.0f;
            scale *= pow(1.5f, -roughness); // Retain more variation as step size decreases

//            heights[z * width + x] = avg + RandomFloatRange(-scale, scale);
            heights[z * width + x] = avg + RandomFloatRange(-scale * 1.5f, scale * 1.5f);
        }
    }
}

void Terrain::squareStep(int stepSize, float scale) {
    int halfStep = stepSize / 2;

    for (int z = 0; z < height; z += halfStep) {
        int shift = (z / halfStep) % 2 == 0 ? halfStep : 0;
        for (int x = shift; x < width; x += stepSize) {
            int count = 0;
            float sum = 0.0f;

            if (x - halfStep >= 0) {
                sum += heights[z * width + x - halfStep];
                count++;
            }
            if (x + halfStep < width) {
                sum += heights[z * width + x + halfStep];
                count++;
            }
            if (z - halfStep >= 0) {
                sum += heights[(z - halfStep) * width + x];
                count++;
            }
            if (z + halfStep < height) {
                sum += heights[(z + halfStep) * width + x];
                count++;
            }
            float roughness = 2.0f;
            scale *= pow(1.5f, -roughness); // Retain more variation as step size decreases

            heights[z * width + x] = (sum / count) + RandomFloatRange(-scale * 1.5f, scale * 1.5f);

//            heights[z * width + x] = (sum / count) + RandomFloatRange(-scale, scale);
        }
    }
}


bool Terrain::loadTerrainData(const std::string& heightmapFile) {
    // Load heightmap image
    int nrComponents;
    unsigned char* data = stbi_load(heightmapFile.c_str(), &width, &height, &nrComponents, 1);
    if (!data) {
        std::cerr << "ERROR::TERRAIN::FAILED_TO_LOAD_HEIGHTMAP: " << heightmapFile << std::endl;
        return false;
    }
    if (width <= 0 || height <= 0) {
           std::cerr << "ERROR: Invalid heightmap dimensions (width: " << width
               << ", height: " << height << ")" << std::endl;
           stbi_image_free(data);
           return false;
    }
   
   

    // Initialize minHeight to maximum possible float value
    minHeight = FLT_MAX;
    heights.resize(width * height);
    maxHeight = -FLT_MAX; // Initialize to lowest possible float value

    heightScale = 105.0f; // Adjust for taller mountains
    horizontalScale = 1.0f; // Prevent excessive flattening

    // Process heightmap data

    for (int i = 0; i < width * height; ++i) {
        float heightValue = data[i] / 255.0f * heightScale;
        heights[i] = heightValue;
        if (heightValue > maxHeight) {
            maxHeight = heightValue;
        }
        if (heightValue < minHeight) {
            minHeight = heightValue;
        }
    }

    stbi_image_free(data);
    // Add random noise to heights to introduce roughness
        float roughness = 1.2f; // Adjust this value as needed
        for (int i = 0; i < width * height; ++i) {
            float noise = RandomFloatRange(-roughness, roughness);
            heights[i] += noise;
        }

        // Recalculate minHeight and maxHeight after adding noise
        minHeight = FLT_MAX;
        maxHeight = -FLT_MAX;
        for (const auto& heightValue : heights) {
            if (heightValue > maxHeight) maxHeight = heightValue;
            if (heightValue < minHeight) minHeight = heightValue;
        }

//    if (rand() % 2 == 0) { // 50% chance to apply steps
        int stepSize = (width - 1) / 2;
        float scale = (maxHeight - minHeight) * 0.6f; // Use a fraction of the height range


        while (stepSize > 1) {
            diamondStep(stepSize, scale);
            squareStep(stepSize, scale);

            stepSize /= 2;
//            scale *= 0.2f; // Reduce scale to add finer details
            scale *= pow(1.5f, -roughness);
        }
//    }
    // Generate positions and normals
    positions.resize(width * height);
    normals.resize(width * height);
    texCoords.resize(width * height);

    float halfWidth = (width - 1) * horizontalScale * 0.5f;
    float halfDepth = (height - 1) * horizontalScale * 0.5f;

    for (int z = 0; z < height; ++z) {
        for (int x = 0; x < width; ++x) {
            float heightValue = heights[z * width + x];
            positions[z * width + x] = glm::vec3(
                x * horizontalScale - halfWidth,
                heights[z * width + x], // heights already include heightScale
                z * horizontalScale - halfDepth
            );

            // Texture coordinates
            texCoords[z * width + x] = glm::vec2(
                static_cast<float>(x) / (width - 1) * textureRepeat,
                static_cast<float>(z) / (height - 1) * textureRepeat
            );
        }
    }

    calculateNormals();
    setupTerrainVAO();

    return true;
}
void Terrain::setupTerrainVAO() {
    // Generate indices
    indices.clear();
    for (int z = 0; z < height - 1; ++z) {
        for (int x = 0; x < width - 1; ++x) {
            int i0 = z * width + x;
            int i1 = z * width + x + 1;
            int i2 = (z + 1) * width + x;
            int i3 = (z + 1) * width + x + 1;

            // First triangle
            indices.push_back(i0);
            indices.push_back(i1);
            indices.push_back(i2);

            // Second triangle
            indices.push_back(i1);
            indices.push_back(i3);
            indices.push_back(i2);
        }
    }

    // Create vertex data
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };
    std::vector<Vertex> vertexData(positions.size());
    for (size_t i = 0; i < positions.size(); ++i) {
        vertexData[i].Position = positions[i];
        vertexData[i].Normal = normals[i];
        vertexData[i].TexCoords = texCoords[i];
    }

    // OpenGL buffer setup (VAO, VBO, EBO)
    glGenVertexArrays(1, &terrainVAO);
    glGenBuffers(1, &terrainVBO);
    glGenBuffers(1, &terrainEBO);

    glBindVertexArray(terrainVAO);

    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vertex), vertexData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Vertex attribute pointers
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // Texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);

    std::cout << "INFO: Terrain VAO, VBO, and EBO setup complete." << std::endl;
}


// Calculate normals for terrain vertices for realistic lighting on the terrain.
void Terrain::calculateNormals() {
    // Initialize normals to zero
    for (auto& normal : normals) {
        normal = glm::vec3(0.0f);
    }

    // Calculate normals
    for (int z = 0; z < height - 1; ++z) {
        for (int x = 0; x < width - 1; ++x) {
            int i0 = z * width + x;
            int i1 = z * width + x + 1;
            int i2 = (z + 1) * width + x;
            int i3 = (z + 1) * width + x + 1;

            glm::vec3 v0 = positions[i0];
            glm::vec3 v1 = positions[i1];
            glm::vec3 v2 = positions[i2];
            glm::vec3 v3 = positions[i3];

            glm::vec3 normal1 = glm::normalize(glm::cross(v1 - v0, v2 - v0));
            glm::vec3 normal2 = glm::normalize(glm::cross(v3 - v1, v2 - v1));

            normals[i0] += normal1;
            normals[i1] += normal1 + normal2;
            normals[i2] += normal1 + normal2;
            normals[i3] += normal2;
        }
    }

    // Normalize the normals
    for (auto& normal : normals) {
        normal = glm::normalize(normal);
    }
}

// Setup VAO, VBO, EBO


// Render terrain
void Terrain::render(const glm::mat4& model, const glm::mat4& view,
    const glm::mat4& projection, const glm::vec3& cameraPosition) {
    if (!terrainShader->isLoaded()) {
        std::cerr << "ERROR: Failed to compile and link terrain shader!" << std::endl;
        std::cerr << terrainShader->getErrorLog() << std::endl;
        return;  // Stop rendering if shader is not loaded
    }
    terrainShader->use();

    terrainShader->setMat4("model", model);
    terrainShader->setMat4("view", view);
    terrainShader->setMat4("projection", projection);
    
    terrainShader->setVec3("viewPos", cameraPosition);
//    terrainShader->setVec3("lightPos", glm::vec3(0.0f, 100.0f, 0.0f));
    terrainShader->setVec3("lightPos", glm::vec3(100.0f, 100.0f, 500.0f)); // Sunlight position
//    terrainShader->setVec3("lightColor", glm::vec3(1.0f, 0.5f, 0.7f)); // Warm sunlight color
    terrainShader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 0.9f)); // Adjust to neutral sunlight

    terrainShader->setFloat("shininess", 24.0f); // Adjust shininess



    // Set the minHeight and maxHeight uniforms
    terrainShader->setFloat("minHeight", minHeight);
    terrainShader->setFloat("maxHeight", maxHeight);
    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    terrainShader->setInt("terrainTexture", 0);

        // Draw the terrain
    glBindVertexArray(terrainVAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
//

}
float Terrain::getHeightAtPosition(float x, float z) const {
    float halfWidth = (width - 1) * horizontalScale * 0.5f;
    float halfDepth = (height - 1) * horizontalScale * 0.5f;

    // Precompute for efficiency
    float invHS = 1.0f / horizontalScale;
    float localX = (x + halfWidth) * invHS;
    float localZ = (z + halfDepth) * invHS;

    // Clamp to grid boundaries without glm::clamp
    if (localX < 0.0f) localX = 0.0f;
    else if (localX > (width - 1)) localX = (float)(width - 1);

    if (localZ < 0.0f) localZ = 0.0f;
    else if (localZ > (height - 1)) localZ = (float)(height - 1);

    // Get integer indices
    int x0 = (int)localX;
    int z0 = (int)localZ;
    int x1 = (x0 < width - 1) ? x0 + 1 : x0;
    int z1 = (z0 < height - 1) ? z0 + 1 : z0;

    // Fractional parts
    float fx = localX - x0;
    float fz = localZ - z0;

    // If you have a heights array:
    // float h00 = heights[z0 * width + x0];
    // float h10 = heights[z0 * width + x1];
    // float h01 = heights[z1 * width + x0];
    // float h11 = heights[z1 * width + x1];

    // Otherwise, use positions:
    float h00 = positions[z0 * width + x0].y;
    float h10 = positions[z0 * width + x1].y;
    float h01 = positions[z1 * width + x0].y;
    float h11 = positions[z1 * width + x1].y;

    // Direct linear interpolation to avoid glm::mix overhead
    float h0 = h00 + fx * (h10 - h00);
    float h1 = h01 + fx * (h11 - h01);
    return h0 + fz * (h1 - h0);
}


bool Terrain::loadTexture(const std::string& textureFile) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Repeat wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Repeat wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Minification filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Magnification filter

    // Load image using stb_image
    int texWidth, texHeight, nrChannels;
    unsigned char* data = stbi_load(textureFile.c_str(), &texWidth, &texHeight, &nrChannels, 0);
    if (data) {
        //Adjust brightness and contrast to simulate daylight
        float brightnessFactor = 1.2f; // Brighten the texture
        float contrastFactor = 1.1f;  // Increase contrast

        for (int i = 0; i < texWidth * texHeight * nrChannels; ++i) {
            float pixel = data[i] / 255.0f; // Normalize pixel value
            pixel = (pixel - 0.5f) * contrastFactor + 0.5f; // Apply contrast
            pixel *= brightnessFactor; // Apply brightness
            data[i] = static_cast<unsigned char>(glm::clamp(pixel, 0.0f, 1.0f) * 255.0f);
        }

        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        else {
            std::cerr << "ERROR::TERRAIN::UNSUPPORTED_TEXTURE_FORMAT: " << textureFile << std::endl;
            stbi_image_free(data);
            return false;
        }

        // Adjust the texture format to ensure proper daylight color rendering
        glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);


        stbi_image_free(data);
        std::cout << "INFO: Terrain texture loaded successfully with daylight adjustments." << std::endl;
        return true;
    } else {
        std::cerr << "ERROR::TERRAIN::FAILED_TO_LOAD_TEXTURE: " << textureFile << std::endl;
        return false;
    }
}

// Cleanup terrain resources
void Terrain::cleanup() {
    if (terrainVAO) glDeleteVertexArrays(1, &terrainVAO);
    if (terrainVBO) glDeleteBuffers(1, &terrainVBO);
    if (terrainEBO) glDeleteBuffers(1, &terrainEBO);

    terrainVAO = 0;
    terrainVBO = 0;
    terrainEBO = 0;
    if (textureID != 0) {
            glDeleteTextures(1, &textureID);
            textureID = 0;
        }
    positions.clear();
    normals.clear();
    indices.clear();
    heights.clear();
    texCoords.clear();

    std::cout << "INFO: Terrain resources cleaned up." << std::endl;
}

void checkOpenGLError(const std::string& location) {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error at " << location << ": " << err << std::endl;
    }
}

// Getters
int Terrain::getWidth() const { return width; }
int Terrain::getHeight() const { return height; }
//Shader& Terrain::getShader() { return terrainShader; }
float Terrain::getHeightScale() const { return heightScale; }
float Terrain::getHorizontalScale() const { return horizontalScale; }

// Getter for minHeight
float Terrain::getMinHeight() const {
    return minHeight;
}

// Getter for maxHeight
float Terrain::getMaxHeight() const {
    return maxHeight;
}
// Setters
void Terrain::setHeightScale(float scale) { heightScale = scale; }
void Terrain::setHorizontalScale(float scale) { horizontalScale = scale; }

//water rendering
void Terrain::renderWater(const glm::mat4& model, const glm::mat4& view,
                          const glm::mat4& projection, const glm::vec3& cameraPosition, Shader& waterShader) {
    if (!waterShader.isLoaded()) {
        std::cerr << "ERROR: Failed to compile and link water shader!" << std::endl;
        std::cerr << waterShader.getErrorLog() << std::endl;
        return;  // Stop rendering if shader is not loaded
    }

    waterShader.use();

    // Set uniform values for the water shader
    waterShader.setMat4("model", model);
    waterShader.setMat4("view", view);
    waterShader.setMat4("projection", projection);
    waterShader.setVec3("cameraPosition", cameraPosition);
    waterShader.setFloat("time", static_cast<float>(glfwGetTime())); // For animations
//    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(waterVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
}


void Terrain::addWaterPlane(const glm::vec3& position, const glm::vec2& size, float height) {
    WaterPlane plane;
    plane.position = position;
    plane.size = size;
    plane.height = height;
    waterPlanes.push_back(plane);
}


void Terrain::setupWaterPlane() {
    waterHeight = 10.0f; // Adjust this height as needed

    // Define a simple quad for the water plane
    float halfWidth = (width - 1) * horizontalScale * 0.3f;
    float halfDepth = (height - 1) * horizontalScale * 0.29f;

    // Define a smaller water plane
//        float halfWidth = 40.0f; // Fixed width of the water plane (e.g., 100x100 size)
//        float halfDepth = 40.0f; // Fixed depth of the water plane

    float waterVertices[] = {
        -halfWidth, waterHeight, -halfDepth,  // Bottom-left
         halfWidth, waterHeight, -halfDepth,  // Bottom-right
         halfWidth, waterHeight,  halfDepth,  // Top-right
        -halfWidth, waterHeight,  halfDepth   // Top-left
    };

    unsigned int waterIndices[] = {
        0, 1, 2, // First triangle
        0, 2, 3  // Second triangle
    };

    // Create VAO and VBO for the water plane
    glGenVertexArrays(1, &waterVAO);
    glGenBuffers(1, &waterVBO);

    GLuint waterEBO;
    glGenBuffers(1, &waterEBO);

    glBindVertexArray(waterVAO);

    glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(waterVertices), waterVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(waterIndices), waterIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

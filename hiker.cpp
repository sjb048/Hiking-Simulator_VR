// Hiker.cpp

#include "Hiker.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // For debugging
#include <iostream>
#include <fstream>
#include <algorithm>

/// Constructor
Hiker::Hiker(const std::string& pathFile)
    : pathFile(pathFile), pathVAO(0), pathVBO(0), currentPosition(glm::vec3(0.0f)),
    maxSlopeAngle(30.0f), progress(0.0f), currentPathIndex(0),
    horizontalScale(1.0f), heightScale(1.0f) {}

// Set horizontal and vertical scales
void Hiker::setScales(float hScale, float vScale) {
    horizontalScale = hScale;
    heightScale = vScale;
}

// Load hiker path data from file and align with terrain
bool Hiker::loadPathData(const Terrain& terrain) {
    std::ifstream file(pathFile);
    if (!file.is_open()) {
        std::cerr << "ERROR: Failed to open path file: " << pathFile << std::endl;
        return false;
    }

    float x, y, z;
    float hScale = terrain.getHorizontalScale();
    float vScale = terrain.getHeightScale();

    while (file >> x >> y >> z) {
        x *= hScale;
        z *= hScale;
        y = terrain.getHeightAtPosition(x, z) + 0.1f; // Offset to ensure visibility
        pathPoints.emplace_back(glm::vec3(x, y, z));
    }
    file.close();

    if (pathPoints.empty()) {
        std::cerr << "ERROR: No path points loaded from file: " << pathFile << std::endl;
        return false;
    }

    currentPosition = pathPoints[0];

    // Output number of path points
//    std::cout << "INFO: Number of hiker path points loaded: " << pathPoints.size() << std::endl;
//    std::cout << "INFO: First path point: " << glm::to_string(pathPoints.front()) << std::endl;
//    std::cout << "INFO: Last path point: " << glm::to_string(pathPoints.back()) << std::endl;

    setupPathVAO();
    return true;
}

// Setup VAO and VBO for the hiker's path
void Hiker::setupPathVAO() {
    glGenVertexArrays(1, &pathVAO);
    glGenBuffers(1, &pathVBO);

    glBindVertexArray(pathVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pathVBO);
    glBufferData(GL_ARRAY_BUFFER, pathPoints.size() * sizeof(glm::vec3), pathPoints.data(), GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glBindVertexArray(0);

    std::cout << "INFO: Hiker path VAO and VBO set up successfully." << std::endl;
}

// Update hiker's position along the path
void Hiker::updatePosition(float deltaTime, const Terrain& terrain) {
    if (currentPathIndex >= pathPoints.size() - 1)
        return;

    glm::vec3 start = pathPoints[currentPathIndex];
    glm::vec3 end = pathPoints[currentPathIndex + 1];
    float speed = 10.0f; // Adjusted speed for visible movement

    progress += speed * deltaTime / glm::distance(start, end);
    if (progress > 1.0f) {
        progress = 0.0f;
        currentPathIndex++;
        if (currentPathIndex >= pathPoints.size() - 1) {
            currentPathIndex = 0; // Loop back to the start
        }
    }

    glm::vec3 nextPosition = glm::mix(start, end, progress);

    float terrainHeight = terrain.getHeightAtPosition(nextPosition.x, nextPosition.z);
    nextPosition.y = terrainHeight; // Ensure hiker is on the terrain

    currentPosition = nextPosition;
}

// Render the hiker's path as a red line
void Hiker::renderPath(const glm::mat4& view, const glm::mat4& projection, Shader& shader) {
    glDisable(GL_DEPTH_TEST); // Disable depth testing to ensure the path is visible
    if (!shader.isLoaded()) {
        std::cerr << "ERROR: Hiker shader not loaded!" << std::endl;
        std::cerr << shader.getErrorLog() << std::endl;
        return;
    }

    shader.use();
    shader.setMat4("model", glm::mat4(1.0f));
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    glBindVertexArray(pathVAO);
    glLineWidth(2.0f); // Increased line width for better visibility
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(pathPoints.size()));
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST); // Re-enable depth testing

    // Optional: Uncomment the line below for debugging purposes
    // std::cout << "INFO: Hiker path rendered." << std::endl;
}

// Cleanup hiker resources
void Hiker::cleanup() {
    if (pathVAO) {
        glDeleteVertexArrays(1, &pathVAO);
    }
    if (pathVBO) {
        glDeleteBuffers(1, &pathVBO);
    }
    std::cout << "INFO: Hiker resources cleaned up successfully." << std::endl;
}

// Get hiker's current position
glm::vec3 Hiker::getPosition() const {
    return currentPosition;
}

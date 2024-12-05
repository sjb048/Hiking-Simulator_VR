// Hiker.cpp

#include "hiker.h"
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> // For debugging
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>   // For isdigit

/// Constructor
Hiker::Hiker(const std::string& pathFile)
    :  terrainRef(nullptr), pathFile(pathFile), pathVAO(0), pathVBO(0), currentPosition(glm::vec3(0.0f)),
    maxSlopeAngle(30.0f), progress(0.0f), currentPathIndex(0),
    horizontalScale(1.0f), heightScale(1.0f) {}


//Load hiker path data from file and align with terrain

bool Hiker::loadPathData(const Terrain& terrain) {
    std::ifstream file(pathFile);
    if (!file.is_open()) {
        std::cerr << "ERROR::HIKER::FAILED_TO_OPEN_PATH_FILE: " << pathFile << std::endl;
        return false;
    }

    float x, y, z;
    pathPoints.clear();

    // Load path points from file
    while (file >> x >> y >> z) {
        pathPoints.emplace_back(x, y, z);
    }
    file.close();

    if (pathPoints.empty()) {
        std::cerr << "ERROR::HIKER::NO_PATH_POINTS_LOADED" << std::endl;
        return false;
    }

    // Validate and adjust the path points against the terrain
    validatePath(terrain);

    // Calculate segment distances and total path length
    calculateSegmentDistances();

    position = pathPoints[0];
    currentDistance = 0.0f;
    currentSegmentIndex = 0;

    // Setup OpenGL buffers for rendering the path
    setupPathVAO();

    return true;
}

void Hiker::validatePath(const Terrain& terrain) {
    if (pathPoints.empty()) return;

    std::vector<glm::vec3> validatedPath;
    validatedPath.reserve(pathPoints.size());

    float terrainWidth = terrain.getWidth() * terrain.getHorizontalScale();
    float terrainDepth = terrain.getHeight() * terrain.getHorizontalScale();
    float minX = -terrainWidth * 0.5f;
    float maxX = terrainWidth * 0.5f;
    float minZ = -terrainDepth * 0.5f;
    float maxZ = terrainDepth * 0.5f;

    // Scale and adjust points
    for (auto& point : pathPoints) {
        point.x = glm::clamp(point.x * horizontalScale, minX, maxX);
        point.z = glm::clamp(point.z * horizontalScale, minZ, maxZ);
        float terrainHeight = terrain.getHeightAtPosition(point.x, point.z);
        point.y = terrainHeight + 0.5f; // Small offset above terrain
        validatedPath.push_back(point);
    }

    pathPoints = std::move(validatedPath);
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

void Hiker::calculateSegmentDistances() {
    segmentDistances.clear();
    segmentDistances.push_back(0.0f);
    totalPathLength = 0.0f;

    for (size_t i = 1; i < pathPoints.size(); ++i) {
        float segmentLength = glm::distance(pathPoints[i - 1], pathPoints[i]);
        totalPathLength += segmentLength;
        segmentDistances.push_back(totalPathLength);
    }
}

// Update hiker's position along the path
void Hiker::updatePosition(float deltaTime, const Terrain& terrain) {
    if (currentPathIndex >= pathPoints.size() - 1)
        return;

    glm::vec3 start = pathPoints[currentPathIndex];
    glm::vec3 end = pathPoints[currentPathIndex + 1];
    float speed = 40.0f; // Adjusted speed for visible movement

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
void Hiker::renderPath(const glm::mat4& view, const glm::mat4& projection, Shader& shader) {
    glDisable(GL_DEPTH_TEST); // Disable depth testing to ensure the path is always visible

    if (!shader.isLoaded()) {
        std::cerr << "ERROR: Hiker shader not loaded!" << std::endl;
        std::cerr << shader.getErrorLog() << std::endl;
        return;
    }

    shader.use();
    shader.setMat4("model", glm::mat4(1.0f));
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    // Loop through the path and render each segment with its own color
    for (size_t i = 0; i < pathPoints.size() - 1; ++i) {
        glm::vec3 start = pathPoints[i];
        glm::vec3 end = pathPoints[i + 1];

        // Calculate the average height of the segment
        float avgHeight = (start.y + end.y) / 2.0f;
        glm::vec3 color;

        // Assign color based on height thresholds
        if (avgHeight > 70.0f) {
            color = glm::vec3(1.0f, 0.0f, 0.0f); // Red for high elevations
        } else if (avgHeight > 50.0f) {
            color = glm::vec3(1.0f, 1.0f, 0.0f); // Yellow for mid elevations
        } else {
            color = glm::vec3(0.0f, 1.0f, 0.0f); // Green for low elevations
        }

        // Update shader uniform for path color
        shader.setVec3("pathColor", color);

        // Render the segment
        glm::vec3 segment[2] = { start, end }; // Segment vertices
        GLuint segmentVAO, segmentVBO;

        // Create VAO and VBO for this segment
        glGenVertexArrays(1, &segmentVAO);
        glGenBuffers(1, &segmentVBO);

        glBindVertexArray(segmentVAO);
        glBindBuffer(GL_ARRAY_BUFFER, segmentVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(segment), segment, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        glDrawArrays(GL_LINES, 0, 2); // Draw the line segment

        glBindVertexArray(0);
        glDeleteVertexArrays(1, &segmentVAO);
        glDeleteBuffers(1, &segmentVBO);
    }

    glEnable(GL_DEPTH_TEST); // Re-enable depth testing
}


void Hiker::setScales(float scale) {
    horizontalScale = scale;
    heightScale = scale;
}

// Set horizontal and vertical scales
void Hiker::setScales(float hScale, float vScale) {
    horizontalScale = hScale;
    heightScale = vScale;
}
const std::vector<glm::vec3>& Hiker::getPathPoints() const {
    return pathPoints;
}


void Hiker::setSpeed(float newSpeed) {
    speed = newSpeed;
}

// Get hiker's current position
glm::vec3 Hiker::getPosition() const {
    return currentPosition;
}
void Hiker::setTerrain(const Terrain* terrain) {
    terrainRef = terrain;
}
void Hiker::moveForward(float deltaTime) {
    movingForward = true;
    if (terrainRef) {
        updatePosition(deltaTime, *terrainRef);
    }
}

void Hiker::moveBackward(float deltaTime) {
    movingForward = false;
    if (terrainRef) {
        updatePosition(deltaTime, *terrainRef);
    }
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

//
//  animator.cpp
//  triangle
//
//  Created by Sumaia Jahan Brinti on 02/12/2024.
//

#include "animator.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Constructor
// In Animator.cpp, modify the constructor:
Animator::Animator()
    : characterVAO(0), characterVBO(0), characterPosition(0.0f),
    progress(0.0f), currentPathIndex(0), movingForward(true),
    movementSpeed(5.0f), // Reduce from 5.0f for smoother movement
    totalPathLength(0.0f), distanceHiked(0.0f),
    distanceRemaining(0.0f), timeElapsed(0.0f), elevationChange(0.0f) {}

// Destructor
Animator::~Animator() {
    cleanup();
}

// Initialize character buffers
void Animator::setupCharacterBuffers() {
    std::vector<float> vertices = {
        // Updated cube vertices
        -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,   1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f
    };


    glGenVertexArrays(1, &characterVAO);
    glGenBuffers(1, &characterVBO);

    glBindVertexArray(characterVAO);
    glBindBuffer(GL_ARRAY_BUFFER, characterVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    std::cout << "INFO: Character buffers initialized." << std::endl;
}

void Animator::moveForward(float deltaTime) {
    if (currentPathIndex < pathPoints.size() - 1) {
        progress += movementSpeed * deltaTime;
        if (progress >= 1.0f) {
            progress = 0.0f;
            currentPathIndex++;
        }
    }
}

void Animator::moveBackward(float deltaTime) {
    if (currentPathIndex > 0) {
        progress -= movementSpeed * deltaTime;
        if (progress <= 0.0f) {
            progress = 1.0f;
            currentPathIndex--;
        }
    }
}

// Load path points for the animation
void Animator::loadPathData(const std::vector<glm::vec3>& path) {
    pathPoints = path;
    if (!pathPoints.empty()) {
        characterPosition = pathPoints[0];
        totalPathLength = calculatePathLength();
        elevationChange = calculateElevationChange();
        distanceHiked = 0.0f;
        distanceRemaining = totalPathLength;
    }
    setupCharacterBuffers();
}

float Animator::calculatePathLength() {
    float length = 0.0f;
    for (size_t i = 1; i < pathPoints.size(); ++i) {
        length += glm::distance(pathPoints[i - 1], pathPoints[i]);
    }
    return length;
}

float Animator::calculateElevationChange() {
    float elevation = 0.0f;
    for (size_t i = 1; i < pathPoints.size(); ++i) {
        elevation += abs(pathPoints[i].y - pathPoints[i - 1].y);
    }
    return elevation;
}

void Animator::updatePosition(float deltaTime, const Terrain& terrain) {
    if (pathPoints.empty() || currentPathIndex >= pathPoints.size() - 1) return;

    glm::vec3 start = pathPoints[currentPathIndex];
    glm::vec3 end = pathPoints[currentPathIndex + 1];

    progress += movementSpeed * deltaTime;

    if (progress >= 1.0f) {
        progress = 0.0f;
        currentPathIndex++;
        if (currentPathIndex >= pathPoints.size() - 1) {
            currentPathIndex = 0;  // Loop back to start
        }
    }

    // Interpolate position along path
    characterPosition = glm::mix(start, end, progress);

    // Ensure character stays within terrain bounds
    characterPosition.x = glm::clamp(characterPosition.x, 0.0f, terrainWidth);
    characterPosition.z = glm::clamp(characterPosition.z, 0.0f, terrainDepth);

    // Adjust y-coordinate based on terrain height
    float terrainHeight = terrain.getHeightAtPosition(characterPosition.x, characterPosition.z);
    characterPosition.y = terrainHeight + 2.0f;

}


void Animator::render(const glm::mat4& view, const glm::mat4& projection, Shader& shader) {
    shader.use();

    // Make character more visible
    glm::mat4 model = glm::translate(glm::mat4(1.0f), characterPosition);
    model = glm::scale(model, glm::vec3(5.0f)); // Larger size for visibility

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    // Draw character in bright color
    shader.setVec3("pathColor", glm::vec3(0.5f, 0.0f, 0.5f)); // Purple color


    glBindVertexArray(characterVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Draw outline for better visibility
    glm::mat4 outlineModel = glm::scale(model, glm::vec3(1.1f));
    shader.setMat4("model", outlineModel);
    shader.setVec3("pathColor", glm::vec3(3.0f)); // White outline
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
}

// Reset hike stats
void Animator::resetHike() {
    currentPathIndex = 0;
    progress = 0.0f;
    movingForward = true;
    distanceHiked = 0.0f;
    distanceRemaining = totalPathLength;
    timeElapsed = 0.0f;
    characterPosition = pathPoints.empty() ? glm::vec3(0.0f) : pathPoints[0];
}

// Cleanup OpenGL resources
void Animator::cleanup() {
    if (characterVAO) glDeleteVertexArrays(1, &characterVAO);
    if (characterVBO) glDeleteBuffers(1, &characterVBO);

    characterVAO = 0;
    characterVBO = 0;

    std::cout << "INFO: Character resources cleaned up." << std::endl;
}

// Getters
float Animator::getDistanceHiked() const {
    return distanceHiked;
}

float Animator::getDistanceRemaining() const {
    return distanceRemaining;
}

float Animator::getTimeElapsed() const {
    return timeElapsed;
}

float Animator::getElevationChange() const {
    return elevationChange;
}

glm::vec3 Animator::getCurrentPosition() const {
    return characterPosition;
}

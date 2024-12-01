// HikingSimulator.cpp

#include "hikingSimulator.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <cmath>


// Constructor
HikingSimulator::HikingSimulator()
    : terrain(),
    hiker("/Users/sumaia/Desktop/triangle/triangle/resources/hiker_path.txt"),
      windowWidth(1280),
      windowHeight(720),
      viewMatrix(glm::mat4(1.0f)),
      projectionMatrix(glm::mat4(1.0f)),
      modelMatrix(glm::mat4(1.0f)),
      cameraPosition(glm::vec3(0.0f, 50.0f, 200.0f)) {}
//


void HikingSimulator::setWindowDimensions(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

bool HikingSimulator::initialize() {
    std::cout << "INFO: Initializing HikingSimulator..." << std::endl;

    // Load terrain data
    if (!terrain.loadTerrainData("/Users/sumaia/Desktop/triangle/triangle/resources/graydata.png")) {
        std::cerr << "ERROR: Failed to load terrain heightmap!" << std::endl;
        return false;
    }
    hiker.setScales(terrain.getHorizontalScale(), terrain.getHeightScale());
    setupMatrices();

    // Load hiker path data
    if (!hiker.loadPathData(terrain)) {
        std::cerr << "ERROR: Failed to load hiker path!" << std::endl;
          return false;
      }
      else {
          std::cout << "INFO: Hiker path loaded successfully." << std::endl;
      }
  
      // Initialize path shader
      pathShader = std::make_unique<Shader>("/Users/sumaia/Desktop/triangle/triangle/shaders/hikerVert.glsl", "/Users/sumaia/Desktop/triangle/triangle/shaders/hikerFrag.glsl");
      if (!pathShader->isLoaded()) {
          std::cerr << "ERROR: Failed to load path hiker shader during initialization." << std::endl;
          return false;
      }
    std::cout << "INFO: HikingSimulator initialized successfully." << std::endl;
    return true;
}
void HikingSimulator::setupMatrices() {
    float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
//    projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 20000.0f);
    projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 1.0f, 50000.0f);


    float terrainWidth = terrain.getWidth() * terrain.getHorizontalScale();
    float terrainHeight = terrain.getHeight() * terrain.getHorizontalScale();

    cameraPosition = glm::vec3(
        terrainWidth / 2.0f,   // Center X
        1000.0f,               // Elevated Y position
        -terrainHeight * 0.3f        // Move back along Z-axis to see the terrain
    );

    glm::vec3 cameraTarget = glm::vec3(
        terrainWidth / 2.0f,   // Center X
        0.0f,                  // Looking at ground level
        terrainHeight / 2.0f   // Center Z
//        terrain.getHeight() * terrain.getHorizontalScale() / 2.0f
    );
    // Use the standard up vector
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

    viewMatrix = glm::lookAt(cameraPosition, cameraTarget, upVector);
    modelMatrix = glm::mat4(1.0f);
    // Apply a rotation of 180 degrees around the X-axis to flip the terrain
//    modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));


//    viewMatrix = glm::lookAt(cameraPosition, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
//    modelMatrix = glm::mat4(1.0f);

    // Output camera position and target for debugging
    std::cout << "Camera Position: " << glm::to_string(cameraPosition) << std::endl;
    std::cout << "Camera Target: " << glm::to_string(cameraTarget) << std::endl;
}



void HikingSimulator::render(float deltaTime) {
    glEnable(GL_DEPTH_TEST);
    
    hiker.updatePosition(deltaTime, terrain);
    
    // Set uniforms for lighting and view projection matrices
    terrain.getShader().use();
    terrain.getShader().setVec3("lightPos", glm::vec3(0.0f, 200.0f, 0.0f));
    terrain.getShader().setMat4("view", viewMatrix);
    terrain.getShader().setMat4("projection", projectionMatrix);
    terrain.getShader().setMat4("model", modelMatrix);
    terrain.getShader().setVec3("viewPos", cameraPosition);
//    terrain.getShader().setFloat("maxHeight", terrain.getHeightScale() * 255.0f * 3.0f); // Adjusted for height amplification
    // Corrected maxHeight
    float maxHeight = terrain.getHeightScale() * 255.0f * 3.0f; // Multiply by 3.0f
    terrain.getShader().setFloat("maxHeight", maxHeight);

    
    // Render terrain
    terrain.render(modelMatrix, viewMatrix, projectionMatrix, cameraPosition);
    
    // Render hiker path
    if (pathShader && pathShader->isLoaded()) {
        hiker.renderPath(viewMatrix, projectionMatrix, *pathShader);
    } else {
        std::cerr << "ERROR: Path shader not loaded." << std::endl;
    }
    
    
    // Render seasonal effects and skybox if applicable
        // ...
}

void HikingSimulator::processCameraInput(GLFWwindow* window, float deltaTime) {
    float cameraSpeed = 500.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPosition += cameraSpeed * glm::vec3(0.0f, 0.0f, -1.0f);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPosition += cameraSpeed * glm::vec3(0.0f, 0.0f, 1.0f);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPosition += cameraSpeed * glm::vec3(-1.0f, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPosition += cameraSpeed * glm::vec3(1.0f, 0.0f, 0.0f);

    // Update view matrix
    glm::vec3 cameraTarget = glm::vec3(
        terrain.getWidth() * terrain.getHorizontalScale() / 2.0f,
        0.0f,
        terrain.getHeight() * terrain.getHorizontalScale() / 2.0f
    );
    viewMatrix = glm::lookAt(cameraPosition, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
}


/**
 * @brief Cleans up all resources.
 */
void HikingSimulator::cleanup() {
    terrain.cleanup();
    hiker.cleanup();
    Skybox::getInstance().cleanup();

    std::cout << "INFO: HikingSimulator cleaned up successfully." << std::endl;
}

/**
 * @brief Retrieves the view matrix.
 */
const glm::mat4& HikingSimulator::getViewMatrix() const { return viewMatrix; }

/**
 * @brief Retrieves the projection matrix.
 */
const glm::mat4& HikingSimulator::getProjectionMatrix() const { return projectionMatrix; }
//void checkOpenGLError(const std::string& location) {
//    GLenum err;
//    while((err = glGetError()) != GL_NO_ERROR) {
//        std::cerr << "OpenGL error at " << location << ": " << err << std::endl;
//    }
//}

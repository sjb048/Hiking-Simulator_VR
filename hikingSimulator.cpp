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
    width(0),
    height(0),
    animator(),
    lastFrameTime(0.0f),
    windowWidth(1280),
    windowHeight(720),
    viewMatrix(glm::mat4(1.0f)),
    projectionMatrix(glm::mat4(1.0f)),
    modelMatrix(glm::mat4(1.0f)),
    cameraMode(CameraMode::OVERVIEW),
    isMouseEnabled(false),
    firstMouse(true),
    pitch(0.0f),
    lastX(0.0f),
    lastY(0.0f),
    cameraPosition(glm::vec3(0.0f, 50.0f, 200.0f)) {}
//


void HikingSimulator::setWindowDimensions(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    updateProjectionMatrix();
}

bool HikingSimulator::initialize() {
    std::cout << "INFO: Initializing HikingSimulator..." << std::endl;

    // Load terrain data
    if (!terrain.loadTerrainData("/Users/sumaia/Desktop/triangle/triangle/resources/graydata.png")) {
        std::cerr << "ERROR: Failed to load terrain heightmap!" << std::endl;
        return false;
    }
   
//    // Set scales and terrain reference for the hiker
    hiker.setScales(1.0f);
    hiker.setTerrain(&terrain);

    windowWidth = terrain.getWidth();
    windowHeight = terrain.getHeight();

    if (windowWidth <= 0 || windowHeight <= 0) {
            std::cerr << "ERROR: Invalid terrain dimensions!" << std::endl;
            return false;
        }

    std::cout << "INFO: Terrain dimensions: " << windowWidth << " x " << windowHeight << std::endl;
    std::cout << "INFO: Terrain scales - Horizontal: " << terrain.getHorizontalScale()
            << ", Height: " << terrain.getHeightScale() << std::endl;
    hiker.setScales(terrain.getHorizontalScale(), terrain.getHeightScale());

    // Load hiker path data
    if (!hiker.loadPathData(terrain)) {
        std::cerr << "ERROR: Failed to load hiker path!" << std::endl;
          return false;
    }
    else {
          std::cout << "INFO: Hiker path loaded successfully." << std::endl;
    }
  
    // Initialize path shader
    pathShader = std::make_unique<Shader>("/Users/sumaia/Desktop/triangle/triangle/shaders/pathVert.glsl", "/Users/sumaia/Desktop/triangle/triangle/shaders/pathFrag.glsl");
    if (!pathShader->isLoaded()) {
        std::cerr << "ERROR: Failed to load path path shader during initialization." << std::endl;
        return false;
    }
    setupMatrices();
    // Load animated character path data
    animator.loadPathData(hiker.getPathPoints());

    lastFrameTime = static_cast<float>(glfwGetTime());
    std::cout << "INFO: HikingSimulator initialized successfully." << std::endl;
    return true;
}
void HikingSimulator::updateProjectionMatrix() {
    float aspectRatio = windowWidth / windowHeight;
    float verticalFOV = 50.0f;  // Wider FOV for better coverage
    float hScale = terrain.getHorizontalScale() * 10.0f;
    float viewDistance = std::max(windowWidth * hScale, windowHeight * hScale);

    projectionMatrix = glm::perspective(
        glm::radians(verticalFOV),
        aspectRatio,
        0.1f,
        viewDistance * 3.0f
    );
}
void HikingSimulator::setupMatrices() {
    updateProjectionMatrix();
    float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
    projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 1.0f, 20000.0f);

//    float hScale = terrain.getHorizontalScale() * 6.0f;
        
    float terrainWidth = terrain.getWidth() * terrain.getHorizontalScale();
    float terrainHeight = terrain.getHeight() * terrain.getHorizontalScale();
//    float maxTerrainHeight = terrain.getMaxHeight();

    cameraPosition = glm::vec3(
        terrainWidth / 2.0f,   // Center X
        1000.0f,               // Elevated Y position
        -terrainHeight * 0.3f        // Move back along Z-axis to see the terrain
    );


}



void HikingSimulator::render(float deltaTime) {
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    // Enable face culling for terrain
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    hiker.updatePosition(deltaTime, terrain);
    animator.updatePosition(deltaTime, terrain);
    // Set uniforms for lighting and view projection matrices
    terrain.getShader()->use();
    terrain.getShader()->setVec3("lightPos", glm::vec3(0.0f, 200.0f, 0.0f));
    terrain.getShader()->setMat4("view", viewMatrix);
    terrain.getShader()->setMat4("projection", projectionMatrix);
    terrain.getShader()->setMat4("model", modelMatrix);
    terrain.getShader()->setVec3("viewPos", cameraPosition);
//    terrain.getShader().setFloat("maxHeight", terrain.getHeightScale() * 255.0f * 3.0f); // Adjusted for height amplification
    // Corrected maxHeight
    float maxHeight = terrain.getHeightScale() * 255.0f * 3.0f; // Multiply by 3.0f
    terrain.getShader()->setFloat("maxHeight", maxHeight);


    // Render terrain
    terrain.render(modelMatrix, viewMatrix, projectionMatrix, cameraPosition);
    // Disable face culling for transparent objects
    glDisable(GL_CULL_FACE);
    // Render hiker path
    if (pathShader && pathShader->isLoaded()) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        pathShader->use();
        pathShader->setMat4("model", modelMatrix);
        pathShader->setMat4("view", viewMatrix);
        pathShader->setMat4("projection", projectionMatrix);
        pathShader->setFloat("heightOffset", 0.05f); // Minimal height offset
        pathShader->setVec3("pathColor", glm::vec3(1.0f, 0.0f, 0.0f));
//        pathShader->setVec3("pathColor", glm::vec3(0.8f, 0.3f, 0.0f));
//        pathShader->setVec3("pathColor", glm::vec3(0.0f, 1.0f, 1.0f)); // Cyan color

        hiker.renderPath(viewMatrix, projectionMatrix, *pathShader);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    } else {
        std::cerr << "ERROR: Path shader not loaded." << std::endl;
    }
    animator.render(viewMatrix, projectionMatrix, *pathShader);
      
}
void HikingSimulator::processCameraInput(GLFWwindow* window, float deltaTime) {
    // Camera mode toggles
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if (cameraMode != CameraMode::OVERVIEW) {
            cameraMode = CameraMode::OVERVIEW;
            isMouseEnabled = false;

            // Set initial camera position and target for OVERVIEW mode
            float centerX = terrain.getWidth() * terrain.getHorizontalScale() / 2.0f;
            float centerZ = terrain.getHeight() * terrain.getHorizontalScale() / 2.0f;
            float terrainMaxHeight = terrain.getMaxHeight();

            cameraPosition = glm::vec3(centerX, terrainMaxHeight + 200.0f, centerZ);
            cameraTarget = glm::vec3(centerX, 0.0f, centerZ);

            // Update view matrix
            viewMatrix = glm::lookAt(cameraPosition, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
        }
    } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (cameraMode != CameraMode::FOLLOW) {
            cameraMode = CameraMode::FOLLOW;
            isMouseEnabled = true;
            // No need to set positions here; we'll update them dynamically below
        }
    } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        if (cameraMode != CameraMode::FIRST_PERSON) {
            cameraMode = CameraMode::FIRST_PERSON;
            isMouseEnabled = true;
            // No need to set positions here; we'll update them dynamically below
        }
    }
    // Movement controls
    if (cameraMode != CameraMode::OVERVIEW) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            hiker.moveForward(deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            hiker.moveBackward(deltaTime);
        }
       
    }
    // Update camera position and view matrix based on camera mode
    if (cameraMode == CameraMode::OVERVIEW) {
        float centerX = terrain.getWidth() * terrain.getHorizontalScale() / 2.0f;
        float centerZ = terrain.getHeight() * terrain.getHorizontalScale() / 2.0f;
        float terrainMaxHeight = terrain.getMaxHeight();

        // Position the camera higher to get a better view
        cameraPosition = glm::vec3(centerX, terrainMaxHeight * 3.0f, centerZ);
        cameraTarget = glm::vec3(centerX, 0.0f, centerZ);

        viewMatrix = glm::lookAt(cameraPosition, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
    }
  else if (cameraMode == CameraMode::FOLLOW) {
        glm::vec3 hikerPosition = hiker.getPosition();
        glm::vec3 hikerFront = cameraFront;
        glm::vec3 hikerUp = cameraUp;

        // Set camera behind and above the hiker
        float distanceBehind = 10.0f; // Adjust as needed
        float heightOffset = 5.0f;    // Adjust as needed
        
        cameraPosition = hikerPosition - hikerFront * distanceBehind + glm::vec3(0.0f, heightOffset, 0.0f);
        cameraTarget = hikerPosition + hikerFront * 5.0f; // Look slightly ahead of the hiker

        viewMatrix = glm::lookAt(cameraPosition, cameraTarget, hikerUp);
    } else if (cameraMode == CameraMode::FIRST_PERSON) {
        glm::vec3 hikerPosition = hiker.getPosition();

        // Position the camera at the hiker's eyes
        float eyeHeight = 1.8f; // Adjust for hiker's eye level
        cameraPosition = hikerPosition + glm::vec3(0.0f, eyeHeight, 0.0f);

        cameraTarget = cameraPosition + cameraFront;
        viewMatrix = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
    }
}


void HikingSimulator::updateViewMatrix() {
    float hScale = terrain.getHorizontalScale() * 6.0f;
    float terrainWidth = width * hScale;
    float terrainDepth = height * hScale;
    float maxTerrainHeight = terrain.getMaxHeight();

    switch (cameraMode) {
    case CameraMode::OVERVIEW: {
//        float aspectRatio = windowWidth / windowHeight;
        float viewDistance = std::max(terrainWidth, terrainDepth) * 0.5f;
        float viewHeight = maxTerrainHeight * 2.5f;

        cameraPosition = glm::vec3(
            0.0f,
            viewHeight,
            viewDistance
        );

        glm::vec3 target(0.0f, 0.0f, 0.0f);

        viewMatrix = glm::lookAt(cameraPosition, target, glm::vec3(0.0f, 1.0f, 0.0f));
        break;
    }
    case CameraMode::FOLLOW: {
        glm::vec3 hikerPos = hiker.getPosition();
        float cameraHeight = maxTerrainHeight * 0.2f;
        float cameraDistance = 50.0f;

        cameraPosition = hikerPos + glm::vec3(0.0f, cameraHeight, cameraDistance);
        glm::vec3 target = hikerPos;
        viewMatrix = glm::lookAt(cameraPosition, target, cameraUp);
        break;
    }
    case CameraMode::FIRST_PERSON: {
        glm::vec3 hikerPos = hiker.getPosition();
        cameraPosition = hikerPos + glm::vec3(0.0f, 2.0f, 0.0f);
        viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        break;
    }
    }
}
/**
 * @brief Cleans up all resources.
 */
void HikingSimulator::cleanup() {
    terrain.cleanup();
    hiker.cleanup();
    animator.cleanup();
//    Skybox::getInstance().cleanup();

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

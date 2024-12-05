#ifndef HIKINGSIMULATOR_H
#define HIKINGSIMULATOR_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "terrain.h"
#include "hiker.h"
#include "Skybox.h"
#include "shader.h"
#include "animator.h"
#include <memory>
enum class CameraMode {
    OVERVIEW,
    FOLLOW,
    FIRST_PERSON
};

class HikingSimulator {
public:
    HikingSimulator();
   
    bool initialize();
    void processCameraInput(GLFWwindow* window, float deltaTime);
    void render(float deltaTime);
    void cleanup();
    const glm::mat4& getViewMatrix() const;
    const glm::mat4& getProjectionMatrix() const;
    void setWindowDimensions(int windowWidth, int windowHeight);
    void updateViewMatrix();
private:
    Terrain terrain;
    Hiker hiker;
    Animator  animator;
    
    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX = 0.0f;
    float lastY = 0.0f;
    
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 modelMatrix;
    bool firstMouse = true;
    bool isMouseEnabled = false;
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    int windowWidth;
    int height,width;
    int windowHeight;
    std::unique_ptr<Shader> pathShader;
    float lastFrameTime;
    void setupMatrices();
    void updateProjectionMatrix();
    void renderSkybox();
    CameraMode cameraMode;
    glm::vec3 cameraPosition;
};

#endif // HIKINGSIMULATOR_H


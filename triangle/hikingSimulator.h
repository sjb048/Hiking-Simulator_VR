#ifndef HIKINGSIMULATOR_H
#define HIKINGSIMULATOR_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "terrain.h"
#include "Hiker.h"
#include "seasonEffect.h"
#include "Skybox.h"
#include "shader.h"

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

private:
    Terrain terrain;
    Hiker hiker;
//    seasonEffect seasonEffect;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 modelMatrix;
    glm::vec3 cameraPosition;
    int windowWidth;
    int windowHeight;
    std::unique_ptr<Shader> pathShader;
    void setupMatrices();
    void renderSkybox();
};

#endif // HIKINGSIMULATOR_H


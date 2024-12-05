#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include "terrain.h"
#include "hiker.h"
#include "camera.h"
#include "hikingSimulator.h"

// Callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// Settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Camera
glm::vec3 cameraPosition = glm::vec3(0.0f, 100.0f, 200.0f);
glm::vec3 cameraFront = glm::normalize(-cameraPosition); // Looking towards the center
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float yaw = -90.0f;
float pitch = 0.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
// Hiker model (as a simple cube)
GLuint hikerVAO = 0;
GLuint hikerVBO = 0;
// Function to initialize hiker model (a simple cube)
void initHikerModel() {
    float cubeVertices[] = {
        // positions         // normals           // texture coordinates
        // Front face
        -0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        0.5f,  1.0f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        0.5f,  1.0f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  1.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        
        // Back face
        -0.5f,  0.0f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        0.5f,  0.0f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
        0.5f,  1.0f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        0.5f,  1.0f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  1.0f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f,  0.0f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        
        // Left face
        -0.5f,  1.0f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  1.0f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f,  0.0f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.0f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.0f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  1.0f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
        // Right face
        0.5f,  1.0f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  1.0f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.0f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.0f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        0.5f,  0.0f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  1.0f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        
        // Bottom face
        -0.5f,  0.0f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  0.0f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  0.0f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  0.0f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        -0.5f,  0.0f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.0f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        
        // Top face
        -0.5f,  1.0f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        0.5f,  1.0f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        0.5f,  1.0f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        0.5f,  1.0f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        -0.5f,  1.0f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        -0.5f,  1.0f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &hikerVAO);
    glGenBuffers(1, &hikerVBO);

    glBindVertexArray(hikerVAO);

    glBindBuffer(GL_ARRAY_BUFFER, hikerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}
// Function to render the hiker at its current position
void renderHiker(const glm::vec3& position, Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    shader.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(5.0f)); // Adjust scale as needed

    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

//    shader.setVec3("pathColor", glm::vec3(1.0f, 0.0f, 0.0f)); // Red color
    shader.setVec3("pathColor", glm::vec3(0.5f, 0.0f, 0.5f));
    glBindVertexArray(hikerVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // For macOS
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hiking Simulator", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    // Callbacks and OpenGL settings
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Capture the mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /*glEnable*/(GL_DEPTH_TEST);

  
    // Load terrain
    Terrain terrain;
    terrain.setHeightScale(50.0f);       // Adjust to make the mountain higher
    terrain.setHorizontalScale(1.0f);    // Adjust as needed
    if (!terrain.loadTerrainData("/Users/sumaia/Desktop/triangle/triangle/resources/graydata.png")) {
            return -1;
    }
    if (!terrain.loadTexture("/Users/sumaia/Desktop/triangle/triangle/resources/tex2.png")) {
        std::cerr << "ERROR: Failed to load terrain texture"<< std::endl;;
            return -1;
        }
    // Load hiker path
    Animator animator;
    Hiker hiker("/Users/sumaia/Desktop/triangle/triangle/resources/hiker_path.txt");
    hiker.setTerrain(&terrain);
    hiker.setScales(terrain.getHorizontalScale(), terrain.getHeightScale());

    if (!hiker.loadPathData(terrain)) {
        std::cerr << "ERROR: Failed to load hiker path data"<< std::endl;
        return -1;
    }
    else {
        std::cout << "INFO: Hiker path data loaded successfully."<< std::endl;
       
    }
    glEnable(GL_DEPTH_TEST);
//    glClearColor(0.5f, 0.7f, 0.9f, 1.0f);  Set a clear color that's not black
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    // Load shaders
    Shader terrainShader("/Users/sumaia/Desktop/triangle/triangle/shaders/terrainVert.glsl", "/Users/sumaia/Desktop/triangle/triangle/shaders/terrainFrag.glsl");
    Shader pathShader("/Users/sumaia/Desktop/triangle/triangle/shaders/pathVert.glsl",
                      "/Users/sumaia/Desktop/triangle/triangle/shaders/pathFrag.glsl");
    Shader hikerShader("/Users/sumaia/Desktop/triangle/triangle/shaders/hikerVert.glsl", "/Users/sumaia/Desktop/triangle/triangle/shaders/hikerFrag.glsl"); // New shader for hiker
    
    Shader waterShader("/Users/sumaia/Desktop/triangle/triangle/shaders/waterVert.glsl",
                       "/Users/sumaia/Desktop/triangle/triangle/shaders/waterFrag.glsl");
   
    if (!waterShader.isLoaded() )
    {
        std::cerr << "ERROR: Failed to load water shader path " << std::endl;
    }
    if (!terrainShader.isLoaded() || !pathShader.isLoaded() || !hikerShader.isLoaded()  ) {
        std::cerr << "ERROR: Failed to load shaders path " << std::endl;
        return -1;
    }

    // Pass terrain shader to terrain
    terrain.setShader(&terrainShader);

    // Initialize hiker model
    initHikerModel();

    // Set initial time
    lastFrame = static_cast<float>(glfwGetTime());
    // Setup water plane
    terrain.setupWaterPlane();
    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Frame timing
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

    
        // Clear the screen
        glClearColor(0.5f, 0.7f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        processInput(window);
        // Update camera front vector based on mouse movement
        cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront.y = sin(glm::radians(pitch));
        cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(cameraFront);

        // Camera/view transformation
        glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 1000.0f);

        // Render terrain
        terrain.render(glm::mat4(1.0f), view, projection, cameraPosition);
        // Render water
        
        waterShader.use();
        waterShader.setFloat("time", static_cast<float>(glfwGetTime())); // Animate water
        terrain.renderWater(glm::mat4(1.0f), view, projection, cameraPosition, waterShader);
//        

        // Update hiker's position
        hiker.updatePosition(deltaTime, terrain);

        // Render hiker's path
        hiker.renderPath(view, projection, pathShader);

        // Render hiker at current position
        glm::vec3 hikerPosition = hiker.getPosition();
        renderHiker(hikerPosition, hikerShader, view, projection);
        // Output hiker progress
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // Cleanup resources
    terrain.cleanup();
    hiker.cleanup();

    // Cleanup hiker model
    glDeleteVertexArrays(1, &hikerVAO);
    glDeleteBuffers(1, &hikerVBO);

//    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    float cameraSpeed = 50.0f * deltaTime; // Adjust accordingly

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPosition += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPosition -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float sensitivity = 0.2f; // Mouse sensitivity

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Constrain the pitch angle
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Update camera front vector
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// Simplified Shader Loader
GLuint compileShader(GLenum shaderType, const char* source) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

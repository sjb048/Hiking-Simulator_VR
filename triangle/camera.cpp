#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
// Constructor: Initialize camera with position, orientation, and settings
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(2.5f), MouseSensitivity(0.1f), Zoom(45.0f)
{
    Position = position; // Camera position
    WorldUp = up;        // Up direction in the world
    Yaw = yaw;           // Horizontal rotation angle
    Pitch = pitch;       // Vertical rotation angle
    updateCameraVectors(); // Update vectors based on yaw and pitch
}

// View Matrix: Calculate based on camera's position and orientation
glm::mat4 Camera::GetViewMatrix()
{
    // Look in the direction Front from the current Position, with Up as the reference
    return glm::lookAt(Position, Position + Front, Up);
}

// Handle Keyboard Input for Movement
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime; // Movement speed scaled by frame time
    if (direction == FORWARD)
        Position += Front * velocity;  // Move forward
    if (direction == BACKWARD)
        Position -= Front * velocity; // Move backward
    if (direction == LEFT)
        Position -= Right * velocity; // Move left
    if (direction == RIGHT)
        Position += Right * velocity; // Move right
}

// Handle Mouse Input for Rotation
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity; // Adjust for sensitivity
    yoffset *= MouseSensitivity;

    Yaw += xoffset;  // Adjust horizontal angle
    Pitch += yoffset; // Adjust vertical angle

    // Constrain pitch to prevent flipping
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    updateCameraVectors(); // Update vectors to reflect new angles
}

// Handle Mouse Scroll for Zoom
void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset; // Adjust zoom level
    if (Zoom < 1.0f)
        Zoom = 1.0f; // Minimum zoom
    if (Zoom > 45.0f)
        Zoom = 45.0f; // Maximum zoom
}

// Update Camera Vectors (Front, Right, Up)
void Camera::updateCameraVectors()
{
    // Calculate new front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front); // Normalize to unit vector

    // Recalculate right and up vectors
    Right = glm::normalize(glm::cross(Front, WorldUp)); // Right is perpendicular to Front and WorldUp
    Up = glm::normalize(glm::cross(Right, Front));      // Up is perpendicular to Right and Front
}

//// hikerVert.glsl
//
//#version 330 core
//
//layout(location = 0) in vec3 aPos;
//
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
//uniform float heightOffset; // Declare heightOffset uniform
//void main() {
//    vec3 position = aPos;
//    position.y += heightOffset; // Use heightOffset to adjust the Y position
//
//    gl_Position = projection * view * model * vec4(position, 1.0);
//}

#version 410 core

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 2.0);
}

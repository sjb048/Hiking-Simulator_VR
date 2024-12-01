// In camerashader.vs (or the appropriate vertex shader)
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos; // light position uniform
uniform vec3 viewPos;  // view (camera) position uniform

out vec3 FragPos;
out vec3 Normal;
out vec3 LightPos;
out vec3 ViewPos;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    LightPos = lightPos;
    ViewPos = viewPos;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}


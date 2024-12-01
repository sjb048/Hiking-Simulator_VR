// In camerashader.fs (or the appropriate fragment shader)
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;
in vec3 ViewPos;

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 FragColor;

void main() {
    // Simple lighting model (just diffuse reflection for now)
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);
    FragColor = vec4(diffuse, 1.0);
}


#version 330 core

out vec4 FragColor;

uniform vec3 cameraPosition; // Uniform for camera position
uniform float time;

void main() {
    // Simulate water color with reflection effect
    vec3 baseColor = vec3(0.0, 0.5, 0.8);
    float wave = sin(time * 2.0) * 0.05;
    float reflection = dot(normalize(cameraPosition), vec3(0.0, 1.0, 0.0)); // Simplistic reflection
    FragColor = vec4(baseColor + wave + reflection * 0.1, 0.5); // Semi-transparent water
}

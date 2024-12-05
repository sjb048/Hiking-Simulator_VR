#version 330 core

out vec4 FragColor;

uniform vec3 pathColor; // Declare pathColor uniform

void main() {
    FragColor = vec4(pathColor, 2.0); // Use pathColor to set the fragment color
}

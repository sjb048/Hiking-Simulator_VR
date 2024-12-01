// snowVert.glsl

#version 330 core

// Vertex attributes
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

// Uniforms
uniform mat4 view;
uniform mat4 projection;
uniform float time;

// Outgoing data to fragment shader
out vec2 TexCoords;
out float particleTime;

void main()
{
    TexCoords = aTexCoords;
    particleTime = time;

    // Calculate the position of the quad in screen space
    vec4 pos = vec4(aPos.x, aPos.y, 0.0, 1.0);
    gl_Position = pos;
}

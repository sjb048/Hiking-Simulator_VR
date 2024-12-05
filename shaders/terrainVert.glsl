
#version 330 core

// Input vertex attributes (from vertex buffer)
layout(location = 0) in vec3 aPosition;   // Matches attribute index 0
layout(location = 1) in vec3 aNormal;     // Matches attribute index 1
layout(location = 2) in vec2 aTexCoords;  // Matches attribute index 2

// Output data to the fragment shader
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Compute the fragment position in world space
    FragPos = vec3(model * vec4(aPosition, 1.0));

    // Transform the normal vector with the inverse transpose of the model matrix
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Pass the texture coordinates to the fragment shader
    TexCoords = aTexCoords;

    // Compute the final vertex position in clip space
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

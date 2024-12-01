// snowFrag.glsl

#version 330 core

// Incoming data from vertex shader
in float particleTime;

// Output fragment color
out vec4 FragColor;

// Constants for snow effect
const float PI = 3.14159265;

// Function to generate random value based on position
float rand(float seed)
{
    return fract(sin(seed) * 43758.5453);
}

void main()
{
    // Number of snowflakes
    float numFlakes = 100.0; // Reduced for performance

    // Initialize color
    vec3 color = vec3(0.0);

    // Loop through snowflakes
    for (float i = 0.0; i < numFlakes; i++) {
        // Generate a random position for each snowflake
        float x = rand(i) * 2.0 - 1.0;
        float y = fract(rand(i + 100.0) + particleTime * 0.1);

        // Calculate distance from current pixel to snowflake
        float dist = length(gl_FragCoord.xy / 800.0 * 2.0 - vec2(x, y)); // Assuming window width 800

        // Accumulate color based on distance (snowflakes appear as small dots)
        color += vec3(1.0) * smoothstep(0.02, 0.0, dist);
    }

    // Final color output with alpha blending
    FragColor = vec4(color, 0.5); // Semi-transparent snow
}

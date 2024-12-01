#version 330 core

in vec3 fragNormal;
in vec3 fragPosition;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float maxHeight;

out vec4 FragColor;

void main() {
//    vec3 color = vec3(0.2, 0.7, 0.3); // Greenish terrain
//    vec3 ambient = 0.2 * color;
//
//    vec3 lightDir = normalize(lightPos - fragPosition);
//    vec3 viewDir = normalize(viewPos - fragPosition);
//
//    float diffIntensity = max(dot(normalize(fragNormal), lightDir), 0.0);
//    vec3 diffuse = diffIntensity * color;
//
//    vec3 finalColor = ambient + diffuse;
//
//    // Clamp the final color to ensure it is within [0,1]
//    finalColor = clamp(finalColor, 0.0, 1.0);
//
//    FragColor = vec4(finalColor, 1.0);
    
    vec3 baseColor = vec3(0.2, 0.7, 0.3); // Greenish terrain

        // Calculate height factor
        float heightFactor = fragPosition.y / maxHeight;

        // Adjust color based on height (e.g., higher areas are lighter)
        vec3 color = mix(baseColor, vec3(1.0, 1.0, 1.0), heightFactor);

        vec3 ambient = 0.2 * color;

        vec3 lightDir = normalize(lightPos - fragPosition);
        vec3 viewDir = normalize(viewPos - fragPosition);

        float diffIntensity = max(dot(normalize(fragNormal), lightDir), 0.0);
        vec3 diffuse = diffIntensity * color;

        vec3 finalColor = ambient + diffuse;

        // Clamp the final color to ensure it is within [0,1]
        finalColor = clamp(finalColor, 0.0, 1.0);

        FragColor = vec4(finalColor, 1.0);
}

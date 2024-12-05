//#version 330 core
//out vec4 FragColor;
//
//in vec3 FragPos;
//in vec3 Normal;
//
//uniform vec3 lightPos;
//uniform vec3 lightColor;
//uniform vec3 viewPos;
//uniform float minHeight; // Minimum height of your terrain
//uniform float maxHeight; // Maximum height of your terrain
//
//void main()
//{
//    // Keep height unchanged and directly use FragPos.y for mountain shape
//
//    // Define colors for green mountain
//    vec3 color;
//    if (FragPos.y < minHeight + 0.3 * (maxHeight - minHeight)) {
//        // Low elevations: dark green
//        color = vec3(0.0, 0.6, 0.0);
//    } else if (FragPos.y < minHeight + 0.6 * (maxHeight - minHeight)) {
//        // Mid elevations: lighter green
//        float factor = (FragPos.y - (minHeight + 0.3 * (maxHeight - minHeight))) / (0.3 * (maxHeight - minHeight));
//        vec3 lowColor = vec3(0.0, 0.6, 0.0);  // Dark green
//        vec3 midColor = vec3(0.2, 0.8, 0.2);  // Light green
//        color = mix(lowColor, midColor, factor);
//    } else {
//        // High elevations: very light green
//        float factor = (FragPos.y - (minHeight + 0.6 * (maxHeight - minHeight))) / (0.4 * (maxHeight - minHeight));
//        vec3 midColor = vec3(0.2, 0.8, 0.2);  // Light green
//        vec3 highColor = vec3(0.8, 1.0, 0.8); // Very light green
//        color = mix(midColor, highColor, factor);
//    }
//
//    // Lighting calculations
//    // Ambient
//    vec3 ambient = 0.2 * color;
//
//    // Diffuse
//    vec3 norm = normalize(Normal);
//    vec3 lightDir = normalize(lightPos - FragPos);
//    float diff = max(dot(norm, lightDir), 0.0);
//    vec3 diffuse = diff * lightColor * color;
//
//    // Specular
//    vec3 viewDir = normalize(viewPos - FragPos);
//    vec3 reflectDir = reflect(-lightDir, norm);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//    vec3 specular = spec * lightColor * vec3(0.3);
//
//    // Final color
//    vec3 result = ambient + diffuse + specular;
//    FragColor = vec4(result, 1.0);
//}


// terrainFrag.glsl
#version 410 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D terrainTexture;
uniform float minHeight; // Minimum height of your terrain
uniform float maxHeight;
// Material properties
uniform float shininess;

void main() {
    vec3 color;
    if (FragPos.y < minHeight + 0.3 * (maxHeight - minHeight)) {
            // Low elevations: dark green
            color = vec3(0.0, 0.6, 0.0);
        } else if (FragPos.y < minHeight + 0.6 * (maxHeight - minHeight)) {
            // Mid elevations: lighter green
            float factor = (FragPos.y - (minHeight + 0.3 * (maxHeight - minHeight))) / (0.3 * (maxHeight - minHeight));
            vec3 lowColor = vec3(0.0, 0.6, 0.0);  // Dark green
            vec3 midColor = vec3(0.2, 0.8, 0.2);  // Light green
            color = mix(lowColor, midColor, factor);
        } else {
            // High elevations: very light green
            float factor = (FragPos.y - (minHeight + 0.6 * (maxHeight - minHeight))) / (0.4 * (maxHeight - minHeight));
            vec3 midColor = vec3(0.2, 0.8, 0.2);  // Light green
            vec3 highColor = vec3(0.8, 1.0, 0.8); // Very light green
            color = mix(midColor, highColor, factor);
        }
    // Ambient lighting
    vec3 ambient = vec3(0.3) * color;
    // Lighting calculations

    // Diffuse lighting
//    vec3 norm = normalize(Normal);
    // Flat shading
    vec3 norm = normalize(cross(dFdx(FragPos), dFdy(FragPos)));
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(0.7) * lightColor;

    // Specular lighting
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = spec *  lightColor;

    // Texture color
    vec3 textureColor = texture(terrainTexture, TexCoords).rgb;

    // Combine results
    vec3 result = (ambient + diffuse + specular) * textureColor;
    FragColor = vec4(result, 1.0);
}

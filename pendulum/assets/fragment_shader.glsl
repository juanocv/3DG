#version 300 es
precision mediump float;

in vec3 fragNormal;
in vec3 fragPosition;

out vec4 outColor;

uniform vec4 color;       // Object color
uniform vec3 lightDir;    // Dynamic light direction in view space

void main() {
    // Normalize the interpolated normal
    vec3 normal = normalize(fragNormal);

    // Normalize the light direction
    vec3 lightDirection = normalize(lightDir);

    // Calculate the diffuse component using Lambert's cosine law
    float diff = max(dot(normal, lightDirection), 0.0);

    // Define ambient lighting factor
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * color.rgb;

    // Define diffuse lighting factor
    vec3 diffuse = diff * color.rgb;

    // Combine ambient and diffuse components
    vec3 result = ambient + diffuse;

    outColor = vec4(result, color.a);
}

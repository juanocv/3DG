#version 300 es
precision mediump float;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

out vec3 fragNormal;
out vec3 fragPosition;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main() {
    // Transform the vertex position to view space
    vec4 viewPos = viewMatrix * modelMatrix * vec4(inPosition, 1.0);
    fragPosition = viewPos.xyz;

    // Transform the normal to view space and normalize
    mat3 normalMatrix = transpose(inverse(mat3(viewMatrix * modelMatrix)));
    fragNormal = normalize(normalMatrix * inNormal);

    // Final vertex position
    gl_Position = projMatrix * viewPos;
}

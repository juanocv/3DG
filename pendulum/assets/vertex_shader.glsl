#version 300 es
precision mediump float;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord; // Texture coordinates

out vec3 fragNormal;
out vec3 fragPosition;
out vec2 fragTexCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main() {
  vec4 viewPos = viewMatrix * modelMatrix * vec4(inPosition, 1.0);
  fragPosition = viewPos.xyz;

  mat3 normalMatrix = transpose(inverse(mat3(viewMatrix * modelMatrix)));
  fragNormal = normalize(normalMatrix * inNormal);

  fragTexCoord = inTexCoord;

  gl_Position = projMatrix * viewPos;
}
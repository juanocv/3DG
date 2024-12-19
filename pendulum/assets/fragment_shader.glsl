#version 300 es
precision mediump float;


in vec3 fragNormal;
in vec3 fragPosition;
in vec2 fragTexCoord;

out vec4 outColor;

uniform sampler2D textureSampler; // The texture sampler
uniform vec3 lightDir;            // Light direction in view space

uniform vec4 color; // This may still be used for tinting

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 lDir = normalize(lightDir);

    float diff = max(dot(normal, lDir), 0.0);

    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * color.rgb;

    vec3 diffuse = diff * color.rgb;

    // Sample the texture
    vec3 texColor = texture(textureSampler, fragTexCoord).rgb;

    // Combine texture color with shading
    vec3 result = (ambient + diffuse) * texColor;

    outColor = vec4(result, 1.0);
}

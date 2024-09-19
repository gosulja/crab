#version 330 core

in vec3 FragPos; // Fragment position
in vec3 Normal;  // Normal vector
in vec2 TexCoord; // Texture coordinates

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 color;
uniform float textureScale;
uniform sampler2D texture1;

void main() {
    // Ambient lighting
    vec3 ambient = 0.3 * lightColor; // Increased ambient factor

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = 0.8 * diff * lightColor; // Increased diffuse factor

    // Combine lighting
    vec3 lighting = (ambient + diffuse) * color;

    // Texture
    vec2 scaledTexCoord = TexCoord * textureScale;
    vec4 texColor = texture(texture1, scaledTexCoord);

    // Final color
    FragColor = vec4(lighting, 1.0) * texColor;
}

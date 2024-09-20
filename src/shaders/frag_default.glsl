#version 330 core

in vec3 FragPos;    // Fragment position
in vec3 Normal;     // Normal vector
in vec2 TexCoord;   // Texture coordinates

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 color;
uniform float textureScale;
uniform sampler2D texture1;
uniform bool hasTexture;

void main() {
    vec3 ambient = 0.2 * lightColor;
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.5) * spec;

    vec3 lighting = ambient + diffuse + specular;
    lighting *= color;

    vec3 finalColor = lighting;
    if (hasTexture) {
        vec2 scaledTexCoord = TexCoord * textureScale;
        vec3 texColor = texture(texture1, scaledTexCoord).rgb;
        finalColor *= texColor;
    }

    float brightness = dot(finalColor, vec3(0.2126, 0.7152, 0.0722));
    vec3 bloom = vec3(0.0);

    float threshold = 0.0;
    if (brightness > threshold) {
        bloom = finalColor * 0.5;
    }

    vec3 resultColor = finalColor + bloom;

    FragColor = vec4(resultColor, 1.0);
}

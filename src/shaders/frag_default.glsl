#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;
uniform vec3 color;

void main() {
    FragColor = texture(texture1, TexCoords);
}
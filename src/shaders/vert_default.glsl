#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec3 color;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;
uniform float textureScale;

void main() {
    // wavy distortion effect
    // vec3 distortion = aPos;
    // distortion.x += sin(time + aPos.y * 10.0) * 0.05;
    // distortion.z += cos(time + aPos.x * 10.0) * 0.05;

    // float gradient = (aPos.y + 0.5) / 1.0;
    // color = mix(vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 0.0), gradient);

    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoords = aTexCoord * textureScale;
}

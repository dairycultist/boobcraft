#version 150 core

in vec3 color;
in float depth;

out vec4 outColor;

void main() {
    outColor = vec4(1 - depth * 0.1, 1 - depth * 0.1, 1 - depth * 0.1, 1.0);
}
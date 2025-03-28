#version 150 core

in vec3 color;
in float depth;

out vec4 outColor;

void main() {
    outColor = vec4(color - vec3(depth, depth, depth) * 0.1, 1.0);
}
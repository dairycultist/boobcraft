#version 150 core

in vec3 normal_frag;
out vec4 outColor;

void main() {
    outColor = vec4(normal_frag, 1.0);
}
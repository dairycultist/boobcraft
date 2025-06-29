#version 150 core

in vec3 normal_camera;
out vec4 outColor;

void main() {

    float c = dot(normal_camera, vec3(0.577, 0.577, -0.577)) * 0.5 + 0.5;

    outColor = vec4(c, c, c, 1.0);
}
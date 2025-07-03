#version 150 core

in vec3 normal_camera;
in vec2 frag_UV;

out vec4 outColor;

void main() {

    float c = dot(normal_camera, vec3(0, 1.0, 0)) * 0.5 + 0.5;

    outColor = vec4(frag_UV.x * c, frag_UV.y * c, c, 1.0);
}
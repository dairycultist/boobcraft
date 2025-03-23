#version 150 core

in vec2 position;

out vec3 color;

void main() {
    color = vec3(position.x, 0.5, position.y);
    gl_Position = vec4(position, 0.0, 1.0);
}
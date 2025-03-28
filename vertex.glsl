#version 150 core

uniform vec3 player_pos;
uniform float player_rot;

in vec3 position;

out vec3 color;
out float depth;

// should probably see if I can enable backface culling for performance

void main() {
    color = vec3(position.x * 2, 0.5, position.y * 2);

    vec3 trans_pos = position - player_pos;

    float s = sin(player_rot);
    float c = cos(player_rot);

    trans_pos = mat3(
        c, 0.0, -s,
        0.0, 1.0, 0.0,
        s, 0.0, c
    ) * trans_pos;

    gl_Position = vec4(trans_pos.xy, 0.0, trans_pos.z);
    depth = gl_Position.w;
}
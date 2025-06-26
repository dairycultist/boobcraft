#version 150 core

uniform vec3 translation;
uniform float pitch;
uniform float yaw;

in vec3 position;

void main() {

    // constructing matrices should probably be done once in C and passed through a uniform...

    mat4 rot_pitch = mat4(
        1, 0,           0,          0,
        0, cos(pitch), -sin(pitch), 0,
        0, sin(pitch),  cos(pitch), 0,
        0, 0,           0,          1
    );
    mat4 rot_yaw = mat4(
         cos(yaw), 0, sin(yaw), 0,
         0,        1, 0,        0,
        -sin(yaw), 0, cos(yaw), 0,
        0,         0, 0,        1
    );

    // construct perspective projection matrix
    float fovY = 90;
    float aspectRatio = 2.0;
    float front = 0.01; // near plane
    float back = 10;    // far plane

    const float DEG2RAD = acos(-1.0f) / 180;

    float tangent = tan(fovY/2 * DEG2RAD);    // tangent of half fovY
    float top = front * tangent;              // half height of near plane
    float right = top * aspectRatio;          // half width of near plane

    mat4 proj;

    proj[0][0] =  front / right;
    proj[1][1] =  front / top;
    proj[2][2] = -(back + front) / (back - front);
    proj[2][3] = -1;
    proj[3][2] = -(2 * back * front) / (back - front);
    proj[3][3] =  0;

    // get final position
    gl_Position = proj * (rot_yaw * rot_pitch * vec4(position.xy, -position.z, 1.0) - vec4(translation.xy, -translation.z, 0.0));
}
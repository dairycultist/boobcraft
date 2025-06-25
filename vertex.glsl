#version 150 core

in vec3 position;

void main() {

    float fovY = 90;
    float aspectRatio = 2.0;
    float front = 0.01; // near plane
    float back = 10;    // far plane

    const float DEG2RAD = acos(-1.0f) / 180;

    float tangent = tan(fovY/2 * DEG2RAD);    // tangent of half fovY
    float top = front * tangent;              // half height of near plane
    float right = top * aspectRatio;          // half width of near plane

    mat4 perspective_projection;

    perspective_projection[0][0] =  front / right;
    perspective_projection[1][1] =  front / top;
    perspective_projection[2][2] = -(back + front) / (back - front);
    perspective_projection[2][3] = -1;
    perspective_projection[3][2] = -(2 * back * front) / (back - front);
    perspective_projection[3][3] =  0;

    gl_Position = perspective_projection * vec4(position.xy, -position.z, 1.0);
}
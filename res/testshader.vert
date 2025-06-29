#version 150 core

uniform vec3 translation;
uniform float pitch;
uniform float yaw;

in vec3 position;
in vec3 normal;
out vec3 normal_camera;

void main() {

    // constructing matrices (and inverse matrices) should probably be done once in C and passed through a uniform...

    mat4 model_matrix;   // to world space
    mat4 view_matrix;    // to view space (aka account for camera transformations)
    mat4 proj_matrix;    // to clip space (projection)
    mat4 normal_matrix;  // for normals, inversion of perceived model rotation

    view_matrix[0][0] = 1.;
    view_matrix[1][1] = 1.;
    view_matrix[2][2] = 1.;
    view_matrix[3][3] = 1.;

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

    normal_matrix = inverse(rot_pitch * rot_yaw);

    model_matrix = rot_yaw * rot_pitch;
    model_matrix[3][0] = -translation.x;
    model_matrix[3][1] = -translation.y;
    model_matrix[3][2] = translation.z;

    // construct perspective projection matrix
    float fovY = 90;
    float aspectRatio = 2.0;
    float front = 0.01; // near plane
    float back = 10;    // far plane

    const float DEG2RAD = acos(-1.0f) / 180;

    float tangent = tan(fovY/2 * DEG2RAD);    // tangent of half fovY
    float top = front * tangent;              // half height of near plane
    float right = top * aspectRatio;          // half width of near plane

    proj_matrix[0][0] =  front / right;
    proj_matrix[1][1] =  front / top;
    proj_matrix[2][2] = -(back + front) / (back - front);
    proj_matrix[2][3] = -1;
    proj_matrix[3][2] = -(2 * back * front) / (back - front);
    proj_matrix[3][3] =  0;

    // get final position
    gl_Position = proj_matrix * view_matrix * model_matrix * vec4(position.xy, -position.z, 1.0);

    normal_camera = (normal_matrix * vec4(normal, 1.0)).xyz;
}
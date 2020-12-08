#version 330 core

precision mediump float;

layout(location = 0) in vec3 IN_VERTEX;
layout(location = 1) in vec3 IN_NORMAL;
layout(location = 2) in mat4 IN_TRANSLATE;
layout(location = 6) in vec3 IN_COLOR;

// NOTE: `U_TIME` unused!
uniform float U_TIME;
uniform vec3  U_POSITION;
uniform mat4  U_PROJECTION;
uniform mat4  U_VIEW;

out vec3 VERT_OUT_VERTEX;
out vec3 VERT_OUT_NORMAL;
out vec3 VERT_OUT_POSITION;
out vec3 VERT_OUT_COLOR;

void main() {
    VERT_OUT_VERTEX = vec3(IN_TRANSLATE * vec4(IN_VERTEX, 1.0));
    VERT_OUT_NORMAL = mat3(transpose(inverse(IN_TRANSLATE))) * IN_NORMAL;
    VERT_OUT_POSITION = U_POSITION;
    VERT_OUT_COLOR = IN_COLOR;
    gl_Position = U_PROJECTION * U_VIEW * IN_TRANSLATE * vec4(IN_VERTEX, 1.0);
}

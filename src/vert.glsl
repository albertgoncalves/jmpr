#version 330 core

precision mediump float;

layout(location = 0) in vec3 IN_VERTEX;
layout(location = 1) in mat4 IN_TRANSLATE;

uniform float U_TIME;
uniform mat4  U_PROJECTION;
uniform mat4  U_VIEW;

out vec3 VERT_OUT_COLOR;

void main() {
    VERT_OUT_COLOR.r = sin((U_TIME + IN_VERTEX.x) * 1.35);
    VERT_OUT_COLOR.g = cos((U_TIME + IN_VERTEX.y) / 2.45);
    VERT_OUT_COLOR.b = sin((U_TIME + IN_VERTEX.x) / 1.35);
    gl_Position = U_PROJECTION * U_VIEW * IN_TRANSLATE * vec4(IN_VERTEX, 1.0);
}

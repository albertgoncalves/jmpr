#version 330 core

precision mediump float;

layout(location = 0) in vec3 IN_VERTEX;
layout(location = 1) in mat4 IN_TRANSLATE;

uniform mat4  U_MODEL;
uniform float U_TIME;
uniform mat4  U_PROJECTION;
uniform mat4  U_VIEW;

out vec3 VERT_OUT_COLOR;

void main() {
    VERT_OUT_COLOR.x = sin((U_TIME + IN_VERTEX.x) * 1.35);
    VERT_OUT_COLOR.y = cos((U_TIME + IN_VERTEX.y) / 2.45);
    VERT_OUT_COLOR.z = sin((U_TIME + IN_VERTEX.x) / 1.35);
    gl_Position =
        U_PROJECTION * U_VIEW * IN_TRANSLATE * U_MODEL * vec4(IN_VERTEX, 1.0);
}
